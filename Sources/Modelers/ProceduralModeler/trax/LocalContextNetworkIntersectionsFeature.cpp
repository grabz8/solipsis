#include <cassert>

#include "LocalContextNetworkIntersectionsFeature.h"

LocalContextNetworkIntersectionsFeature::LocalContextNetworkIntersectionsFeature()
	: LocalContextFeature( LocalContextFeature::INTERSECTIONS )
{}

LocalContextNetworkIntersectionsFeature::~LocalContextNetworkIntersectionsFeature()
{}

void LocalContextNetworkIntersectionsFeature::analyse( Geom* g )
{
	m_geom = g;
	m_direction = arVec3d( 0, 0, 0);
	m_length = m_geom->length();
	updateSearchBounds();
	std::vector< Geom* > geoms = geomsInsideSearchBounds();
	if( !geoms.empty() )
		refine( geoms );
	
	// test for intersections and proximity vertices
		
	// if there are, move the heading, pitch, length of the m_geom to the point of intersection
	// AND set the weight of the module to a value where it stops proliferating.
}

std::vector< Geom* > LocalContextNetworkIntersectionsFeature::geomsInsideSearchBounds()
{
	std::vector< Geom* > geoms;
	return geoms;		
}

void LocalContextNetworkIntersectionsFeature::updateSearchBounds()
{
	assert( m_geom != NULL );
	arVec3d pos = m_geom->position();
	double length = m_geom->length();
	arVec3d halfSize( length/2, length/2, length/2 );
	m_searchBounds = arBox3<double>( pos-halfSize, pos+halfSize );
}

void LocalContextNetworkIntersectionsFeature::refine( std::vector< Geom* >& geoms ) 
{	
//	m_direction = ( geoms[0]->position() - m_geom->position() );
//	m_length = m_direction.length();
//	m_direction.normalize();
//	std::cout << "DIRECTION FROM LOCAL CONTEXT INTERSECTIONS: " << m_direction << std::endl;
}
