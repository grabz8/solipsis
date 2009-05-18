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

#ifndef _SEGMENT_MODULE_H_
#define _SEGMENT_MODULE_H_

#include <map>
#include <string>

#include "Module.h"

namespace pm_trax {

/**
 * Definition of a L-System Forward Module
 * @author		Vincent Huiban
 */

class SegmentModule : public Module 
{
	
public:
		
	/** Constructor. */
	SegmentModule( SegmentModule* prevSegmentModule, int featureId, double weight, Module* attachedGeomModule, const Symbol s = Module::module2symbol( NEW_SEGMENT ) ) : Module( attachedGeomModule, s ),  m_prevSegmentModule( prevSegmentModule ), m_featureId( featureId ), m_weight( weight ) { /*std::cout << "NEW SEGMENT MODULE @: " << this << std::endl;*/ };

	/** Destructor. */
	virtual ~SegmentModule() {};
			
	virtual void produce( std::vector< Module* >& successors );
		
	void setWeight( double weight ) { m_weight = weight; };
	
	double weight() { return m_weight; };
		
	void setFeatureId( int id ) { m_featureId = id; };
	
	void setPointIdInFeature( int id ) { m_pointIdInFeature = id; };
	
	int featureId() { return m_featureId; };

 	int pointIdInFeature() { return m_pointIdInFeature; };

protected:
		
	double m_weight;
	
	int m_featureId;
	
	int m_pointIdInFeature;

	SegmentModule* m_prevSegmentModule;
	
}; // SegmentModule

}; // pm_trax

#endif // _SEGMENT_MODULE_H_
