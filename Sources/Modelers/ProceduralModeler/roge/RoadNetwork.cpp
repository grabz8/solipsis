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

#include <algorithm>

#include "RoadNetwork.h"
#include "Pavement.h"
#include "LineWay.h"

namespace roge {

	#define INTERSECTION_NULL 99999999
	#define ROADWAY_NULL 99999999
	
	float toleranceValue = 0.5;
	#define EPSILON 0.001
	
	class sort_inter
	{
	 public:
	  sort_inter(RoadWay r, int i, bool s): rw(r),index(i), side(s) {};
	
	  RoadWay rw;
	  int index;
	  bool side;
	};
	
	
	class sort_inter_merged
	{
	 public:
	  sort_inter_merged(RoadWay r, int i, bool s, Vec3<double> c, double rad): rw(r),index(i), side(s), center(c), radius(rad){};
	
	  RoadWay rw;
	  int index;
	  bool side;
	  Vec3<double> center;
	  double radius;
	};
	
	double calcule_angle(Vec3<double> v1, Vec3<double> v2) {
		v1.normalize();
		v2.normalize();
		Vec3<double> cross = v1.cross(v2);
		double angle;
		double v = v1.dot(v2);
		//as we are using floating point computing, v can be a little more than 1
		//or a little less than -1, not even noticeable using cout,
		//waiting in the darkness to corrupt our poor network...
		//so acos(v) will lead to a NaN fault
		v = (v>1)?1:v;
		v = (v<-1)?-1:v;
		
		if ((cross[1] != 0 && cross[1] > 0)){
			angle = acos(v);
		} else {
			angle = -acos(v);		
		}
		return angle;
	}
	
	struct less_angle_inter : public binary_function<sort_inter, sort_inter, bool> {
		bool operator()(sort_inter r1, sort_inter r2) {
			Vec3<double> ref(1, 0, 0);
			Vec3<double> v1, v2;
			vector<Vec3<double> > axis1 = r1.rw.getPavement(0)->getAxis();
			if (r1.side) {
				v1 = axis1[axis1.size()-1] - axis1[axis1.size()-2];
			} else {
				v1 = axis1[0] - axis1[1];
			}
			vector<Vec3<double> > axis2 = r2.rw.getPavement(0)->getAxis();
			if (r2.side) {
				v2 = axis2[axis2.size()-1] - axis2[axis2.size()-2];
			} else {
				v2 = axis2[0] - axis2[1];
			}
			return calcule_angle(ref, v1) < calcule_angle(ref, v2);
		}
	};
	
	struct less_angle_inter_merged : public binary_function<sort_inter_merged, sort_inter_merged, bool> {
		bool operator()(sort_inter_merged r1, sort_inter_merged r2) {
			Vec3<double> ref(1, 0, 0);
			Vec3<double> v1, v2;
			if (r1.side) {
				v1 = r1.rw.getPavement(0)->getAxis()[(r1.rw.getPavement(0)->getAxis()).size()-2];
			} else {
				v1 = r1.rw.getPavement(0)->getAxis()[1];
			}
			if (r2.side) {
				v2 = r2.rw.getPavement(0)->getAxis()[(r2.rw.getPavement(0)->getAxis()).size()-2];
			} else {
				v2 = r2.rw.getPavement(0)->getAxis()[1];
			}
			v1 -= r1.center;
			v2 -= r2.center;
			return calcule_angle(ref, v1) < calcule_angle(ref, v2);
		}
	};
	
	RoadNetwork::RoadNetwork() 
		: _dataSource( Config::UNDEFINED /* Config::IGN_BDTOPO_SOURCE */ ), m_datasource( NULL ), m_layer( NULL )
	{
	
		_roadWaysList.clear();

		std::vector< OGRFeature* > features;
		int nbSources = Config::instance()->getSourceFileNum();
	    OGRRegisterAll();
	
		for (int i = 0; i < nbSources; i++) {

			// Use the OGR lib read the source file
			std::string ogrFilename( "" );
			Config::DataSource dataSource;
			Config::instance()->getSourceFileName( ogrFilename, dataSource, i );
			setDataSource( dataSource );

		    OGRDataSource* poDS = NULL;
		    poDS = OGRSFDriverRegistrar::Open( ogrFilename.c_str(), FALSE );
		    if( poDS == NULL ) {
		        printf( "Open failed.\n%s" );
		        exit( 1 );
		    }
			m_datasource = poDS;

			OGRLayer  *poLayer;
			std::string filename = ogrFilename.substr( ogrFilename.rfind( '/' )+1 );
			std::string layername = filename.substr( 0, filename.size()-4 );
	    	poLayer = poDS->GetLayerByName( layername.c_str() );
	    	if( poLayer == NULL ) {
				std::cerr << "No such layer: " << layername << std::endl;
	            exit( 1 );
			}
			m_layer = poLayer;

		    OGRFeature* poFeature = NULL;
		    poLayer->ResetReading();
		    while( (poFeature = poLayer->GetNextFeature()) != NULL )
				features.push_back( poFeature );

			//    projectContours( root.get(), dynamic_cast<arFbDtm*>(dtmRoot->getChild(0)) );
			Config::instance()->getTranslation( _translation );
			//	translateContour(root.get(), _translation);
	
			//addNetwork( features );

		}
	}
	
	RoadNetwork::RoadNetwork(const RoadNetwork& right) {
		*this = right;
	}
	
