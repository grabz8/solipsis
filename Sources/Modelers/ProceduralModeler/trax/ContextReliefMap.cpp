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

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include "ContextReliefMap.h"
#include "GdalImage.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::ContextReliefMap::ContextReliefMap( const pm_trax::ContextReliefMap::MapAttributes& attributes, const std::string& filename )
	: ContextMap( filename ), m_attributes( attributes )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "CONTEXT RELIEF MAP LOADING..." << std::endl;
	load( filename );
	// compute min & max
	// compute marching squares for an efficient value so find peaks
	computePeaks();
}

pm_trax::ContextReliefMap::~ContextReliefMap()
{}

void pm_trax::ContextReliefMap::analyse( pm_trax::Geom* g ) 
{
	m_geom = g;
	m_direction = bestDirection();
	m_length = bestLength();
}

double pm_trax::ContextReliefMap::bestLength()
{
	return 0.0;
}

Vec3d pm_trax::ContextReliefMap::bestPeakDirection( pm_trax::Geom* g ) 
{
	Vec3d geomPosition( g->position() );
	Vec3d geomDirection( g->length()*cos( g->heading() )*cos( g->pitch() ),
						   g->length()*sin( g->heading() )*cos( g->pitch() ),
						   g->length()*sin( g->pitch() ) );
	geomDirection.normalize();
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> Geom Direction: " << geomDirection << std::endl;

	double smallestAngle = ( std::numeric_limits<double>::max )();
	int smallestAnglePeakId = ( std::numeric_limits<int>::max )();
	Vec3d smallestAngleDirection = Vec3d( 0, 0, 0 );

	int peakId = bestPeakForFeature( g->featureId() );
	std::vector< int > availables = availablePeaks( ( int )g->width() );
	if( peakId != -1 && std::find( availables.begin(), availables.end(), peakId ) != availables.end() ) {
		Vec3d geomToPeakDirection = m_valuePeaks[peakId]->position() - geomPosition;
		geomToPeakDirection.normalize();
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> Geom To Peak Direction: " << geomToPeakDirection << std::endl;
		double angle = acos( geomDirection.dot( geomToPeakDirection ) );
		angle = ISNAN( angle ) ? 0 : ( ISINF( angle ) ? 0 : angle );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> ANGLE BETWEEN GEOM AND PEAK: " << angle* Linear::RadToDeg << std::endl;
		if( angle < smallestAngle ) { //&& angle < m_attributes.m_section2AngleToPeak[m_geom->width()] ) {
			smallestAngle = angle;
			smallestAnglePeakId = peakId;
			smallestAngleDirection = geomToPeakDirection;
		}
	}
	else {
		std::map< int, ContextMap::MapPeak* >::iterator it = m_valuePeaks.begin();
		for( it = m_valuePeaks.begin(); it != m_valuePeaks.end(); it++ ) {
			if( std::find( availables.begin(), availables.end(), it->first ) != availables.end() ) {
				Vec3d geomToPeakDirection = it->second->position() - geomPosition;
				geomToPeakDirection.normalize();
				pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextreliefMap::bestPeakDirection -> Geom To Peak Direction: " << geomToPeakDirection << std::endl;
				double angle = acos( geomDirection.dot( geomToPeakDirection ) );
				angle = ISNAN( angle ) ? 0 : ( ISINF( angle ) ? 0 : angle );
				pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> ANGLE BETWEEN GEOM AND PEAK: " << angle* Linear::RadToDeg << std::endl;
				if( angle < smallestAngle /* && angle < m_attributes.m_section2AngleToPeak[g->width()] */ ) {
					smallestAngle = angle;
					smallestAnglePeakId = it->first;
					setFeatureBestPeak( g->featureId(), it->first );
					smallestAngleDirection = geomToPeakDirection;
				}
			}
		}
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextreliefMap::bestPeakDirection -> CHOOSED ANGLE, PEAK, DIRECTION: " << smallestAngle*Linear::RadToDeg << ", " << smallestAnglePeakId << ", " << smallestAngleDirection << std::endl; 
	return smallestAngleDirection;
}

int pm_trax::ContextReliefMap::nearestPointIdInFeature( int featureId, const Vec3d& point, int fromId )
{
	int nearestPointId = -1;
	OGRLinearRing* lr = followingValueRingForFeature( featureId );
	assert( lr != NULL );
	int i = fromId;
	double minLength = ( std::numeric_limits< double >::max )();
	while( i < lr->getNumPoints() ) {
		OGRPoint pt;
		lr->getPoint( i, &pt );
		double distance = ( point-Vec3d( pt.getX(), pt.getY(), pt.getZ() ) ).length();
		if(  distance < minLength ) {
			minLength = distance;
			nearestPointId = i;
		}
		i++;
	}
	return nearestPointId;
}

Vec3d pm_trax::ContextReliefMap::bestFollowingValueDirection( pm_trax::Geom* g )
{
	OGRLinearRing* lr = followingValueRingForFeature( g->featureId() );
	//if( lr == NULL ) { // LIGNE DE DIRECTION PAS ENCORE CALCULEE
		float value = valueAtPos( g->position() );
		setFeatureFollowingValue( g->featureId(), value );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> DENSITY AT POS: " << g->position() << " IS: " << value << std::endl;

		std::vector< Vec2d > positions;
		std::vector< double > values;
		long i;
		for( i = 0; i < m_size[1]; i++ ) {
			for( long j = 0; j < m_size[0]; j++ ) {
				positions.push_back( Vec2d( j, m_size[1]-i ) );
				values.push_back( m_valueArray[j+i*m_size[0]] );
			}
		}		
		MarchingSquares ms( m_size, positions, values );
		ms.compute( value );
		std::vector< MarchingSquares::Level > curves = ms.getResult();

		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> MARCHING CURVES: " << curves.size() << std::endl;
		for( i = 0; i < curves.size() /* && idInFeature == -1 */; i++ ) {
			int j;
			int idInFeature = -1;
			OGRLinearRing* ring = new OGRLinearRing;
			for( j = 0; j < curves[i].size(); j++ ){
				OGRPoint pt;
				pt.setX( m_extents[0] + curves[i][j].pt[0]*m_pixelsize[0] );
				pt.setY( m_extents[1] + curves[i][j].pt[1]*m_pixelsize[1] );
				pt.setZ( 0 );
				ring->addPoint( &pt );
				Vec3d point = pix2pos( Vec2< long >( curves[i][j].pt[0], m_size[1]-curves[i][j].pt[1] ) );
				if( ( point-g->position() ).length() < m_pixelsize[0] ) {
					setFeatureFollowingIdInFeature( g->featureId(), j );
					idInFeature = followingIdInFeature( g->featureId() );

				}
			}
			if( idInFeature != -1 ) {
				setFeatureFollowingValueRing( g->featureId(), ring );
				lr = ring;
			}
			//else delete ring;
		}
	//}
	float amplitude = m_max - m_min;
	// LIGNE DEJA CALCULEE
	Vec3d direction( 0.0, 0.0, 0.0 );
	if( lr != NULL ) {
		Vec3d v0 = g->direction();
		int pId = nearestPointIdInFeature( g->featureId(), g->position() );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> POINTS IN LINEAR RING: " << lr->getNumPoints() << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> ID IN LINEAR RING: " << pId << std::endl;
		OGRPoint pt0;
		lr->getPoint( pId, &pt0 );
		if( lr->getNumPoints() > pId+1 ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> POINTS IN LINEAR RING: " << lr->getNumPoints() << std::endl;
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> ID IN LINEAR RING: " << pId << std::endl;
			OGRPoint pt1;
			lr->getPoint( pId+1, &pt1 );
			Vec3d  v1( pt1.getX()-pt0.getX(), pt1.getY()-pt0.getY(), pt1.getZ()-pt0.getZ() );
			if( v0.dot( v1 ) > 0 )
				direction = v1;
			else
				direction = -v1;
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestFollowingValueDirection -> DIRECTION: " << direction << std::endl;
		}
		direction.normalize();
	}
	direction *= followingValueForFeature( g->featureId() )/amplitude;
	return direction;
}

Vec3d pm_trax::ContextReliefMap::bestDirection()
{
	// prerequisites: analyse( geom ) has been called
	assert( m_geom != NULL );
	std::string type = m_attributes.m_section2Type[m_geom->width()];
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextReliefMap::bestDirection -> TYPE: " << type << " for width: " << m_geom->width() << std::endl;

	if( !posInsideBounds( m_geom->position() ) )
		return Vec3d( 0.0, 0.0, 0.0 );

	if( type == "PEAK_DIRECTION" ) {
		return bestPeakDirection( m_geom );
	}
	else if( type == "FOLLOW_VALUE_LINES" ) {
		return bestFollowingValueDirection( m_geom );
	}
	else if( type == "NO_VALUE_IMPACT" ) {
		return m_geom->direction();
	}
	return Vec3d( 0, 0, 0 );
}
