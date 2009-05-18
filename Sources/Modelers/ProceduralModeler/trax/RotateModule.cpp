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

#include "Config.h"
#include "GeomBuilder.h"
#include "BranchModule.h"
#include "RotateModule.h"
#include "SegmentModule.h"

pm_trax::RotateModule::RotateModule( pm_trax::Module* attachedGeomModule, double heading, double pitch, const Symbol s  ) 
: pm_trax::Module( attachedGeomModule, s ), m_heading( heading ), m_pitch( pitch ) 
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "NEW ROTATE MODULE @: " << this << ", heading: " << m_heading*Linear::RadToDeg << std::endl;
	// correct the 'from context computed' angles of the already created geometry
	int fid = static_cast< SegmentModule* >( m_attachedModule )->featureId();
	pm_trax::Geom* g = pm_trax::GeomBuilder::instance()->lastGeomInFeature( fid );
	g->setHeading( heading );
	g->setPitch( pitch );

};

void pm_trax::RotateModule::produce( std::vector< pm_trax::Module* >& modules )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ROTATE TERMINAL MODULE PRODUCE @: " << this << std::endl;
	// add a terminal rotate module
	modules.push_back( new pm_trax::Module( NULL, pm_trax::Module::module2symbol( ROTATE ) ) ); // terminal module ; won't produce anything
}