	RoadNetwork::RoadNetwork( const std::vector< pm_trax::GeomBuilder::Feature* >& features ) 
		: _dataSource( Config::UNDEFINED /* Config::IGN_BDTOPO_SOURCE */ ), m_datasource( NULL ), m_layer( NULL )
	{
		_roadWaysList.clear();
		addNetwork( features );
	}
	
	RoadNetwork::~RoadNetwork() 
	{
		//to avoid an in-depth copy of pavements each time a roadway is copied,
		//destruction of pavements is not handled in roadway's destructor
		//so we delete them when destructing the network
		for(unsigned int i = 0 ; i < _roadWaysList.size() ; ++i){
			_roadWaysList[i].deletePavements();
		}

		if( m_datasource != NULL ) {
			OGRDataSource::DestroyDataSource( m_datasource );
		}
	}
	
	void RoadNetwork::translateContour( std::vector< OGRFeature* >& features, const Vec3<double>& translation ) 
	{
		//translate each contour in the network
		for( int i = 0; i < features.size(); i++ ) {
			OGRFeature* f = features[i];
			OGRLineString* ls = static_cast< OGRLineString* >( f->GetGeometryRef() );
			for( int j = 0; j < ls->getNumPoints(); j++ ) {
				OGRPoint pt;
				ls->getPoint( j, &pt );
				pt.setX( pt.getX()+translation[0] ); pt.setY( pt.getY()+translation[1] ); pt.setZ( pt.getZ()+translation[2] );
				ls->setPoint( j, pt.getX(), pt.getY(), pt.getZ() );
			}
		}
	}
	
	void RoadNetwork::addNetwork( const std::vector< pm_trax::GeomBuilder::Feature* >& features ) 
	{
		if( features.empty() ) {
			std::cout << "Empty network" << std::endl;
			return;
		}
	
		unsigned int numOfFeatures = features.size();
		
		//loop on all the children of root
		for( unsigned int i = 0; i < numOfFeatures; i++ ) {

			pm_trax::GeomBuilder::Feature* f = features[i];

			//only one road
			RoadWay* road = new RoadWay();
			Pavement* pavement = new Pavement();
	
			//switching on the source type, choose the best way to translate data.
			switch (getDataSource()) {
			case Config::UNDEFINED: {
				road->setName( f->m_name );
				road->setNature( f->m_nature );
				int numberOfWays = 2; // f->m_weight;
				road->setNbLine( numberOfWays );
				std::string dummy;
				double texLength = 3.0;
				road->setTexLength( texLength );
	
				for( int j = 0; j < numberOfWays; j++ ) {
					//create ways for the road
					LineWay *line = new LineWay();
					double laeur = 0.0; // f->GetFieldAsDouble( f->GetFieldIndex( "LAEUR" ) );
					double laeurfin = 0.0; // f->GetFieldAsDouble( f->GetFieldIndex( "LAEURFIN" ) );
					if( laeur > 0 ) {
						//if width is defined, get it
						line->setStartingWidth(laeur/numberOfWays);
						line->setEndingWidth(laeur/numberOfWays);
						//check if ending width is defined
						if(laeurfin > 0){
							//set ending width of the line
							line->setEndingWidth(laeurfin/numberOfWays);
						}
					} 
					else {
						//else check the nature of the road, width is defined in the xml file for a simple path
						if( road->getNature() == "Chemin" )
							line->setStartingWidth( Config::instance()->getWidth("CHEMIN")/numberOfWays );
						else {
							//if it's not a simple path and there is no width,
							//take the default value in the file regarding the number of ways
							switch (numberOfWays) {
								case 1: {
									line->setStartingWidth( Config::instance()->getWidth( "x1" )/numberOfWays );
									line->setEndingWidth( Config::instance()->getWidth( "x1" )/numberOfWays );
									break;
								}
								case 2: {
									line->setStartingWidth(Config::instance()->getWidth("x2")/numberOfWays);
									line->setEndingWidth(Config::instance()->getWidth("x2")/numberOfWays);
									break;
								}
								case 3: {
									line->setStartingWidth(Config::instance()->getWidth("x3")/numberOfWays);
									line->setEndingWidth(Config::instance()->getWidth("x3")/numberOfWays);
									break;
								}
								case 4: {
									line->setStartingWidth(Config::instance()->getWidth("x4")/numberOfWays);
									line->setEndingWidth(Config::instance()->getWidth("x4")/numberOfWays);
									break;
								}
								default: {
									line->setStartingWidth(Config::instance()->getWidth("DEFAULT")/numberOfWays);
									line->setEndingWidth(Config::instance()->getWidth("DEFAULT")/numberOfWays);
									break;
								}
							}
						}
					}
					//add the created line
					pavement->addLine( line );
				}
				break;
			}
			
			default:
				break;
		}
	
		//smoothen the road, there axis are not generally defined with
		//enough points
		OGRLineString* lineString = f->toLineString();
		std::vector< Vec3< double > > vertices;
		for( int j = 0; j < lineString->getNumPoints(); j++ ) {
			OGRPoint pt;
			lineString->getPoint( j, &pt );
			vertices.push_back( Vec3< double >( pt.getX(), pt.getY(), pt.getZ() ) );
		}

		double smoothness;
		Config::instance()->getSmoothness( smoothness );
		if( smoothness != 0.0 )
			smoothness = 1/smoothness;
		else
			smoothness = 1.0;

		BSpline spline( vertices, smoothness, false );
		spline.setThroughEndPoints( true );
		vector< Vec3< double > >* splinePoints = spline.compute();
							
		// GIS data is z-up whereas Ogre is y-up
		//so translate
		for( unsigned int j = 0 ; j < splinePoints->size() ; ++j )
			(*splinePoints)[j].toYup();
						
		//set the axis of the road with the smoothened line
		pavement->setAxis( *splinePoints );
	
		if (lineString->getNumPoints() > 1) {	
			//finally add the road to the network
			road->addPavement(pavement);
							
			if(fabs(road->getStartingWidth() - road->getEndingWidth()) > EPSILON){
				road->setDifferentWidth(true);
			}
							
			addRoadWay(*road);
		}
						
		//don't delete pavement as it is owned as a pointer by the copy of the roadway
		delete road;
		
		}

		std::cout << getNumRoadWays() << " roadways found." << std::endl;

	}
	
