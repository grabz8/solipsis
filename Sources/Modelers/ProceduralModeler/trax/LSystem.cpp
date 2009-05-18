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

#ifdef WIN32
#include <compat.h>
#endif

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

#include <utils/Utils.h>

#include "Config.h"
#include "LSystem.h"
#include "GeomBuilder.h"
#include "SegmentModule.h"
#include "ContextModule.h"

pm_trax::LSystem::LSystem() 
{}

pm_trax::LSystem::~LSystem()
{}

void pm_trax::LSystem::init()
{
	// Init map of module meanings 
	Module::init();
	
	// Alphabet Definition
	m_alphabet += Module::module2symbol( Module::CONTEXT );
	m_alphabet += Module::module2symbol( Module::FORWARD );
	m_alphabet += Module::module2symbol( Module::NEW_SEGMENT );
	m_alphabet += Module::module2symbol( Module::NEW_BRANCH );
	m_alphabet += Module::module2symbol( Module::BEGIN_BRANCH );
	m_alphabet += Module::module2symbol( Module::END_BRANCH );
	m_alphabet += Module::module2symbol( Module::ROTATE );
			
	ContextAgent::setGlobalContextFeature( new GlobalContextFeature() );
	ContextAgent::setLocalContextFeature( new LocalContextFeature() );
	
 	// Geometrical representation at start from the XML config file
	Vec3d position = pm_trax::Config::instance()->axiomPosition();
	double length = pm_trax::Config::instance()->axiomLength();
	double weight = pm_trax::Config::instance()->axiomWeight();
	double heading = pm_trax::Config::instance()->axiomHeading();
	double pitch = pm_trax::Config::instance()->axiomPitch();

	int fid = pm_trax::GeomBuilder::instance()->addFeature( weight );
	pm_trax::GeomBuilder::instance()->setCurrentFeatureId( fid );
	pm_trax::Geom* geom = new Geom( fid, position, length, weight, heading, pitch );
	pm_trax::GeomBuilder::instance()->addGeomToFeatureById( fid, geom );
					
	// Axiom Definition
	// A Segment Module
	SegmentModule* segmentModule = new SegmentModule( NULL, fid, weight, NULL /* no attached module at start */, Module::module2symbol( Module::NEW_SEGMENT ) );
	// With a right Context Module
	ContextModule* contextModule = new ContextModule( segmentModule, Module::module2symbol( Module::CONTEXT ) );

	// The Axiom only contains a segment module and a context module
	m_axiom.push_back( segmentModule );
	m_axiom.push_back( contextModule );
			
}

void pm_trax::LSystem::iterate( unsigned int depth )
{
	// init with axiom
	unsigned int i;
	std::vector< Module* > state;
	for( i = 0; i < m_axiom.size(); i++ )	
		state.push_back( m_axiom[i] );
	
	m_states.push_back( state );
	
	i = 0;
	// for each level until depth reached
	for( i = 0; i < depth; i++ ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LSystem::iterate -> CURRENT LSYSTEM DEPTH: " << i << std::endl;
		unsigned int j;
		unsigned int length = m_states[i].size();
		std::vector< Module* > state;
		for( j = 0; j < length; j++ ) {
			Module* predecessor = m_states[i][j];
			predecessor->setLeftContext( ( j > 0 ? m_states[i][j-1] : NULL ) );
			predecessor->setRightContext( ( j < length-1 ? m_states[i][j+1] : NULL ) );
			std::vector< Module* > successors;
			predecessor->produce( successors );
			unsigned int k;
			for( k = 0; k < successors.size(); k++ )	
				state.push_back( successors[k] );
		}

		if( state.size() == m_states.back().size() && state == m_states.back() )
			return; // lsystem has ended

		m_states.push_back( state );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LSystem::iterate -> CURRENT STATE FOR DEPTH #" << i << ": " << asString( state ) << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "LSystem::iterate -> CURRENT GEOMETRIES FOR DEPTH #" << i << ": " << std::endl << GeomBuilder::instance()->geometriesAsText() << std::endl;
	}
}
