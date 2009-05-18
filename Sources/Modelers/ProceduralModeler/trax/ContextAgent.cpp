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

#include "ContextAgent.h"
#include "ContextModule.h"
#include "GlobalContextFeature.h"
#include "LocalContextFeature.h"

pm_trax::GlobalContextFeature* pm_trax::ContextAgent::m_globalContextFeature = NULL;
pm_trax::LocalContextFeature* pm_trax::ContextAgent::m_localContextFeature = NULL;

pm_trax::ContextAgent* theAgent = NULL;

pm_trax::ContextAgent* pm_trax::ContextAgent::instance()
{
  if( NULL == theAgent )
    theAgent = new pm_trax::ContextAgent;
  return theAgent;
}

bool pm_trax::ContextAgent::throwGlobal() 
{
	assert( m_geom != NULL );
	m_globalContextFeature->analyse( m_geom );
	if( m_globalContextFeature->direction() != Vec3d( 0, 0, 0) ) { 
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextAgent::throwGlobal DIRECTION: " << m_globalContextFeature->direction()[0] << " " << m_globalContextFeature->direction()[1] << " " << m_globalContextFeature->direction()[2] << std::endl;
		setDirection( m_globalContextFeature->direction() );
		setLength( m_globalContextFeature->length() );
		return true;
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextAgent::throwGlobal -> NULL DIRECTION" << std::endl;
	return false;
}

bool pm_trax::ContextAgent::throwLocal() 
{
	assert( m_geom != NULL );
	m_localContextFeature->analyse( m_geom );
	if( m_localContextFeature->direction() != Vec3d( 0, 0, 0 ) ) { 
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextAgent::throwLocal DIRECTION: " << m_localContextFeature->direction()[0] << " " << m_localContextFeature->direction()[1] << " " << m_localContextFeature->direction()[2] << std::endl;
		setDirection( m_localContextFeature->direction() );
		setLength( m_localContextFeature->length() );
		return true;
	}
	return false;
}

bool pm_trax::ContextAgent::requestForLeftBranch()
{
	return false;
}

bool pm_trax::ContextAgent::requestForRightBranch()
{
	return false;
}
