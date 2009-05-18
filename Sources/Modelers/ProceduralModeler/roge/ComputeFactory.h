/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef _COMPUTE_H_
#define _COMPUTE_H_

#include <ctime>

#include <ogr_core.h>
#include <ogr_feature.h>
#include <ogrsf_frmts.h>

#include <Ogre.h>

#include "Extruder/Extrusion.h"
#include "Linear/Linear.h"

class OgreExtrusion;

namespace roge {
	
	typedef enum intersectionType { ENDPOINT, INTERSECT, COMPLEX_INTERSECT, HIGHWAY };
	typedef enum sideSelector { NONE, RIGHT, LEFT };
		
	struct roadIntersectionPoints {
		roadIntersectionPoints() :
			rightIntersection(ENDPOINT), leftIntersection(ENDPOINT),
					projSide(NONE), rightPrimRank(-1), leftPrimRank(-1),
					highwayEndPrimRank(0), leftContour(false), interIndex(-1) {
		}
		intersectionType rightIntersection; //is there an intersection with the road on the right or do we just took the end point
		intersectionType leftIntersection; //is there an intersection with the road on the left or do we just took the end point
		Ogre::Vector3 rightExtremity; //the extremity on the right of the road (intersection or end point)
		Ogre::Vector3 leftExtremity; //the extremity on the left of the road (intersection or end point)
		sideSelector projSide; //to know where we need a projection (if any)
		Ogre::Vector3 projection; //the projection from the extremity on the side (if any)
		int projPrimRank; //the number of the primitive were we found the projection
		std::vector<Ogre::Vector3> highwaySidePoints; //store the points of the road belonging to the highway (if any)
		std::vector<Ogre::Vector3> sidePoints; //store the points from the side where we projected
		int rightPrimRank; //the number of the primitive were we found the new end of the road on its right side
		int leftPrimRank; //the number of the primitive were we found the new end of the road on its left side
		Ogre::Vector3 highwayPoint; //if there is a "highway problem" somewhere
		int highwayPrimRank; //to know from which primitive start adding border points of the wider road
		int highwayEndPrimRank; //in case of an intersection from a previous road with the wider road, to know where to stop
		Ogre::Vector3 rightMarkingExtremity; //a point to lengthen a bit the geometry, to have more room to texture
		Ogre::Vector3 leftMarkingExtremity; //a point to lengthen a bit the geometry, to have more room to texture
		int rightMarkingPrimRank; //the number of the primitive were we found the end of the marking of the road on its right side
		int leftMarkingPrimRank; //the number of the primitive were we found the end of the marking of the road on its left side
		
		//next members are useful fo contour computation, but not for intersections
		bool leftContour; //boolean to know if we have already computed contour for this side of the road
		unsigned int interIndex; //index (in the network) of the intersection bound to this road on this side
	};
	
	class RoadWay;
	
	class interInfo {
	public:
		interInfo() {
		}
		interInfo(unsigned int i, Ogre::SubMesh* r, bool p,
				double tl, RoadWay* rw) :
			index(i), roadGeom(r), position(p), texLength(tl),
					roadway(rw) {
		}
		unsigned int index;
		Ogre::SubMesh* roadGeom;
		bool position;
		double texLength;
		const RoadWay* roadway;
	
	};
	
	//------------------------------------------------
	
	class RoadNetwork;
	class Intersection;
	
	/*! Class that creates roads and intersection geometry
	 */
	class ComputeFactory {
	public:
		ComputeFactory() : _randomness(time(NULL)), _interMaterialName("arInterMaterial"){		
		}
	
		~ComputeFactory() {
		}
	
		/*! Extrude all roads of the network
		 * \param network network which contains the roads to extrude
		 * \return a pointer on osg::Geode
		 */
		Ogre::MeshPtr extrude(RoadNetwork& network, Extrusion& extruder);
	
		/*!  Create intersections geometry and contours if needed
		 * \param geode the geode containing the roads previously extruded
		 * \param network the road network from which we want to create intersections
		 * \param createContours tells us if contours need to be extracted
		 * \param contours a reference to a pointer on the arFbNode where we will add arFbContours
		 * \return the Geode with intersections
		 */
		Ogre::MeshPtr createIntersections(Ogre::MeshPtr& roadNetworkMesh,
			RoadNetwork& network, bool createContours, std::vector< OGRLineString* >& contours);
	
