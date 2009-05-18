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

#include <stack>

#include "BeginBranchModule.h"
#include "BranchModule.h"
#include "EndBranchModule.h"
#include "SegmentModule.h"
#include "ContextModule.h"

void pm_trax::BranchModule::produce( std::vector< pm_trax::Module* >& modules )
{
	pm_trax::BeginBranchModule* beginBranchModule = new pm_trax::BeginBranchModule( m_attachedModule, m_up, pm_trax::Module::module2symbol( pm_trax::Module::BEGIN_BRANCH ) ); 
	modules.push_back( beginBranchModule );

	pm_trax::SegmentModule* segmentModule = new pm_trax::SegmentModule( NULL, featureId(), weight(), beginBranchModule, pm_trax::Module::module2symbol( pm_trax::Module::NEW_SEGMENT ) );
	modules.push_back( segmentModule );
	modules.push_back( new pm_trax::ContextModule( segmentModule, pm_trax::Module::module2symbol( pm_trax::Module::CONTEXT ) ) );
	modules.push_back( new pm_trax::EndBranchModule( m_attachedModule, m_up, pm_trax::Module::module2symbol( pm_trax::Module::END_BRANCH ) ) );
}
