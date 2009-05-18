#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include "ContextIntersectionMap.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::ContextIntersectionMap::ContextIntersectionMap( const pm_trax::ContextIntersectionMap::MapAttributes& attributes )
	: ContextMap( "" ), m_attributes( attributes )
{
}

pm_trax::ContextIntersectionMap::~ContextIntersectionMap()
{}

void pm_trax::ContextIntersectionMap::analyse( pm_trax::Geom* g ) 
{
	m_geom = g;
	m_direction = bestDirection();
	m_length = bestLength();
}

bool selfIntersects( pm_trax::GeomBuilder::Feature* feat )
{
	if( feat->m_geoms.size() < 3 || feat->m_weight == 2 )
		return false;
	OGRLineString ls0;
	OGRPoint pt0;
	pt0.setX( feat->m_geoms[feat->m_geoms.size()-2]->position()[0] ); pt0.setY( feat->m_geoms[feat->m_geoms.size()-2]->position()[1] ); pt0.setZ( feat->m_geoms[feat->m_geoms.size()-2]->position()[2] );
	ls0.addPoint( &pt0 );
	OGRPoint pt1;
	pt1.setX( feat->m_geoms[feat->m_geoms.size()-1]->position()[0] ); pt1.setY( feat->m_geoms[feat->m_geoms.size()-2]->position()[1] ); pt1.setZ( feat->m_geoms[feat->m_geoms.size()-2]->position()[2] );
	ls0.addPoint( &pt1 );
	int i = 0;
	while( i < feat->m_geoms.size()-3 ) {
		OGRLineString ls1;
		OGRPoint pt0;
		pt0.setX( feat->m_geoms[i]->position()[0] ); pt0.setY( feat->m_geoms[i]->position()[1] ); pt0.setZ( feat->m_geoms[i]->position()[2] );
		ls1.addPoint( &pt0 );
		OGRPoint pt1;
		pt1.setX( feat->m_geoms[i+1]->position()[0] ); pt1.setY( feat->m_geoms[i+1]->position()[1] ); pt1.setZ( feat->m_geoms[i+1]->position()[2] );
		ls1.addPoint( &pt1 );
		if( ls1.Intersects( &ls0 ) ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "selfIntersects -> INTERSECTION AT SEGMENT [(" << feat->m_geoms[i]->position()[0] << "," << feat->m_geoms[i]->position()[1] << "), (" << feat->m_geoms[i+1]->position()[0] << "," << feat->m_geoms[i+1]->position()[1] << ")]" << std::endl; 
			pm_utils::notify( pm_utils::PM_DEBUG ) << "selfIntersects -> INTERSECTION AT SEGMENT [" << i << "," << i+1 << "]" << std::endl; 
			OGRPoint *pt = static_cast< OGRPoint* >( ls1.Intersection( &ls0 ) );
			feat->m_geoms[feat->m_geoms.size()-1]->setPosition( Vec3d( pt->getX(), pt->getY(), pt->getZ() ) );
			feat->m_geoms[feat->m_geoms.size()-2]->setLength( ( Vec3d( pt->getX(), pt->getY(), pt->getZ() )-feat->m_geoms[feat->m_geoms.size()-2]->position() ).length() );
			return true;
		}
		i++;
	}
	return false;
}

