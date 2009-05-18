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

#ifndef _ROTATE_MODULE_H_
#define _ROTATE_MODULE_H_

#include <map>
#include <string>

#include "Module.h"

namespace pm_trax {

/**
 * Definition of a L-System Terminal Rotate Module
 * @author		Vincent Huiban
 */

class RotateModule : public Module 
{
	
public:
		
	/** Constructor. */
	RotateModule( Module* attachedModule, double heading, double pitch = 0.0, const Symbol s = Module::module2symbol( ROTATE ) );
	
	/** Destructor. */
	virtual ~RotateModule() {};
			
	virtual void produce( std::vector< Module* >& successors );
	
protected:

	double m_heading;
	
	double m_pitch;
				
}; // RotateModule

}; // pm_trax

#endif // _ROTATE_MODULE_H_
