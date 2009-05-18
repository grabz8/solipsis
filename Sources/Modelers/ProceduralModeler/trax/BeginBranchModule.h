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

#ifndef _BEGIN_BRANCH_MODULE_H_
#define _BEGIN_BRANCH_MODULE_H_

#include <iostream>
#include <map>
#include <string>

#include "Module.h"

namespace pm_trax {

/**
 * Definition of a L-System Terminal Rotate Module
 * @author		Vincent Huiban
 */

class BeginBranchModule : public Module 
{
	
public:
		
	/** Constructor. */
	BeginBranchModule( Module* attachedGeomModule, bool up = true, const Symbol s = Module::module2symbol( BEGIN_BRANCH ) ) : Module( attachedGeomModule, s ), m_up( up ) { /*pm_utils::notify( pm_utils::PM_DEBUG ) << "NEW BEGIN BRANCH MODULE @:" << this << std::endl;*/ };

	/** Destructor. */
	virtual ~BeginBranchModule() {};
			
	virtual void produce( std::vector< Module* >& successors );
		
	bool isUp() { return m_up; };
	
protected:
	
	bool m_up;
		
}; // BeginBranchModule

}; // pm_trax

#endif // _BEGIN_BRANCH_MODULE_H_
