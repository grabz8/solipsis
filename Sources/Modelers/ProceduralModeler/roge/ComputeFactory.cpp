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

#include <cmath>
#include <cfloat>
#include <ostream>

#include <Ogre.h>

#include "Extruder/Extrusion.h"
#include "Extruder/gle.h"
#include "Extruder/texInfo.h"

#include "ComputeFactory.h"
#include "Config.h"
#include "RoadNetwork.h"
#include "RoadWay.h"
#include "Intersection.h"
#include "Pavement.h"
#include "Triangulator.h"

#define MINANGLE 1
#define PI 3.14159265358978
#define EPSILON 0.1
#define STRETCHLENGTH 0.5
#define CONFLICTANGLE -0.707
#define SMOOTHPORTION 0.5
#define SMOOTHMAXLENGTH 100

namespace roge {
	
	extern float toleranceValue;
	
	using namespace Ogre;
	
	//------------------------------------------------------------
	// EXTRUSION
	//------------------------------------------------------------
	
	double doubleModulus(double a, double b){
		int result = static_cast<int>(floor(a / b));
		return a - static_cast<double>( result ) * b;
	}
	
	// extrude roads from the network
	MeshPtr ComputeFactory::extrude(RoadNetwork& network, Extrusion& extruder) {
	
		unsigned int nbRoadways = network.getRoadWayList().size();
		
		//get texture informations
		std::map< std::pair< std::string, int >, texInfo* > texInformations;
		Config::instance()->getTextureInfo( texInformations );
	
		//Create materials for each texture used for the network
		std::map< std::pair< std::string, int >, texInfo* >::iterator iter;
		for(iter = texInformations.begin() ; iter != texInformations.end() ; ++iter){
			extruder.createMaterial(iter->second->filename);
		}
	
		//IDs for submeshes of roadways in ogre
		int* submeshIDs = (int*)malloc(nbRoadways*sizeof(int));
			
		//start the extrusion of roadways
		for (unsigned int i=0; i < nbRoadways; i++) {
				
			//get the current roadway
			RoadWay roadway = network.getRoadWayList()[i];
	
			//number of contour points 
			int ncp = 2;
			
			//width of the road
			double width = roadway.getStartingWidth();
			
			//store contour points
			gleDouble** contour = (gleDouble**)malloc(ncp*sizeof(gleDouble*));
			contour[0] = (gleDouble*)malloc(2*sizeof(gleDouble));
			contour[1] = (gleDouble*)malloc(2*sizeof(gleDouble));
			contour[0][0] = -width/2;
			contour[0][1] = 0;
			contour[1][0] = width/2;
			contour[1][1] = 0;
			
			//normals for the points
			gleDouble** norm_contour = (gleDouble**)malloc(ncp*sizeof(gleDouble*));
			norm_contour[0] = (gleDouble*)malloc(2*sizeof(gleDouble));
			norm_contour[1] = (gleDouble*)malloc(2*sizeof(gleDouble));
			norm_contour[0][0] = 0;
			norm_contour[0][1] = 1;
			norm_contour[1][0] = 0;
			norm_contour[1][1] = 1;
	
			Pavement* pavement = roadway.getPavement(0);
	
			unsigned int numberOfWays = pavement->getLineList().size();	
			
			//get texture informations for this roadway
			texInfo* geometryTexInfos;
			map<pair<string, int>, texInfo*>::iterator it = texInformations.find(make_pair(roadway.getNature(), numberOfWays));
			if(it != texInformations.end()){
				geometryTexInfos = it->second;
			} else{
				string defaultNature("CHEMIN");
				int defaultNbOfWays = 1;
				geometryTexInfos = texInformations.find(make_pair(defaultNature, defaultNbOfWays))->second;
			}
	
			//get axis that will be used to define the extrusion path
			vector<Vec3<double> > axis = pavement->getAxis();
			
			//we need to transform the vector of Vec3 in something understandable by the OgreExtruder
			vector<Vec3<double> >::iterator iter;
			int roadLength = axis.size();
			
			//first and last extruded segment won't be drawn, so we add two points in the path.
			int extrusionPathSize = roadLength + 2;
			gleDouble** extrusionPath = (gleDouble**)malloc(extrusionPathSize*sizeof(gleDouble*));
			
			//here we add the first point of the path, symetric of the second point of the axis
			//by the first point of the axis
			extrusionPath[0] = (gleDouble*)malloc(3*sizeof(gleDouble));
			extrusionPath[0][0] = 2*((axis[0])[0]) - (axis[1])[0];
			extrusionPath[0][1] = 2*((axis[0])[1]) - (axis[1])[1];
			extrusionPath[0][2] = 2*((axis[0])[2]) - (axis[1])[2];
			
			//we copy each point of the path
			int j = 1;
			for(iter = axis.begin() ; iter != axis.end() ; ++iter, ++j){
				extrusionPath[j] = (gleDouble*)malloc(3*sizeof(gleDouble));
				extrusionPath[j][0] = (*iter)[0];
				extrusionPath[j][1] = (*iter)[1];
				extrusionPath[j][2] = (*iter)[2];
			}
			
			//we add a last point, same technique than first point
			extrusionPath[j] = (gleDouble*)malloc(3*sizeof(gleDouble));
			extrusionPath[j][0] = 2*((axis[roadLength-1])[0]) - (axis[roadLength-2])[0];
			extrusionPath[j][1] = 2*((axis[roadLength-1])[1]) - (axis[roadLength-2])[1];
			extrusionPath[j][2] = 2*((axis[roadLength-1])[2]) - (axis[roadLength-2])[2];
		
			//the up vector for the contour
			gleDouble up_vector[3] = {0,1,0};
			
			//a osstream to generate the submesh name.
			ostringstream oss;
			oss << "submesh" << _randomness << "_" << i;
			
			//xform array may be used if a road have different widths at its start
			//and at its end, useful to connect two roads of different widths
			//xform array contain scaling to be applied on each segment of the road
			//to have a simple interpolation
			gleDouble*** xformArray = NULL;
			if(roadway.getDifferentWidth()){
				double scalingFactor = roadway.getEndingWidth() / width;
				//fill the xform array with transformations to go from the starting width
				//to the ending width
				xformArray = (gleDouble***)malloc((axis.size()+2) * sizeof(gleDouble**));
				for(unsigned int j = 0 ; j < axis.size()+2 ; ++j){
					xformArray[j] = (gleDouble**)malloc(2*sizeof(gleDouble*));
					xformArray[j][0] = (gleDouble*)malloc(3*sizeof(gleDouble));
					xformArray[j][1] = (gleDouble*)malloc(3*sizeof(gleDouble));
				}
				//no scaling for first an last segment as they are dummies
				xformArray[0][0][0] = 1;
				xformArray[0][0][1] = 0;
				xformArray[0][0][2] = 0;
				xformArray[0][1][0] = 0;
				xformArray[0][1][1] = 1;
				xformArray[0][1][2] = 0;
				for(unsigned int i = 0 ; i < axis.size() ; ++i){
					//scaling is done on x coordinate of the contour
					xformArray[i+1][0][0] = 1+i*((scalingFactor-1)/(axis.size()-1));
					xformArray[i+1][0][1] = 0;
					xformArray[i+1][0][2] = 0;
					xformArray[i+1][1][0] = 0;
					xformArray[i+1][1][1] = 1;
					xformArray[i+1][1][2] = 0;
				}
				xformArray[axis.size()+1][0][0] = 1;
				xformArray[axis.size()+1][0][1] = 0;
				xformArray[axis.size()+1][0][2] = 0;
				xformArray[axis.size()+1][1][0] = 0;
				xformArray[axis.size()+1][1][1] = 1;
				xformArray[axis.size()+1][1][2] = 0;
			}
			
			//first extrude this submesh, identified by its ID (we use i here) rather than its name
			extruder.extrudeGeometry(i, oss.str().c_str(), ncp, contour, norm_contour, up_vector, extrusionPathSize, extrusionPath, xformArray, geometryTexInfos);
	
			//add the last extruded submesh to the list of submeshes needed to create the big mesh
			submeshIDs[i] = i;
			
			//free the memory we used
			if(roadway.getDifferentWidth()){
				for(j = 0 ; j < extrusionPathSize ; ++j){
					free(xformArray[j][0]);
					free(xformArray[j][1]);
					free(xformArray[j]);
				}
				free(xformArray);
			}
			for(j = 0 ; j < extrusionPathSize ; ++j){
				free(extrusionPath[j]);
			}
			free(extrusionPath);
			free(contour[0]);
			free(contour[1]);
			free(contour);
			free(norm_contour[0]);
			free(norm_contour[1]);
			free(norm_contour);
		}
		//a name for the mesh
		ostringstream oss;
		oss << "arRoadNetwork"<< _randomness;
		
		//create the mesh using extruded geometries
		MeshPtr mesh = extruder.createMesh(submeshIDs, nbRoadways, 0, oss.str().c_str());
		
		//free some memory
		delete submeshIDs;
		map<pair<string, int>, texInfo*>::iterator iterDeleter;
		for(iterDeleter = texInformations.begin() ; iterDeleter != texInformations.end() ; ++iterDeleter){
			delete iterDeleter->second;
		}
		
		return mesh;
	}
	
	//--------------------------------------------------------------------
	//INTERSECTIONS
	//--------------------------------------------------------------------
	
	
	// Compute intersections
	MeshPtr ComputeFactory::createIntersections(MeshPtr& roadNetworkMesh,
		RoadNetwork& network, bool computeAllContours, std::vector< OGRLineString* >& contours) {
	
		vector<Intersection> list = network.getIntersectionsList();
		unsigned int nbIntersections = list.size();
		
		//the MeshPtr will hold the geometries of intersections
		ostringstream oss;
		oss << "arIntersections" << _randomness;
		
		MeshPtr intersectionsMesh = MeshManager::getSingleton().createManual(oss.str().c_str(),"RoadGen");
	    
	    //A bounding box for culling
	    AxisAlignedBox aabox;
	    
	    //storage to keep informations on what will be computed to use it for 
	    //contours computing (if needed)
	    vector<pair<vector<interInfo>, vector<roadIntersectionPoints> > > computingInformations;
	    
	    //a material for the intersections
	    MaterialPtr material = MaterialManager::getSingleton().create(_interMaterialName,"RoadGen");
		
		//Start with a fresh one
		material->removeAllTechniques();
		
		//Create a technique
		Technique* tech = material->createTechnique();
		
		//Create a pass
		tech->removeAllPasses();
		Pass* pass = tech->createPass();
	
		//No backface culling for this pass
		pass->setCullingMode(CULL_NONE);
		pass->setManualCullingMode(MANUAL_CULL_NONE);
		
		//Add a texture unit
		string centerTextureName;
		double centerTexLength;
		if(Config::instance()->getIntersectionCenterTexture(centerTextureName, centerTexLength)){
			pass->createTextureUnitState(centerTextureName);
		} else {
			cout << "No texture defined for center, will be let blank." << endl;
		}
		
		//export material
		MaterialSerializer serializer;
		
		//Add the extension to the name
		oss << ".material";
		
		//export this material
		serializer.exportMaterial(material, oss.str().c_str());
			
		for (unsigned int i = 0; i < nbIntersections; i++) {
			//gather informations on roads
			computingInformations.push_back(make_pair(createRoadList(roadNetworkMesh, network,i), vector<roadIntersectionPoints>()));
			
			//building intersection geometry
			buildGeometry(intersectionsMesh, roadNetworkMesh, Config::instance()->getSegmentLimit(),
					computingInformations[i].first, computingInformations[i].second,
					&(list[i]), i, aabox);
		}
		
		intersectionsMesh->_setBounds(aabox);
	
		vector<RoadWay> roadList = network.getRoadWayList();
		for (unsigned int i=0; i<roadList.size(); i++) {
			//recompute the texture coordinates because of modifications on roads to fit the intersection
			computeRoadTexCoords(roadNetworkMesh->getSubMesh(i), roadList[i].getTexLength());
		}
		
		//load this mesh in ogre
		intersectionsMesh->load();
		
		//creation of contours
		if(computeAllContours){
			contours = createContours(computingInformations, nbIntersections);
		}
		
		//simplify mesh
		//we have used one submesh for each road, each intersection center
		//and each road end
		//it will be expensive to display as one submesh = one draw call
		//so regroup submeshes that use the same material
		//beware : it will destroy the old mesh and create a new one
		//meaning some informations will be lost (like pointers to submeshes
		//in interInfo) so be sure to do it when there is no need to compute anymore
		simplifyMesh(intersectionsMesh, Config::instance()->getTexturesNames());
		simplifyMesh(roadNetworkMesh, Config::instance()->getTexturesNames());
		
		return intersectionsMesh;
	}
	
	vector<interInfo> ComputeFactory::createRoadList(MeshPtr& roadNetworkMesh, RoadNetwork& network, unsigned int i) {
		//get the list of index of roads an their positions in the intersection
		vector<pair<unsigned int,bool> > intersectionRoadwayList = network.getIntersectionsList()[i].getRoadWayList();	
		//vector<Intersection> list = network.getIntersectionsList();
		unsigned int rwNum = intersectionRoadwayList.size();
	
		//vector to store the informations on each road
		vector<interInfo> roadList;
		for (unsigned int j=0; j<rwNum; j++) {
			// index in the mesh == the index in the network's roadway list due to extrusion
			unsigned int index = intersectionRoadwayList[j].first;
			//Roadway number j
			RoadWay roadway = network.getRoadWayList()[index];
			//create interInfo for roadway j
			roadList.push_back(interInfo(index, //index in the network
					roadNetworkMesh->getSubMesh(index), //geometry of the road
					intersectionRoadwayList[j].second, //position of this road for the intersection (end or beginning)
					roadway.getTexLength(),
					network.getRoadWayPtr(index)));
		}
		return roadList;
	}
	