	private:
	
		/*! Create a road list with useful information to create intersections
		 * \param geode geode containing extruded roads
		 * \param network network from which we already have extruded roads
		 * \param i index of current intersection in network
		 * \return a vector containing informations on the roads of the current intersection
		 */
		std::vector<interInfo> createRoadList(Ogre::MeshPtr&	 roadNetworkMesh, RoadNetwork& network, unsigned int i);
	
		/*! build geometry of an intersection
		 * \param inters the geode in which we will add intersections geometry
		 * \param geode the geode containing the extruded roads
		 * \param segmentLimit parameter from config file to know where to stop when searching for intersections between roads
		 * \param roadList list of roads of this intersection
		 * \param intersectionPoints vector of informations on points taken from each road of the intersection
		 * \param inter the adress of the intersection
		 * \param interIndex the index in the network of the intersection
		 */
		void buildGeometry(Ogre::MeshPtr intersectionsMesh,
				Ogre::MeshPtr roadNetworkMesh, int segmentLimit,
				std::vector<interInfo>& roadList,
				std::vector<roadIntersectionPoints>& intersectionPoints,
				Intersection* inter, unsigned int interIndex,
				Ogre::AxisAlignedBox& aabox);
	
		/*! compute intersection geometry
		 * \param roadList list of roads of this intersection
		 * \param intersectionPoints vector of informations on points taken from each road of the intersection
		 * \param isComplex complexity of this intersection (true if merged intersection)
		 * \param outRoads vertices of portions of roads entering the intersection
		 * \param normalRoads normals of portions of roads entering the intersection
		 * \param texRoads texture coordinate of portions of roads entering the intersection, first layer for pavement
		 * \param texMarking texture coordinate of portions of roads entering the intersection, second layer for marking
		 * \param outInter vertices of the intersection
		 * \param colorInter color of the intersection
		 * \param normalInter normals of the intersection
		 * \param texInter texture coordinates of the intersection
		 * \param segmentLimit parameter from config file to know where to stop when searching for intersections between roads
		 * \param inter pointer on the intersection currently computed
		 * \param interIndex the index in the network of the intersection
		 * \return true if none error occured
		 */
		bool computeIntersectionGeometry(std::vector<interInfo>& roadList,
			std::vector<roadIntersectionPoints>& intersectionPoints, bool isComplex,
			std::vector<std::vector<Ogre::Vector3> >& outRoads,
			std::vector<std::vector<Ogre::Vector3> >& normalRoads,
			std::vector<std::vector<Ogre::Vector2> >& texRoads,
				std::vector<std::vector<Ogre::Vector2> >& texMarking,
				std::vector<Ogre::Vector3>& outInter,
				std::vector<Ogre::Vector4>& colorInter,
				std::vector<Ogre::Vector3>& normalInter,
				std::vector<Ogre::Vector2>& texInter, const int& segmentLimit,
				Intersection* inter, unsigned interIndex);
	
		/*! try to get intersections between roads
		 * \param roadList list of roads of this intersection
		 * \param segmentLimit number of segments of the road to go through before stopping
		 * \param nb number of roads in the intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 * \param isComplex whether or not the intersection is complex
		 * \param intersection a pointer to the intersection we are computing the geometry
		 * \return true if nothing bad happened
		 */
		bool computeIntersectionBetweenRoads(std::vector<interInfo>& roadList,
				int segmentLimit, unsigned int nb,
				std::vector<roadIntersectionPoints>& intersectionPoints, bool isComplex,
				Intersection* intersection);
	
		/*! get additional points to complete the geometry of a complex intersection from two roads
		 * \param currentPosition the position of the intersection for the current road (end or begin)
		 * \param nextPosition the position of the intersection for the next road (end or begin)
		 * \param currentGeom the geometry of the current road
		 * \param nexttGeom the geometry of the next road
		 * \param rIPCurrent informations on the points taken from the current road
		 * \param rIPNext informations on the points taken from the next road
		 * \param intersection a pointer to the intersection currently computed
		 * \return true if no error occured
		 */
		bool getComplexIntersectionAdditionalPoints(bool currentPosition, bool nextPosition,
				float* vdataCurrent, unsigned int nbVerticesCurrent,
				float* vdataNext, unsigned int nbVerticesNext,
				roadIntersectionPoints* rIPCurrent,
				roadIntersectionPoints* rIPNext,
				Intersection* intersection);
	
