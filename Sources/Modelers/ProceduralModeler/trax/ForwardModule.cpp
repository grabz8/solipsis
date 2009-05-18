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

#include "ForwardModule.h"
#include "GeomBuilder.h"
#include "SegmentModule.h"

pm_trax::ForwardModule::ForwardModule( pm_trax::Module* attachedGeomModule, double length, const Symbol s ) 
: pm_trax::Module( attachedGeomModule, s ), m_length( length ) 
{ 
	pm_utils::notify( pm_utils::PM_DEBUG ) << "NEW FORWARD MODULE @: " << this << ", length: " << m_length << std::endl;

	int fid = static_cast< SegmentModule* >( m_attachedModule )->featureId();
	pm_trax::Geom* g = pm_trax::GeomBuilder::instance()->lastGeomInFeature( fid );
	g->setLength( length );
	g->updatePosition();
}

void pm_trax::ForwardModule::produce( std::vector< Module* >& modules )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "FORWARD TERMINAL MODULE PRODUCE @: " << this << std::endl;
	// add a terminal forward module
	modules.push_back( new pm_trax::Module( NULL, pm_trax::Module::module2symbol( pm_trax::Module::FORWARD ) ) );
}