bool intersects( pm_trax::GeomBuilder::Feature* current, pm_trax::GeomBuilder::Feature* other )
{
	if( current->m_geoms.size() < 4 || other->m_geoms.size() < 2 )
		return false;
	OGRLineString ls0;
	OGRPoint pt0;
	pt0.setX( current->m_geoms[current->m_geoms.size()-3]->position()[0] ); pt0.setY( current->m_geoms[current->m_geoms.size()-3]->position()[1] ); pt0.setZ( current->m_geoms[current->m_geoms.size()-3]->position()[2] );
	ls0.addPoint( &pt0 );
	OGRPoint pt1;
	pt1.setX( current->m_geoms[current->m_geoms.size()-2]->position()[0] ); pt1.setY( current->m_geoms[current->m_geoms.size()-2]->position()[1] ); pt1.setZ( current->m_geoms[current->m_geoms.size()-2]->position()[2] );
	ls0.addPoint( &pt1 );
	int i = 1;
	while( i < other->m_geoms.size()-1 ) {
		OGRLineString ls1;
		OGRPoint pt0;
		pt0.setX( other->m_geoms[i]->position()[0] ); pt0.setY( other->m_geoms[i]->position()[1] ); pt0.setZ( other->m_geoms[i]->position()[2] );
		ls1.addPoint( &pt0 );
		OGRPoint pt1;
		pt1.setX( other->m_geoms[i+1]->position()[0] ); pt1.setY( other->m_geoms[i+1]->position()[1] ); pt1.setZ( other->m_geoms[i+1]->position()[2] );
		ls1.addPoint( &pt1 );
		if( ls1.Intersects( &ls0 ) ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "intersects -> INTERSECTION AT FEATURE#: " << current->m_geoms[0]->featureId() << " AND " << other->m_geoms[0]->featureId() << " SEGMENTS [" << i << "," << i+1 << "]" << std::endl; 
			pm_utils::notify( pm_utils::PM_DEBUG ) << "intersects -> INTERSECTION AT SEGMENTS [(" << other->m_geoms[i]->position()[0] << "," << other->m_geoms[i]->position()[1] << "), (" << other->m_geoms[i+1]->position()[0] << "," << other->m_geoms[i+1]->position()[1] << ")]" << std::endl; 
			OGRPoint *pt = static_cast< OGRPoint* >( ls1.Intersection( &ls0 ) );
			current->m_geoms[current->m_geoms.size()-2]->setPosition( Vec3d( pt->getX(), pt->getY(), pt->getZ() ) );
			current->m_geoms[current->m_geoms.size()-3]->setLength( ( Vec3d( pt->getX(), pt->getY(), pt->getZ() )-current->m_geoms[current->m_geoms.size()-2]->position() ).length() );
			return true;
		}
		i++;
	}
	return false;
}

bool intersects( pm_trax::GeomBuilder::Feature* current, std::vector< pm_trax::GeomBuilder::Feature* > others )
{
	std::vector< pm_trax::GeomBuilder::Feature* >::iterator it;
	for( it = others.begin(); it != others.end(); it++ ) {
		if( current != ( *it ) && current->m_weight <= ( *it )->m_weight ) {
			if( intersects( current, *it ) ) {
				return true;
			}
		}
	}
	return false;
}

Vec3d pm_trax::ContextIntersectionMap::bestDirection()
{
	Vec3d direction( 0.0, 0.0, 0.0 );
	pm_trax::GeomBuilder::Feature* currentFeature = pm_trax::GeomBuilder::instance()->feature( m_geom->featureId() );

	if( currentFeature->m_geoms.size() < 1 )
		return direction;

	//if( !posInsideBounds( m_geom->position() ) )
	//	return Vec3d( 0.0, 0.0, 0.0 );


	if( selfIntersects( currentFeature ) ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextIntersectionMap::bestDirection -> SELFINTERSECTS !!!" << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextIntersectionMap::bestDirection -> FOUND AN INTERSECTION -> STOP CONDITION" << std::endl;
		// remove last point from currentFeature
		currentFeature->m_geoms.pop_back();
		if( currentFeature->m_geoms.size() < 10 )
			currentFeature->m_geoms.clear();
		return direction;
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextIntersectionMap::bestDirection -> NO SELF INTERSECTION" << std::endl;
	if( intersects( currentFeature, pm_trax::GeomBuilder::instance()->features() ) ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextIntersectionMap::bestDirection -> FOUND AN INTERSECTION -> STOP CONDITION" << std::endl;
		// remove last point from currentFeature
		currentFeature->m_geoms.pop_back();
		if( currentFeature->m_geoms.size() < 10 )
			currentFeature->m_geoms.clear();
		return direction;
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextIntersectionMap::bestDirection -> NO INTERSECTION WITH OTHER FEATURES" << std::endl;

	direction = m_geom->direction();
	return direction;
}

double pm_trax::ContextIntersectionMap::bestLength()
{
	return( m_geom->length() );
}