	//Add a roadway to the network
	void RoadNetwork::addRoadWay(const RoadWay& rw) {
		_roadWaysList.push_back(rw);
	}
	
	//remove a roadway from the network
	//return true if found
	bool RoadNetwork::remRoadWay(const RoadWay& rw) {
		//find the right roadway in the vector and delete it
		for (vector<RoadWay>::iterator it1=_roadWaysList.begin(); it1
				!= _roadWaysList.end(); it1++) {
	
			if (*it1 == rw) {
				_roadWaysList.erase(it1); //delete the roadway in the network
				return true;
			}
		}
	
		return false; // couldn't find the roadway in the network!
	
	}
	
	//add an intersection to the network
	void RoadNetwork::addIntersection(const Intersection& inter) {
		_intersectionsList.push_back(inter);
	}
	
	//remove an intersection to the network
	//deletes also all the roadways related to the intersection
	//return true if the intersection was found in the vector
	bool RoadNetwork::remIntersection(const Intersection& inter) {
		//find the right intersection in the vector adn delete it
		for (vector<Intersection>::iterator interIt = _intersectionsList.begin(); interIt
				!= _intersectionsList.end(); interIt++) {
			if ((*interIt) == inter) { //the intersection was found
	
				//delete all the roadways related to this intersection
				vector<pair<unsigned int,bool> > rwPtrList =
						interIt->getRoadWayList();
				vector<pair<unsigned int,bool> >::iterator rwIt = rwPtrList.begin();
				while (rwIt != rwPtrList.end()) {
					remRoadWay(_roadWaysList[rwIt->first]); //remove roadway from the network
					rwIt = rwPtrList.erase(rwIt); //erase returns the next iterator
				}
	
				_intersectionsList.erase(interIt); //remove intersection from the network
				return true;
			}
		}
		return false;
	}
	
