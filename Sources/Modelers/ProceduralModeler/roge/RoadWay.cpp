#include "RoadWay.h"

#include "Pavement.h"
#include "LineWay.h"

namespace roge {
	
	using namespace std;
	
	RoadWay::RoadWay() :
		_intersections(make_pair(INTERSECTION_NULL, INTERSECTION_NULL)),
				_differentWidths(false) {
	}
	
	RoadWay::RoadWay(const RoadWay& right) {
		*this = right;
	}
	
	RoadWay::~RoadWay() {
		//pavements are deleted by a call to deletePavements
	}
	
	string RoadWay::getName() const {
		return _name;
	}
	
	void RoadWay::setName(string name) {
		_name = name;
	}
	
	unsigned int RoadWay::getIntersection(unsigned int number) const {
		if (number == 0) {
			return _intersections.first;
		}
		if (number == 1) {
			return _intersections.second;
		} else {
			cerr << "number not in range" << endl;
			return 0;
		}
	}
	
	void RoadWay::setIntersection(unsigned int interIndex, unsigned int number) {
		if (number == 0) {
			_intersections.first = interIndex;
		}
		if (number == 1) {
			_intersections.second = interIndex;
		}
	}
	
	RoadWay& RoadWay::operator=(const RoadWay& right) {
		_name = right._name;
		_nature = right._nature;
		
		_nbLine = right._nbLine;
	
		_pavements.clear();
		for(unsigned int i=0; i<right._pavements.size();i++) {
			_pavements.push_back(right._pavements[i]);
		}
	
		_intersections.first = right._intersections.first;
		_intersections.second = right._intersections.second;
	
		_texLength = right._texLength;
	
		_differentWidths = right._differentWidths;
		return *this;
	}
	
	bool RoadWay::operator==(const RoadWay& right) {
		if(_nature == right.getNature() &&  _differentWidths == right._differentWidths) {
			if(_name == right.getName()) {
				// test if the two roadways have the same axis
				if(_pavements.size() == right._pavements.size()) {
					vector<Vec3<double> > axis1 = _pavements[0]->getAxis();
					vector<Vec3<double> > axis2 = right._pavements[0]->getAxis();
					if(axis1[0] == axis2[0]) {
						unsigned int vertexNum = axis2.size();
						if(vertexNum == axis1.size()) {
							if(axis1[vertexNum-1] == axis2[vertexNum-1]) {
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	
	Pavement* RoadWay::getPavement(int number) const {
		return _pavements[number];
	}
	
	void RoadWay::addPavement(Pavement* right) {
		_pavements.push_back(right);
	}
	
	void RoadWay::remPavement(int number) {
		delete _pavements[number];
		_pavements.erase(_pavements.begin()+number);
	}
	
	void RoadWay::deletePavements(){
		for(unsigned int i = 0 ; i < _pavements.size() ; ++i){
			_pavements[i]->deleteLines();
			delete _pavements[i];
		}
	}
	
	unsigned int RoadWay::getPavementNum() const {
		return _pavements.size();
	}
	
	string RoadWay::getNature() const {
		return _nature;
	}
	
	void RoadWay::setNature(string nature) {
		_nature = nature;
	}
	
	double RoadWay::getStartingWidth() const {
		unsigned int nbLine = getPavement(0)->getLineList().size();
		double width = 0;
	
		for (unsigned int j=0; j<nbLine; j++) {
			width+=getPavement(0)->getLine(j)->getStartingWidth();
		}
		return width;
	}
	
	double RoadWay::getEndingWidth() const {
		unsigned int nbLine = getPavement(0)->getLineList().size();
		double width = 0;
	
		for (unsigned int j=0; j<nbLine; j++) {
			width+=getPavement(0)->getLine(j)->getEndingWidth();
		}
		return width;
	}

} //namespace arSolipsisRoadGenerator