	// build intersection geometry
	// finds intersection points between roads to create geometry
	void ComputeFactory::buildGeometry(MeshPtr intersectionsMesh, 
			MeshPtr roadNetworkMesh, 
			int segmentLimit, 
			vector<interInfo>& roadList, 
			vector<roadIntersectionPoints>& intersectionPoints, 
			Intersection* inter, unsigned interIndex,
			AxisAlignedBox& aabox){
		//create some objects for the properties of the model
		vector<Vector3> outInter;
		vector<Vector4> colorInter;
		vector<Vector3> normalInter;
		vector<Vector2> texInter;
		vector<vector<Vector3> > outRoads;
		vector<vector<Vector3> > normalRoads;
		vector<vector<Vector2> > texRoads;
		vector<vector<Vector2> > texMarking;
		//prepare the arrays for each road of the intersection
		for(unsigned int i = 0 ; i < roadList.size() ; ++i){
			outRoads.push_back(vector<Vector3>());
			normalRoads.push_back(vector<Vector3>());
			texRoads.push_back(vector<Vector2>());
			texMarking.push_back(vector<Vector2>());
		}
		
		if(roadList.size() > 1){ //if it's not a dead end
			//compute the geometry of the intersection
			if (!computeIntersectionGeometry(roadList, intersectionPoints, inter->isComplex(), 
					outRoads, normalRoads, texRoads, texMarking,
					outInter, colorInter, normalInter, texInter,
					segmentLimit, inter, interIndex)) {
				cout << "******* Error while creating intersection" << endl;
				return;
			}
			
			//prepare the vector for the triangulation of the intersection
			vector<Vec3d> centerPolyline;
			for(unsigned int i = 0 ; i < outInter.size() ; ++i){
				//Ogre is y-up whereas arTriangulator(lib triangle) uses z-up data
				//so translate
				centerPolyline.push_back(Vec3d(outInter[i][0], -outInter[i][2], outInter[i][1]));
			}
			
			//create triangulator and effectively compute triangulation
			Triangulator centerTriangulator( centerPolyline );
			centerTriangulator.compute();
			
			//compute texture coordinates of the intersection center
			vector<Vector2> texCoords = computeIntersectionCenterTexCoords(roadList, outInter, 3);
			
			//create submesh for intersections center
			createIntersectionCenterSubmesh(intersectionsMesh, 
					centerTriangulator.getVertices(), centerTriangulator.getTriangles(), 
					texCoords, outInter, aabox);
			
			for(unsigned int i = 0 ; i < roadList.size() ; ++i){
				//for each road of the intersection, create a submesh for the part of it
				//belonging to the intersection
				createIntersectionRoadPartSubmesh(intersectionsMesh, roadList[i], outRoads[i], texRoads[i], aabox);
			}
		} else { //dead end
			//fill all informations here
			HardwareVertexBufferSharedPtr bufferPtr = roadList[0].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			roadIntersectionPoints rIP;
			rIP.interIndex = interIndex;
			//lock buffer for reading and get pointer on data
			float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			//we are aware of the vertex declaration used to extrude the model 
			//(see file OgreExtrusion.cpp) so we simply get it directly, without index computing
			//format : 3 floats for position, 3 for normal and 2 for tex coords
			//geometry of the dead end (useful for contours)
			if(roadList[0].position){
				outInter.push_back(Vector3(*(vdata+8*(bufferPtr->getNumVertices()-1)),
											*(vdata+8*(bufferPtr->getNumVertices()-1)+1),
											*(vdata+8*(bufferPtr->getNumVertices()-1)+2)));
				outInter.push_back(Vector3(*(vdata+8*(bufferPtr->getNumVertices()-3)),
											*(vdata+8*(bufferPtr->getNumVertices()-3)+1),
											*(vdata+8*(bufferPtr->getNumVertices()-3)+2)));
				rIP.rightExtremity[0] = *(vdata+8*(bufferPtr->getNumVertices()-1));
				rIP.rightExtremity[1] = *(vdata+8*(bufferPtr->getNumVertices()-1)+1);
				rIP.rightExtremity[2] = *(vdata+8*(bufferPtr->getNumVertices()-1)+2);
				rIP.leftExtremity[0] = *(vdata+8*(bufferPtr->getNumVertices()-3));
				rIP.leftExtremity[1] = *(vdata+8*(bufferPtr->getNumVertices()-3)+1);
				rIP.leftExtremity[2] = *(vdata+8*(bufferPtr->getNumVertices()-3)+2);
				//update bounding box
				aabox.merge(Vector3(*(vdata+8*(bufferPtr->getNumVertices()-1)),
						*(vdata+8*(bufferPtr->getNumVertices()-1)+1),
						*(vdata+8*(bufferPtr->getNumVertices()-1)+2)));
				aabox.merge(Vector3(*(vdata+8*(bufferPtr->getNumVertices()-3)),
						*(vdata+8*(bufferPtr->getNumVertices()-3)+1),
						*(vdata+8*(bufferPtr->getNumVertices()-3)+2)));
			} else {
				outInter.push_back(Vector3(*(vdata),
											*(vdata+1),
											*(vdata+2)));
				outInter.push_back(Vector3(*(vdata+8*2),
											*(vdata+8*2+1),
											*(vdata+8*2+2)));
				rIP.rightExtremity[0] = *(vdata);
				rIP.rightExtremity[1] = *(vdata+1);
				rIP.rightExtremity[2] = *(vdata+2);
				rIP.leftExtremity[0] = *(vdata+8*2);
				rIP.leftExtremity[1] = *(vdata+8*2+1);
				rIP.leftExtremity[2] = *(vdata+8*2+2);
				aabox.merge(Vector3(*(vdata),
						*(vdata+1),
						*(vdata+2)));
				aabox.merge(Vector3(*(vdata+8*2),
						*(vdata+8*2+1),
						*(vdata+8*2+2)));
			}
			//release buffer
			bufferPtr->unlock();
			intersectionPoints.push_back(rIP);
			colorInter.push_back(Vector4(1, 1, 1, 1));
			normalInter.push_back(Vector3(0, 1, 0));
			texInter.push_back(Vector2(0, 0));
			texInter.push_back(Vector2(0, 1));
		}		
		return;
	}
	
	
	bool ComputeFactory::computeIntersectionGeometry(vector<interInfo>& roadList,
			vector<roadIntersectionPoints>& intersectionPoints, 
			bool isComplex,
			vector<vector<Vector3> >& outRoads,
			vector<vector<Vector3> >& normalRoads,
			vector<vector<Vector2> >& texRoads,
			vector<vector<Vector2> >& texMarking,
			vector<Vector3>& outInter,
			vector<Vector4>& colorInter,
			vector<Vector3>& normalInter,
			vector<Vector2>& texInter,
			const int& segmentLimit, Intersection* intersection, unsigned int intersectionIndex) {
		//how many road do we need to check in this intersection
		unsigned int nb = roadList.size();
		
		//indices of the vertices are described below, knowing the road is a triangle strip
		//moreover except for the end and beginning, vertices have two indices
		//
		// 2--------0
		// |        |
		// |        |
		// |        |
		// 3/6     1/4
		// |        |
		// |        |
		// |        |
		// 7/10    5/8
		// .....
		
		for(unsigned int i = 0 ; i < nb ; ++i){
			//build the structure to hold information on intersection points,
			//as we are going to fill them circularly
			roadIntersectionPoints rIP;
			rIP.interIndex = intersectionIndex;
			intersectionPoints.push_back(rIP);
		}
		
		//compute intersections between roads
		computeIntersectionBetweenRoads(roadList, segmentLimit, nb, intersectionPoints, isComplex, intersection);
	
		
		//try to get additionnal points when not complex by stretching roads
		//segment correction won't be applied to complex roads as there are often
		//large gaps because of the deleted road and they have a special method
		//to detect additional points
		if(!isComplex){
			getStretchedSegmentsIntersection(roadList, segmentLimit, nb, intersectionPoints);
		}
	
		//project intersection points on each road
		projectPoints(roadList, nb, intersectionPoints);
		
		//stretch roads intersections part a little to have room to texturate
		stretchRoadsIntersectionParts(roadList, nb, intersectionPoints);
	
		//merge points of the intersection in the out array
		mergePoints(outRoads, normalRoads, texRoads, 
					outInter, colorInter, normalInter, texInter, 
					nb, roadList, intersectionPoints, intersection);
	
		//clean the primitives of the road that are inside of the intersection
		for (unsigned int i=0; i<nb; i++) {
			int primRank = (intersectionPoints[i].leftMarkingPrimRank > intersectionPoints[i].rightMarkingPrimRank)?
					intersectionPoints[i].leftMarkingPrimRank:intersectionPoints[i].rightMarkingPrimRank;
			removePrimitives(roadList[i].roadGeom, roadList[i].position, primRank);
		}
		
		//flatten intersection
		levelIntersection(outRoads, outInter, intersectionPoints);
		
		//move the extremities of the roads and set them to the good height
		updateIntersection(roadList, intersectionPoints);
		
		return true;
	}
	
