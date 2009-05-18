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

#include <cstdlib>

#include "Config.h"
#include "GeomBuilder.h"
#include "BranchModule.h"
#include "ContextModule.h"
#include "ForwardModule.h"
#include "SegmentModule.h"
#include "RotateModule.h"

void pm_trax::SegmentModule::produce( std::vector< Module* >& modules )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "SEGMENT MODULE PRODUCE @: " << this << std::endl;
	if( weight() >= 0 && ( m_prevSegmentModule == NULL || ( m_prevSegmentModule != NULL && m_prevSegmentModule->weight() >= 0 ) ) ) {
		pm_trax::ContextModule* contextModule = NULL;
		if( ( contextModule = dynamic_cast< pm_trax::ContextModule* >( rightContext() ) ) != NULL ) {
			
			// Computes geometric attributes from global constraints
			// At start m_attachedModule==NULL. Checks if m_attachedModule was a ForwardModule or a BeginBranchModule
			contextModule->execute( pm_trax::ContextModule::GLOBAL, m_attachedModule ); 
			// IF EXECUTION OK THEN STATE=SUCCESSFUL ELSE STATE=FAILED

			switch( contextModule->globalState() ) {
			case pm_trax::ContextModule::FAILED: // Could not compute valid context-dependant values so returns an empty successor.
				pm_utils::notify( pm_utils::PM_DEBUG ) << "SegmentModule::produce -> NO SUCCESSOR FROM CONTEXT - END OF THE PROGRESSION." << std::endl;
				return; // empty successor
			case pm_trax::ContextModule::SUCCESSFUL:
			{			
				// Do all stuff: 
				// 1) create effective geometry according to Global Context ( rotation angle + line drawing ) -> +F
				// 2) create 2 branch modules -> BB
				// 3) create 1 segment module -> S
				// 4) create 1 context module for future context -> C

				std::map< double, std::vector< double > > branches = pm_trax::Config::instance()->branchProperties();
				// proba to have branching starting from here
				double proba = ( weight() > 0 ) ? branches[weight()-1][1] : 0.0;
				bool leftbranch = ( ( double )rand()/( double )RAND_MAX < proba ) ? true : false;
				bool rightbranch = ( ( double )rand()/( double )RAND_MAX < proba ) ? true : false;

				// next geom == copy of the last inserted
				Geom* sg = new pm_trax::Geom( *( pm_trax::GeomBuilder::instance()->lastGeomInFeature( featureId() ) ) );
				int FID = featureId();
				pm_trax::GeomBuilder::instance()->addGeomToFeatureById( FID, sg );

				// BEGIN FIXME !!! 
				// If a new branch occurs, it ends the current feature (end of a linestring), and begins a new one
				if( leftbranch || rightbranch ) {
					// new feature in the GeomBuilder
					FID = pm_trax::GeomBuilder::instance()->addFeature( weight() );
					pm_trax::GeomBuilder::instance()->addGeomToFeatureById( FID, sg );
				}
				// END FIXME !!!

				// Creation of a Rotate module which updates the angles of the geom it is associated to.
				pm_trax::RotateModule* rotateModule = new pm_trax::RotateModule( this, contextModule->heading(), contextModule->pitch(), pm_trax::Module::module2symbol( ROTATE ) );
				modules.push_back( rotateModule );
				// Creation of a Forward module which updates the length of the geom it is associated to.
				pm_trax::ForwardModule* forwardModule = new pm_trax::ForwardModule( this, contextModule->length(), pm_trax::Module::module2symbol( FORWARD ) );
				modules.push_back( forwardModule );

				if( weight() > 0 ) {
					if( leftbranch ) {
						// new feature in the GeomBuilder
						int fid = pm_trax::GeomBuilder::instance()->addFeature( weight()-1 );
						// next geom
						Geom *bg = new Geom( *sg );
						bg->setFeatureId( fid );
						bg->setWidth( weight()-1 );
						bg->setLength( branches[weight()-1][0] );
						pm_trax::GeomBuilder::instance()->addGeomToFeatureById( fid, bg );
						// add branch module to current lsystem state
						modules.push_back( new pm_trax::BranchModule( fid, weight()-1, forwardModule, true, pm_trax::Module::module2symbol( NEW_BRANCH ) ) ); // up branch
					}
					if( rightbranch ) {
						// new feature in the GeomBuilder
						int fid = pm_trax::GeomBuilder::instance()->addFeature( weight()-1 );
						// next geom
						Geom *bg = new Geom( *sg );
						bg->setFeatureId( fid );
						bg->setWidth( weight()-1 );
						bg->setLength( branches[weight()-1][0] );
						pm_trax::GeomBuilder::instance()->addGeomToFeatureById( fid, bg );
						// add branch module to current lsystem state
						modules.push_back( new pm_trax::BranchModule( fid, weight()-1, forwardModule, false, pm_trax::Module::module2symbol( NEW_BRANCH ) ) ); // down branch
					}
				}

				pm_trax::SegmentModule* segmentModule = new pm_trax::SegmentModule( this, FID, weight(), forwardModule, pm_trax::Module::module2symbol( NEW_SEGMENT ) ); 
				modules.push_back( segmentModule );
				modules.push_back( new pm_trax::ContextModule( segmentModule, pm_trax::Module::module2symbol( CONTEXT ) ) ); 
				
				// Default States set to UNASSIGNED 

				break;
			}
			default:
				break;
			}
		}
	}
}