	void RoadNetwork::createIntersections() {
		_intersectionsList.clear();
	
		unsigned int nbRoadways = _roadWaysList.size();
	
		// true if the first vertices of the roadway belong to an intersection 
		bool inter1created;
		// true if the last vertices of the roadway belong to an intersection
		bool inter2created;
		
		for (unsigned int i = 0; i < nbRoadways; i++) {
	
			inter1created = false;
			inter2created = false;
	
			Vec3<double> axisBegin = _roadWaysList[i].getPavement(0)->getAxis()[0];
			int axisSize = _roadWaysList[i].getPavement(0)->getAxis().size();
			Vec3<double> axisEnd = _roadWaysList[i].getPavement(0)->getAxis()[axisSize-1];
	
			//if the intersection at the beginning of the road hasn't been created yet
			if (_roadWaysList[i].getIntersection(0) == INTERSECTION_NULL) {
				Intersection intersection;
				//add information on the current road 
				pair<int, bool> info(i, false);
				intersection.addRoadWay(info);
				
				//set center of the intersection
				intersection.setCenter(axisBegin);
				
				//for all roads in the network
				for (unsigned int j = i; j < nbRoadways; j++) {
					//avoid constructing a temporary variable and use directly the function
					//less readable, but big performance gain
					//vector<Vec3<double> > axis2 = _roadWaysList[j].getPavement(0)->getAxis();
	
					if (j != i) {
						//if the beginning of the road j is near the current road
						if (axisBegin.equals(_roadWaysList[j].getPavement(0)->getAxis()[0], toleranceValue)) {
							info = pair<int, bool>(j, false);
							intersection.addRoadWay(info);
						}
					}
					//if the end of the road j is near the current road
					if (axisBegin.equals(_roadWaysList[j].getPavement(0)->getAxis()[_roadWaysList[j].getPavement(0)->getAxis().size()-1], toleranceValue)) {
						info = pair<int, bool>(j, true);
						intersection.addRoadWay(info);
					}
				}
	
				// we sort the roads in counter clockwise order
				// for easier use later
				vector<pair<unsigned int, bool> > rwList =
						intersection.getRoadWayList();
				vector<sort_inter> sortList;
				for (unsigned int l = 0; l < rwList.size(); l++) {
					sortList.push_back(sort_inter(_roadWaysList[rwList[l].first],
							rwList[l].first, rwList[l].second));
				}
				std::sort(sortList.begin(), sortList.end(), less_angle_inter());
				rwList.clear();
				for (unsigned int l=0; l<sortList.size(); l++) {
					rwList.push_back(pair<int, bool>(sortList[l].index,
							sortList[l].side));
				}
	
				intersection.setRoadWayList(rwList);
				intersection.setComplexity(false);
	
				addIntersection(intersection);
	
				//set the intersection in the road we have added
				for (unsigned int l = 0; l < intersection.getRoadWayList().size(); l++) {
					_roadWaysList[intersection.getRoadWayList()[l].first].setIntersection(
							_intersectionsList.size()-1,
							intersection.getRoadWayList()[l].second);
				}
			}
	
			// same for the intersection at the end of the road
			if (_roadWaysList[i].getIntersection(1) == INTERSECTION_NULL) { 
				Intersection intersection;
	
				pair<int, bool> info(i, true);
				intersection.addRoadWay(info);
				
				intersection.setCenter(axisEnd);
	
				for (unsigned int j = i; j < nbRoadways; j++) {
					RoadWay roadway2 = _roadWaysList[j];
					//vector<Vec3<double> > axis2 = roadway2.getPavement(0)->getAxis();
	
					if (axisEnd.equals(roadway2.getPavement(0)->getAxis()[0], toleranceValue)) {
						info = pair<int, bool>(j, 0);
						intersection.addRoadWay(info);
					}
	
					if (j != i) {
						if (axisEnd.equals(roadway2.getPavement(0)->getAxis()[roadway2.getPavement(0)->getAxis().size()-1], toleranceValue)) {
							info = pair<int, bool>(j, 1);
							intersection.addRoadWay(info);
						}
					}
				}
				vector<pair<unsigned int, bool> > rwList1 =
						intersection.getRoadWayList();
				vector<sort_inter> sortList;
				for (unsigned int l = 0; l < rwList1.size(); l++) {
					sortList.push_back(sort_inter(_roadWaysList[rwList1[l].first],
							rwList1[l].first, rwList1[l].second));
				}
				sort(sortList.begin(), sortList.end(), less_angle_inter());
				rwList1.clear();
				for (unsigned int l = 0; l < sortList.size(); l++) {
					rwList1.push_back(pair<int, bool>(sortList[l].index,
							sortList[l].side));
				}
				intersection.setRoadWayList(rwList1);
				intersection.setComplexity(false);
				addIntersection(intersection);
	
				for (unsigned int l=0; l<intersection.getRoadWayList().size(); l++) {
					_roadWaysList[intersection.getRoadWayList()[l].first].setIntersection(
							_intersectionsList.size()-1,
							intersection.getRoadWayList()[l].second);
				}
			}
		}
	
		std::cout << _intersectionsList.size() << " intersections found." << std::endl;
	
		// merge intersections if they are too close
		for (unsigned int i = 0; i < _intersectionsList.size(); i++) {
			unsigned int size = _intersectionsList[i].getRoadWayList().size();
	
			// if there are at least 2 roads in the intersection
			if (size > 1) {
				for (unsigned int j = 0; j < size; j++) {
	
					//indexes
					// next road in the intersection
					unsigned int road2Index = _intersectionsList[i].getRoadWayList()[(j+1)%size].first;
					// previous road
					unsigned int road1Index = _intersectionsList[i].getRoadWayList()[(j+size-1)%size].first;
					// current road
					unsigned int roadIndex = _intersectionsList[i].getRoadWayList()[j].first;
	
					// road objects
					RoadWay prevRoadway = _roadWaysList[road1Index];
					RoadWay nextRoadway = _roadWaysList[road2Index]; 
					RoadWay currRoadway = _roadWaysList[roadIndex];
	
					//width of the roads
					double prevWidth = _intersectionsList[i].getRoadWayList()[(j+size-1)%size].second?
							prevRoadway.getEndingWidth():prevRoadway.getStartingWidth();
					double nextWidth = _intersectionsList[i].getRoadWayList()[(j+1)%size].second?
							nextRoadway.getEndingWidth():nextRoadway.getStartingWidth();
					double currWidth = _intersectionsList[i].getRoadWayList()[j].second?
							currRoadway.getEndingWidth():currRoadway.getStartingWidth();
	
					// axis of the roads
					vector<Vec3<double> > prevAxis = prevRoadway.getPavement(0)->getAxis();
					vector<Vec3<double> > nextAxis = nextRoadway.getPavement(0)->getAxis();
					vector<Vec3<double> > currAxis = currRoadway.getPavement(0)->getAxis();
	
					//we treat points as if they were in 2D
					Vec3<double> endPoint;
					// compute the normal of the axis of the current road
					Vec3<double> endSegmentPt1, endSegmentPt2;
					if (_intersectionsList[i].getRoadWayList()[j].second) {
						//if we are at the end of the road get the starting point
						endPoint = Vec3<double>(currAxis[0][0], currAxis[0][1], currAxis[0][2]);
						Vec3<double> normal(currAxis[1][2]-currAxis[0][2],
								0,
								currAxis[0][0]-currAxis[1][0]);
						normal.normalize();
						
						//use the normal to compute the two end points of the road
						endSegmentPt1 = endPoint+(currWidth/2)*normal;
						endSegmentPt2 = endPoint-(currWidth/2)*normal;
					} else {
						//same in case of the beginning of the road in the intersection
						//get the ending point
						endPoint = Vec3<double>(currAxis[currAxis.size()-1][0],
								currAxis[currAxis.size()-1][1],
								currAxis[currAxis.size()-1][2]);
						Vec3<double> normal(currAxis[currAxis.size()-1][2]-currAxis[currAxis.size()-2][2],
								0,
								currAxis[currAxis.size()-2][0]-currAxis[currAxis.size()-1][0]);
						normal.normalize();
						endSegmentPt1 = endPoint+(currWidth/2)*normal;
						endSegmentPt2 = endPoint-(currWidth/2)*normal;
					}
	
					// test between current road and next road
					bool interWithNormalSuiv = false;
					double minLength = (currWidth+nextWidth)+1;
	
					for (unsigned int k = 0; k < nextAxis.size()-1; k++) {
						Vec3<double> suiv2dP1(nextAxis[k][0], nextAxis[k][1], nextAxis[k][2]);
						Vec3<double> suiv2dP2(nextAxis[k+1][0], nextAxis[k+1][1],	nextAxis[k+1][2]);
	
						//check if the side of the next road intersects with the end of the current one 
						interWithNormalSuiv = intersectEndRoadNormal(suiv2dP1,
								suiv2dP2, nextWidth, endSegmentPt1, endSegmentPt2);
	
						double tmp = distanceSegmentToPoint(suiv2dP1, suiv2dP2,
								endPoint);
	
						if (tmp < minLength) {
							minLength = tmp;
						}
	
						if (minLength <= EPSILON) {
							break;
						}
					}
	
					bool interMerged = false;
					
					//if minlength is nearly zero, it means that the two roads share two intersections
					//(a road is not going to simply pass just near the end of another one)
					//so we don't merge them
					if (minLength > EPSILON && (interWithNormalSuiv || (minLength
							<= (nextWidth+currWidth)/2))) {
					
						// axis of the next road is too close to the end of the current road,
						// the next road is going to overlap the intersection,
						// so merge the two intersections and erase the current road
	
						unsigned int interIndex;
						if (_intersectionsList[i].getRoadWayList()[j].second) {
							interIndex = currRoadway.getIntersection(0);
						} else {
							interIndex = currRoadway.getIntersection(1);
						}
						if (i != interIndex) {
							mergeIntersections(i, interIndex, roadIndex);
							interMerged = true;
							break;
							i--;
						}
					}
					
					//if current road hasn't been merged with the next road, do the test with the previous one
					if (!interMerged) {
						bool interWithNormalPrev = false;
						minLength = (currWidth+prevWidth)+1;
						for (unsigned int k = 0; k < prevAxis.size()-1; k++) {
							Vec3<double> prev2dP1(prevAxis[k][0], prevAxis[k][1],
									prevAxis[k][2]);
							Vec3<double> prev2dP2(prevAxis[k+1][0],
									prevAxis[k+1][1], prevAxis[k+1][2]);
	
							//check if the side of the previous road intersects with the end of the current one 
							interWithNormalPrev = intersectEndRoadNormal(prev2dP1,
									prev2dP2, prevWidth, endSegmentPt1,
									endSegmentPt2);
	
							// test to know if the two axis are too close to each other
							double tmp = distanceSegmentToPoint(prev2dP1, prev2dP2,
									endPoint);
							if (tmp<minLength) {
								minLength = tmp;
							}
	
							if (minLength <= EPSILON) {
								break;
							}
						}
	
						//if minlength is nearly zero, it means that the two roads share two intersections
						//(a road is not going to simply pass just near the end of another one)
						//so we don't merge them
						if (minLength > EPSILON && (interWithNormalPrev
								|| (minLength <= (prevWidth+currWidth)/2))) {
							
							// axis of the previous road is too close to the end of the current road,
							// the previous road is going to overlap the intersection,
							// so merge the two intersections and erase the current road
	
							unsigned int interIndex;
							if (_intersectionsList[i].getRoadWayList()[j].second) {
								interIndex = currRoadway.getIntersection(0);
							} else {
								interIndex = currRoadway.getIntersection(1);
							}
							if (i != interIndex) {
								mergeIntersections(i, interIndex, roadIndex);
								break;
								i--;
							}
						}
					}
				}
			}
		}
		std::cout << _intersectionsList.size()
				<< " intersections remaining after having merged closest ones."
				<< std::endl << std::endl;
	}
	
