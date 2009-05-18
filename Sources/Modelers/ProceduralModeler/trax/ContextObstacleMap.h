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

#ifndef _CONTEXT_OBSTACLE_MAP_H_
#define _CONTEXT_OBSTACLE_MAP_H_

#include "ContextMap.h"

namespace pm_trax {

class ContextObstacleMap : public ContextMap
{
	
public:
	
	class MapAttributes {
		
	public:
		
		MapAttributes( const std::map< double, std::string >& sectionWeight2Type,
					   const std::map< double, double >& sectionWeight2DilatationDistance ) 
			: m_sectionWeight2Type( sectionWeight2Type ), m_sectionWeight2DilatationDistance( sectionWeight2DilatationDistance ) 
		{};
		
		virtual ~MapAttributes() {};

		std::map< double, std::string > m_sectionWeight2Type;

		std::map< double, double > m_sectionWeight2DilatationDistance;
		
	};

	/** Constructor. */
	ContextObstacleMap( const MapAttributes& dma, const std::string& filename = "" );

	/** Destructor. */
	virtual ~ContextObstacleMap();
		
	std::vector< OGRPolygon* > obstacles() { return m_obstacles; };

	virtual void analyse( pm_trax::Geom* g );

protected:
			
	Vec3d bestDirection();

	double bestLength();

	std::vector< Vec3d > exteriorRing( OGRPolygon* pol );

	void readObstaclesFromFile( const std::string& filename, bool force = false );

	void createObstacleMapFromObstacles( const std::vector< OGRPolygon* >& obstacles );
 
	void createObstacleMapFromObstaclesFile( const std::string& filename );
	
	std::vector< OGRPolygon* >  m_obstacles;

	MapAttributes m_attributes;
		
}; // ContextObstacleMap

}; // pm_trax

#endif // _CONTEXT_OBSTACLE_MAP_H_
