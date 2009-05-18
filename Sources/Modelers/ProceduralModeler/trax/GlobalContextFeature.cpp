/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include "Config.h"
#include "ContextDensityMap.h"
#include "ContextObstacleMap.h"
#include "ContextReliefMap.h"
#include "GdalImage.h"
#include "GlobalContextFeature.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::GlobalContextFeature::GlobalContextFeature()
{
	std::map< std::string, std::string > contextMaps = pm_trax::Config::instance()->globalContextMaps();
	std::map< std::string, std::string >::iterator mit;
	for( mit = contextMaps.begin(); mit != contextMaps.end(); mit++ ) {
		if( mit->first == "PATTERN" ) {

			//ContextPatternMap::MapAttributes mapAttributes;
			//ContextPatternMap* rmap = new ContextPatternMap( mapAttributes, mit->second );
			//std::map< double, double > strengths;
			//std::map< std::string, std::vector< double > >::iterator pit;
			//for( pit = contextPatterns.begin(); pit != contextPatterns.end(); pit++ )
			//	strengths.insert( std::make_pair( ( pit->second )[0], ( pit->second )[2] ) );
			//addContextMap( rmap, strengths );
		
		}
		else if( mit->first == "DENSITY" ) {
	
			std::map< double, double > section2AngleToPeak = pm_trax::Config::instance()->contextDensitySection2AngleToPeak();	
			std::map< double, double > section2DistanceToPeak = pm_trax::Config::instance()->contextDensitySection2DistanceToPeak();	
			std::map< double, double > section2MinValueToTrigger = pm_trax::Config::instance()->contextDensitySection2MinValueToTrigger();
			std::map< double, std::string > section2Type = pm_trax::Config::instance()->contextDensitySection2Type();
			pm_trax::ContextDensityMap::MapAttributes mapAttributes( section2AngleToPeak, section2DistanceToPeak, section2MinValueToTrigger, section2Type );

			pm_trax::ContextDensityMap* dmap = new pm_trax::ContextDensityMap( mapAttributes, mit->second );
			std::map< double, double > impacts = pm_trax::Config::instance()->contextDensitySection2Impact();
			addContextMap( dmap, impacts );
		}
		else if( mit->first == "RELIEF" ) {

			std::map< double, std::string > section2Type = pm_trax::Config::instance()->contextReliefSection2Type();
			pm_trax::ContextReliefMap::MapAttributes mapAttributes( section2Type );
			pm_trax::ContextReliefMap* rmap = new pm_trax::ContextReliefMap( mapAttributes, mit->second );
			std::map< double, double > impacts = pm_trax::Config::instance()->contextReliefSection2Impact();
			addContextMap( rmap, impacts );

		}
	}
}

void pm_trax::GlobalContextFeature::analyse( pm_trax::Geom* g )
{
	std::vector< std::pair< ContextMap*, std::map< double, double > > >::iterator it;
	Vec3d direction( 0.0, 0.0, 0.0 );
	double length = 0.0;
	for( it = m_contextMaps.begin(); it != m_contextMaps.end(); it++ ){
		pm_utils::notify( pm_utils::PM_DEBUG ) << "GlobalContextFeature::analyse -> ANALYSING GLOBAL CONTEXT MAP." << std::endl;
		it->first->analyse( g );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "GlobalContextFeature::analyse -> GEOM WIDTH: " << g->width() << ", WEIGHT: " << it->second[g->width()] << ", DIRECTION: " << it->first->direction()[0] << " " << it->first->direction()[1] << std::endl;
		direction += it->second[g->width()]*( it->first->direction() );
		//length += it->second[g->width()]*( it->first->length() );
	}
	direction.normalize();
	pm_utils::notify( pm_utils::PM_DEBUG ) << "GlobalContextFeature::analyse -> GLOBAL CONTEXT DIRECTION: " << direction << std::endl;
	m_direction = direction;
	std::map< double, std::vector< double > > branches = pm_trax::Config::instance()->branchProperties();
	m_length = g->width() == 2 ? pm_trax::Config::instance()->axiomLength() : branches[g->width()][0];
}
