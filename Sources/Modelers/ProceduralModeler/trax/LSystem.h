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

#ifndef _LSYSTEM_H_
#define _LSYSTEM_H_

#include <map>
#include <string>
#include <vector>

#include "Module.h"

/**
 * Generate a tree from a Parametric Stochastic Context-Dependant L-system.
 * @author		Vincent Huiban
 */

namespace pm_trax {

class LSystem {
	
public:
	
	/** Constructor. */	
	LSystem();

	/** Destructor. */
	virtual ~LSystem();
	
	void init();
	
	/**
	 * Generate the tree by applying the rules until the
	 * description has a given depth.
	 * @param		depth		maximal depth of tree until we continue to iterate.
	 */
	void iterate( unsigned int depth );
	
	 /** 
	  * Make the axiom of the L-system available, e.g. for
	  * interpreting the sentence using turtle graphics commands.
	  */
	std::vector< Module* > axiom() { return m_axiom; };

	/** 
	  * Make the current state of the L-system available, e.g. for
	  * interpreting the sentence using turtle graphics commands.
	  */
	std::vector< Module* > state( int i ) { return m_states[i]; };
	
	std::vector< std::vector < Module* > > states() { return m_states; };
	
	std::string asString( const std::vector< Module* > modules ) {
		std::string modulesAsString;
		for( unsigned int i = 0; i < modules.size(); i++ ) {
			modulesAsString += modules[i]->symbol();
		}
		return modulesAsString;
	}

protected:
		
	/** 
	 * Set of available symbol keys. 
	 */
	Alphabet m_alphabet;
	
	/**
	 * Initial configuration of the L-system. 
	 */
	std::vector< Module* > m_axiom;
		
	/** Container to hold the successive states of the L-system. */
	std::vector< std::vector< Module* > > m_states;
	
}; // LSystem

}; // pm_trax

#endif // _LSYSTEM_H_
