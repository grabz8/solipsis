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

#include <cassert>

#include "Config.h"
#include "ContextIntersectionMap.h"
#include "ContextObstacleMap.h"
#include "ContextReliefMap.h"
#include "LocalContextFeature.h"

pm_trax::LocalContextFeature::LocalContextFeature()
{
	std::map< std::string, std::string > contextMaps = pm_trax::Config::instance()->localContextMaps();
	std::map< std::string, std::string >::iterator mit;
	for( mit = contextMaps.begin(); mit != contextMaps.end(); mit++ ) {
		if( mit->first == "OBSTACLE" ) {

			std::map< double, std::string > section2Type = pm_trax::Config::instance()->contextObstacleSection2Type();
			std::map< double, double > section2Dilatation = pm_trax::Config::instance()->contextObstacleSection2Dilatation();
			pm_trax::ContextObstacleMap::MapAttributes mapAttributes( section2Type, section2Dilatation );
			pm_trax::ContextObstacleMap* omap = new pm_trax::ContextObstacleMap( mapAttributes, mit->second );
			std::map< double, double > strengths;
			addContextMap( omap, strengths );

		}
		if( mit->first == "INTERSECTION" ) {

			std::map< double, std::string > section2Type = pm_trax::Config::instance()->contextIntersectionSection2Type();
			pm_trax::ContextIntersectionMap::MapAttributes mapAttributes( section2Type );
			pm_trax::ContextIntersectionMap* imap = new pm_trax::ContextIntersectionMap( mapAttributes );
			std::map< double, double > strengths;
			addContextMap( imap, strengths );
		}
	}
}

void pm_trax::LocalContextFeature::analyse( pm_trax::Geom* g )
{
	std::vector< std::pair< ContextMap*, std::map< double, double > > >::iterator it;
	Vec3d direction( 0.0, 0.0, 0.0 );
	double length = 0.0;
	for( it = m_contextMaps.begin(); it != m_contextMaps.end(); it++ ){
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LocalContextFeature::analyse -> ANALYSING LOCAL CONTEXT MAP." << std::endl;
		it->first->analyse( g );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LocalContextFeature::analyse -> LOCAL CONTEXT DIRECTION: " << it->first->direction()[0] << " " << it->first->direction()[1] << " " << it->first->direction()[2] << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LocalContextFeature::analyse -> LOCAL CONTEXT LENGTH: " << it->first->length() << std::endl;
		if( it->first->direction() == Vec3d( 0.0, 0.0, 0.0 ) ) {
			m_direction = it->first->direction();
			m_length = it->first->length();
			return;
		}
		direction = it->first->direction();
		length = it->first->length();
	}
	if( length == 0 )
		length = g->length();
	pm_utils::notify( pm_utils::PM_DEBUG ) << "LocalContextFeature::analyse -> ANALYSED LOCAL CONTEXT DIRECTION: " << direction << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "LocalContextFeature::analyse -> ANALYSED LOCAL CONTEXT LENGTH: " << length << std::endl;
	m_direction = direction;
	m_length = length;
}
