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

#ifndef _CONTEXT_AGENT_H_
#define _CONTEXT_AGENT_H_

#include <cassert>

#include "GeomBuilder.h"
#include "GlobalContextFeature.h"
#include "LocalContextFeature.h"

namespace pm_trax {

class ContextAgent
{
	
public:
	
	/** Constructor */
	ContextAgent() : m_geom( NULL ), m_direction( Vec3d( 0, 0, 0 ) ), m_length( 0.0 ) {};
	
	/** Destructor. */
	virtual ~ContextAgent() {};
				
	static ContextAgent* instance();
	
	static void setGlobalContextFeature( GlobalContextFeature* f ) { m_globalContextFeature = f; };

	static GlobalContextFeature* globalContextFeature() { return m_globalContextFeature; };

	static void setLocalContextFeature( LocalContextFeature* f ) { m_localContextFeature = f; };
	
	static LocalContextFeature* localContextFeature() { return m_localContextFeature; };
	
	bool throwGlobal();

	bool throwLocal();
	
	bool requestForLeftBranch();

	bool requestForRightBranch();

	void setGeom( pm_trax::Geom* g ) { m_geom = g; };
	
	pm_trax::Geom* geom() { return m_geom; };

	void setDirection( Vec3d direction ) { m_direction = direction; };
	
	Vec3d direction() { return m_direction; };

	void setLength( double length ) { m_length = length; };
	
	double length() { return m_length; };
	
protected:	
	
	static GlobalContextFeature* m_globalContextFeature;

	static LocalContextFeature*  m_localContextFeature;
	
	pm_trax::Geom* m_geom;

	Vec3d m_direction;
	
	double m_length;
	
}; // ContextAgent

}; // pm_trax

#endif // _CONTEXT_AGENT_H_