	void RoadNetwork::mergeIntersections(const unsigned int& index1,
			const unsigned int&index2, const unsigned int& roadIndex) {
		Intersection inter;
	
		// erase the road between the two intersections
	
		// for merged intersections, some points used to compute geometry can 
		// be inside the deleted road, but as we are going to delete the road
		// we need to save some informations on it (basically the bounding rectangle
		// of the road) to use them later
		Pavement* pavement = (*(_roadWaysList.begin()+roadIndex)).getPavement(0);
		double startingWidth = 0;
		double endingWidth = 0;
		for (unsigned int i = 0; i < pavement->getNbLine() ; ++i) {
			startingWidth += pavement->getLine(i)->getStartingWidth();
			endingWidth += pavement->getLine(i)->getEndingWidth();
		}
		Vec3<double> startPoint = pavement->getAxis()[0];
		Vec3<double> endPoint = pavement->getAxis()[pavement->getAxis().size()-1];
		Vec3<double> normal(startPoint[2] - endPoint[2],
				0,
				endPoint[0] - startPoint[0]);
		normal.normalize();
		inter.setDeletedRoad(startPoint - normal * startingWidth/2, startPoint + normal
				* startingWidth/2, endPoint + normal * endingWidth/2, endPoint - normal * endingWidth/2);
	
		// delete the road
		_roadWaysList.erase(_roadWaysList.begin()+roadIndex);		
		
		//erase every reference to the road in the intersections
		//and update road indices
		bool erased = false;
		for(unsigned int k = 0 ; !erased && k < _intersectionsList[index1].getRoadWayList().size() ; ++k){
			if (_intersectionsList[index1].getRoadWayList()[k].first == roadIndex) {
				_intersectionsList[index1].remRoadWay(k);
				erased = true;
			}
		}
		erased = false;
		for(unsigned int k = 0 ; !erased && k < _intersectionsList[index2].getRoadWayList().size() ; ++k){
			if (_intersectionsList[index2].getRoadWayList()[k].first == roadIndex) {
				_intersectionsList[index2].remRoadWay(k);
				erased = true;
			}
		}
		for (unsigned int k=0; k<_intersectionsList.size(); k++) {
			for (unsigned int h=0; h<_intersectionsList[k].getRoadWayList().size(); h++) {
				if (_intersectionsList[k].getRoadWayList()[h].first > roadIndex) {
					_intersectionsList[k].setRoadWay(h, _intersectionsList[k].getRoadWayList()[h].first-1);
				}
			}
		}
		
		if(_intersectionsList[index1].isComplex() || _intersectionsList[index2].isComplex()){
			inter.setMultiMerging(true);
			//current road is not the only one between the two merged intersections,
			//there is possibly another one (if the previously merged intersections and the other to merge
			//form some kind of triangle) so check if we can find one, anf it it's the case,
			//erase it and all reference to it
			bool moreErasing = false;
			for(unsigned int i = 0 ; !moreErasing && i < _intersectionsList[index1].getNumRoadWays() ; ++i){
				for(unsigned int j = 0 ; !moreErasing && j < _intersectionsList[index2].getNumRoadWays() ; ++j){
					if(_intersectionsList[index1].getRoadWay(i).first == _intersectionsList[index2].getRoadWay(j).first){
						//road found, erase it and every reference to it
						moreErasing = true;
						int indexMoreErasing = _intersectionsList[index1].getRoadWay(i).first;
						
						//delete the road
						_roadWaysList.erase(_roadWaysList.begin()+indexMoreErasing);
						//erase references in merged intersections
						_intersectionsList[index1].remRoadWay(i);
						_intersectionsList[index2].remRoadWay(j);
						//update road indices
						for (unsigned int k=0; k<_intersectionsList.size(); k++) {
							for (unsigned int h=0; h<_intersectionsList[k].getRoadWayList().size(); h++) {
								if (_intersectionsList[k].getRoadWayList()[h].first > indexMoreErasing) {
									_intersectionsList[k].setRoadWay(h, _intersectionsList[k].getRoadWayList()[h].first-1);
								}
							}
						}
					}
				}
			}
		}
	
		//update the information of the new merged intersection by getting the informations
		//in the two intersections we are merging
		
		for (unsigned int k=0; k<_intersectionsList[index1].getRoadWayList().size(); k++) {
			unsigned int rIndex = _intersectionsList[index1].getRoadWayList()[k].first;
			if (rIndex != ROADWAY_NULL) { // si on n'est pas sur la route a effacer	      
				bool sens = _intersectionsList[index1].getRoadWayList()[k].second;
				// 	    _roadWaysList[roadIndex].setIntersection(_intersectionsList.size()-2, sens);
				pair<int,bool> info(rIndex, sens);
	
				inter.addRoadWay(info);
			}
		}
	
		//get the informations of the second intersection
		for (unsigned int k=0; k<_intersectionsList[index2].getRoadWayList().size(); k++) {
			unsigned int rIndex = _intersectionsList[index2].getRoadWayList()[k].first;
			if (rIndex != ROADWAY_NULL) { // si on n'est pas sur la route a effacer
				bool sens = _intersectionsList[index2].getRoadWayList()[k].second;
				// 	    _roadWaysList[roadIndex].setIntersection(_intersectionsList.size()-2, sens);
				pair<int,bool> info(rIndex, sens);
				inter.addRoadWay(info);
			}
		}
	
		//get the center of the merged intersection as the barycenter of the two intersections
		inter.setCenter((_intersectionsList[index1].getCenter() + _intersectionsList[index2].getCenter()) / 2);
		
		//radius useful for sorting of intersections
		double radius = (inter.getCenter() - _intersectionsList[index1].getCenter()).length() > (inter.getCenter() - _intersectionsList[index2].getCenter()).length()?
				(inter.getCenter() - _intersectionsList[index1].getCenter()).length()*1.1:(inter.getCenter() - _intersectionsList[index2].getCenter()).length()*1.1;
		
		
		// erase the two intersections
		if (index1<index2) {
			_intersectionsList.erase(_intersectionsList.begin()+index1);
			_intersectionsList.erase(_intersectionsList.begin()+(index2-1));
		} else {
			_intersectionsList.erase(_intersectionsList.begin()+index2);
			_intersectionsList.erase(_intersectionsList.begin()+(index1-1));
		}
	
		// as we erased the intersections, we must update informations of each roadway
		// to tell them about the new intersection
		if (index1 < index2) {
			for (unsigned int k=index1; k<_intersectionsList.size(); k++) {
				vector<pair<unsigned int, bool> > roadList =
						_intersectionsList[k].getRoadWayList();
				for (unsigned int h=0; h<roadList.size(); h++) {
					_roadWaysList[roadList[h].first].setIntersection(k,
							roadList[h].second);
				}
			}
		} else {
			for (unsigned int k=index2; k<_intersectionsList.size(); k++) {
				vector<pair<unsigned int, bool> > roadList =
						_intersectionsList[k].getRoadWayList();
				for (unsigned int h=0; h<roadList.size(); h++) {
					_roadWaysList[roadList[h].first].setIntersection(k,
							roadList[h].second);
				}
			}
		}
		// sort the roads in counter clockwise order 
		//for easier later use
		
		vector<pair<unsigned int,bool> > rwList1 = inter.getRoadWayList();
		
		//sort using the same technique than the normals intersections can lead to errors
		//in sorting, so proceed a bit differently taking in account the center of
		//the merged intersection
		
		vector<sort_inter_merged> sortList;
	
		for (unsigned int l=0; l<rwList1.size(); l++) {
			sortList.push_back(sort_inter_merged(_roadWaysList[rwList1[l].first],
					rwList1[l].first, rwList1[l].second, inter.getCenter(), radius));
		}
		
		std::sort(sortList.begin(), sortList.end(), less_angle_inter_merged());
		rwList1.clear();
		for (unsigned int l=0; l<sortList.size(); l++) {
			rwList1.push_back(pair<int, bool>(sortList[l].index, sortList[l].side));
		}
		
		inter.setRoadWayList(rwList1);
		// declare the intersection as complex
		inter.setComplexity(true); 
		
		//finally add the intersection
		addIntersection(inter);
	
		//update the informations of the road in the new intersection
		for (unsigned int k=0; k<inter.getRoadWayList().size(); k++) {
			_roadWaysList[inter.getRoadWayList()[k].first].setIntersection(_intersectionsList.size()-1,
					inter.getRoadWayList()[k].second);
		}
	}
	