		/*! try to get intersections between roads by stretching them
		 * \param roadList list of roads of this intersection
		 * \param segmentLimit number of segments of the road to go through before stopping
		 * \param nb number of roads in the intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 * \return true if nothing bad happened
		 */
		bool getStretchedSegmentsIntersection(std::vector<interInfo>& roadList,
				int segmentLimit, unsigned int nb,
				std::vector<roadIntersectionPoints>& intersectionPoints);
	
		/*! project points for each road to end it perpendicularly to its axis
		 * \param roadList list of roads of this intersection
		 * \param nb number of roads in the intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 * \return true if nothing bad happened
		 */
		bool projectPoints(std::vector<interInfo>& roadList, unsigned int nb,
				std::vector<roadIntersectionPoints>& intersectionPoints);
	
		/*! stretch a bit the roads intersection parts to have room for texturing
		 * \param roadList list of roads of this intersection
		 * \param nb number of roads in the intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 * \return true if nothing bad happened
		 */
		bool stretchRoadsIntersectionParts(std::vector<interInfo>& roadList,
				unsigned int nb, std::vector<roadIntersectionPoints>& intersectionPoints);
	
		/*! merge all points of the intersection, in a useful order
		 * and handles texture coordinates
		 * \param outRoads points taken from the roads of the intersection
		 * \param normalRoads normal for the points taken from the roads
		 * \param texRoads texture coordinates for the points taken from the roads
		 * \param outInter points for the geometry of the intersection
		 * \param colorInter colors for the points of the intersection
		 * \param normalInter normals for the points of the intersection
		 * \param texInter texture coordinates for the points of the intersection
		 * \param roadList informations on roads of this intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 * \param intersection a pointer on the intersection currently computed
		 * \return true if nothing bad happened
		 */
		bool mergePoints(std::vector<std::vector<Ogre::Vector3> >& outRoads,
				std::vector<std::vector<Ogre::Vector3> >& normalRoads,
				std::vector<std::vector<Ogre::Vector2> >& texRoads,
				std::vector<Ogre::Vector3>& outInter,
				std::vector<Ogre::Vector4>& colorInter,
				std::vector<Ogre::Vector3>& normalInter,
				std::vector<Ogre::Vector2>& texInter, int nb,
				std::vector<interInfo>& roadList,
				std::vector<roadIntersectionPoints>& intersectionPoints,
				Intersection* intersection);
	
		/*! detect if the intersection can be malformed
		 * \param roadList list of roads of this intersection
		 * \param nb number of roads in the intersection
		 * \param intersection a pointer to the intersection we want to detect the (mal)formation
		 * \param outRoads points of the roads of the intersection
		 * \param colorInter color of the intersection
		 * \param nbInterPoints number of points of the intersection
		 * \param intersectionPoints informations on roads of the intersection
		 * \param segmentLimit the number of segment of a road to test before stopping
		 * \return whether or not the intersection seems to be malformed
		 */
		bool detectMalformedIntersection(std::vector<interInfo>& roadList,
				unsigned int nb, Intersection* intersection,
				std::vector<std::vector<Ogre::Vector3> >& outRoads,
				std::vector<Ogre::Vector4>& colorInter,
				unsigned int nbInterPoints,
				std::vector<roadIntersectionPoints>& intersectionPoints,
				const int& segmentLimit);
	
		/*! remove primitives that are recovered by intersection
		 * \param roadGeom geometry of the road
		 * \param sens direction of the road
		 * \param nbPrim number of primitive to erase
		 */
		void removePrimitives(Ogre::SubMesh* roadGeom, bool sens,
				unsigned int nbPrim);
	
		/*! level current intersection
		 * \param outRoads all points of roads of the intersection
		 * \param outInter all points of the intersection
		 */
		void levelIntersection(std::vector<std::vector<Ogre::Vector3> >& outRoads,
				std::vector<Ogre::Vector3>& outInter,
				std::vector<roadIntersectionPoints>& intersectionPoints);
	
