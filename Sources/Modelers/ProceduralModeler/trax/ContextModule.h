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

#ifndef _CONTEXT_MODULE_H_
#define _CONTEXT_MODULE_H_

#include <cassert>

#include "Module.h"
#include "ContextAgent.h"

namespace pm_trax {

class ContextModule : public Module
{
	
public:
	
	typedef enum { UNASSIGNED, FAILED, SUCCESSFUL } State;
	
	typedef enum { GLOBAL, LOCAL } Type;
	
	/** Constructor. */
	ContextModule( Module* attachedModule, const Symbol s = Module::module2symbol( CONTEXT ) ) : Module( attachedModule, s ), m_globalState( UNASSIGNED ), m_localState( UNASSIGNED ), m_heading( 0.0 ), m_pitch( 0.0 ), m_length( 0.0 ) { /*pm_utils::notify( pm_utils::PM_DEBUG ) << "NEW CONTEXT MODULE @: " << this << std::endl;*/ };

	ContextModule( const ContextModule& right ) : Module( right.m_attachedModule, right.m_symbol ) { *this = right; /*pm_utils::notify( pm_utils::PM_DEBUG ) << "NEW CONTEXT MODULE (COPY) @: " << this << std::endl;*/ };
	
	ContextModule& operator=( const ContextModule& right ) { m_globalState = right.m_globalState; m_localState = right.m_localState; m_heading = right.m_heading; m_pitch = right.m_pitch; m_length = right.m_length; return( *this ); };
	
	/** Destructor. */
	virtual ~ContextModule() {};
	
	/** */
	State globalState() { return m_globalState; };
	
	State localState() { return m_localState; };
			
	virtual void produce( std::vector< Module* >& successors );
		
	void setHeading( double h ) { m_heading = h; };
	
	double heading() { return m_heading; };

	void setPitch( double p ) { m_pitch = p; };
	
	double pitch() { return m_pitch; };

	void setLength( double l ) { m_length = l; };
	
	double length() { return m_length; };

	/** Create / refine geometric attributes from Global / Local context */
	void execute( Type t, Module* module = NULL );

	std::pair< bool, bool > requestForBranches();

protected:	

	/** Create geometry attributes */
	bool create();
	
	/** Refine geometry attributes */
	bool refine();
		
	State m_globalState;
	
	State m_localState;
		
	double m_heading;
	
	double m_pitch;
	
	double m_length;
				
}; // ContextModule

}; // pm_trax

#endif // _CONTEXT_MODULE_H_