	double RoadNetwork::distanceSegmentToPoint(Vec3<double>& segmentPt1,
			Vec3<double>& segmentPt2, Vec3<double>& endPoint) {
		Line<double> line(segmentPt1, segmentPt2);
	
		Vec3<double> tmp = line.getClosestPoint(endPoint);
		
		//check using dot product properties if the tmp point is not out of the segment
		//as the previous instructions computed the closest point on the LINE held by the segment
		//and not the segment itself
		//if it's the case, choose the appropriate extremity
		if ((tmp-segmentPt1).dot(segmentPt2-segmentPt1) <=0) {
			tmp = segmentPt1;
		}
		if ((tmp-segmentPt2).dot(segmentPt1-segmentPt2) <=0) {
			tmp = segmentPt2;
		}
		return (tmp-endPoint).length();
	}
	
	bool RoadNetwork::intersectEndRoadNormal(Vec3<double>& lineP1,
			Vec3<double>& lineP2, double& width, Vec3<double>& endNormalPt1,
			Vec3<double>& endNormalPt2) {
	
		bool interWithNormal = false;
	
		Line<double> endNormalLine(endNormalPt1, endNormalPt2);
	
		Vec3<double> normal(lineP2[2]-lineP1[2], 0, lineP1[0]-lineP2[0]);
		normal.normalize();
	
		// first side of the road
		Vec3<double> P1 = lineP1 + (width/2)*normal;
		Vec3<double> P2 = lineP2 + (width/2)*normal;
		Line<double> segment(P1, P2);
	
		Vec3<double> normalPt, linePt;
		
		//compute the nearests points between the two LINES held by the segments
		endNormalLine.getClosestPoints(segment, normalPt, linePt);
		
		//if the lines held by the two segments are intersecting 
		if (normalPt.equals(linePt, 0.2)) {
			//if the point on the first line is included in the segment
			if ((normalPt-endNormalPt1).dot(endNormalPt1-endNormalPt2)*(normalPt-endNormalPt2).dot(endNormalPt1-endNormalPt2) < 0) {
				//if the point on the second line is included in the segment
				if ((linePt-P1).dot(P1-P2)*(linePt-P2).dot(P1-P2) < 0) {
					//there is intersection
					interWithNormal = true;
				}
			}
		}
		if(!interWithNormal){
			// same thing for the other side
			P1 = lineP1 - (width/2)*normal;
			P2 = lineP2 - (width/2)*normal;
			segment.setValue(P1, P2);
	
			//same as the first side
			endNormalLine.getClosestPoints(segment, normalPt, linePt);
			if (normalPt.equals(linePt, 0.2)) {
				if (((normalPt-endNormalPt1).dot(endNormalPt1-endNormalPt2)*(normalPt-endNormalPt2).dot(endNormalPt1-endNormalPt2) < 0)) {
					if ((linePt-P1).dot(P1-P2)*(linePt-P2).dot(P1-P2) < 0) {
						interWithNormal = true;
					}
				}
			}
			
		}
	
		return interWithNormal;
	
	}
	
