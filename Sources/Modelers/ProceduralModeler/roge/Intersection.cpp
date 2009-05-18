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

#include "Intersection.h"

namespace roge {
	
	Intersection::Intersection() : _isComplex(false), _isMultiMerged(false){
	}
	
	
	Intersection::Intersection(const Intersection& right) {
	  *this = right;
	}
	
	Intersection::Intersection(vector<pair<unsigned int,bool> > right) {
	  _roadWayList = right;
	}
	
	Intersection::~Intersection() {
	}
	
	vector<pair<unsigned int,bool> > Intersection::getRoadWayList() const{
	    return _roadWayList;
	}
	
	void Intersection::setRoadWayList(vector<pair<unsigned int,bool> >& list) {
	  _roadWayList.clear();
	  for(unsigned int i=0; i<list.size(); i++) {
	    _roadWayList.push_back(list[i]);
	  }
	}
	
	
	void Intersection::addRoadWay(pair<int,bool>& right) {
	  _roadWayList.push_back(right);
	}
	
	void Intersection::remRoadWay(int number) {
	  _roadWayList.erase(_roadWayList.begin()+number);
	}
	
	void Intersection::setRoadWay(int index, int indexRoadway) {
	  _roadWayList[index].first = indexRoadway;
	}
	
	Intersection& Intersection::operator=(const Intersection& right) {
	  _isComplex = right._isComplex;
	  _roadWayList.clear();
	  _center = right._center;
	  for(unsigned int i=0; i<right.getRoadWayList().size(); i++) {
	    _roadWayList.push_back(right.getRoadWayList()[i]);
	  }
	  for(int i = 0 ; i < 4 ; ++i){
		  _deletedRoadBoundingRectangle[i] = right._deletedRoadBoundingRectangle[i];
	  }
	  
	  return *this;
	}
		
	bool Intersection::operator==(const Intersection& right) {
	  if(_isComplex == right._isComplex && _center == right._center) {
	    if(_roadWayList.size() == right.getRoadWayList().size()) {
	      unsigned int i=0;
	      
	      bool found = true;
	      while(found && i<_roadWayList.size()) {
		found = (_roadWayList[i] == right._roadWayList[i]);
		i++;
	      }
	      return found;
	    }
	  }
	  return false;
	}
	
	bool Intersection::operator!=(const Intersection& right) {
	  return !(*this == right);
	}
	
	void Intersection::setDeletedRoad(Vec3<double> p1, Vec3<double> p2, Vec3<double> p3, Vec3<double> p4){
		_deletedRoadBoundingRectangle[0] = p1;
		_deletedRoadBoundingRectangle[1] = p2;
		_deletedRoadBoundingRectangle[2] = p3;
		_deletedRoadBoundingRectangle[3] = p4;
	}

} // of roge namespace