	bool ComputeFactory::computeIntersectionBetweenRoads(vector<interInfo>& roadList,
			int segmentLimit,
			unsigned int nb,
			vector<roadIntersectionPoints>& intersectionPoints,
			bool isComplex, 
			Intersection* intersection){
		//boolean to know if we have detected an intersection so far
		bool interFound = false;
		
		//pointers on vertices of the roads we will use to compute points
		HardwareVertexBufferSharedPtr bufferCurrentPtr, bufferNextPtr;
		float* vdataCurrent;
		float* vdataNext;
		
		//number of vertices of each array
		unsigned int nbVerticesCurrent, nbVerticesNext;
		
		//to know where is the intersection for each road(end/true or beginning/false) 
		bool positionCurrent, positionNext;
		
		//points for the arLines
		Vec3<double> rSCExterior, rSCInterior, 
					   lSNExterior, lSNInterior,
					   lSCExterior, lSCInterior,
					   rSNExterior, rSNInterior;
		
		//pointers to know which struture to fill
		roadIntersectionPoints* rIPCurrent;
		roadIntersectionPoints* rIPNext;
		
		//compute geometric intersection between each couple of road
		for(unsigned int i = 0 ; i < nb ; ++i){
			//set the variable we will need to compute
			interFound = false;
	
			//get informations on current and next road : geometry, position in intersection,
			//number of vertices and where to send computed data
			bufferCurrentPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			vdataCurrent = static_cast<float*>(bufferCurrentPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			bufferNextPtr = roadList[(i+1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			vdataNext = static_cast<float*>(bufferNextPtr->lock(HardwareBuffer::HBL_READ_ONLY));
	
			positionCurrent = roadList[i].position;
			positionNext = roadList[(i+1)%nb].position;
	
			nbVerticesCurrent = bufferCurrentPtr->getNumVertices();
			nbVerticesNext = bufferNextPtr->getNumVertices();
			
			rIPCurrent = &(intersectionPoints[i]);
			rIPNext = &(intersectionPoints[(i+1)%nb]);
			//now compute the intersection between the right side of the current road and the left side of the next road
			for( int j = 0 ; !interFound && j < nbVerticesCurrent && j < 4*segmentLimit ; j += 4){
				//go through the primitives of the current road
				//get arLine for the right side of the current road
				getSide(positionCurrent, true, vdataCurrent, nbVerticesCurrent, j, rSCExterior, rSCInterior);
				
				for( unsigned int k = 0 ; !interFound && k < nbVerticesNext && k < 4*segmentLimit ; k +=4){
					//go through the primitives of the next road
					//get arLine for the left side of the next road
					getSide(positionNext, false, vdataNext, nbVerticesNext, k, lSNExterior, lSNInterior);
					
					//compute intersection between arLines (if any)
					Vec3<double> dummy, intersectCurrent;
					interFound = getIntersectionFrom2Segments(rSCExterior, rSCInterior, lSNExterior, lSNInterior, intersectCurrent, dummy, 0.2);
					
					//if intersection has been found
					if(interFound){
						//fill the informations
						rIPNext->leftIntersection = INTERSECT;
						rIPNext->leftExtremity[0] = intersectCurrent[0];
						rIPNext->leftExtremity[1] = intersectCurrent[1];
						rIPNext->leftExtremity[2] = intersectCurrent[2];
						rIPNext->leftPrimRank = (int)floor((double)k/4);
						
						//if current road has been designated as entering a highway
						//by the right, try to know where is the intersection considering the highway
						if(rIPCurrent->rightIntersection == HIGHWAY){
							bool previousNextIntersect = false;
							if((int)floor((double)j/4) > rIPCurrent->rightPrimRank){
								//intersection is further than the entrance by the right in the highway, update
								rIPCurrent->rightIntersection = INTERSECT;
								rIPCurrent->rightExtremity[0] = intersectCurrent[0];
								rIPCurrent->rightExtremity[1] = intersectCurrent[1];
								rIPCurrent->rightExtremity[2] = intersectCurrent[2];
								rIPCurrent->rightPrimRank = (int)floor((double)j/4);		
							} else if((int)floor((double)j/4) == rIPCurrent->rightPrimRank){
								//we don't know if we are in the previous case or in the one that will be treated next,
								//let's do a little more computation
								Vec3<double> VecHighwayIntersect(rIPCurrent->highwayPoint[0],
																	 rIPCurrent->highwayPoint[1],
																	 rIPCurrent->highwayPoint[2]);
								if((rSCExterior - VecHighwayIntersect).length() < (rSCExterior-intersectCurrent).length()){
									//intersection is further than the highway intersection, update
									rIPCurrent->rightIntersection = INTERSECT;
									rIPCurrent->rightExtremity[0] = intersectCurrent[0];
									rIPCurrent->rightExtremity[1] = intersectCurrent[1];
									rIPCurrent->rightExtremity[2] = intersectCurrent[2];
									rIPCurrent->rightPrimRank = (int)floor((double)j/4);						
								} else {
									//intersection may be between next road and previous road (the highway), compute
									previousNextIntersect = true;
								}	
							} else{
								//intersection may be between next road and previous road (the highway), compute
								previousNextIntersect = true;					
							}
							
							if(previousNextIntersect){
								//store the geometry of the highway
								HardwareVertexBufferSharedPtr bufferPreviousPtr = roadList[(i+nb-1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
								float* vdataPrevious = static_cast<float*>(bufferPreviousPtr->lock(HardwareBuffer::HBL_READ_ONLY));
								bool positionPrevious = roadList[(i-1+nb)%nb].position;
								unsigned int nbVerticesPrevious = bufferPreviousPtr->getNumVertices();
								
								//to store the right side of the highway
								Vec3<double> rSPExterior, rSPInterior;
								
								//get the left side of the next road
								getSide(positionNext, false, vdataNext, nbVerticesNext, 4*(rIPNext->leftPrimRank), lSNExterior, lSNInterior);
								//go through the primitives of the previous road (highway) until where we found the right entrance of the current road in it
								for(int n = 0 ; n <= rIPCurrent->highwayPrimRank ; n+=4){
									getSide(positionPrevious, true, vdataPrevious, nbVerticesPrevious, n, rSPExterior, rSPInterior);
									Vec3<double> PNHighwayIntersect;
									if(getIntersectionFrom2Segments(lSNExterior, lSNInterior, rSPExterior, rSPInterior, dummy, PNHighwayIntersect, 0.2)){
										//update taking in account we must finish the border of the highway
										//to the intersection between highway(previous) and next road
										
										//the right extremity will be set as the intersection
										rIPCurrent->rightExtremity[0] = PNHighwayIntersect[0];
										rIPCurrent->rightExtremity[1] = PNHighwayIntersect[1];
										rIPCurrent->rightExtremity[2] = PNHighwayIntersect[2];
										//Store primitive where we found the intersection to know where to stop when adding points
										rIPCurrent->highwayEndPrimRank = (int)floor((double)n/4);													
									}
								}
								
								//release buffer
								bufferPreviousPtr->unlock();
							}					
						} else {
							//simply update
							rIPCurrent->rightIntersection = INTERSECT;
							rIPCurrent->rightExtremity[0] = intersectCurrent[0];
							rIPCurrent->rightExtremity[1] = intersectCurrent[1];
							rIPCurrent->rightExtremity[2] = intersectCurrent[2];						
							rIPCurrent->rightPrimRank = (int)floor((double)j/4);						
						}
	
						//symptom of the highway
						//some roads are slip roads : they totally enter in a bigger road
						//if we don't take this in account, the resulting geometry will contain errors
						Vec3<double> highwayIntersect;
						
						//compute an approximation of the sides of the roads between 
						//their extremity and the end of the primitive where we found
						//the intersection
						if(positionCurrent){
							lSCExterior = Vec3<double>(*(vdataCurrent+8*(nbVerticesCurrent-3)),
									*(vdataCurrent+8*(nbVerticesCurrent-3)+1),
									*(vdataCurrent+8*(nbVerticesCurrent-3)+2));
							lSCInterior = Vec3<double>(*(vdataCurrent+8*(nbVerticesCurrent-4-j)),
									*(vdataCurrent+8*(nbVerticesCurrent-4-j)+1),
									*(vdataCurrent+8*(nbVerticesCurrent-4-j)+2));
							rSCExterior = Vec3<double>(*(vdataCurrent+8*(nbVerticesCurrent-1)),
									*(vdataCurrent+8*(nbVerticesCurrent-1)+1),
									*(vdataCurrent+8*(nbVerticesCurrent-1)+2));
							rSCInterior = Vec3<double>(*(vdataCurrent+8*(nbVerticesCurrent-2-j)),
									*(vdataCurrent+8*(nbVerticesCurrent-2-j)+1),
									*(vdataCurrent+8*(nbVerticesCurrent-2-j)+2));
						} else {
							lSCExterior = Vec3<double>(*(vdataCurrent+8*2),
									*(vdataCurrent+8*2+1),
									*(vdataCurrent+8*2+2));
							lSCInterior = Vec3<double>(*(vdataCurrent+8*(3+j)),
									*(vdataCurrent+8*(3+j)+1),
									*(vdataCurrent+8*(3+j)+2));
							rSCExterior = Vec3<double>(*(vdataCurrent),
									*(vdataCurrent+1),
									*(vdataCurrent+2));
							rSCInterior = Vec3<double>(*(vdataCurrent+8*(1+j)),
									*(vdataCurrent+8*(1+j)+1),
									*(vdataCurrent+8*(1+j)+2));
						}
						
						if(positionNext){
							lSNExterior = Vec3<double>(*(vdataNext+8*(nbVerticesNext-3)),
									*(vdataNext+8*(nbVerticesNext-3)+1),
									*(vdataNext+8*(nbVerticesNext-3)+2));
							lSNInterior = Vec3<double>(*(vdataNext+8*(nbVerticesNext-4-k)),
									*(vdataNext+8*(nbVerticesNext-4-k)+1),
									*(vdataNext+8*(nbVerticesNext-4-k)+2));
							rSNExterior = Vec3<double>(*(vdataNext+8*(nbVerticesNext-1)),
									*(vdataNext+8*(nbVerticesNext-1)+1),
									*(vdataNext+8*(nbVerticesNext-1)+2));
							rSNInterior = Vec3<double>(*(vdataNext+8*(nbVerticesNext-2-k)),
									*(vdataNext+8*(nbVerticesNext-2-k)+1),
									*(vdataNext+8*(nbVerticesNext-2-k)+2));
						} else {
							lSNExterior = Vec3<double>(*(vdataNext+8*2),
									*(vdataNext+8*2+1),
									*(vdataNext+8*2+2));
							lSNInterior = Vec3<double>(*(vdataNext+8*(3+k)),
									*(vdataNext+8*(3+k)+1),
									*(vdataNext+8*(3+k)+2));
							rSNExterior = Vec3<double>(*(vdataNext),
									*(vdataNext+1),
									*(vdataNext+2));
							rSNInterior = Vec3<double>(*(vdataNext+8*(1+k)),
									*(vdataNext+8*(1+k)+1),
									*(vdataNext+8*(1+k)+2));
						}
						
						//if the approximated left sides intersect...
						if(getIntersectionFrom2Segments(lSCExterior, lSCInterior, lSNExterior, lSNInterior, dummy, highwayIntersect, 0.2)){
							//let's go for a deeper search 
							bool interSameSideFound = false;
							Line<double> leftSideCurrent;
							for(int l = 0 ; !interSameSideFound && l <= j ; l+=4){
								//get the arline for the left side of the current road of (l/4)-th primitive
								getSide(positionCurrent, false, vdataCurrent, nbVerticesCurrent, l, lSCExterior, lSCInterior);
								for(int m = 0 ; !interSameSideFound && m <= k ; m+=4){
									//get the arline for the left side of the next road of (m/4)-th primitive
									getSide(positionNext, false, vdataNext, nbVerticesNext, m, lSNExterior, lSNInterior);
									interSameSideFound = getIntersectionFrom2Segments(lSCExterior, lSCInterior, lSNExterior, lSNInterior, dummy, highwayIntersect, 0.2);
									//if there is an intersection between left sides
									if(interSameSideFound){
										//if left extremity of current road has already been computed has an intersection
										if(rIPCurrent->leftIntersection == INTERSECT){
											bool previousNextIntersect = false;
											if(rIPCurrent->leftPrimRank > (int)floor((double)m/4)){
												//the intersection between current road and previous road is further than
												//the left entering of the current road into next road, so keep the informations
												//on the classic intersection
											} else if(rIPCurrent->leftPrimRank == (int)floor((double)m/4)){
												//we don't know if we are in the previous case or in the one that will be treated next,
												//let's do a little more computation
												Vec3<double> leftIntersect(rIPCurrent->leftExtremity[0],
														rIPCurrent->leftExtremity[1],
														rIPCurrent->leftExtremity[2]);
												if((lSCExterior - highwayIntersect).length() < (lSCExterior-leftIntersect).length()){
													//intersection is further than the highway intersection, do nothing
												} else {
													//the intersection may be between the previous and the next road (highway) : try to find where
													previousNextIntersect = true;											
												}											
											} else {
												//the intersection may be between the previous and the next road (highway) : try to find where
												previousNextIntersect = true;
											}
											
											if(previousNextIntersect){
												//find where the previous and next road (highway) intersects
												
												//geometry of the previous road
												HardwareVertexBufferSharedPtr bufferPreviousPtr = roadList[(i+nb-1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
												float* vdataPrevious = static_cast<float*>(bufferPreviousPtr->lock(HardwareBuffer::HBL_NORMAL));
												bool positionPrevious = roadList[(i-1+nb)%nb].position;
												unsigned int nbVerticesPrevious = bufferPreviousPtr->getNumVertices();
												
												//to store the right side of the previous road
												Vec3<double> rSPExterior, rSPInterior;
												
												//to get some informations
												roadIntersectionPoints* rIPPrevious =  &(intersectionPoints[(i-1+nb)%nb]);
												
												//get the right side
												getSide(positionPrevious, true, vdataPrevious, nbVerticesPrevious, 4*(rIPPrevious->rightPrimRank), rSPExterior, rSPInterior);
	
												//if the intersection is between left sides, it must be 
												//the current road that is entering the highway
												rIPCurrent->leftIntersection = HIGHWAY;
												rIPCurrent->highwayPoint[0] = highwayIntersect[0];
												rIPCurrent->highwayPoint[1] = highwayIntersect[1];
												rIPCurrent->highwayPoint[2] = highwayIntersect[2];
												//we will store in leftPrimRank the rank of the primitive on the narrowest road
												//to add its border points 
												rIPCurrent->leftPrimRank = (int)floor((double)l/4);
												//store primitive of the wider road to add its border points too
												rIPCurrent->highwayPrimRank = (int)floor((double)m/4);
												
												//go through the primitives of the next road until where we found the left entrance of the current road in it
												bool intersectNPFound = false;
												for(int n = 0 ; !intersectNPFound && n <= m ; n+=4){
													getSide(positionNext, false, vdataNext, nbVerticesNext, n, lSNExterior, lSNInterior);
													Vec3<double> PNHighwayIntersect;
													if(getIntersectionFrom2Segments(lSNExterior, lSNInterior, rSPExterior, rSPInterior, PNHighwayIntersect, dummy, 0.2)){
														intersectNPFound = true;
														//update taking in account we must finish the border of the highway
														//to the intersection between highway(next) and previous road
														//the left extremity will be set as the intersection
														rIPCurrent->leftExtremity[0] = PNHighwayIntersect[0];
														rIPCurrent->leftExtremity[1] = PNHighwayIntersect[1];
														rIPCurrent->leftExtremity[2] = PNHighwayIntersect[2];
														//Store primitive where we found the intersection to know where to stop when adding points
														rIPCurrent->highwayEndPrimRank = (int)floor((double)n/4);													
													}
												}
												if(!intersectNPFound){
													//no intersection between next and previous road, register highway normally
													//the left extremity will be set as the left endpoint of the next road
													if(positionNext){
														rIPCurrent->leftExtremity[0] = *(vdataNext+8*(nbVerticesNext-3));
														rIPCurrent->leftExtremity[1] = *(vdataNext+8*(nbVerticesNext-3)+1);
														rIPCurrent->leftExtremity[2] = *(vdataNext+8*(nbVerticesNext-3)+2);
													} else {
														rIPCurrent->leftExtremity[0] = *(vdataNext+8*2);
														rIPCurrent->leftExtremity[1] = *(vdataNext+8*2+1);
														rIPCurrent->leftExtremity[2] = *(vdataNext+8*2+2);
													}
												}
												
												//release buffer
												bufferPreviousPtr->unlock();
											}
										} else {
											//simply update
											//if the intersection is between left sides, it must be 
											//the current road that is entering the highway
											rIPCurrent->leftIntersection = HIGHWAY;
											//the left extremity will be set as the left endpoint of the next road
											if(positionNext){
												rIPCurrent->leftExtremity[0] = *(vdataNext+8*(nbVerticesNext-3));
												rIPCurrent->leftExtremity[1] = *(vdataNext+8*(nbVerticesNext-3)+1);
												rIPCurrent->leftExtremity[2] = *(vdataNext+8*(nbVerticesNext-3)+2);
											} else {
												rIPCurrent->leftExtremity[0] = *(vdataNext+8*2);
												rIPCurrent->leftExtremity[1] = *(vdataNext+8*2+1);
												rIPCurrent->leftExtremity[2] = *(vdataNext+8*2+2);
											}
											rIPCurrent->highwayPoint[0] = highwayIntersect[0];
											rIPCurrent->highwayPoint[1] = highwayIntersect[1];
											rIPCurrent->highwayPoint[2] = highwayIntersect[2];
											//we will store in leftPrimRank the rank of the primitive on the narrowest road
											//to add its border points 
											rIPCurrent->leftPrimRank = (int)floor((double)l/4);
											//store primitive of the wider road to add its border points too
											rIPCurrent->highwayPrimRank = (int)floor((double)m/4);
										}
									}
								}
							}
						//else if the right sides intersect...
		 				} else if(getIntersectionFrom2Segments(rSCExterior, rSCInterior, rSNExterior, rSNInterior, dummy, highwayIntersect, 0.2)){
		 					//let's go for a deeper search 
		 					bool interSameSideFound = false;
							for(int l = 0 ; !interSameSideFound && l <= j ; l+=4){
								//get the arline for the right side of the current road of (l/4)-th primitive
								getSide(positionCurrent, true, vdataCurrent, nbVerticesCurrent, l, rSCExterior, rSCInterior);
								for(int m = 0 ; !interSameSideFound && m <= k ; m+=4){
									//get the arline for the right side of the next road of (m/4)-th primitive
									getSide(positionNext, true, vdataNext, nbVerticesNext, m, rSNExterior, rSNInterior);
									interSameSideFound = getIntersectionFrom2Segments(rSCExterior, rSCInterior, rSNExterior, rSNInterior, dummy, highwayIntersect, 0.2);
									//if there is an intersection between right sides
									if(interSameSideFound){ 
										//if the intersection is between right sides, it must be 
										//the next road that is entering the highway
										rIPNext->rightIntersection = HIGHWAY;
										//the right extremity will be set as the right endpoint of the current road
										if(positionCurrent){
											rIPNext->rightExtremity[0] = *(vdataCurrent+8*(nbVerticesCurrent-1));
											rIPNext->rightExtremity[1] = *(vdataCurrent+8*(nbVerticesCurrent-1)+1);
											rIPNext->rightExtremity[2] = *(vdataCurrent+8*(nbVerticesCurrent-1)+2);
										} else {
											rIPNext->rightExtremity[0] = *(vdataCurrent);
											rIPNext->rightExtremity[1] = *(vdataCurrent+1);
											rIPNext->rightExtremity[2] = *(vdataCurrent+2);										
										}
										rIPNext->highwayPoint[0] = highwayIntersect[0];
										rIPNext->highwayPoint[1] = highwayIntersect[1];
										rIPNext->highwayPoint[2] = highwayIntersect[2];
										//we will store in rightPrimRank the rank of the primitive on the narrowest road
										//to add its border points 
										rIPNext->rightPrimRank = (int)floor((double)m/4);
										//store primitive of the wider road to add its border points too
										rIPNext->highwayPrimRank = (int)floor((double)l/4);
									}
								}
							}
		 				}
					}
				}
			}
			
			if(!interFound){ //if no intersection has been found between current and next road
				if(!isComplex){ //if it's a simple intersection
					//just add the end point of the right side of the current road as the extremity
					//if it has not already been computed in the case of a highway
					if(rIPCurrent->rightIntersection != HIGHWAY){
						if(positionCurrent){ //intersection at the end of the road
							rIPCurrent->rightExtremity[0] = *(vdataCurrent+8*(nbVerticesCurrent-1));
							rIPCurrent->rightExtremity[1] = *(vdataCurrent+8*(nbVerticesCurrent-1)+1);
							rIPCurrent->rightExtremity[2] = *(vdataCurrent+8*(nbVerticesCurrent-1)+2);
							//let rightIntersection at ENDPOINT and rightPrimRank at -1
						} else { //intersection at the beginning of the road
							rIPCurrent->rightExtremity[0] = *(vdataCurrent);
							rIPCurrent->rightExtremity[1] = *(vdataCurrent+1);
							rIPCurrent->rightExtremity[2] = *(vdataCurrent+2);
							//let rightIntersection at ENDPOINT and rightPrimRank at -1					
						}
					}
					
					//same for the next road but for the left side
					if(rIPNext->leftIntersection != HIGHWAY){
						if(positionNext){
							rIPNext->leftExtremity[0] = *(vdataNext+8*(nbVerticesNext-3));
							rIPNext->leftExtremity[1] = *(vdataNext+8*(nbVerticesNext-3)+1);
							rIPNext->leftExtremity[2] = *(vdataNext+8*(nbVerticesNext-3)+2);
						} else {
							rIPNext->leftExtremity[0] = *(vdataNext+8*2);
							rIPNext->leftExtremity[1] = *(vdataNext+8*2+1);
							rIPNext->leftExtremity[2] = *(vdataNext+8*2+2);
						}
					}
				} else { //if it's a complex intersection, from the merging of two or more intersections
					//try to get more points (typically intersection between the roads of the intersection
					//and the road that has been deleted during the merge of the intersections)
					getComplexIntersectionAdditionalPoints(positionCurrent, positionNext, 
							vdataCurrent, nbVerticesCurrent, vdataNext, nbVerticesNext,
							rIPCurrent, rIPNext, intersection);
				}
			}
			
			//release buffers
			bufferCurrentPtr->unlock();
			bufferNextPtr->unlock();
		}
		
		return true;
	}
	
	
	bool ComputeFactory::getComplexIntersectionAdditionalPoints(bool currentPosition, bool nextPosition,
			float* vdataCurrent, unsigned int nbVerticesCurrent,
			float* vdataNext, unsigned int nbVerticesNext,
			roadIntersectionPoints* rIPCurrent, roadIntersectionPoints* rIPNext,
			Intersection* intersection){
		//get the bounding rectangle of the deleted road
		Vec3<double> ptCenter0 = intersection->getDeletedRoadPoint(2);
		Vec3<double> ptCenter1 = intersection->getDeletedRoadPoint(1);
		Vec3<double> ptCenter2 = intersection->getDeletedRoadPoint(3);
		Vec3<double> ptCenter3 = intersection->getDeletedRoadPoint(0);
		
		//if intersection is the result of the merging of more than two intersections, 
		//try to get the deleted road from informations of existing roads
		if(intersection->isMultiMerged()){
			Vector3 currentRightEnd, currentLeftEnd, nextRightEnd, nextLeftEnd;
			//get the end center of the current road 
			if(currentPosition){
				currentRightEnd[0] = *(vdataCurrent+8*(nbVerticesCurrent-1));
				currentRightEnd[1] = *(vdataCurrent+8*(nbVerticesCurrent-1)+1);
				currentRightEnd[2] = *(vdataCurrent+8*(nbVerticesCurrent-1)+2);
				currentLeftEnd[0] = *(vdataCurrent+8*(nbVerticesCurrent-3));
				currentLeftEnd[1] = *(vdataCurrent+8*(nbVerticesCurrent-3)+1);
				currentLeftEnd[2] = *(vdataCurrent+8*(nbVerticesCurrent-3)+2);
			} else {
				currentRightEnd[0] = *(vdataCurrent);
				currentRightEnd[1] = *(vdataCurrent+1);
				currentRightEnd[2] = *(vdataCurrent+2);
				currentLeftEnd[0] = *(vdataCurrent+8*2);
				currentLeftEnd[1] = *(vdataCurrent+8*2+1);
				currentLeftEnd[2] = *(vdataCurrent+8*2+2);
			}
			Vec3<double> arCenter0(((currentRightEnd+currentLeftEnd)/2)[0],
					((currentRightEnd+currentLeftEnd)/2)[1],
					((currentRightEnd+currentLeftEnd)/2)[2]);
			//get the end center of the next road
			if(nextPosition){
				nextRightEnd[0] = *(vdataNext+8*(nbVerticesNext-1));
				nextRightEnd[1] = *(vdataNext+8*(nbVerticesNext-1)+1);
				nextRightEnd[2] = *(vdataNext+8*(nbVerticesNext-1)+2);
				nextLeftEnd[0] = *(vdataNext+8*(nbVerticesNext-3));
				nextLeftEnd[1] = *(vdataNext+8*(nbVerticesNext-3)+1);
				nextLeftEnd[2] = *(vdataNext+8*(nbVerticesNext-3)+2);
			} else {
				nextRightEnd[0] = *(vdataNext);
				nextRightEnd[1] = *(vdataNext+1);
				nextRightEnd[2] = *(vdataNext+2);
				nextLeftEnd[0] = *(vdataNext+8*2);
				nextLeftEnd[1] = *(vdataNext+8*2+1);
				nextLeftEnd[2] = *(vdataNext+8*2+2);
			}
			Vec3<double> arCenter1(((nextRightEnd+nextLeftEnd)/2)[0],
					((nextRightEnd+nextLeftEnd)/2)[1],
					((nextRightEnd+nextLeftEnd)/2)[2]);
			//get the end center of the next road
			
			//get the normal to the axis between the two roads
		    Vec3<double> translation(arCenter1[2]-arCenter0[2],0,arCenter0[0]-arCenter1[0]);
		    translation.normalize();
		    
		    //compute the width of the biggest road
		    double roadWidth = (currentRightEnd-currentLeftEnd).length();
			if(roadWidth < (nextRightEnd-nextLeftEnd).length()){
				  roadWidth = (nextRightEnd-nextLeftEnd).length();
			}
			
			//finally recompute the points
		    ptCenter0 = arCenter0+translation*roadWidth;
		    ptCenter1 = arCenter1+translation*roadWidth;
	
		    ptCenter2 = arCenter0-translation*roadWidth;
		    ptCenter3 = arCenter1-translation*roadWidth;
		}
		
		//intersection with the deletedRoad
		Vec3<double> newPointRoad;
		
		//boolean to know if we have found an intersection between roads from the intersection
		//and the deleted road
		bool vertexAdded = false;	
		
		//no code factorisation, as the loops are not processed in the same way 
		//(starting from the beginning or end of the array depending of where the intersection
		//is for the road, to avoid going through a huge array) 
		
		//do the computation only if the road hasn't already been flagged as entering a highway
		if(rIPCurrent->rightIntersection != HIGHWAY){
			//process with the left road
			if(currentPosition){ //intersection at the end of the road
				for(int i = nbVerticesCurrent-4 ; !vertexAdded && i >= 0 ; i -= 4){
					//get the right side of the road
					Vec3<double> currentRoadInt(*(vdataCurrent+8*(i+2)),*(vdataCurrent+8*(i+2)+1),*(vdataCurrent+8*(i+2)+2));
					Vec3<double> currentRoadExt(*(vdataCurrent+8*(i+3)),*(vdataCurrent+8*(i+3)+1),*(vdataCurrent+8*(i+3)+2));
					
					//dummy point, we only want the point on the real road
					Vec3<double> dummy;
					
					//if there is an intersection between the right side of the road and the deleted road...
					if(getIntersectionFrom2Segments(ptCenter0, ptCenter1, currentRoadInt, currentRoadExt, dummy, newPointRoad, 0.2) ||
							getIntersectionFrom2Segments(ptCenter2, ptCenter3, currentRoadInt, currentRoadExt, dummy, newPointRoad, 0.2)){
						//update road points informations
						rIPCurrent->rightIntersection = COMPLEX_INTERSECT;
						rIPCurrent->rightExtremity[0] = newPointRoad[0];
						rIPCurrent->rightExtremity[1] = newPointRoad[1];
						rIPCurrent->rightExtremity[2] = newPointRoad[2];
						rIPCurrent->rightPrimRank = (int)floor((double)(nbVerticesCurrent-(i+4))/4);
						vertexAdded = true;
					}
				}
				
				if(!vertexAdded){ //if no intersection found
					//add the end point of the road
					rIPCurrent->rightExtremity[0] = *(vdataCurrent+8*(nbVerticesCurrent-1));
					rIPCurrent->rightExtremity[1] = *(vdataCurrent+8*(nbVerticesCurrent-1)+1);
					rIPCurrent->rightExtremity[2] = *(vdataCurrent+8*(nbVerticesCurrent-1)+2);
				}
			} else { //intersection at the beginning of the road
				for(unsigned int i = 0 ; !vertexAdded && i < nbVerticesCurrent - 1 ; i += 4){
					//get the right side of the road
					Vec3<double> currentRoadInt(*(vdataCurrent+8*i),    *(vdataCurrent+8*i+1),    *(vdataCurrent+8*i+2));
					Vec3<double> currentRoadExt(*(vdataCurrent+8*(i+1)),*(vdataCurrent+8*(i+1)+1),*(vdataCurrent+8*(i+1)+2));
					
					//dummy point, we only want the point on the real road
					Vec3<double> dummy;
					
					//if there is an intersection between the right side of the road and the deleted road...
					if(getIntersectionFrom2Segments(ptCenter0, ptCenter1, currentRoadInt, currentRoadExt, dummy, newPointRoad, 0.2) ||
							getIntersectionFrom2Segments(ptCenter2, ptCenter3, currentRoadInt, currentRoadExt, dummy, newPointRoad, 0.2)){
						//update road points informations
						rIPCurrent->rightIntersection = COMPLEX_INTERSECT;
						rIPCurrent->rightExtremity[0] = newPointRoad[0];
						rIPCurrent->rightExtremity[1] = newPointRoad[1];
						rIPCurrent->rightExtremity[2] = newPointRoad[2];
						rIPCurrent->rightPrimRank = (int)floor((double)i/4);
						vertexAdded = true;
					}
				}
				
				if(!vertexAdded){ //if no intersection found
					//add the end point of the road
					rIPCurrent->rightExtremity[0] = *(vdataCurrent);
					rIPCurrent->rightExtremity[1] = *(vdataCurrent+1);
					rIPCurrent->rightExtremity[2] = *(vdataCurrent+2);
				}
			}
			
			vertexAdded = false;
		}
	
		//do the computation only if the road hasn't already been flagged as entering a highway
		if(rIPNext->leftIntersection != HIGHWAY){
			//same thing but between the left side of the next road and the deleted road
			if(nextPosition){ //intersection at the end of the road
				for(int i = nbVerticesNext-4 ; !vertexAdded && i >= 0 ; i -= 4){
					//get the left side of the road
					Vec3<double> nextRoadInt(*(vdataNext+8*i),     *(vdataNext+8*i+1),     *(vdataNext+8*i+2));
					Vec3<double> nextRoadExt(*(vdataNext+8*(i+1)), *(vdataNext+8*(i+1)+1), *(vdataNext+8*(i+1)+2));
					
					//dummy point, we only want the point on the real road
					Vec3<double> dummy;
					
					//if there is an intersection between the left side of the road and the deleted road...
					if(getIntersectionFrom2Segments(ptCenter0, ptCenter1, nextRoadInt, nextRoadExt, dummy, newPointRoad, 0.2) ||
							getIntersectionFrom2Segments(ptCenter2, ptCenter3, nextRoadInt, nextRoadExt, dummy, newPointRoad, 0.2)){
						//update road points informations
						rIPNext->leftIntersection = COMPLEX_INTERSECT;
						rIPNext->leftExtremity[0] = newPointRoad[0];
						rIPNext->leftExtremity[1] = newPointRoad[1];
						rIPNext->leftExtremity[2] = newPointRoad[2];
						rIPNext->leftPrimRank = (int)floor((double)(nbVerticesNext-(i+4))/4);
						vertexAdded = true;
					}
				}
				
				if(!vertexAdded){ //if no intersection found
					//add the end point of the road
					rIPNext->leftExtremity[0] = *(vdataNext+8*(nbVerticesNext-3));
					rIPNext->leftExtremity[1] = *(vdataNext+8*(nbVerticesNext-3)+1);
					rIPNext->leftExtremity[2] = *(vdataNext+8*(nbVerticesNext-3)+2);
				}
			} else { //intersection at the beginning of the road
				for(unsigned int i = 0 ; !vertexAdded && i < nbVerticesNext - 1 ; i += 4){
					//get the left side of the road
					Vec3<double> nextRoadInt(*(vdataNext+8*(i+2)), *(vdataNext+8*(i+2)+1), *(vdataNext+8*(i+2)+2));
					Vec3<double> nextRoadExt(*(vdataNext+8*(i+3)), *(vdataNext+8*(i+3)+1), *(vdataNext+8*(i+3)+2));
					
					//dummy point, we only want the point on the real road
					Vec3<double> dummy;
					
					//if there is an intersection between the left side of the road and the deleted road...
					if(getIntersectionFrom2Segments(ptCenter0, ptCenter1, nextRoadInt, nextRoadExt, dummy, newPointRoad, 0.2) ||
							getIntersectionFrom2Segments(ptCenter2, ptCenter3, nextRoadInt, nextRoadExt, dummy, newPointRoad, 0.2)){
						//update road points informations
						rIPNext->leftIntersection = COMPLEX_INTERSECT;
						rIPNext->leftExtremity[0] = newPointRoad[0];
						rIPNext->leftExtremity[1] = newPointRoad[1];
						rIPNext->leftExtremity[2] = newPointRoad[2];
						rIPNext->leftPrimRank = (int)floor((double)i/4);
						vertexAdded = true;
					}
				}
				
				if(!vertexAdded){ //if no intersection found
					//add the end point of the road
					rIPNext->leftExtremity[0] = *(vdataNext+8*2);
					rIPNext->leftExtremity[1] = *(vdataNext+8*2+1);
					rIPNext->leftExtremity[2] = *(vdataNext+8*2+2);
				}
			}		
		}
		
		return true;
	}
	
	bool ComputeFactory::getStretchedSegmentsIntersection(vector<interInfo>& roadList,
			int segmentLimit,
			unsigned int nb,
			vector<roadIntersectionPoints>& intersectionPoints){
		//segment correction : sometimes, errors in the road network (because of 
		//approximative data) leads to errors in intersection geometry computation
		//as a try to correct these errors, when no intersection has been found
		//but the two concerned roads have intersections on their others sides
		//(meaning they are not isolated from the intersection because of
		//a deleted road, or whatever...), we will try to recompute
		//an intersection with the first segments of the road, stretching them
		//a little bit
		//cout << "stretching segments..." << endl;
		//points for the arLines
		Vec3<double> rSCExterior, rSCInterior, 
					   lSNExterior, lSNInterior;
		
		for(unsigned int i = 0 ; i < nb ; ++i){
			//cout << "stretching for road " << i << endl;
			//we test of the two endpoints are not two close to each other, to avoid 
			//applying the method on roads simply facing each other without intersecting
			//cout << intersectionPoints[i].rightIntersection << " " << intersectionPoints[(i+1)%nb].leftIntersection << endl;
			if((intersectionPoints[i].rightIntersection == ENDPOINT || intersectionPoints[i].rightIntersection == HIGHWAY) &&
					(intersectionPoints[(i+1)%nb].leftIntersection == ENDPOINT || intersectionPoints[(i+1)%nb].leftIntersection == HIGHWAY) /*&&
					intersectionPoints[i].leftIntersection != ENDPOINT && intersectionPoints[(i+1)%nb].rightIntersection != ENDPOINT*/ /*&&
					(intersectionPoints[i].rightExtremity - intersectionPoints[(i+1)%nb].leftExtremity).length() > 0.5*/){
				//cout << "effective test" << endl;
				//get geomtries to gain access to points
				HardwareVertexBufferSharedPtr bufferCurrentPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				float* vdataCurrent = static_cast<float*>(bufferCurrentPtr->lock(HardwareBuffer::HBL_READ_ONLY));
				HardwareVertexBufferSharedPtr bufferNextPtr = roadList[(i+1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				float* vdataNext = static_cast<float*>(bufferNextPtr->lock(HardwareBuffer::HBL_READ_ONLY));
				
				int nbVerticesCurrent = bufferCurrentPtr->getNumVertices();
				int nbVerticesNext = bufferNextPtr->getNumVertices();
				
				//get points of the right side of the first segment of the current road
				//in order to fake a stretched segment
				getSide(roadList[i].position, true, vdataCurrent, nbVerticesCurrent, 0, rSCExterior, rSCInterior);
				Vec3<double> normal = rSCExterior - rSCInterior;
				normal.normalize();
				//the end point of the stretched segment
				Vec3<double> rSCExteriorTrans = rSCExterior + normal * 3;
	
				//get points of the left side of the first segment of the next road
				//in order to fake a stretched segment
				getSide(roadList[(i+1)%nb].position, false, vdataNext, nbVerticesNext, 0, lSNExterior, lSNInterior);
				normal = lSNExterior - lSNInterior;
				normal.normalize();
				//the end point of the stretched segment
				Vec3<double> lSNExteriorTrans = lSNExterior + normal * 3;
				
				bool correctedInterFound = false;
				//points for the intersection
				Vec3<double> dummy, intersectCurrent;
				for(int j = 0 ; !correctedInterFound && j < nbVerticesNext && j < 4*segmentLimit ; j+=4){
					//get points for the left side of the j-th segment of the next road
					getSide(roadList[(i+1)%nb].position, false, vdataNext, nbVerticesNext, j, lSNExterior, lSNInterior);
					//compute intersection between j-th segment and stretched first segment of the right road
					if(getIntersectionFrom2Segments(rSCExteriorTrans, rSCInterior, lSNExterior, lSNInterior, dummy, intersectCurrent, 0.2)){
						//if any, fill informations as if it was a normal intersection
						correctedInterFound = true;
						intersectionPoints[i].rightIntersection = INTERSECT;
						intersectionPoints[i].rightExtremity[0] = intersectCurrent[0];
						intersectionPoints[i].rightExtremity[1] = intersectCurrent[1];
						intersectionPoints[i].rightExtremity[2] = intersectCurrent[2];
						intersectionPoints[i].rightPrimRank = 0;
						intersectionPoints[(i+1)%nb].leftIntersection = INTERSECT;
						intersectionPoints[(i+1)%nb].leftExtremity[0] = intersectCurrent[0];
						intersectionPoints[(i+1)%nb].leftExtremity[1] = intersectCurrent[1];
						intersectionPoints[(i+1)%nb].leftExtremity[2] = intersectCurrent[2];
						intersectionPoints[(i+1)%nb].leftPrimRank = (int)floor((double)j/4);
					}
				}
				
				//if we didn't found an intersection with current road stretched,
				//try with next road
				if(!correctedInterFound){
					//reset point of the next road that has been going through the road
					if(roadList[(i+1)%nb].position){
						lSNInterior = Vec3<double>(*(vdataNext+8*(nbVerticesNext-4)),
								*(vdataNext+8*(nbVerticesNext-4)+1),
								*(vdataNext+8*(nbVerticesNext-4)+2));
					} else {
						lSNInterior = Vec3<double>(*(vdataNext+8*3),
								*(vdataNext+8*3+1),
								*(vdataNext+8*3+2));
					}
					//cout << "searching for left stretched segment" << endl;
					for(int j = 0 ; !correctedInterFound && j < nbVerticesCurrent && j < 4*segmentLimit ; j+=4){
						//get points for the right side of the j-th segment of the current road
						getSide(roadList[i].position, true, vdataCurrent, nbVerticesCurrent, j, rSCExterior, rSCInterior);
						//compute intersection between j-th segment and stretched first segment of the left road
						if(getIntersectionFrom2Segments(lSNExteriorTrans, lSNInterior, rSCExterior, rSCInterior, dummy, intersectCurrent, 0.2)){
							//if any, fill informations as if it was a normal intersection
							//cout << "found stretch segment" << endl;
							correctedInterFound = true;
							intersectionPoints[i].rightIntersection = INTERSECT;
							intersectionPoints[i].rightExtremity[0] = intersectCurrent[0];
							intersectionPoints[i].rightExtremity[1] = intersectCurrent[1];
							intersectionPoints[i].rightExtremity[2] = intersectCurrent[2];
							intersectionPoints[i].rightPrimRank = (int)floor((double)j/4);
							intersectionPoints[(i+1)%nb].leftIntersection = INTERSECT;
							intersectionPoints[(i+1)%nb].leftExtremity[0] = intersectCurrent[0];
							intersectionPoints[(i+1)%nb].leftExtremity[1] = intersectCurrent[1];
							intersectionPoints[(i+1)%nb].leftExtremity[2] = intersectCurrent[2];
							intersectionPoints[(i+1)%nb].leftPrimRank = 0;
						}
					}
				}
				
				//release buffers
				bufferCurrentPtr->unlock();
				bufferNextPtr->unlock();
			}
		}	
		
		return true;
	}
	
	bool ComputeFactory::projectPoints(vector<interInfo>& roadList,
			unsigned int nb,
			vector<roadIntersectionPoints>& intersectionPoints){
		//now that we have computed intersections, we need, for each road,
		//to end them perpendicularly to their axis, projecting the further intersection
		//on the other side of the road and adding points of the border
		for(unsigned int i = 0 ; i < nb ; ++i){
			if(intersectionPoints[i].leftPrimRank == intersectionPoints[i].rightPrimRank && intersectionPoints[i].leftPrimRank == -1){
					//two end points, do nothing (projSide is already at NONE)
			} else if(intersectionPoints[i].leftPrimRank != intersectionPoints[i].rightPrimRank){
				bool leftFurther = (intersectionPoints[i].leftPrimRank > intersectionPoints[i].rightPrimRank);
				//leftFurther == true => left intersection is further from the intersection,
				//so project on right side
				
				//get the good rank and extremity
				int betterRank = leftFurther?intersectionPoints[i].leftPrimRank:intersectionPoints[i].rightPrimRank;
				Vector3 extremity = leftFurther?intersectionPoints[i].leftExtremity:intersectionPoints[i].rightExtremity;
				
				//get the geometry of the road
				HardwareVertexBufferSharedPtr bufferPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_READ_ONLY));
				unsigned int nbVertices = bufferPtr->getNumVertices();
				
				//get the position of the road 
				bool position = roadList[i].position;
				
				//get arLine for side
				Line<double> side;
				Vec3<double> sideExt, sideInt;
				//leftFurther == true => getArline will get us the right side
				getArline(position, leftFurther, vdata, nbVertices, 4*betterRank, side, sideExt, sideInt);
				
				//project intersection on side
				Vec3<double> intersectPoint(extremity[0], extremity[1], extremity[2]);
				Vec3<double> projOnSide = side.getClosestPoint(intersectPoint);
				
				//save informations
				intersectionPoints[i].projSide = leftFurther?RIGHT:LEFT;
				intersectionPoints[i].projection[0] = projOnSide[0];
				intersectionPoints[i].projection[1] = projOnSide[1];
				intersectionPoints[i].projection[2] = projOnSide[2];
				intersectionPoints[i].projPrimRank = leftFurther?intersectionPoints[i].leftPrimRank:intersectionPoints[i].rightPrimRank;
				
				//get points of the border from intersection to projection
				
				//leftFurther == true => get points on the right side
	
				//get the extremity rank on the side we projected to know when we need to stop while adding border points
				int projRank = leftFurther?intersectionPoints[i].rightPrimRank:intersectionPoints[i].leftPrimRank;
				
				//test if we detected the "highway syndrom" to add good border points
				//further intersection on right side => entering highway by the left
				if(intersectionPoints[i].rightIntersection == HIGHWAY){
					//add border of the current road from projection primitive
					//to highway intersection primitive
					for(int j = intersectionPoints[i].leftPrimRank ; j > intersectionPoints[i].rightPrimRank ; --j){
						if(position) {
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(nbVertices-2-4*j)),
											*(vdata+8*(nbVertices-2-4*j)+1),
											*(vdata+8*(nbVertices-2-4*j)+2)));
						} else {
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(4*j+1)),
											*(vdata+8*(4*j+1)+1),
											*(vdata+8*(4*j+1)+2)));
						}
					}
					//get the geometry of the previous road
					HardwareVertexBufferSharedPtr bufferPreviousPtr = roadList[(i+nb-1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
					float* vdataPrevious = static_cast<float*>(bufferPreviousPtr->lock(HardwareBuffer::HBL_READ_ONLY));
					unsigned int nbVerticesPrevious = bufferPreviousPtr->getNumVertices();
					//finally add border points from the primitive where we found 
					//an intersection to the start of the road
					for(int j = intersectionPoints[i].highwayPrimRank - 1 ; j >= intersectionPoints[i].highwayEndPrimRank ; --j){
						if(roadList[(i+nb-1)%nb].position){
							intersectionPoints[i].highwaySidePoints.push_back(
									Vector3(*(vdataPrevious+8*(nbVerticesPrevious-2-4*j)),
											*(vdataPrevious+8*(nbVerticesPrevious-2-4*j)+1),
											*(vdataPrevious+8*(nbVerticesPrevious-2-4*j)+2)));
						} else {
							intersectionPoints[i].highwaySidePoints.push_back(
									Vector3(*(vdataPrevious+8*(4*j+1)),
											*(vdataPrevious+8*(4*j+1)+1),
											*(vdataPrevious+8*(4*j+1)+2)));
						}
					}
					
					//release previous buffer
					bufferPreviousPtr->unlock();
				} else if(intersectionPoints[i].leftIntersection == HIGHWAY){
					//get the geometry of the next road
					HardwareVertexBufferSharedPtr bufferNextPtr = roadList[(i+1)%nb].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
					float* vdataNext = static_cast<float*>(bufferNextPtr->lock(HardwareBuffer::HBL_READ_ONLY));
					unsigned int nbVerticesNext = bufferNextPtr->getNumVertices();
					
					//add border points from the start of the road to the primitive 
					//where we have found an intersection
					for(int j = intersectionPoints[i].highwayEndPrimRank ; j < intersectionPoints[i].highwayPrimRank ; ++j){
						if(roadList[(i+1)%nb].position){
							intersectionPoints[i].highwaySidePoints.push_back(
									Vector3(*(vdataNext+8*(nbVerticesNext-4-4*j)),
											*(vdataNext+8*(nbVerticesNext-4-4*j)+1),
											*(vdataNext+8*(nbVerticesNext-4-4*j)+2)));
						} else {
							intersectionPoints[i].highwaySidePoints.push_back(
									Vector3(*(vdataNext+8*(4*j+3)),
											*(vdataNext+8*(4*j+3)+1),
											*(vdataNext+8*(4*j+3)+2)));
						}
					}
					//then the highway intersection point
					//intersectionPoints[i].sidePoints.push_back(intersectionPoints[i].highwayPoint);
					//finally, border of the current road from highway intersection primitive
					//to projection primitive
					for(int j = intersectionPoints[i].leftPrimRank ; j < intersectionPoints[i].rightPrimRank ; ++j){
						if(position) {
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(nbVertices-4-4*j)),
											*(vdata+8*(nbVertices-4-4*j)+1),
											*(vdata+8*(nbVertices-4-4*j)+2)));
						} else {
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(4*j+3)),
											*(vdata+8*(4*j+3)+1),
											*(vdata+8*(4*j+3)+2)));
						}
					}
					bufferNextPtr->unlock();
				} else { //add points normally
					if(leftFurther){ //border points on the right, add them in reverse order
						 			 //so they will be in the right order for merging
						//we test j>=0 because if projRank == -1...
						for(int j = betterRank - 1 ; j >= 0 && j >= projRank ; --j){
							if(position){ //end of the road
								intersectionPoints[i].sidePoints.push_back(
										Vector3(*(vdata+8*(nbVertices-2-4*j)),
												*(vdata+8*(nbVertices-2-4*j)+1),
												*(vdata+8*(nbVertices-2-4*j)+2)));
							} else { //beginning of the road
								intersectionPoints[i].sidePoints.push_back(
										Vector3(*(vdata+8*(4*j+1)),
												*(vdata+8*(4*j+1)+1),
												*(vdata+8*(4*j+1)+2)));
							}
						}					
					} else { //border points on the left, follow the road
						for(int j = (projRank<0?0:projRank) ; j < betterRank ; ++j){
							if(position){ //end of the road
								intersectionPoints[i].sidePoints.push_back(
										Vector3(*(vdata+8*(nbVertices-2-4*j-2)),
												*(vdata+8*(nbVertices-2-4*j-2)+1),
												*(vdata+8*(nbVertices-2-4*j-2)+2)));
							} else { //beginning of the road
								intersectionPoints[i].sidePoints.push_back(
										Vector3(*(vdata+8*(4*j+1+2)),
												*(vdata+8*(4*j+1+2)+1),
												*(vdata+8*(4*j+1+2)+2)));
							}						
						}
					}
				}
				