	struct lessSegmentIndex {
		bool operator()(pair<unsigned int, unsigned int> v1,
				pair<unsigned int, unsigned int> v2) {
			return v1.second < v2.second;
		}
	};
	
	bool RoadNetwork::verifyNetwork() {
		std::cout << "Checking the resulting network..." << std::flush;
		unsigned int nbRoadways = _roadWaysList.size();
		unsigned int nbInter = _intersectionsList.size();
		if (_roadWaysList.empty() || _intersectionsList.empty()) {
			return false;
		}
		
		for (unsigned int i = 0; i < nbRoadways; i++) {
		
			for (unsigned int j=0; j<_roadWaysList[i].getPavementNum(); j++) {
				if (_roadWaysList[i].getPavement(j) == NULL) {
					return false;
				} else {
					for (unsigned int k=0; k<_roadWaysList[i].getPavement(j)->getLineList().size(); k++) {
						if (_roadWaysList[i].getPavement(j)->getLine(0) == NULL) {
							return false;
						}
					}
				}
			}
			if (_roadWaysList[i].getIntersection(0) > _intersectionsList.size()) {
				cerr << "false intersection: "
						<< _roadWaysList[i].getIntersection(0) << " i: " << i
						<< endl;
				return false;
			}
			if (_intersectionsList[_roadWaysList[i].getIntersection(0)].getRoadWayList().size() == 0) {
				cerr << "intersection : road list size == 0" << endl;
				return false;
			}
			if (_roadWaysList[i].getIntersection(1) > _intersectionsList.size()) {
				cerr << "false intersection : "
						<< _roadWaysList[i].getIntersection(1) << " i: " << i
						<< endl;
				return false;
			}
			if (_intersectionsList[_roadWaysList[i].getIntersection(1)].getRoadWayList().size() == 0) {
				cerr << "intersection : road list size == 0" << endl;
				return false;
			}
	
			bool found = false;
			for (unsigned int j=0; j
					<_intersectionsList[_roadWaysList[i].getIntersection(0)].getRoadWayList().size(); j++) {
				if (_intersectionsList[_roadWaysList[i].getIntersection(0)].getRoadWayList()[j].first == i) {
					found = true;
				}
			}
			if (!found) {
				cerr << "1) Test sur la route: " << i << endl;
				cout << "Intersection: " << _roadWaysList[i].getIntersection(0)
						<< endl;
				for (unsigned int j=0; j
						<_intersectionsList[_roadWaysList[i].getIntersection(0)].getRoadWayList().size(); j++) {
					cerr << "Num Possibles: "
							<< _intersectionsList[_roadWaysList[i].getIntersection(0)].getRoadWayList()[j].first << endl;
				}
				cerr << "Indexes in intersection list and roadway list don't match"
						<< endl;
				//       return false;
			}
			found = false;
			for (unsigned int j=0; j
					<_intersectionsList[_roadWaysList[i].getIntersection(1)].getRoadWayList().size(); j++) {
				if (_intersectionsList[_roadWaysList[i].getIntersection(1)].getRoadWayList()[j].first == i) {
					found = true;
				}
			}
			if (!found) {
				cerr << "2) Test sur la route: " << i << endl;
				cout << "Intersection: " << _roadWaysList[i].getIntersection(1)
						<< endl;
				for (unsigned int j=0; j
						<_intersectionsList[_roadWaysList[i].getIntersection(1)].getRoadWayList().size(); j++) {
					cerr << "Num Possibles: "
							<< _intersectionsList[_roadWaysList[i].getIntersection(1)].getRoadWayList()[j].first << endl;
				}
				cerr << "Indexes in intersection list and roadway list don't match"
						<< endl;
				//       return false;
			}
		}
	
		for (unsigned int i = 0; i < nbInter; i++) {
		
			if (_intersectionsList[i].getRoadWayList().size() < 1) {
				return false;
			}
			for (unsigned int j=0; j<_intersectionsList[i].getRoadWayList().size(); j++) {
				if (_intersectionsList[i].getRoadWayList()[j].first < 0 || _intersectionsList[i].getRoadWayList()[j].first> _roadWaysList.size()) {
						cerr << "false index in intersection" << endl;
						return false;
					}
					if(_roadWaysList[_intersectionsList[i].getRoadWayList()[j].first].getIntersection(0) != i && _roadWaysList[_intersectionsList[i].getRoadWayList()[j].first].getIntersection(1) != i) {
						cout << "inter: " << i << endl;
						cout << "_intersectionsList[" << i << "].getRoadWayList()[" << j << "].first: " << _intersectionsList[i].getRoadWayList()[j].first << endl;
						cout << "_roadWaysList[_intersectionsList["<< i << "].getRoadWayList()[" << j << "].first].getIntersection(0): " << _roadWaysList[_intersectionsList[i].getRoadWayList()[j].first].getIntersection(0) << endl;
						cout << "_roadWaysList[_intersectionsList[" << i << "].getRoadWayList()[" << j << "].first].getIntersection(1): " << _roadWaysList[_intersectionsList[i].getRoadWayList()[j].first].getIntersection(1) << endl;
						cerr << "Error: indexes in roadway list and intersection list don't match" << endl;
						// 	return false;
					}
				}
			}
	
			std::cout << std::endl;
			return true;
	}
		
