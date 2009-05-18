#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include <ar/arGdalImage.h>

#include "Config.h"
#include "ContextDensityMap.h"
#include "ContextObstacleMap.h"
#include "ContextReliefMap.h"
#include "GlobalContextNetworkPatternFeature.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

GlobalContextNetworkPatternFeature::GlobalContextNetworkPatternFeature(  )
	: GlobalContextFeature( GlobalContextFeature::PATTERN )
{
	std::map< std::string, std::vector< double > > contextPatterns = Config::instance()->contextPatterns();

	std::map< std::string, std::string > contextMaps = Config::instance()->contextMaps();
	std::map< std::string, std::string >::iterator mit;
	for( mit = contextMaps.begin(); mit != contextMaps.end(); mit++ ) {
		if( mit->first == "DENSITY" ) {

			std::map< double, double > section2AngleToPeak = Config::instance()->contextDensitySection2AngleToPeak();	
			std::map< double, double > section2DistanceToPeak = Config::instance()->contextDensitySection2DistanceToPeak();	
			std::map< double, double > section2MinValueToTrigger = Config::instance()->contextDensitySection2MinValueToTrigger();
			std::map< double, std::string > section2Type = Config::instance()->contextDensitySection2Type();
			ContextDensityMap::MapAttributes mapAttributes( section2AngleToPeak, section2DistanceToPeak, section2MinValueToTrigger, section2Type );

			ContextDensityMap* dmap = new ContextDensityMap( mapAttributes, mit->second );
			std::map< double, double > strengths;
			std::map< std::string, std::vector< double > >::iterator pit;
			for( pit = contextPatterns.begin(); pit != contextPatterns.end(); pit++ )
				strengths.insert( std::make_pair( ( pit->second )[0], ( pit->second )[1] ) );
			addContextMap( dmap, strengths );
		}
		else if( mit->first == "RELIEF" ) {

			ContextReliefMap::MapAttributes mapAttributes;
			ContextReliefMap* rmap = new ContextReliefMap( mapAttributes, mit->second );
			std::map< double, double > strengths;
			std::map< std::string, std::vector< double > >::iterator pit;
			for( pit = contextPatterns.begin(); pit != contextPatterns.end(); pit++ )
				strengths.insert( std::make_pair( ( pit->second )[0], ( pit->second )[2] ) );
			addContextMap( rmap, strengths );
		
		}
		else if( mit->first == "OBSTACLE" ) {

			std::map< double, std::string > section2Type = Config::instance()->contextObstacleSection2Type();
			ContextObstacleMap::MapAttributes mapAttributes( section2Type );
			ContextObstacleMap* omap = new ContextObstacleMap( mapAttributes, mit->second );
			std::map< double, double > strengths;
			std::map< std::string, std::vector< double > >::iterator pit;
			for( pit = contextPatterns.begin(); pit != contextPatterns.end(); pit++ )
				strengths.insert( std::make_pair( ( pit->second )[0], ( pit->second )[3] ) );
			addContextMap( omap, strengths );

		}
	}
}

GlobalContextNetworkPatternFeature::~GlobalContextNetworkPatternFeature()
{}

void GlobalContextNetworkPatternFeature::analyse( Geom* g )
{
	std::vector< std::pair< ContextMap*, std::map< double, double > > >::iterator it;
	arVec3d direction( 0.0, 0.0, 0.0 );
	double length = 10.0;
	for( it = m_contextMaps.begin(); it != m_contextMaps.end(); it++ ){
		std::cout << "ANALYSING CONTEXT MAP." << std::endl;
		it->first->analyse( g );
		if( dynamic_cast< ContextObstacleMap* >( it->first ) != NULL && it->first->direction() != arVec3d( 0, 0, 0 ) ) {
			direction = it->second[g->width()]*( it->first->direction() );
		}
		else
			direction += it->second[g->width()]*( it->first->direction() );
		//length += it->second[g->width()]*( it->first->length() );
	}
	direction.normalize();
	std::cout << "PATTERN DIRECTION: " << direction << std::endl;
	std::cout << "PATTERN LENGTH: " << length << std::endl;
	m_direction = direction;
	m_length = length;
}
