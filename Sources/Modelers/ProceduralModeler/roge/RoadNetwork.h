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

#ifndef _RoadNetwork_h_
#define _RoadNetwork_h_ 1

#include <vector>

#include <ogr_core.h>
#include <ogr_feature.h>
#include <ogrsf_frmts.h>

#include "BSpline/BSpline.h"

#include "trax/GeomBuilder.h"

#include "Config.h"
#include "RoadWay.h"
#include "Intersection.h"

namespace roge {
	
	/*!
	 * class representing  a road network with intersections
	 */
	class RoadNetwork
	{
	  public:
	
	
	  /*!Default constructor
	   */
	  RoadNetwork();
	
	
	  /*!Copy constructor
	   */
	  RoadNetwork(const RoadNetwork& right);
	
	  /*!Constructor
	   *@param linestrings
	   * The network gets its informations from features. 
	   */
	  RoadNetwork( const std::vector< pm_trax::GeomBuilder::Feature* >& features );
		
	  virtual ~RoadNetwork();
	
	
	  /*! Adds a new network to current one
	    @param linestrings
	    @sa RoadNetwork(const std::vector< OGRFeature* > >& features )
	   */
	  void addNetwork( const std::vector< pm_trax::GeomBuilder::Feature* >& features );
	
	  /*!
	  *Adds a roadway to the network
	  *@param rw pointer on the roadway to add
	  */
	  void addRoadWay(const RoadWay& rw);

	  /*!
	  *Remove a roadway from the network
	  *@param rw a pointer on the roadway to remove
	  *@return true if the roadway was found in the vector
	  */
	  bool remRoadWay(const RoadWay& rw);
	
	  /*!
	   *Adds an Intersection to the network
	   *@param inter pointer on the intersection to add
	   */
	  void addIntersection(const Intersection& inter);
	
	  /*!
	   *Remove an intersection from the network
	   *Warning: also remove all roadways related to this intersection
	   *@param inter a pointer on the intersection to remove
	   *@return true if the intersection was found in the vector
	   */
	  bool remIntersection(const Intersection& inter);
	
	
	  
	  /*! compute distance of the segment to a point
	   */
	  double distanceSegmentToPoint(Vec3<double>&, Vec3<double>&, Vec3<double>&);
	
	  /*!
	   * tests if an axis of a road intersects the ends of another road
	   * if true, the two intersections should be merged to avoid problems
	   *@param lineP1 first point of the segment of the axis of the road
	   *@param lineP2 the other point
	   *@param endNormalPt1 point of the end of the road to test with
	   *@param endNormalPt2 the second point of the end
	   *@return true if there is intersection
	   */
	  bool intersectEndRoadNormal(Vec3<double>& lineP1, 
				      Vec3<double>& lineP2,
				      double& width,
				      Vec3<double>& endNormalPt1,
				      Vec3<double>& endNormalPt2);
	
	  /*!
	   * merge intersections
	   *@param index1 index of the first intersection
	   *@param index2 index of the second intersection
	   *@param roadIndex ndex of the road to erase while merging
	   */
	  void mergeIntersections(const unsigned int& index1, const unsigned int& index2, const unsigned int& roadIndex);
	
	  /*!create all the intersections from the roadway list
	   *
	   */
	  void createIntersections();
	
	  /*!
	  *Gets the list of roadway
	  @return the roadway list
	  */
	  vector<RoadWay>& getRoadWayList();
	
	  RoadWay getRoadWay(int index) { return _roadWaysList[index]; };
	
	  RoadWay* getRoadWayPtr(int index) { return &(_roadWaysList[index]); };
	
	  int getNumRoadWays() { return _roadWaysList.size(); };
	
	  /*!
	  *Get the list of intersection
	  *@return the intersections list
	  */
	  vector<Intersection>& getIntersectionsList();
	
	  Intersection getIntersection(int index) { return _intersectionsList[index]; };
	
	  /*!assignement operator
	   *
	   */
	  RoadNetwork& operator=(const RoadNetwork& right);
		
	  /*!Verify the network
	   *@return true if network is usable, else returns false
	   */
	  bool verifyNetwork();
	
	  /*! translate axis of roads
	    @param linestrings linestrings to be translated
	    @param translation translation vector(default is written in configuration file)
	  */
	  void translateContour( std::vector< OGRFeature* >& features, const Vec3<double>& translation );
	
	  /*! verify integrity of node
	    @param root node to verify
	   */
	  void checkNode( const std::vector< OGRFeature* >& features );
	
	  inline void setDataSource(Config::DataSource dataSource) { _dataSource = dataSource; }
	  
	  inline Config::DataSource getDataSource() { return _dataSource; }
	  
	  Vec3<double> getTranslation(){return _translation;}
	
	  /*! project roads on a given dtm if their data seems corrupted (z=0)
	    @param root node to project
	    @param dtm Dtm file to project on
	  */
	  //void projectContours(arFbNode* root, arFbDtm* dtm);
	
	 protected:
	
	  Config::DataSource _dataSource;
		
	  vector< RoadWay > _roadWaysList; // Contains all the roadways
	
	  vector< Intersection > _intersectionsList; //Contains all intersections AND all extremities of roadways
	  
	  Vec3d _translation;

	  OGRDataSource* m_datasource;

  	  OGRLayer* m_layer;

	};

} // namespace roge

#endif /* _RoadNetwork_h_ */