	//##ModelId=4051925902B2
	vector<RoadWay>& RoadNetwork::getRoadWayList() {
		return _roadWaysList;
	}
	
	//##ModelId=4051927C01F4
	vector<Intersection>& RoadNetwork::getIntersectionsList() {
		return _intersectionsList;
	}
	
	RoadNetwork& RoadNetwork::operator=(const RoadNetwork& right) {
		_roadWaysList.clear();
		for(unsigned int i=0; i<right._roadWaysList.size(); i++) {
			_roadWaysList.push_back(right._roadWaysList[i]);
		}
	
		_intersectionsList.clear();
		for(unsigned int j=0; j<right._intersectionsList.size(); j++) {
			_intersectionsList.push_back(right._intersectionsList[j]);
		}
	
		return *this;
	}
	
	//void RoadNetwork::projectContours( const std::vector< OGRFeature* >& features ) // , arFbDtm* dtm ) {
	  //for(int i=0; i<root->getChildrenNum(); i++) {
	  //  arFbContour* co = NULL;
	  //  if( ( co = dynamic_cast< arFbContour* >(root->getChild(i)) ) != NULL ) {
	  //  	if( co->getVertex(0)[2] == 0 )
	  //  		dtm->project( *co );
	  //  }
	  //}
	//}

} // namespace roge

