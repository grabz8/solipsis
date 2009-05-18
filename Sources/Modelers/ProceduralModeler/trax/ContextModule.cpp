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

#include <cstdlib>

#include "Config.h"
#include "ContextModule.h"
#include "SegmentModule.h"
#include "BeginBranchModule.h"
#include "GeomBuilder.h"

void pm_trax::ContextModule::produce( std::vector< pm_trax::Module* >& modules )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "CONTEXT MODULE PRODUCE @: " << this << std::endl;
	pm_trax::SegmentModule* segmentModule = NULL;
	if( ( segmentModule = dynamic_cast< pm_trax::SegmentModule* >( leftContext() ) ) != NULL )
		if( segmentModule->weight() < 0 )
			return;

	if( globalState() == pm_trax::ContextModule::SUCCESSFUL && localState() == pm_trax::ContextModule::UNASSIGNED ) {
		
		// refine geometric attributes from local context
		execute( pm_trax::ContextModule::LOCAL, NULL ); // IF EXECUTION OK STATE SET TO SUCCESSFUL ELSE TO FAILED
		
		// localState is now != UNASSIGNED ( == ( FAILED || SUCCESSFUL ) ) so module will be deleted at next iteration
		modules.push_back( new pm_trax::ContextModule( *this ) ); 

		if( localState() == pm_trax::ContextModule::FAILED ) {
			if( segmentModule != NULL )	{
				segmentModule->setWeight( -1 ); // WILL STOP PROGRESSION AT NEXT ITERATION
			}	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::produce -> WILL STOP PROGRESSION AT NEXT ITERATION" << std::endl;
		}
		
	}
}

void pm_trax::ContextModule::execute( pm_trax::ContextModule::Type t, pm_trax::Module* module )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "CONTEXT MODULE EXECUTE @: " << this << std::endl;
	double rotation = 0;
	switch( t ){
	case pm_trax::ContextModule::GLOBAL:
	{
		//double dAngle = ( double )rand()/( double )RAND_MAX*5;
		//int sign = ( ( ( double )rand()/( double )RAND_MAX ) > 0.5 ? 1 : -1 );
		//rotation = sign*dAngle*Linear::DegToRad;
		//if( module != NULL ) { 
		//	pm_trax::BeginBranchModule* beginBranchModule = NULL; 
		//	if( ( beginBranchModule = dynamic_cast< pm_trax::BeginBranchModule* >( module ) ) != NULL ) {
		//		rotation = ( beginBranchModule->isUp() ? ( 90.0+sign /* *dAngle */ )*Linear::DegToRad : ( -90.0+sign /* *dAngle */ )*Linear::DegToRad );
		//	}
		//}
		if( create() )
			m_globalState = pm_trax::ContextModule::SUCCESSFUL;
		else m_globalState = pm_trax::ContextModule::FAILED;
		break;
	}
	case pm_trax::ContextModule::LOCAL:
	{
		if( refine() )
			m_localState = pm_trax::ContextModule::SUCCESSFUL;
		else { 
			m_localState = pm_trax::ContextModule::FAILED;
		}
		break;
	}
	default:
		break;
	}	
}

bool pm_trax::ContextModule::create() 
{
	int fid = static_cast< pm_trax::SegmentModule* >( m_attachedModule )->featureId();

	pm_trax::Geom* geom = pm_trax::GeomBuilder::instance()->lastGeomInFeature( fid );
	if( geom == NULL ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::create -> LAST GEOM IN FEATURE#: " << static_cast< SegmentModule* >( m_attachedModule )->featureId() << " IS NULL" << std::endl;
		return false;
	}

	double dAngle = ( double )rand()/( double )RAND_MAX*Linear::DegToRad*5; // 5°
	int sign = ( ( ( double )rand()/( double )RAND_MAX ) > 0.5 ? 1 : -1 );
	double rotation = sign*dAngle;
	if( m_attachedModule->attachedModule() != NULL ) { 
		pm_trax::BeginBranchModule* beginBranchModule = NULL; 
		// if a new branch is beginning, alter its heading from a near PI/2 angle and return
		if( ( beginBranchModule = dynamic_cast< pm_trax::BeginBranchModule* >( m_attachedModule->attachedModule() ) ) != NULL ) {
			rotation += ( beginBranchModule->isUp() ? ( M_PI/2 ) : ( -M_PI/2 ) );
			setHeading( geom->heading()+rotation );
			setPitch ( geom->pitch() );
			setLength( geom->length() );
			return true;
		}
	}

	// Geom heading correction
	pm_trax::ContextAgent::instance()->setGeom( geom );
	if( pm_trax::ContextAgent::instance()->throwGlobal() ) {
		Vec3d direction = pm_trax::ContextAgent::instance()->direction(); // direction retournée par les cartes de contexte
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::create -> CREATE DIRECTION: " << direction[0] << " " << direction[1] << " " << direction[2] << std::endl;
		if( direction == Vec3d( 0, 0, 0 ) )
			return false;
		double length = ContextAgent::instance()->length();
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::create -> CREATE LENGTH: " << length << std::endl;
		if( length == 0.0 )
			return false;
		double heading = asin( direction[1] / direction.length() );
		heading = ( direction[0] < 0.0 ? ( M_PI - heading ) : heading );
		double pitch = asin( direction[2] / direction.length() );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::create -> CREATE WITH HEADING: " << heading*Linear::RadToDeg << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::create -> CREATE WITH PITCH: " << pitch*Linear::RadToDeg << std::endl;
		setHeading( heading+rotation );
		setPitch ( pitch );
		setLength( length );
		return true;
	}
	return false;
}

