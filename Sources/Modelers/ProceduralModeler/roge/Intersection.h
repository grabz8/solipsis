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

#ifndef _Intersection_h_
#define _Intersection_h_ 1

#include <vector>

#include "Linear/Linear.h"

using namespace std;

namespace roge {
	
	/*! \brief class representing an interection between multiple roadways
	 */
	
	class Intersection
	{
	 public:
	//   typedef enum format {ORDINAIRE, PONT} type;
	
	  Intersection();
	  
	  Intersection(const Intersection& right);
	
	  Intersection(vector<pair<unsigned int,bool> > right);
	
	  ~Intersection();
	
	  /*!Gets the whole roadway list
	   *@return
	   */
	  vector<pair<unsigned int,bool> > getRoadWayList() const;
	
	  void setRoadWayList(vector<pair<unsigned int,bool> >& list);
	  
	  /*! Adds a roadway to the roadway list
	   *@param left Roadway to add
	   */
	  void addRoadWay(pair<int,bool> & right);
	
	  /*! Removes a roadway from the list
	   *@param number place of the roadway in the list
	   */
	  void remRoadWay(int number);
	
	  void setRoadWay(int index, int indexRoadway);
	
	  pair<int,bool> getRoadWay(int index) { return _roadWayList[index];};
	
	  int getNumRoadWays() { return _roadWayList.size(); };
	
	  Intersection& operator=(const Intersection& right);

	  bool operator==(const Intersection& right);
	
	  bool operator!=(const Intersection& right);
	
	  void setComplexity(bool comp) { _isComplex = comp; };
	  void setMultiMerging(bool multi){_isMultiMerged = multi;}
	
	  bool isComplex() { return _isComplex; };
	  bool isMultiMerged(){return _isMultiMerged;}
	  
	  void setDeletedRoad(Vec3<double> p1, Vec3<double> p2, Vec3<double> p3, Vec3<double> p4);
	  
	  Vec3<double> getDeletedRoadPoint(unsigned int i){return _deletedRoadBoundingRectangle[i];}
	  
	  Vec3<double> getCenter(){return _center;}
	  
	  void setCenter(Vec3<double> center){_center = center;}
	
	 private:
	
	  vector<pair<unsigned int,bool> > _roadWayList; //first: index of the road in the network
	                                        //second: side of the road (begin or end)
	  
	  bool _isComplex;
	  bool _isMultiMerged;
	  
	  Vec3<double> _deletedRoadBoundingRectangle[4];
	  
	  Vec3<double> _center;
	  
	};

} // of namespace roge

#endif /* Intersection_h_ */
