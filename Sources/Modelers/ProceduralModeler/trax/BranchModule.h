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

#ifndef _BRANCH_MODULE_H_
#define _BRANCH_MODULE_H_

#include <stack> 

#include "Module.h"

namespace pm_trax {

/**
 * Definition of a L-System Branch Module
 * @author		Vincent Huiban
 */

class BranchModule : public Module 
{
	
public:

	/** Constructor. */
	BranchModule( int featureId, double weight, Module* attachedModule, bool up = true, const Symbol s = Module::module2symbol( NEW_BRANCH ) ) : Module( attachedModule, s ), m_featureId( featureId ), m_weight( weight ), m_up( up ) {};

	/** Destructor. */
	virtual ~BranchModule() {};
		
	virtual void produce( std::vector< Module* >& successors );
			
	double weight() { return m_weight; };
	
	void setFeatureId( int id ) { m_featureId = id; };
	
	void setPointIdInFeature( int id ) { m_pointIdInFeature = id; };
	
	int featureId() { return m_featureId; };

 	int pointIdInFeature() { return m_pointIdInFeature; };

protected:
							
	double m_weight;

	bool m_up;
	
	int m_featureId;
	
	int m_pointIdInFeature;
	
}; // BranchModule

}; // pm_trax

#endif // _BRANCH_MODULE_H_
