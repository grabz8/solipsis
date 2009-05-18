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

#ifndef _LOCAL_CONTEXT_FEATURE_H_
#define _LOCAL_CONTEXT_FEATURE_H_

#include <map>

#include <Linear/Linear.h>

#include "ContextFeature.h"
#include "ContextMap.h"

namespace pm_trax {

class LocalContextFeature : public ContextFeature
{
	
public:
		
	typedef enum { INTERSECTIONS, OBSTACLE } LocalContextFeatureType;

	LocalContextFeature();
	
	virtual ~LocalContextFeature() {};
		
	virtual void analyse( pm_trax::Geom* g );
	
	virtual Vec3d direction() { return m_direction; };

	virtual double length() { return m_length; };

protected:

	void addContextMap( ContextMap* m, std::map< double, double >& strengths ) { m_contextMaps.push_back( std::make_pair< ContextMap*, std::map< double, double > >( m, strengths ) ); };
	
	std::vector< std::pair< ContextMap*, std::map< double, double > > > m_contextMaps; 

	LocalContextFeatureType m_contextFeatureType;		

}; // LocalContextFeature

}; // pm_trax

#endif // _LOCAL_CONTEXT_FEATURE_H_
