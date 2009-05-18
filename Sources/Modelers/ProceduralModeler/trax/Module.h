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

#ifndef _MODULE_H_
#define _MODULE_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "utils/Utils.h"

/**
 * Definition of a L-System symbol
 * @author		Vincent Huiban
 */

namespace pm_trax {

typedef char Symbol;
	
typedef std::string Alphabet;
typedef std::string Word;
typedef std::vector< Word > Sentence;

class Module {
	
public:
	
	typedef enum { CONTEXT,
				   FORWARD,
				   NEW_SEGMENT,
				   NEW_BRANCH,
				   BEGIN_BRANCH,
				   END_BRANCH,
				   ROTATE,
				   DEFAULT
	} ModuleMeaning;
	
	class RuleAttributes {
		
	public:
		
		RuleAttributes() {};
		
		virtual ~RuleAttributes() {};
		
	};
	
	/** Constructor. */
	Module( Module* attachedModule, const Symbol s = Module::module2symbol( Module::DEFAULT ) ) : m_symbol( s ), m_attachedModule( attachedModule ) {};

	/** Destructor. */
	virtual ~Module() {};
	
	void setSymbol( const Symbol s ) { m_symbol = s; };
	
	Symbol symbol() { return m_symbol; };
			
	void setLeftContext( Module* lc ) { m_leftContext = lc; };

	Module* leftContext() { return m_leftContext; };
	
	void setRightContext( Module* rc ) { m_rightContext = rc; };
	
	Module* rightContext() { return m_rightContext; };

	static void init();

	static const Symbol module2symbol( ModuleMeaning sm ) { return m_moduleSymbol[sm]; };
		
	virtual void produce( std::vector< Module* >& successors ) { successors.push_back( new Module( 0, m_symbol ) ); };
			
	Module* attachedModule() { return m_attachedModule; };
	
protected:
	
	/** 
	 * set of available symbol meanings
	 */
	static std::map< ModuleMeaning, Symbol > m_moduleSymbol;
	
	Symbol m_symbol;
		
	Module* m_leftContext;

	Module* m_rightContext;
	
	Module* m_attachedModule;
		
}; // Module

}; // pm_trax

#endif // _MODULE_H_