bool pm_trax::ContextModule::refine() {

	int fid = static_cast< pm_trax::SegmentModule* >( m_attachedModule )->featureId();

	if( dynamic_cast< pm_trax::BeginBranchModule* >( m_attachedModule->attachedModule() ) ) // do not refine a geom coming from an intersection 
		return true; // But return true to do not stop lsystem progression
	
	pm_trax::Geom *geom = pm_trax::GeomBuilder::instance()->lastGeomInFeature( fid );
	if( geom == NULL ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> Geom == NULL could not be refined" << std::endl;
		return false; // Stop lsystem progression
	}

	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINING GEOM POSITION: " << geom->position()[0] << " " << geom->position()[1] << " " << geom->position()[2] << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINING GEOM HEADING: " << geom->heading()*Linear::RadToDeg << std::endl;

	pm_trax::ContextAgent::instance()->setGeom( geom );
	if( pm_trax::ContextAgent::instance()->throwLocal() ) {
		Vec3d	direction = pm_trax::ContextAgent::instance()->direction();
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINE DIRECTION: " << direction[0] << " " << direction[1] << " " << direction[2] << std::endl;
		if( direction == Vec3d( 0, 0, 0 ) )
			return false; // Wrong direction -> So stop lsystem progression
		double length = pm_trax::ContextAgent::instance()->length();
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINE LENGTH: " << length << std::endl;
		if( length == 0.0 )
			return false; // Wrong length -> So stop lsystem progression
		double heading = asin( direction[1] / direction.length() );
		heading = ( direction[0] < 0.0 ? ( M_PI - heading ) : heading );
		double pitch = asin( direction[2] / direction.length() );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINE WITH HEADING: " << heading*Linear::RadToDeg << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::refine -> REFINE WITH PITCH: " << pitch*Linear::RadToDeg << std::endl;

		// Differs from create method. 
		// Here the geom is directly updated by the context module local computations.
		// Return back to previous position
		geom->setPosition( geom->previousPosition() );
		// Assign new computed parameters
		geom->setHeading( heading );
		geom->setPitch( pitch );
		geom->setLength( length );
		// Then update new position
		geom->updatePosition();
		return true; // Continue lsystem progression
	}
	return false; // Stop lsystem progression as the agent could not get context-computed parameters
}

std::pair< bool, bool > pm_trax::ContextModule::requestForBranches()
{
	std::pair< bool, bool > branch( false, false );
	int fid = static_cast< pm_trax::SegmentModule* >( m_attachedModule )->featureId();

	
	pm_trax::Geom *geom = pm_trax::GeomBuilder::instance()->lastGeomInFeature( fid );
	if( geom == NULL ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextModule::requestForBranches -> Geom == NULL could not request for branches" << std::endl;
		return  std::make_pair< bool, bool >( false, false );
	}
	if( geom->width() != pm_trax::Config::instance()->axiomWeight() ) // Create branches only for large width geoms
		return std::make_pair< bool, bool >( false, false ); 

	pm_trax::ContextAgent::instance()->setGeom( geom );
	return std::make_pair< bool, bool >( pm_trax::ContextAgent::instance()->requestForLeftBranch(), pm_trax::ContextAgent::instance()->requestForRightBranch() );
}