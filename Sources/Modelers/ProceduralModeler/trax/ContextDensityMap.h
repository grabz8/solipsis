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

#ifndef _CONTEXT_DENSITY_MAP_H_
#define _CONTEXT_DENSITY_MAP_H_

#include <map>

#include "ContextMap.h"

namespace pm_trax {

class ContextDensityMap : public ContextMap
{
	
public:
	
	class MapAttributes {
		
	public:
		
		MapAttributes( const std::map< double, double >& section2AngleToPeak, 
				       const std::map< double, double >& section2DistanceToPeak,
					   const std::map< double, double >& section2MinValue,
					   const std::map< double, std::string >& section2Type ) : m_section2AngleToPeak( section2AngleToPeak ), m_section2DistanceToPeak( section2DistanceToPeak ), m_section2MinValue( section2MinValue ), m_section2Type( section2Type ) {};
		
		virtual ~MapAttributes() {};
		
		std::map< double, double > m_section2AngleToPeak;
		
		std::map< double, double > m_section2DistanceToPeak;
		
		std::map< double, double > m_section2MinValue;

		std::map< double, std::string > m_section2Type;
		
	};

	/** Constructor. */
	ContextDensityMap( const MapAttributes& dma, const std::string& filename = "" );

	/** Destructor. */
	virtual ~ContextDensityMap();
		
	std::vector< Vec3d > iosPositions() { return m_iosPositions; };

	std::vector< OGRPolygon* > cellsFootprints() { return m_cellsFootprints; };

	virtual void analyse( pm_trax::Geom* g );

protected:

	ContextMap::MapPeak* bestPeak();
		
	Vec3d bestPeakDirection( pm_trax::Geom* g );

	int nearestPointIdInFeature( int featureId, const Vec3d& point, int fromId = 0 );

	Vec3d bestFollowingValueDirection( pm_trax::Geom* g );

	Vec3d bestDirection();

	double bestLength() { return m_geom->length(); };

	void readPositionsFromFile( const std::string& filename, bool force = false );

	void createDensityMapFromPositions( const std::vector< Vec3d >& positions );
 
	void createDensityMapFromPositionsFile( const std::string& filename );

	void adjustPeaksPosition();

	std::vector< Vec3d > m_iosPositions;
	
	std::vector< OGRPolygon* > m_cellsFootprints;

	MapAttributes m_attributes;
		
}; // ContextDensityMap

}; // pm_trax

#endif // _CONTEXT_DENSITY_MAP_H_