		/*! update current intersection: move road extremities to fit on the intersection 
		 * \param roadList list of roads of this intersection
		 * \param intersectionPoints the informations for each road of the intersection
		 */
		void updateIntersection(std::vector<interInfo>& roadList,
				std::vector<roadIntersectionPoints>& intersectionPoints);
	
		/*! Gets lines of the road
		 * \param position position (end or begin) of the road on current intersection
		 * \param cross to indicate whether we want the right (true) or left (false) side
		 * \param geom array of points of the road
		 * \param indice (index of current segment)*4
		 * \param line result line (side of the real intersections)
		 * \param p10 result first extremity of line
		 * \param p11 result second extremity of line
		 */
		void getArline(bool position, bool cross, float* vadata, unsigned int nbVertices,
				unsigned int indice, Line<double> & line, Vec3d& p10,
				Vec3<double>& p11);
	
		/*! Gets points of a side of the road as Vec3<double>
		 * \param position position (end or begin) of the road on current intersection
		 * \param cross to indicate whether we want the right (true) or left (false) side
		 * \param geom array of points of the road
		 * \param indice (index of current segment)*4
		 * \param p10 exterior point of the side
		 * \param p11 interior point of the side
		 */
		void getSide(bool position, bool cross, float* vdata, unsigned int nbVertices,
				unsigned int indice, Vec3d& p10, Vec3d& p11);
	
		/*! gets intersection from 2 segments
		 * \param p10 first point of first segment
		 * \param p11 second point of first segment
		 * \param p20 first point of second segment
		 * \param p21 second point of second segment
		 * \param pLine1 intersection point on the first segment (with the tolerance, this function could return 2 differents intersection points...)
		 * \param pLine2 intersection point on the second segment
		 * \param tolerance tolerance
		 * \return true if an intersection was found
		 */
		bool getIntersectionFrom2Segments(Vec3<double> p10, Vec3<double> p11,
				Vec3<double> p20, Vec3<double> p21, Vec3<double>& pLine1,
				Vec3<double>& pLine2, double tolerance);
		
		double getRoadSegmentLength(float* vdata, unsigned int segmentNumber);
		
		std::vector<Ogre::Vector2> computeIntersectionCenterTexCoords(std::vector<interInfo>& roadList,
				std::vector<Ogre::Vector3>& outInter, double texLength);
		
		void createIntersectionCenterSubmesh(Ogre::MeshPtr intersectionsMesh,
				const std::vector<Vec3d>& vertices, const std::vector<Vec3i>& triangles,
				std::vector<Ogre::Vector2>& texCoords, std::vector<Ogre::Vector3>& outInter,
				Ogre::AxisAlignedBox& aabox);
		
		void createIntersectionRoadPartSubmesh(Ogre::MeshPtr intersectionsMesh,
				interInfo& roadInfos,
				std::vector<Ogre::Vector3>& outRoad,
				std::vector<Ogre::Vector2>& texRoad,
				Ogre::AxisAlignedBox& aabox);
	
		/*! Compute texture coordinates of a road
		 * \param roadGeom geometry of the road
		 * \param texLength unit length of the texture
		 */
		void computeRoadTexCoords(Ogre::SubMesh* roadGeom,
				double texLength);
		
		void simplifyMesh(Ogre::MeshPtr& mesh, std::vector<std::string> textureNames);
	
		/*! Create contours for the whole network
		 * \param computingInformations vector containing the informations taken from road intersections during the computing of intersections geometry
		 * \param nbIntersections number of intersections in the network
		 * \return an arFbNode containing the contours
		 */
		std::vector< OGRLineString* > createContours(std::vector<std::pair<std::vector<interInfo>,
				std::vector<roadIntersectionPoints> > >& computingInformations,
				int nbIntersections);
	
		/*! Compute a contour
		 * \param interIndex the index of the intersection in the network where we start the contour
		 * \param roadIndex the index of the road in the intersection's list where we are starting the contour
		 * \param position where do we start for the road (true => end, false => beginning)
		 * \return an arFbContour representing the computed contour
		 */
		OGRLineString* computeContour(
				unsigned int interIndex,unsigned int roadIndex,
				std::vector<std::pair<std::vector<interInfo>, std::vector<roadIntersectionPoints> > >& computingInformations);
		
		time_t _randomness;
		
		Ogre::String _interMaterialName;
	};

} //namespace arSolipsisRoadGenerator

#endif