				//release buffer
				bufferPtr->unlock();
			} else {
				//leftPrimRank == rightPrimRank && leftPrimRank != -1
				//computed intersections are on the same primitive but are not endPoints,
				//we need to compute the two projections in order to know
				//which one is further from the intersection
				
				//get the geometry of the road
				HardwareVertexBufferSharedPtr bufferPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_READ_ONLY));
				unsigned int nbVertices = bufferPtr->getNumVertices();
				
				//get the position of the road 
				bool position = roadList[i].position;
				
				//get the sides of the road for this primitive
				Line<double> rightSide, leftSide;
				Vec3<double> rightSideExt, rightSideInt, leftSideExt, leftSideInt;
	
				getArline(position, true, vdata, nbVertices, 4*intersectionPoints[i].rightPrimRank, rightSide, rightSideExt, rightSideInt);
				getArline(position, false, vdata, nbVertices, 4*intersectionPoints[i].leftPrimRank, leftSide, leftSideExt, leftSideInt);
				
				//project intersections on each arLine
				Vec3<double> rightIntersect(intersectionPoints[i].rightExtremity[0],
						intersectionPoints[i].rightExtremity[1],
						intersectionPoints[i].rightExtremity[2]);
				Vec3<double> projOnLeftSide = leftSide.getClosestPoint(rightIntersect);
				Vec3<double> leftIntersect(intersectionPoints[i].leftExtremity[0],
						intersectionPoints[i].leftExtremity[1],
						intersectionPoints[i].leftExtremity[2]);
				Vec3<double> projOnRightSide = rightSide.getClosestPoint(leftIntersect);
				
				//check if the projection points are on the sides of the roads
				//if not, take one of the extremity instead 
				if ((projOnLeftSide-leftSideInt).dot(leftSideExt-leftSideInt) < 0) {
					projOnLeftSide = leftSideInt;
				} else if ((projOnLeftSide-leftSideExt).dot(leftSideInt-leftSideExt) < 0) {
					projOnLeftSide = leftSideExt;
				}
				
				if ((projOnRightSide-rightSideInt).dot(rightSideExt-rightSideInt) < 0) {
					projOnRightSide = rightSideInt;
				} else if ((projOnRightSide-rightSideExt).dot(rightSideInt-rightSideExt) < 0) {
					projOnRightSide = rightSideExt;
				}
				
				//the point further from the intersection will be the one 
				//whose sum of (the distance from it to the beginning of the primitive
				//on his side (exterior point) + the distance from its projection to the beginning
				//of the primitive on the other side) is the greatest
				//(maybe there is a better mathematical solution...)
	
				double leftPointDistance = (leftIntersect-leftSideExt).length() + (projOnRightSide-rightSideExt).length();
				double rightPointDistance = (rightIntersect-rightSideExt).length() + (projOnLeftSide-leftSideExt).length();
				
				if(leftPointDistance >= rightPointDistance){
					//left point is further, save the informations
					intersectionPoints[i].projSide = RIGHT;
					intersectionPoints[i].projection[0] = projOnRightSide[0];
					intersectionPoints[i].projection[1] = projOnRightSide[1];
					intersectionPoints[i].projection[2] = projOnRightSide[2];
					intersectionPoints[i].projPrimRank = intersectionPoints[i].leftPrimRank;
					
					//get points of the right border from projection to intersection	
					//as we will go through the intersection on clockwise order when merging points,
					//to have points in the right order now, we will add them in inverse order
					//we test j>=0 because if rightPrimRank == -1...
					for(int j = intersectionPoints[i].leftPrimRank - 1 ; j >= 0 && j >= intersectionPoints[i].rightPrimRank ; --j){
						if(position){ //end of the road
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(nbVertices-2-4*j)),
											*(vdata+8*(nbVertices-2-4*j)+1),
											*(vdata+8*(nbVertices-2-4*j)+2)));
						} else { //beginning of the road
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(4*j+1)),
											*(vdata+8*(4*j+1)+1),
											*(vdata+8*(4*j+1)+2)));
						}
					}
				} else {
					//right point is further, save the informations
					intersectionPoints[i].projSide = LEFT;
					intersectionPoints[i].projection[0] = projOnLeftSide[0];
					intersectionPoints[i].projection[1] = projOnLeftSide[1];
					intersectionPoints[i].projection[2] = projOnLeftSide[2];
					intersectionPoints[i].projPrimRank = intersectionPoints[i].rightPrimRank;
					
					//get points of the left border from intersection to projection		
					//if left intersection is an endpoint, leftPrimRank = -1 => adjust
					//(as we are using an unsigned we will get 2billions+...undefined behaviour, yikes !)
					int lowerRank = (intersectionPoints[i].leftPrimRank < 0)?0:intersectionPoints[i].leftPrimRank;
					for(int j = lowerRank ; j > intersectionPoints[i].rightPrimRank ; ++j){
						if(position){ //end of the road
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(nbVertices-4-4*j)),
											*(vdata+8*(nbVertices-4-4*j)+1),
											*(vdata+8*(nbVertices-4-4*j)+2)));
						} else { //beginning of the road
							intersectionPoints[i].sidePoints.push_back(
									Vector3(*(vdata+8*(4*j+3)),
											*(vdata+8*(4*j+3)+1),
											*(vdata+8*(4*j+3)+2)));
						}
					}
				}
				
				//release buffer
				bufferPtr->unlock();
			}
		}
		
		return true;
	}
	
	bool ComputeFactory::stretchRoadsIntersectionParts(vector<interInfo>& roadList, 
			unsigned int nb,
			vector<roadIntersectionPoints>& intersectionPoints){
		for(unsigned int i = 0 ; i < nb ; ++i){
			//points of the limit between the intersection and the road
			Vector3 leftEndPart = intersectionPoints[i].leftExtremity;
			Vector3 rightEndPart = intersectionPoints[i].rightExtremity;
			unsigned int j = (intersectionPoints[i].leftPrimRank>0)?intersectionPoints[i].leftPrimRank:0;
			unsigned int k = (intersectionPoints[i].rightPrimRank>0)?intersectionPoints[i].rightPrimRank:0;
			switch(intersectionPoints[i].projSide){
			case NONE:
				//no projection, end points are limits
				break;
				
			case RIGHT:
				//projection on right, right limit is projection
				rightEndPart = intersectionPoints[i].projection;
				k = intersectionPoints[i].projPrimRank;
				break;
				
			case LEFT:
				//projection on left, left limit is projection
				leftEndPart = intersectionPoints[i].projection;	
				j = intersectionPoints[i].projPrimRank;		
			}
			bool foundLeft = false;
			HardwareVertexBufferSharedPtr bufferPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			unsigned int nbVertices = bufferPtr->getNumVertices();
			Vector3 endLeftPrim, endRightPrim;
			//search on the left side on which primitive we should end the marking 
			for(; !foundLeft && j < (nbVertices/4) ; ++j){
				//get the end od the j-th primitive
				if(roadList[i].position){
					endLeftPrim[0] = *(vdata+8*(nbVertices-4-4*j));
					endLeftPrim[1] = *(vdata+8*(nbVertices-4-4*j)+1);
					endLeftPrim[2] = *(vdata+8*(nbVertices-4-4*j)+2);
				} else {
					endLeftPrim[0] = *(vdata+8*(3+4*j));
					endLeftPrim[1] = *(vdata+8*(3+4*j)+1);
					endLeftPrim[2] = *(vdata+8*(3+4*j)+2);
				}
				if((endLeftPrim - leftEndPart).length() > STRETCHLENGTH){
					//we are on the primitive where is the point we want, to stretch the road
					foundLeft = true;
				}
			}
			bool foundRight = false;
			//same thing, but on the right now
			for(; foundLeft && !foundRight && k < (nbVertices/4) ; ++k){
				//get the end of the k-th primitive
				if(roadList[i].position){
					endRightPrim[0] = *(vdata+8*(nbVertices-2-4*k));
					endRightPrim[1] = *(vdata+8*(nbVertices-2-4*k)+1);
					endRightPrim[2] = *(vdata+8*(nbVertices-2-4*k)+2);
				} else {
					endRightPrim[0] = *(vdata+8*(1+4*k));
					endRightPrim[1] = *(vdata+8*(1+4*k)+1);
					endRightPrim[2] = *(vdata+8*(1+4*k)+2);
				}
				if((endRightPrim - rightEndPart).length() > STRETCHLENGTH){
					//we are on the primitive where is the point we want, to stretch the road
					foundRight = true;
				}				
			}
			unsigned int maxJK = j>k?j:k;
			//j+k >= nbVertices/4 -> road isn't long enough for the two stretching
			if(foundLeft && foundRight && 2*maxJK + 1 < (nbVertices/4)){
				//get the points
				Vector3 direction = endLeftPrim - leftEndPart;
				direction.normalise();
				direction *= STRETCHLENGTH;
				intersectionPoints[i].leftMarkingExtremity = leftEndPart + direction;
				intersectionPoints[i].leftMarkingPrimRank = j;
				direction = endRightPrim - rightEndPart;
				direction.normalise();
				direction *= STRETCHLENGTH;
				intersectionPoints[i].rightMarkingExtremity = rightEndPart + direction;
				intersectionPoints[i].rightMarkingPrimRank = k;
			} else {
				//road isn't long enough to stretch road intersection part
				//it possibly means that this road is between two intersections
				//but is a major road and will be merged with others, 
				//or that the network is badly formed
				intersectionPoints[i].rightMarkingExtremity = intersectionPoints[i].rightExtremity;
				intersectionPoints[i].rightMarkingPrimRank = intersectionPoints[i].rightPrimRank;
				intersectionPoints[i].leftMarkingExtremity = intersectionPoints[i].leftExtremity;
				intersectionPoints[i].leftMarkingPrimRank = intersectionPoints[i].leftPrimRank;			
				switch(intersectionPoints[i].projSide){
				case NONE:
					break;
					
				case LEFT:
					intersectionPoints[i].leftMarkingExtremity = intersectionPoints[i].projection;
					intersectionPoints[i].leftMarkingPrimRank = intersectionPoints[i].projPrimRank;
					break;
					
				case RIGHT:
					intersectionPoints[i].rightMarkingExtremity = intersectionPoints[i].projection;
					intersectionPoints[i].rightMarkingPrimRank = intersectionPoints[i].projPrimRank;
					break;	
				}
			}
			bufferPtr->unlock();
		}
		
		return true;
	}
	
	bool ComputeFactory::mergePoints(vector<vector<Vector3> >& outRoads,
			vector<vector<Vector3> >& normalRoads,
			vector<vector<Vector2> >& texRoads,
			vector<Vector3>& outInter,
			vector<Vector4>& colorInter,
			vector<Vector3>& normalInter,
			vector<Vector2>& texInter,
			int nb, vector<interInfo>& roadList,
			vector<roadIntersectionPoints>& intersectionPoints,
			Intersection* intersection) {
		//for the merging, as we are going through the roads circularly,
		//to have the same algorith on each road, we will add for each road
		//the points that strictly belongs to it (projection point, border points)
		//and his right extremity (and left extremity if it is an end point/complex_intersect/highway,
		//as it ins't shared with any other road)
		
		for(int i = 0 ; i < nb ; ++i){
			bool position = roadList[i].position;
			
			double sideLength = 0;
			//get the geometry of the road
			HardwareVertexBufferSharedPtr bufferPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			unsigned int nbVertices = bufferPtr->getNumVertices();
			
			Vector3 leftTextureEnd, rightTextureEnd;
			switch(intersectionPoints[i].projSide){
			case LEFT:
				//where will be the left end of the texture
				if(intersectionPoints[i].leftIntersection == HIGHWAY){
					leftTextureEnd = intersectionPoints[i].highwayPoint;
				} else {
					leftTextureEnd = intersectionPoints[i].leftExtremity;
				}
				
				//get length of the projection side to correctly apply texture
				if(intersectionPoints[i].sidePoints.size() > 0){ //if they are side points
					//first add the distance between the texture end and the first side point
					sideLength += (leftTextureEnd-intersectionPoints[i].sidePoints[0]).length();
					//then the distance between each sidePoints
					for(unsigned int j = 0 ; j < intersectionPoints[i].sidePoints.size() -1 ; ++j){
						sideLength += (intersectionPoints[i].sidePoints[j+1]-intersectionPoints[i].sidePoints[j]).length();
					}
					//and the distance between the last side point and the projection
					sideLength += (intersectionPoints[i].projection - intersectionPoints[i].sidePoints[intersectionPoints[i].sidePoints.size()-1]).length();
				} else { //no side points
					//add the distance between the texture end and the projection
					sideLength += (intersectionPoints[i].projection - leftTextureEnd).length();
				}
				//finally add the distance between projection and marking left end
				sideLength += (intersectionPoints[i].leftMarkingExtremity - intersectionPoints[i].projection).length();
				break;
				
			case RIGHT:
				//where will be the right end of the texture
				if(intersectionPoints[i].rightIntersection == HIGHWAY){
					rightTextureEnd = intersectionPoints[i].highwayPoint;
				} else {
					rightTextureEnd = intersectionPoints[i].rightExtremity;
				}
				break;
			}
			
			//now start to add points
			
			//first if the left extremity is not an intersect (only kind of points shared by two roads)
			if(intersectionPoints[i].leftIntersection == ENDPOINT || intersectionPoints[i].leftIntersection == COMPLEX_INTERSECT || intersectionPoints[i].leftIntersection == HIGHWAY){
				//add it in outInter
				outInter.push_back(intersectionPoints[i].leftExtremity);
			}
			
			if(intersectionPoints[i].leftIntersection == HIGHWAY){
				//add the highwayPoint in the geometry of the road
				outRoads[i].push_back(intersectionPoints[i].highwayPoint);
				//add the highway border points and the highway point in the geometry of the intersection
				for(unsigned int j = 0 ; j < intersectionPoints[i].highwaySidePoints.size() ; ++j){
					outInter.push_back(intersectionPoints[i].highwaySidePoints[j]);
				}
				outInter.push_back(intersectionPoints[i].highwayPoint);
			} else {
				//add the left extremity in the geometry of the road
				outRoads[i].push_back(intersectionPoints[i].leftExtremity);	
			}	
			
			//if we are at the end of the road (position == true) we will texturate forward,
			//but if we are texturing from the start, we must texturate backward, so use a factor
			//rather than testing position 
			int factor = position?1:-1;
			
			//where is the projection ?
			switch(intersectionPoints[i].projSide){
			case NONE:
				//two endpoints, simply add end points
				//add texture coordinates for the left extremity
				texRoads[i].push_back(Vector2(0.0,
						factor*(intersectionPoints[i].leftExtremity - intersectionPoints[i].leftMarkingExtremity).length()/roadList[i].texLength));
				
				//add end points
				outRoads[i].push_back(intersectionPoints[i].leftMarkingExtremity);
				texRoads[i].push_back(Vector2(0.0, 0.0));
				
				outRoads[i].push_back(intersectionPoints[i].rightMarkingExtremity);
				texRoads[i].push_back(Vector2(1.0, 0.0));
				
				//add texture coordinates for the right extremity
					texRoads[i].push_back(Vector2(1.0,
							factor*(intersectionPoints[i].rightExtremity - intersectionPoints[i].rightMarkingExtremity).length()/roadList[i].texLength));				
				break;
				
			case RIGHT:
				//better intersect on the left => projection and border points on the right
				//add texture coordinates for the left extremity
				//as we are projecting on the right, leftExtremity can't be messed up by a highway point
				texRoads[i].push_back(Vector2(0.0, 
						factor*(intersectionPoints[i].leftExtremity - intersectionPoints[i].leftMarkingExtremity).length()/roadList[i].texLength));				
				
				//first add the marking extremity
				outRoads[i].push_back(intersectionPoints[i].leftMarkingExtremity);
				texRoads[i].push_back(Vector2(0.0,0.0));
				
				outRoads[i].push_back(intersectionPoints[i].rightMarkingExtremity);
				texRoads[i].push_back(Vector2(1.0,0.0));
	
				sideLength += (intersectionPoints[i].rightMarkingExtremity-intersectionPoints[i].projection).length();
				
				//then the projection on the right side
				outRoads[i].push_back(intersectionPoints[i].projection);
				texRoads[i].push_back(Vector2(1.0, 
						factor*sideLength/roadList[i].texLength));
				
				if(intersectionPoints[i].sidePoints.size() > 0){ //if they are border points
					//add the distance between the projection and the first border point
					sideLength += (intersectionPoints[i].sidePoints[0] - intersectionPoints[i].projection).length();
					
					//add the border points
					for(unsigned int j = 0 ; j < intersectionPoints[i].sidePoints.size() - 1 ; ++j){
						outRoads[i].push_back(intersectionPoints[i].sidePoints[j]);
						texRoads[i].push_back(Vector2(1, 
								factor*sideLength/roadList[i].texLength));
						sideLength += (intersectionPoints[i].sidePoints[j] - intersectionPoints[i].sidePoints[j+1]).length();
					}
					//add the last border point
					outRoads[i].push_back(intersectionPoints[i].sidePoints[intersectionPoints[i].sidePoints.size() - 1]);
					texRoads[i].push_back(Vector2(1, 
							factor*sideLength/roadList[i].texLength));
					sideLength += (intersectionPoints[i].sidePoints[intersectionPoints[i].sidePoints.size() - 1] - rightTextureEnd).length();
					
					//texture coordinates for the right extremity
					texRoads[i].push_back(Vector2(1, 
							factor*sideLength/roadList[i].texLength));	
				} else { //no border points
					//add the distance between the projection and the right extremity
					sideLength += (intersectionPoints[i].projection - rightTextureEnd).length();
					
					//texture coordinates for the right extremity
					texRoads[i].push_back(Vector2(1, 
							factor*sideLength/roadList[i].texLength));
				}
				break;
				
			case LEFT:
				//better intersect on the right => projection and border points on the left
				//add texture coordinates for the left extremity
					texRoads[i].push_back(Vector2(0.0, 
							factor*sideLength/roadList[i].texLength));
				
				if(intersectionPoints[i].sidePoints.size() > 0){ //if they are side points
					sideLength -= (leftTextureEnd - intersectionPoints[i].sidePoints[0]).length();
					//first add the border points
					for(unsigned int j = 0 ; j < intersectionPoints[i].sidePoints.size() - 1 ; ++j){
						outRoads[i].push_back(intersectionPoints[i].sidePoints[j]);
						texRoads[i].push_back(Vector2(0, 
								factor*sideLength/roadList[i].texLength));
						sideLength -= (intersectionPoints[i].sidePoints[j] - intersectionPoints[i].sidePoints[j+1]).length();
					}
					//add the last border point and decrease sideLength from the distance between this border point and the projection
					outRoads[i].push_back(intersectionPoints[i].sidePoints[intersectionPoints[i].sidePoints.size()-1]);
					texRoads[i].push_back(Vector2(0.0, 
							factor*sideLength/roadList[i].texLength));
					sideLength -= (intersectionPoints[i].sidePoints[intersectionPoints[i].sidePoints.size()-1] - intersectionPoints[i].projection).length();
				} else { //no side points
					//just decrease the side length
					sideLength -= (leftTextureEnd - intersectionPoints[i].projection).length();
				}
				//then the projection on the left side
				outRoads[i].push_back(intersectionPoints[i].projection);
				texRoads[i].push_back(Vector2(0.0, 
						factor*sideLength/roadList[i].texLength));
				//useless decrementation, see below why 
				//sideLength -= (intersectionPoints[i].projection - intersectionPoints[i].leftMarkingExtremity).length();
				//then the marking extremity
				outRoads[i].push_back(intersectionPoints[i].leftMarkingExtremity);
				//directly use 0 this time for y coordinates rather than sideLength to prevent floating point computation errors
				texRoads[i].push_back(Vector2(0.0,0.0));				
				
				outRoads[i].push_back(intersectionPoints[i].rightMarkingExtremity);
				texRoads[i].push_back(Vector2(1.0,0.0));				
				
				//texture coordinates for the right extremity
				//as projection is on the left, rightExtremity can't be messed up by a highway point
				texRoads[i].push_back(Vector2(1.0,
							factor*(intersectionPoints[i].rightExtremity - intersectionPoints[i].rightMarkingExtremity).length()/roadList[i].texLength));				
				break;			
			}
			
			//finally push_back the right intersection int the road geometry and in the intersection
			if(intersectionPoints[i].rightIntersection == HIGHWAY){
				//add the highwayPoint in the geometry of the road
				outRoads[i].push_back(intersectionPoints[i].highwayPoint);
				outInter.push_back(intersectionPoints[i].highwayPoint);
				//add the highway border points and the highway point in the geometry of the intersection
				for(unsigned int j = 0 ; j < intersectionPoints[i].highwaySidePoints.size() ; ++j){
					outInter.push_back(intersectionPoints[i].highwaySidePoints[j]);
				}
			} else {
				//add the right extremity in the geometry of the road
				outRoads[i].push_back(intersectionPoints[i].rightExtremity);
				outInter.push_back(intersectionPoints[i].rightExtremity);	
			}
			
			//reverse order of vertices if projection is on the left
			//the tesselation create a texturing problem with the current order 
			//of vertices when side points are on the left
			if(intersectionPoints[i].projSide == LEFT){
				std::reverse(outRoads[i].begin(), outRoads[i].end());
				std::reverse(texRoads[i].begin(), texRoads[i].end());
			}
			
			//release buffer 
			bufferPtr->unlock();
		}
		
		//end of merging
		return true;
	}
	
	//"pop" useless primitives of the road (the ones inside the geometry of the intersection)
	void ComputeFactory::removePrimitives(SubMesh* roadGeom,
			bool sens, unsigned int nbPrim) {
	
		if (nbPrim > 0) {
			//if there are enough primitives in the road
			if (roadGeom->vertexData->vertexCount/4 > nbPrim) {
				if (sens) {
					//4 vertices per primitive (2 triangles)
					roadGeom->vertexData->vertexCount -= 4*nbPrim;
					//2 triangles, 3 vertices per triangle => 6 indices to pop per primitive
					roadGeom->indexData->indexCount -= 2*3*nbPrim;
				} else {
					//get the old vertex buffer 
					HardwareVertexBufferSharedPtr vbufferOld = roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
					float* vdataOld = static_cast<float*>(vbufferOld->lock(HardwareBuffer::HBL_READ_ONLY));
				    
					unsigned int nbVertices = roadGeom->vertexData->vertexCount - 4*nbPrim;
					
				    //Create a new vertex buffer
				    HardwareVertexBufferSharedPtr vbufferNew =
				    HardwareBufferManager::getSingletonPtr()->createVertexBuffer(roadGeom->vertexData->vertexDeclaration->getVertexSize(0), // == offset
				                                 nbVertices,
				                                 HardwareBuffer::HBU_STATIC_WRITE_ONLY,
				                                 true);
				    
				    //Lock the buffer for writing
				    float* vdataNew = static_cast<float*>(vbufferNew->lock(HardwareBuffer::HBL_DISCARD));
	
				    for (unsigned int i=0;i<nbVertices;++i)
				    {
				        // Position
				        *vdataNew++ = *(vdataOld+8*(i+4*nbPrim));
				        *vdataNew++ = *(vdataOld+8*(i+4*nbPrim)+1);
				        *vdataNew++ = *(vdataOld+8*(i+4*nbPrim)+2);
				        // Normal
				        *vdataNew++ = 0;
				        *vdataNew++ = 1;
				        *vdataNew++ = 0;
				        // Texture coordinate
				        *vdataNew++ = *(vdataOld+8*(i+4*nbPrim)+6);
				        *vdataNew++ = *(vdataOld+8*(i+4*nbPrim)+7);
				    }
				    
				    //Release the buffers
					vbufferNew->unlock();
					vbufferOld->unlock();
					
					//set the new number of vertices
					roadGeom->vertexData->vertexCount = nbVertices;
				    
				    //Set the new buffer binding
				    roadGeom->vertexData->vertexBufferBinding->setBinding(0,vbufferNew);
					
					//we don't need the old buffer anymore, it will destroyed at the end of the function 
				    //as the last reference on it (vbufferOld) will be lost
	
				    //get the old index buffer
				    HardwareIndexBufferSharedPtr ibufferOld = roadGeom->indexData->indexBuffer;
				    uint32* idataOld = static_cast<uint32*>(ibufferOld->lock(HardwareBuffer::HBL_READ_ONLY));
				    
				    unsigned int nbIndexes = roadGeom->indexData->indexCount - 2*3*nbPrim;
				    unsigned int nbTriangles = nbIndexes/3;
				    
				   // Creates the new index data
				   HardwareIndexBufferSharedPtr ibufferNew =
				    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
				                                nbIndexes,
				                                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
				   
				   //lock this new buffer
				   uint32* idataNew = static_cast<uint32*>(ibufferNew->lock(HardwareBuffer::HBL_DISCARD));
				   
				   //fill it
				   for(unsigned int i = 0 ; i < nbTriangles ; ++i){
					   // 2*3*nbPrim : for each primitive (segment of the road), 2 triangles with 3 indexes
					   //don't forget to substract primitives that will be popped
					   *idataNew++ = *(idataOld+3*i+2*3*nbPrim)-4*nbPrim;
					   *idataNew++ = *(idataOld+3*i+2*3*nbPrim+1)-4*nbPrim;
					   *idataNew++ = *(idataOld+3*i+2*3*nbPrim+2)-4*nbPrim;
				   }
				   
				   //unlock it
				   ibufferNew->unlock();
				   ibufferOld->unlock();
				   
				   //bind it to the submesh
				   roadGeom->indexData->indexBuffer = ibufferNew;
				   
				   //indicate the new number of indexes
				   roadGeom->indexData->indexCount = nbIndexes;
				}
			}
		}
	}
	
	
	void ComputeFactory::levelIntersection(vector<vector<Vector3> >& outRoads,
			vector<Vector3>& outInter, vector<roadIntersectionPoints>& intersectionPoints) {
		// put all points from the intersection to the same level
		double averageHeight = 0.0;
		double count = 0;
		//first compute the average height
		for(unsigned int i = 0 ; i < outRoads.size() ; ++i){
			//using the part of the roads belonging to the intersection
			for (unsigned int j = 0 ; j < outRoads[i].size() ; ++j) {
				averageHeight += outRoads[i][j][1];
				++count;
			}		
		}
		//and the geometry of the intersection itself
		for(unsigned int i = 0 ; i < outInter.size() ; ++i){
			averageHeight += outInter[i][1];
			++count;
		}
	
		//compute average height
		averageHeight /= count;
		
		//Apply it on geometries
		for(unsigned int i = 0 ; i < outRoads.size() ; ++i){
			for (unsigned int j = 0 ; j < outRoads[i].size() ; ++j) {
				outRoads[i][j][1] = averageHeight;
			}		
		}
		for(unsigned int i = 0 ; i < outInter.size() ; ++i){
			outInter[i][1] = averageHeight;
		}
		
		//don't forget to level points in rIPs struct, has they have been computed
		//before this leveling and thus won't be affected directly as out arrays are
		//copies of the points of this structures
		for(unsigned int i = 0 ; i < intersectionPoints.size() ; ++i){
			intersectionPoints[i].rightExtremity.y = averageHeight;
			intersectionPoints[i].leftExtremity.y = averageHeight;
			intersectionPoints[i].projection.y = averageHeight;
			intersectionPoints[i].highwayPoint.y = averageHeight;
			for(unsigned int j = 0 ; j < intersectionPoints[i].highwaySidePoints.size() ; ++j){
				intersectionPoints[i].highwaySidePoints[j].y = averageHeight;
			}
			for(unsigned int j = 0 ; j < intersectionPoints[i].sidePoints.size() ; ++j){
				intersectionPoints[i].sidePoints[j].y = averageHeight;
			}
			intersectionPoints[i].rightMarkingExtremity.y = averageHeight;
			intersectionPoints[i].leftMarkingExtremity.y = averageHeight;
		}
	}
	
	// update intersection geometry
	// move road extremities to fit the intersection 
	void ComputeFactory::updateIntersection(vector<interInfo>& roadList,
			vector<roadIntersectionPoints>& intersectionPoints) {
	
		for (unsigned int i=0; i<roadList.size(); i++) {
			HardwareVertexBufferSharedPtr bufferPtr = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
			float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_NORMAL));
			unsigned int nbVertices = roadList[i].roadGeom->vertexData->vertexCount;
			int rightExtremityInd, leftExtremityInd;
			
			if (roadList[i].position) {
				//intersection at the end of the road
				rightExtremityInd = nbVertices-1;
				leftExtremityInd = nbVertices-3;
			} else {
				//intersection at the beginning of the road
				rightExtremityInd = 0;
				leftExtremityInd = 2;
			}
			
			//update the geometry of the road
			*(vdata+8*rightExtremityInd) = intersectionPoints[i].rightMarkingExtremity[0];
			*(vdata+8*rightExtremityInd+1) = intersectionPoints[i].rightMarkingExtremity[1];
			*(vdata+8*rightExtremityInd+2) = intersectionPoints[i].rightMarkingExtremity[2];
			*(vdata+8*leftExtremityInd) = intersectionPoints[i].leftMarkingExtremity[0];
			*(vdata+8*leftExtremityInd+1) = intersectionPoints[i].leftMarkingExtremity[1];
			*(vdata+8*leftExtremityInd+2) = intersectionPoints[i].leftMarkingExtremity[2];
			
			//now we need to smooth the road because if there is a big difference
			//between different roads of the intersection it won't look good
			
			unsigned int maxSegmentNumber = (int)floorf((double)(nbVertices)*SMOOTHPORTION/4);
			//maxSegmentNumber == 0 => there is only segment in the road,
			//no need to smooth
			if(maxSegmentNumber != 0){
				int segmentNumber;
				float smoothLength = 0;
				if(roadList[i].position){
					int lastSegmentNumber = nbVertices/4 -1;
					for(segmentNumber = lastSegmentNumber ; segmentNumber > lastSegmentNumber - maxSegmentNumber && smoothLength < SMOOTHMAXLENGTH ; --segmentNumber){
						smoothLength += getRoadSegmentLength(vdata, segmentNumber);
					}
					//increase segmentNumber to have the number of the last segment added
					++segmentNumber;
					if(smoothLength > SMOOTHMAXLENGTH){
						//decrease smoothLength as the last segment length addition made it
						//above SMOOTHMAXLENGTH
						smoothLength -= getRoadSegmentLength(vdata, segmentNumber);
						++segmentNumber;
					}
				} else {
					for(segmentNumber = 0 ; segmentNumber < maxSegmentNumber && smoothLength < SMOOTHMAXLENGTH ; ++segmentNumber){
						smoothLength += getRoadSegmentLength(vdata, segmentNumber);
					}
					//decrease segmentNumber to have the number of the last segment added
					--segmentNumber;
					if(smoothLength > SMOOTHMAXLENGTH){
						//decrease smoothLength as the last segment length addition made it
						//above SMOOTHMAXLENGTH
						smoothLength -= getRoadSegmentLength(vdata, segmentNumber);
						--segmentNumber;
					}
				}
				//now that we have the total length we want to smooth on and the number
				//of the segment we want to stop at, smooth
				double computedLength = 0;
				if(roadList[i].position){
					//compute the difference between end of the road and previous point
					double cutHeight = *(vdata+8*(nbVertices-2)+1) - *(vdata+8*(nbVertices-1)+1);
					//if the slope is less than 1/100, don't smooth
					if(fabs(cutHeight/getRoadSegmentLength(vdata, (nbVertices/4 -1))) > 0.01){
						int j = nbVertices/4 -1;
						computedLength += getRoadSegmentLength(vdata, j);
						
						*(vdata+8*(4*j)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);	
						*(vdata+8*(4*j+2)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						
						for(j = nbVertices/4 - 2 ; j > segmentNumber ; --j){
							*(vdata+8*(4*j+1)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							*(vdata+8*(4*j+3)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							
							computedLength += getRoadSegmentLength(vdata, j);
							
							*(vdata+8*(4*j)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							*(vdata+8*(4*j+2)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						}
						//do the last modification "by hand", avoiding floating point computation errors
						//that will break the road
						*(vdata+8*(4*j+1)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						*(vdata+8*(4*j+3)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
					}
				} else {
					//compute the difference between end of the road and previous point
					double cutHeight = *(vdata+9) - *(vdata+1);
					if(fabs(cutHeight/getRoadSegmentLength(vdata, 0)) > 0.01){
						unsigned int j = 0;
						computedLength += getRoadSegmentLength(vdata, j);
						
						*(vdata+8*(4*j+1)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						*(vdata+8*(4*j+3)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						
						for(j = 1 ; j < segmentNumber ; ++j){
							*(vdata+8*(4*j)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							*(vdata+8*(4*j+2)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							
							computedLength += getRoadSegmentLength(vdata, j);
							
							*(vdata+8*(4*j+1)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
							*(vdata+8*(4*j+3)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						}
						*(vdata+8*(4*j)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
						*(vdata+8*(4*j+2)+1) -= cutHeight*((smoothLength-computedLength)/smoothLength);
					}
				}
			}
			
			bufferPtr->unlock();
		}
	}
	
	void ComputeFactory::getArline(
			bool position,
			bool rightSide, // true if right side of the road
			float* vdata,
			unsigned int nbVertices,
			unsigned int indice,
			Line<double>& line, 
			Vec3<double>& p10, Vec3<double>& p11) {	
		int sideExt, sideInt;
	
		//get the points of the side of the road, depending on where we are 
		//(end or beginning chosen from position) and where is the side we want the points (choosen from sens)
		if(position){ // ==END
			if(rightSide){
				sideExt = nbVertices-indice-1;
				sideInt = nbVertices-indice-2;
			} else {
				sideExt = nbVertices-indice-3;
				sideInt = nbVertices-indice-4;		
			}
		} else { //==BEGIN
			if(rightSide){
				sideExt = indice;
				sideInt = indice+1;
			} else {
				sideExt = indice+2;
				sideInt = indice+3;		
			}
		}
		p10 = Vec3<double>(*(vdata+8*sideExt), *(vdata+8*sideExt+1), *(vdata+8*sideExt+2));
		p11 = Vec3<double>(*(vdata+8*sideInt), *(vdata+8*sideInt+1), *(vdata+8*sideInt+2));
	
		line.setValue(p11, p10);
	}
	
	void ComputeFactory::getSide(
			bool position,
			bool rigthSide, // true if right side of the road
			float* vdata,
			unsigned int nbVertices,
			unsigned int indice,
			Vec3<double>& p10, Vec3<double>& p11) {
		
		int sideExt, sideInt;
	
		//get the points of the side of the road, depending on where we are 
		//(end or beginning chosen from position) and where is the side we want the points (choosen from sens)
		if(position){ // ==END
			if(rigthSide){
				sideExt = nbVertices-indice-1;
				sideInt = nbVertices-indice-2;
			} else {
				sideExt = nbVertices-indice-3;
				sideInt = nbVertices-indice-4;		
			}
		} else { //==BEGIN
			if(rigthSide){
				sideExt = indice;
				sideInt = indice+1;
			} else {
				sideExt = indice+2;
				sideInt = indice+3;		
			}
		}
		p10 = Vec3<double>(*(vdata+8*sideExt), *(vdata+8*sideExt+1), *(vdata+8*sideExt+2));
		p11 = Vec3<double>(*(vdata+8*sideInt), *(vdata+8*sideInt+1), *(vdata+8*sideInt+2));
	}
	
	// get intersection between two segments (2D intersection)
	// returns false if there is no intersection
	bool ComputeFactory::getIntersectionFrom2Segments(Vec3<double> p10,
			Vec3<double> p11, Vec3<double> p20, Vec3<double> p21,
			Vec3<double>& pLine1, Vec3<double>& pLine2, double tolerance) {
		bool intersectionFound = false;
		Vec3<double> p2D10(p10[0], 0, p10[2]);
		Vec3<double> p2D11(p11[0], 0, p11[2]);
		Vec3<double> p2D21(p21[0], 0, p21[2]);
		Vec3<double> p2D20(p20[0], 0, p20[2]);
		Line<double> line2D1(p2D11, p2D10);
		Line<double> line2D2(p2D21, p2D20);
	
		Vec3<double> pLine2D1, pLine2D2;
	
		if (line2D1.getClosestPoints(line2D2, pLine2D1, pLine2D2)) {
			//if the two points are nearly the same, we have an intersection
			//between the lines held by the segments
			if ((pLine2D1-pLine2D2).length() <= tolerance) {
				//is really intersection on the segments using properties of dot product
				if ((((p2D10 - pLine2D1).dot(p2D11-p2D10) * (p2D11 - pLine2D1).dot(p2D11-p2D10)) < 0) && (((p2D20 - pLine2D2).dot(p2D21-p2D20) * (p2D21 - pLine2D2).dot(p2D21-p2D20)) < 0)) {
					Vec3<double> tmp;
					Line<double> line1(p11, p10);
					Line<double> line2(p21, p20);
					Line<double> verticalVect(pLine2D1, Vec3<double>(
							pLine2D1[0], pLine2D1[1]-2, pLine2D1[2]));
					verticalVect.getClosestPoints(line1, tmp, pLine1);
					Line<double> verticalVect2(pLine2D2, Vec3<double>(
							pLine2D2[0], pLine2D2[1]-2, pLine2D2[2]));
					verticalVect2.getClosestPoints(line2, tmp, pLine2);
	
					intersectionFound = true;
				}
			}
		}
		return intersectionFound;
	}
	
	//get the length of a road segment, meaning the average between the length of its two sides
	double ComputeFactory::getRoadSegmentLength(float* vdata, unsigned int segmentNumber){
		return (sqrt(pow(*(vdata+8*(4*segmentNumber)) - *(vdata+8*(4*segmentNumber+1)), 2) +
					 pow(*(vdata+8*(4*segmentNumber)+1) - *(vdata+8*(4*segmentNumber+1)+1), 2) +
					 pow(*(vdata+8*(4*segmentNumber)+2) - *(vdata+8*(4*segmentNumber+1)+2), 2)) +
				sqrt(pow(*(vdata+8*(4*segmentNumber+2)) - *(vdata+8*(4*segmentNumber+3)), 2) + 
					 pow(*(vdata+8*(4*segmentNumber+2)+1) - *(vdata+8*(4*segmentNumber+3)+1), 2) + 
					 pow(*(vdata+8*(4*segmentNumber+2)+2) - *(vdata+8*(4*segmentNumber+3)+2), 2)))/2; 
	}
	
	//compute texture coordinates for the center of the intersection
	vector<Vector2> ComputeFactory::computeIntersectionCenterTexCoords(vector<interInfo>& roadList,
			vector<Vector3>& outInter, double texLength){
		
		HardwareVertexBufferSharedPtr vbuffer = roadList[0].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
		float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_READ_ONLY));
		
		//Ogre::Vector3 to hold direction used to calculate coordinates
		Vector3 direction;
		//use 0 for y coordinate as intersections are leveled so they are flat
		direction.y = 0;
		//get direction of the first road using its right side
		if(roadList[0].position){
			unsigned int nbVertices = roadList[0].roadGeom->vertexData->vertexCount;
			direction.x = *(vdata+8*(nbVertices-1)) - *(vdata+8*(nbVertices-2));
			direction.z = *(vdata+8*(nbVertices-1)+2) - *(vdata+8*(nbVertices-2)+2);
		} else {
			direction.x = *(vdata) - *(vdata+8);
			direction.z = *(vdata+2) - *(vdata+10);
		}
		vbuffer->unlock();
		int chosenRoadIndex = 0;
		
		//get the width of the first road of the intersection
		double chosenRoadWidth = roadList[0].position?
				roadList[0].roadway->getEndingWidth():roadList[0].roadway->getStartingWidth();
		
		for(unsigned int i = 1 ; i < roadList.size() ; ++i){
			double testedRoadWidth = roadList[i].position?
					roadList[i].roadway->getEndingWidth():roadList[i].roadway->getStartingWidth();
			//if we detect a bigger road or a road of same width with more lines
			if((testedRoadWidth > chosenRoadWidth && fabs(testedRoadWidth-chosenRoadWidth) > EPSILON) || 
					(fabs(testedRoadWidth-chosenRoadWidth) < EPSILON && 
					 roadList[i].roadway->getNbLine() > roadList[chosenRoadIndex].roadway->getNbLine())){
				//update direction with the direction of the road
				chosenRoadWidth = testedRoadWidth;
				chosenRoadIndex = i;
				vbuffer = roadList[i].roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_READ_ONLY));
				if(roadList[i].position){
					unsigned int nbVertices = roadList[i].roadGeom->vertexData->vertexCount;
					direction.x = *(vdata+8*(nbVertices-1)) - *(vdata+8*(nbVertices-2));
					direction.z = *(vdata+8*(nbVertices-1)+2) - *(vdata+8*(nbVertices-2)+2);
				} else {
					direction.x = *(vdata) - *(vdata+8);
					direction.z = *(vdata+2) - *(vdata+10);
				}
				vbuffer->unlock();
			}
		}
		direction.normalise();
		Vector3 crossResult = Vector3::UNIT_X.crossProduct(direction);
		double theta;
		double dotResult = Vector3::UNIT_X.dotProduct(direction);
		//avoid floating point computation error
		dotResult = (dotResult>1)?1:dotResult;
		dotResult = (dotResult<-1)?-1:dotResult;
		
		//test CCW or CW using property of cross product
		//vertical coordinate > 0 => CCW
		if ((crossResult[1] != 0 && crossResult[1] > 0)){
			theta = acos(dotResult);
		} else {
			theta = -acos(dotResult);		
		}
		vector<Vector2> texCoords;
		double minU = DBL_MAX;
		double minV = DBL_MAX;
		for(unsigned int i = 0 ; i < outInter.size() ; ++i){
			//as we are computing in the x-z plane, we have positive x coordinates
			//in up direction and positive z coordinates in right direction
			//so use z coordinate for first texture coordinate and x coordinate 
			//for second texture coordinate
			
			//use a simple coordinate system rotation, from world coordinate system
			//to texture coordinate system which v axis is the direction computed before
			//and u axis is it's orthogonal in the x-z world plane
			texCoords.push_back(Vector2((sin(theta)*outInter[i][0] + cos(theta)*outInter[i][2])/chosenRoadWidth,
					(cos(theta)*outInter[i][0] - sin(theta)*outInter[i][2])/texLength));
			if(texCoords[i][0] < minU){
				minU = texCoords[i][0];
			}
			if(texCoords[i][1] < minV){
				minV = texCoords[i][1];
			}
		}
		for(unsigned int i = 0 ; i < texCoords.size() ; ++i){
			//translate texture coordinates
			texCoords[i][0] -= minU;
			texCoords[i][1] -= minV;
		}
		return texCoords;
	}
	
	void ComputeFactory::createIntersectionCenterSubmesh(MeshPtr intersectionsMesh, 
			const vector<Vec3d>& triangulatedVertices, const vector<Vec3i>& triangles,
			vector<Vector2>& texCoords, vector<Vector3>& outInter,
			AxisAlignedBox& aabox){
		vector<Vector2> mappedTexCoords;
		bool texCoordsHasBeenMapped = (outInter.size() >= triangulatedVertices.size());
		if(texCoordsHasBeenMapped){
			//size when tesselated > previous size means points have been added,
			//possibly because contour is intersecting himself, so we won't compute 
			//tex coords...for now.
			for(unsigned int i = 0 ; texCoordsHasBeenMapped && i < triangulatedVertices.size() ; ++i){
				//for each triangulated point, see if we can find the corresponding tex coord
				int j;
				texCoordsHasBeenMapped = false;
				for(j = 0 ; !texCoordsHasBeenMapped && j < texCoords.size() ; ++j){
					if(fabs(outInter[j][0] - triangulatedVertices[i][0]) < EPSILON &&
					   //don't forget Ogre is y-up whereas triangle is z-up
					   fabs(-outInter[j][2] - triangulatedVertices[i][1]) < EPSILON &&
					   fabs(outInter[j][1] - triangulatedVertices[i][2]) < EPSILON){
						//this triangulated vertex correspond to one of the original
						//road part geometry, map it with tex coords
						mappedTexCoords.push_back(texCoords[j]);
						texCoordsHasBeenMapped = true;
					}
				}
			}
		}
		
		//Create a submesh for this intersection center
		SubMesh* center = intersectionsMesh->createSubMesh();
	
		unsigned int nbVertices = triangulatedVertices.size();
		
		center->useSharedVertices = false;
	    
	    //Create the Vertex data
		center->vertexData = new VertexData();
		center->vertexData->vertexStart = 0;
		center->vertexData->vertexCount = nbVertices;
	    
	    //We must now declare what the vertex data contains
	    VertexDeclaration* declaration = center->vertexData->vertexDeclaration;
	    static const unsigned short source = 0;
	    size_t offset = 0;
	    
	    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_POSITION).getSize();
	    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_NORMAL).getSize();
	    offset += declaration->addElement(source,offset,VET_FLOAT2,VES_TEXTURE_COORDINATES).getSize();
	    
	    //Create a vertex buffer
	    HardwareVertexBufferSharedPtr vbuffer =
	    HardwareBufferManager::getSingletonPtr()->createVertexBuffer(declaration->getVertexSize(source), // == offset
	    		center->vertexData->vertexCount,   // == nbVertices
	            HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	    
	    //Lock the buffer for writing
	    float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_DISCARD));
	    for (unsigned int i=0;i<nbVertices;++i)
	    {
	    	//result of triangulation is z-up, don't forget to translate
	        // Position
	        Vector3 position(triangulatedVertices[i][0], triangulatedVertices[i][2], -triangulatedVertices[i][1]);
	        *vdata++ = position.x;
	        *vdata++ = position.y;
	        *vdata++ = position.z;
	        //Update the bounding box
	        aabox.merge(position);
	        // Normal
	        *vdata++ =  0.0;
	        *vdata++ =  1.0;
	        *vdata++ =  0.0;
	        if(texCoordsHasBeenMapped){
	        	//texture coordinates
	        	*vdata++ = mappedTexCoords[i][0];
	        	*vdata++ = mappedTexCoords[i][1];
	        } else {
	        	//if texCoords haven't been mapped, had dummy informations
	        	*vdata++ = 0;
	        	*vdata++ = 0;
	        }
	    }
	    
	    //Release the buffer
	    vbuffer->unlock();
	    
	    //Set the buffer binding
	    center->vertexData->vertexBufferBinding->setBinding(source,vbuffer);
	    
	    unsigned int nbTriangles = triangles.size();
	    
	    // Creates the index data
	    center->indexData->indexStart = 0;
	    center->indexData->indexCount = 3*nbTriangles;
	    center->indexData->indexBuffer =
	    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
	    			center->indexData->indexCount,
	                                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	    
	    //Lock the buffer
	    uint32* idata = static_cast<uint32*>(center->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
	    //Fill it
	    for(unsigned int i = 0 ; i < nbTriangles ; ++i)
	    {
	        *idata++ = (uint32)triangles[i][0];
	        *idata++ = (uint32)triangles[i][1];
	        *idata++ = (uint32)triangles[i][2];
	    }
	    
	    //And unlock it
	    center->indexData->indexBuffer->unlock();
	    
	    center->setMaterialName(_interMaterialName);
	}
	
	void ComputeFactory::createIntersectionRoadPartSubmesh(MeshPtr intersectionsMesh,
			interInfo& roadInfos,
			vector<Vector3>& outRoad,
			vector<Vector2>& texRoad,
			AxisAlignedBox& aabox){
		//triangulation
		vector<Vec3d> roadPartPolyline;
		for(unsigned int i = 0 ; i < outRoad.size() ; ++i){
			roadPartPolyline.push_back(Vec3d(outRoad[i][0], -outRoad[i][2], outRoad[i][1]));
		}
		
		Triangulator roadPartTriangulator(roadPartPolyline);
		roadPartTriangulator.compute();
		vector<Vector2> mappedTexCoords;
		vector<Vec3d> triangulatedVertices = roadPartTriangulator.getVertices();
		bool mapTexCoords = (roadPartPolyline.size() >= triangulatedVertices.size());
		if(mapTexCoords){
			//size when tesselated > previous size means points have been added,
			//possibly because contour is intersecting himself, so we won't compute 
			//tex coords...for now.
			for(unsigned int i = 0 ; mapTexCoords && i < triangulatedVertices.size() ; ++i){
				//for each triangulated point, see if we can find the corresponding tex coord
				int j;
				mapTexCoords = false;
				for(j = 0 ; !mapTexCoords && j < texRoad.size() ; ++j){
					if(fabs(outRoad[j][0] - triangulatedVertices[i][0]) < EPSILON &&
					   //don't forget Ogre is y-up whereas triangle is z-up
					   fabs(-outRoad[j][2] - triangulatedVertices[i][1]) < EPSILON &&
					   fabs(outRoad[j][1] - triangulatedVertices[i][2]) < EPSILON){
						//this tirangulated vertex correspond to one of the original
						//road part geometry, map it with tex coords
						mappedTexCoords.push_back(texRoad[j]);
						mapTexCoords = true;
					}
				}
			}
		}
		
		SubMesh* roadPart = intersectionsMesh->createSubMesh();
		
		unsigned int nbVertices = triangulatedVertices.size();
		
		roadPart->useSharedVertices = false;
		
		//create the vertex data
		roadPart->vertexData = new VertexData();
		roadPart->vertexData->vertexStart = 0;
		roadPart->vertexData->vertexCount = nbVertices;
		
		//We must declare what the vertex contains
		VertexDeclaration* declaration = roadPart->vertexData->vertexDeclaration;
		static const unsigned short source = 0;
		size_t offset = 0;
		
		offset += declaration->addElement(source, offset, VET_FLOAT3, VES_POSITION).getSize();
		offset += declaration->addElement(source, offset, VET_FLOAT3, VES_NORMAL).getSize();
		//if(mapTexCoords){
			offset += declaration->addElement(source, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES).getSize();
		//}
		
		//create a new vertex buffer for the road part
		HardwareVertexBufferSharedPtr vbuffer =
			HardwareBufferManager::getSingletonPtr()->createVertexBuffer(declaration->getVertexSize(source),
					roadPart->vertexData->vertexCount,
					HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		
		//lock the buffer for writing
		float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_DISCARD));
		for(unsigned int i = 0 ; i < nbVertices ; ++i){
			//result of triangulation is z-up, don't forget to translate
			//position
			Vector3 position(triangulatedVertices[i][0], triangulatedVertices[i][2], -triangulatedVertices[i][1]);
			*vdata++ = position.x;
			*vdata++ = position.y;
			*vdata++ = position.z;
			//update the bounding box
			aabox.merge(position);
			//normal
			*vdata++ = 0.0;
			*vdata++ = 1.0;
			*vdata++ = 0.0;
			if(mapTexCoords){
				*vdata++ = mappedTexCoords[i][0];
				*vdata++ = mappedTexCoords[i][1];
			} else {
	        	*vdata++ = 0;
	        	*vdata++ = 0;
	        }
		}
	    
	    //Release the buffer
	    vbuffer->unlock();
	    
	    //Set the buffer binding
	    roadPart->vertexData->vertexBufferBinding->setBinding(source,vbuffer);
	    
	    vector<Vec3i> triangulatedIndexes = roadPartTriangulator.getTriangles();
	    
	    unsigned int nbTriangles = triangulatedIndexes.size();
	    
	    // Creates the index data
	    roadPart->indexData->indexStart = 0;
	    roadPart->indexData->indexCount = 3*nbTriangles;
	    roadPart->indexData->indexBuffer =
	    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
	    			roadPart->indexData->indexCount,
	                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	    
	    //Lock the buffer
	    uint32* idata = static_cast<uint32*>(roadPart->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
	    
	    //Fill it
	    //cout << "indices : " << endl;
	    for(unsigned int i = 0 ; i < nbTriangles ; ++i)
	    {
	        *idata++ = (uint32)triangulatedIndexes[i][0];
	        *idata++ = (uint32)triangulatedIndexes[i][1];
	        *idata++ = (uint32)triangulatedIndexes[i][2];
	    }
	    //And unlock it
	    roadPart->indexData->indexBuffer->unlock();
	    
	    roadPart->setMaterialName(roadInfos.roadGeom->getMaterialName());
	}
	
	// recompute texture coordinates of the roads passed in parameter
	// as its extremities as been updated to fit the intersection, we can't avoid it
	void ComputeFactory::computeRoadTexCoords(SubMesh* roadGeom, double texLength) {
		//get vertex buffer of the road
		HardwareVertexBufferSharedPtr bufferPtr = roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
		
		float* vdata = static_cast<float*>(bufferPtr->lock(HardwareBuffer::HBL_NORMAL));
		
		//compute length of the road
		double roadLength = 0.0;
		for(unsigned int i = 0 ; i < bufferPtr->getNumVertices() ; i+=4){
			roadLength += ((Vector3(*(vdata+8*i), *(vdata+8*i+1), *(vdata+8*i+2)) - 
					        Vector3(*(vdata+8*(i+1)), *(vdata+8*(i+1)+1), *(vdata+8*(i+1)+2))).length() +
					       (Vector3(*(vdata+8*(i+2)), *(vdata+8*(i+2)+1), *(vdata+8*(i+2)+2)) - 
					    	Vector3(*(vdata+8*(i+3)), *(vdata+8*(i+3)+1), *(vdata+8*(i+3)+2))).length())/2;
		}
		
		//sometimes we get a null length, modify this
		roadLength = (roadLength>0)?roadLength:0.1;
		
		//compute the number of times we will repeat the texture
		double texNumber = floor(roadLength/texLength + 0.5);
		
		//if roadLength < texLength/2, we get 0 => adjust
		texNumber = (texNumber>0)?texNumber:1;
		
		//recompute texture coordinates
		double currentLength = 0.0;
		double previousLength = 0.0;
		//in our vertex declaration, texture coordinates are in 7th and 8th position
		for (unsigned int i=0; i<bufferPtr->getNumVertices(); i+=4) {
			//how much distance we've been through until now 
			currentLength += ((Vector3(*(vdata+8*i), *(vdata+8*i+1), *(vdata+8*i+2)) - 
							   Vector3(*(vdata+8*(i+1)), *(vdata+8*(i+1)+1), *(vdata+8*(i+1)+2))).length() +
							  (Vector3(*(vdata+8*(i+2)), *(vdata+8*(i+2)+1), *(vdata+8*(i+2)+2)) - 
							   Vector3(*(vdata+8*(i+3)), *(vdata+8*(i+3)+1), *(vdata+8*(i+3)+2))).length())/2;
			*(vdata+8*i+6) = 0;
			*(vdata+8*i+7) = previousLength*texNumber/roadLength;
			*(vdata+8*(i+1)+6) = 0;
			*(vdata+8*(i+1)+7) = currentLength*texNumber/roadLength;
			*(vdata+8*(i+2)+6) = 1;
			*(vdata+8*(i+2)+7) = previousLength*texNumber/roadLength;
			*(vdata+8*(i+3)+6) = 1;
			*(vdata+8*(i+3)+7) = currentLength*texNumber/roadLength;
			previousLength = currentLength;
		}
		
		bufferPtr->unlock();
	}
	
	void ComputeFactory::simplifyMesh(MeshPtr& mesh, vector<string> texturesNames){
		//Create a new mesh to hold the data
		ostringstream oss;
		oss << mesh->getName() << "summarized";
		MeshPtr summarizedMesh = MeshManager::getSingleton().createManual(oss.str().c_str(),"RoadGen");
		//a map to collect infos on submeshes we want to create to summarize
		//key is the name of the texture used for this submesh (submeshes will
		//be summarize by material
		//value is composed by a pair representing number of vertices and indexes
		//for the submesh we want to create, and by a vector of index of submesh to summarize
		map<string, pair< pair<int, int>, vector<int> > > submeshInfos;
		for(unsigned int i = 0 ; i < texturesNames.size() ; ++i){
			submeshInfos[texturesNames[i]] = make_pair(make_pair(0, 0), vector<int>());
		}
		//get number of vertices and indexes for each material used
		unsigned int submeshesToPop = mesh->getNumSubMeshes();
		for(unsigned int i = 0 ; i < submeshesToPop ; ++i){
			string materialName = mesh->getSubMesh(i)->getMaterialName();
			submeshInfos[materialName].first.first += mesh->getSubMesh(i)->vertexData->vertexCount;
			submeshInfos[materialName].first.second += mesh->getSubMesh(i)->indexData->indexCount;
			submeshInfos[materialName].second.push_back(i);
		}
		
		//create a submesh for each material
		map<string, pair< pair<int, int>, vector<int> > >::iterator iter;
		for(iter = submeshInfos.begin() ; iter != submeshInfos.end() ; ++iter){
			cout << "simplifying " << iter->first << endl;
			SubMesh* summarizedSubmesh = summarizedMesh->createSubMesh();
			
			summarizedSubmesh->useSharedVertices = false;
			
			summarizedSubmesh->vertexData = new VertexData();
			summarizedSubmesh->vertexData->vertexStart = 0;
			summarizedSubmesh->vertexData->vertexCount = iter->second.first.first;
			
			VertexDeclaration* declaration = summarizedSubmesh->vertexData->vertexDeclaration;
		    static const unsigned short source = 0;
		    size_t offset = 0;
		    
		    //Tell we want to have a position, a normal and texture coordinates for each vertex
		    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_POSITION).getSize();
		    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_NORMAL).getSize();
		    offset += declaration->addElement(source,offset,VET_FLOAT2,VES_TEXTURE_COORDINATES).getSize();
		    
		    //Create a vertex buffer
		    HardwareVertexBufferSharedPtr vbuffer =
		    HardwareBufferManager::getSingletonPtr()->createVertexBuffer(declaration->getVertexSize(source), // == offset
		                                 summarizedSubmesh->vertexData->vertexCount,   // == nbVertices
		                                 HardwareBuffer::HBU_STATIC_WRITE_ONLY,
		                                 true);

			int numOfVertices = summarizedSubmesh->vertexData->vertexCount;
		    
		    // Creates the index data
		    summarizedSubmesh->indexData->indexStart = 0;
		    summarizedSubmesh->indexData->indexCount = iter->second.first.second;
		    summarizedSubmesh->indexData->indexBuffer =
		    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
		    			summarizedSubmesh->indexData->indexCount,
		    			HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		    
			//Lock the buffers for writing
			float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_DISCARD));
		    uint32* idata = static_cast<uint32*>(summarizedSubmesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
			    
		    //fill the buffers
		    vector<int> indexSubmeshesToCopy = iter->second.second;
		    unsigned int verticesAdded = 0;
		    for(unsigned int i = 0 ; i < indexSubmeshesToCopy.size() ; ++i){
		    	int index = indexSubmeshesToCopy[i];
		    	SubMesh* submeshToCopy = mesh->getSubMesh(index);
		    	unsigned int nbVertexToCopy = submeshToCopy->vertexData->vertexCount;
		    	unsigned int nbIndexToCopy = submeshToCopy->indexData->indexCount;
		    	//get buffers of this submesh
		    	HardwareVertexBufferSharedPtr vbuffToCopy = submeshToCopy->vertexData->vertexBufferBinding->getBuffer(0);
		    	HardwareIndexBufferSharedPtr ibuffToCopy = submeshToCopy->indexData->indexBuffer;
		    	//lock this buffers for reading
		    	float* vdataToCopy = static_cast<float*>(vbuffToCopy->lock(HardwareBuffer::HBL_READ_ONLY));
		    	uint32* idataToCopy = static_cast<uint32*>(ibuffToCopy->lock(HardwareBuffer::HBL_READ_ONLY));
		    	
		    	for(unsigned int j = 0 ; j < nbVertexToCopy ; ++j){
		    		//copy position
		    		*vdata++ = *(vdataToCopy+8*j);
		    		*vdata++ = *(vdataToCopy+8*j+1);
		    		*vdata++ = *(vdataToCopy+8*j+2);
		    		//copy normal
		    		*vdata++ = *(vdataToCopy+8*j+3);
		    		*vdata++ = *(vdataToCopy+8*j+4);
		    		*vdata++ = *(vdataToCopy+8*j+5);
		    		//copy texture coordinates
		    		*vdata++ = *(vdataToCopy+8*j+6);
		    		*vdata++ = *(vdataToCopy+8*j+7);
		    	}
		    	
		    	for(unsigned int j = 0 ; j < nbIndexToCopy/3 ; ++j){
		    		*idata++ = *(idataToCopy+3*j) + verticesAdded;
		    		*idata++ = *(idataToCopy+3*j+1) + verticesAdded;
		    		*idata++ = *(idataToCopy+3*j+2) + verticesAdded;
		    	}
		    	
		    	verticesAdded += nbVertexToCopy;
		    	
		    	//release buffers
		    	vbuffToCopy->unlock();
		    	ibuffToCopy->unlock();
		    }
		    
		    //release summarized buffers
		    vbuffer->unlock();
		    summarizedSubmesh->indexData->indexBuffer->unlock();
		    
		    //Set the buffer binding
		    summarizedSubmesh->vertexData->vertexBufferBinding->setBinding(source,vbuffer);
	    	
	    	//add material
	    	summarizedSubmesh->setMaterialName(iter->first);
		}
		
		//set the bounding box of this new mesh
		AxisAlignedBox aabox = mesh->getBounds();
		summarizedMesh->_setBounds(aabox);
		summarizedMesh->_setBoundingSphereRadius((aabox.getMaximum()-aabox.getMinimum()).length()/2.0);
		
		//unload old mesh
		mesh->unload();
		
		//reassign pointer to new mesh
		mesh = summarizedMesh;
	}
	
	std::vector< OGRLineString* > ComputeFactory::createContours(vector<pair<vector<interInfo>, vector<roadIntersectionPoints> > >& computingInformations,
				int nbIntersections){
		std::vector< OGRLineString* > contours;
		cout << endl << "Computing contours...";
		
		//log file in case of errors due to malformed intersections
		ofstream out("logContours.txt");
		
		//redirect cerr on logfile
		streambuf* orig = std::cerr.rdbuf(out.rdbuf());
		bool computingError = false;
		for(unsigned int i = 0 ; i < nbIntersections ; ++i){
			for(unsigned int j = 0 ; j < computingInformations[i].first.size() ; ++j){
				//if the contour going through the left side (from the intersection's point of view)
				//of this road hasn't already been computed
				if(!((computingInformations[i].second)[j].leftContour)){
					OGRLineString* contour = computeContour(i, j, computingInformations);
					computingError = computingError || (contour == NULL);
					//contour == NULL means an error during computing
					if(contour != NULL){
						contours.push_back( contour );
					}
				}
			}
		}
		
		//redirect cerr to its origin
		cerr.rdbuf(orig);
		
		if(computingError){
			cout << "Error in computing, see file logContours.txt for details" << endl;
		}
		
		return contours;
	}
	
	OGRLineString* ComputeFactory::computeContour(unsigned int interIndex, unsigned int roadIndex, vector<pair<vector<interInfo>, vector<roadIntersectionPoints> > >& computingInformations){
		OGRLineString* contour = new OGRLineString();
		vector<Vector3> contourApproximation;
		unsigned int currentInter = interIndex;
		unsigned currentRoad = roadIndex;
		bool currentPosition;
		interInfo currentRoadInfo;
		//store informations of the first road before starting the loop
		//pointer as we want to update it later on
		roadIntersectionPoints* rIPCurrent = &((computingInformations[interIndex].second)[roadIndex]);
		//bool to know if we must stop the loop (in case of an error)
		bool continueLoop = true;
		
		do{
			if(rIPCurrent->leftContour == true){
				//we are going through an already visited intersection, meaning contour has crossed
				//a badly formed intersection
				cerr << "Error computing contour !" << endl
					 << "Approximation of the contour computed so far :" << endl
					 << "(malformation in one of the intersection of the appoximation could have lead to this error)" << endl;
				for(unsigned int i = 0 ; i < contourApproximation.size() ; ++i){
					cerr << contourApproximation[i].x << " " << contourApproximation[i].y << " " << contourApproximation[i].z << endl;
				}
				continueLoop = false;
				contour = NULL;
			} else {			
				//get information on the current road relative to the current intersection
				currentRoadInfo = (computingInformations[currentInter].first)[currentRoad];
				
				//as road geometries have been recomputed to fit intersections, it avoids us some tricky readjustments
				//get position of the road relative to the intersections
				currentPosition = currentRoadInfo.position;
				//get geometry of the road we are working on
				HardwareVertexBufferSharedPtr vbuffer = currentRoadInfo.roadGeom->vertexData->vertexBufferBinding->getBuffer(0);
				float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_READ_ONLY));
				unsigned int nbVertices = currentRoadInfo.roadGeom->vertexData->vertexCount;
				
				//add points of the road in the contour
				if(currentPosition){
					//push the first point of the road in the contour bounding box
					contourApproximation.push_back(Vector3(*(vdata+8*(nbVertices-1)), *(vdata+8*(nbVertices-1)+1), *(vdata+8*(nbVertices-1)+2)));
					for(int i = nbVertices-1 ; i > 0 ; i-=4){
						//warning : as we are starting at the end of the road here, the left side of this road
						//from the intersection's point of view is the right side of the road
						contour->addPoint( *(vdata+8*(i-1)), *(vdata+8*(i-1)+1), *(vdata+8*(i-1)+2 ) );
					}
					//add the end point of the road (same that left marking extremity), which is point with index 2
					contour->addPoint( *(vdata+16), *(vdata+17), *(vdata+18) );
					//now select the following intersection
					currentInter = currentRoadInfo.roadway->getIntersection(0);
				} else {
					//push the first point of the road in the contour bounding box
					contourApproximation.push_back(Vector3(*(vdata+8*(nbVertices-1)), *(vdata+8*(nbVertices-1)+1), *(vdata+8*(nbVertices-1)+2)));
				
					//add the start point of the road
					contour->addPoint( *(vdata), *(vdata+1), *(vdata+2) );
					for(unsigned int i = 1 ; i < nbVertices ; i+=4){
						//here, left side from intersection point of view is the road's real left side
						contour->addPoint( *(vdata+8*i), *(vdata+8*i+1), *(vdata+8*i+2) );
					}
					//no need to add endpoint as it's vertex (geom->size()-3) has been handled in the loop
					//now select the following intersection
					currentInter = currentRoadInfo.roadway->getIntersection(1);
				}
				//say this road has been gone through
				rIPCurrent->leftContour = true;
				//first find the current road in the following intersection
				if(computingInformations[currentInter].first.size() == 1){
					//there is only one road in the following intersection : it's a dead end
					//so simply use the other side of the road 
					//as it's a dead end, road index is 0
					currentRoad = 0;
					rIPCurrent = &((computingInformations[currentInter].second)[currentRoad]);
					//add the endpoints
					contour->addPoint(rIPCurrent->leftExtremity.x, rIPCurrent->leftExtremity.y, rIPCurrent->leftExtremity.z );
					contour->addPoint( rIPCurrent->rightExtremity.x, rIPCurrent->rightExtremity.y, rIPCurrent->rightExtremity.z );
				} else {
					//there is more than one road in the intersection, so first
					//find the index of the current road in the intersection,
					//and select next road in the intersection to proceed with the next loop
					unsigned int i = 0;
					bool found = false;
					for(; !found && i < computingInformations[currentInter].first.size() ;){
						if((computingInformations[currentInter].first)[i].roadway == currentRoadInfo.roadway){
							found = true;
						} else {
							++i;
						}
					}
					currentRoad = i;
					
					//add the points between current and next road
					//first add points of current road
					rIPCurrent = &((computingInformations[currentInter].second)[currentRoad]);
					
					//check if there is a projection on the left side of the road
					if(rIPCurrent->projSide == LEFT){
						//add projection
						contour->addPoint( rIPCurrent->projection.x, rIPCurrent->projection.y, rIPCurrent->projection.z );
						//add border points
						for(int j = rIPCurrent->sidePoints.size() - 1 ; j >= 0 ; --j){
							contour->addPoint( rIPCurrent->sidePoints[j].x, rIPCurrent->sidePoints[j].y, rIPCurrent->sidePoints[j].z );
						}
					}
					if(rIPCurrent->leftIntersection == ENDPOINT || rIPCurrent->leftIntersection == COMPLEX_INTERSECT){
						//there is no common point between current and next road
					} else if(rIPCurrent->leftIntersection == HIGHWAY){
						//if the left intersection is a highway slip, projection has been done on the left
						//so border points have already been handled
						//add the highway intersect
						contour->addPoint( rIPCurrent->highwayPoint.x, rIPCurrent->highwayPoint.y, rIPCurrent->highwayPoint.z );
						//add highway border points 
						for(unsigned int j = 0 ; j < rIPCurrent->highwaySidePoints.size() ; ++j){
						contour->addPoint( rIPCurrent->highwaySidePoints[j].x, rIPCurrent->highwaySidePoints[j].y, rIPCurrent->highwaySidePoints[j].z );
						}
					}
					//add left extremity
					contour->addPoint( rIPCurrent->leftExtremity.x, rIPCurrent->leftExtremity.y, rIPCurrent->leftExtremity.z );
					
					//warning : don't tell the rIP between current and next we've gone through, as it will be used 
					//for other side of the road
					
					//get index of the next road
					//warning : as we are taking left sides of roads, the next road is taken
					//in clockwise order, but in roadlists, they are sorted counter clockwise
					currentRoad = (i+computingInformations[currentInter].first.size()-1)%computingInformations[currentInter].first.size();
					//now select next road to had its point and have it as current road for next iteration of the loop
					rIPCurrent = &((computingInformations[currentInter].second)[currentRoad]);
					
					//add rightExtremity
					contour->addPoint( rIPCurrent->rightExtremity.x, rIPCurrent->rightExtremity.y, rIPCurrent->rightExtremity.z );
					
					//check if the next road is an highway slip
					if(rIPCurrent->rightIntersection == HIGHWAY){
						//add highway border points
						for(int j = rIPCurrent->highwaySidePoints.size() - 1 ; j >= 0 ; --j){
							contour->addPoint( rIPCurrent->highwaySidePoints[j].x, rIPCurrent->highwaySidePoints[j].y, rIPCurrent->highwaySidePoints[j].z );
						}
						//add the highway intersect
						contour->addPoint( rIPCurrent->highwayPoint.x, rIPCurrent->highwayPoint.y, rIPCurrent->highwayPoint.z );
						//border points will be handled later
					}
					
					//check if projection is on the right side
					if(rIPCurrent->projSide == RIGHT){
						//add border points 
						for(int j = rIPCurrent->sidePoints.size() - 1 ; j >= 0 ; --j){
							contour->addPoint( rIPCurrent->sidePoints[j].x, rIPCurrent->sidePoints[j].y, rIPCurrent->sidePoints[j].z );
						}
						//add projection		
						contour->addPoint( rIPCurrent->projection.x, rIPCurrent->projection.y, rIPCurrent->projection.z );		
					}
				}
			}
		} while(continueLoop && (interIndex != currentInter || currentRoad != roadIndex));
		if(continueLoop){
			//add the first point of the contour without forcing to close it
			OGRPoint pt;
			contour->getPoint( 0, &pt );
			contour->addPoint( &pt );
		}
		return contour;
	}
	
} //namespace roge
