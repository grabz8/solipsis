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

#include "ContextDensityMap.h"
#include "GdalImage.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::ContextDensityMap::ContextDensityMap( const pm_trax::ContextDensityMap::MapAttributes& attributes, const std::string& filename )
	: pm_trax::ContextMap( filename ), m_attributes( attributes )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::ContextDensityMap -> CONTEXT DENSITY MAP COMPUTING..." << std::endl;
	// read positions (cells/ios OGR datasource), // get bounding box, fill data strucure
	// create a density image with bounding box size
	createDensityMapFromPositionsFile( filename );

	// compute min & max
	// compute marching squares for an efficient value so find peaks
	computePeaks();

	adjustPeaksPosition();
}

pm_trax::ContextDensityMap::~ContextDensityMap()
{}

void pm_trax::ContextDensityMap::readPositionsFromFile( const std::string& filename, bool forceReading )
{
	if( !forceReading && m_extents[0] < std::numeric_limits< double >::max() )
		return;
	
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::readPositionsFromFile -> Reading cells and inputs/outputs positions..." << std::endl;
	OGRRegisterAll();

	OGRDataSource* datasource = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
	if( datasource == NULL ) {
		std::cerr << "ContextDensityMap::readPositionsFromFile -> Open failed" << std::endl;
		exit( 1 );
	}
	
	OGRLayer* cellsLayer = datasource->GetLayerByName( "cells" );
	OGRLayer* iosLayer = datasource->GetLayerByName( "ios" );
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::readPositionsFromFile -> " << cellsLayer->GetFeatureCount() << " cells footprints." << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::readPositionsFromFile -> " << iosLayer->GetFeatureCount() << " inputs/outputs positions." << std::endl;  
	   
	OGREnvelope oExt;	
	if( cellsLayer->GetExtent( &oExt, TRUE ) == OGRERR_NONE ) {
		m_extents[0] = oExt.MinX;
		m_extents[1] = oExt.MinY;
		m_extents[2] = oExt.MaxX;
		m_extents[3] = oExt.MaxY;
	}

	m_iosPositions.clear();
	OGRFeature *feature;
	iosLayer->ResetReading();
	while( ( feature = iosLayer->GetNextFeature() ) != NULL ) {
		
		OGRGeometry *geometry;
		geometry = feature->GetGeometryRef();
		if( geometry != NULL  && geometry->getGeometryType() == wkbLineString ) {
			OGRLineString* ls = static_cast< OGRLineString* >( geometry );
			Vec3d center( 0, 0, 0 );
			int i;
			for( i = 0; i < ls->getNumPoints(); i++ ) {
				OGRPoint pt; 
				ls->getPoint( i, &pt );
				center += Vec3d( pt.getX(), pt.getY(), pt.getZ() );
			}
			center /= ls->getNumPoints();
			m_iosPositions.push_back( center );
		}
		OGRFeature::DestroyFeature( feature );
	}
	
	for( int i = 0; i < m_cellsFootprints.size(); i++ )
		delete m_cellsFootprints[i];
	m_cellsFootprints.clear();
	
	cellsLayer->ResetReading();
	while( ( feature = cellsLayer->GetNextFeature() ) != NULL ) {
		
		OGRGeometry *geometry;
		geometry = feature->GetGeometryRef();
		if( geometry != NULL  && ( geometry->getGeometryType() == wkbPolygon || geometry->getGeometryType() == wkbPolygon25D ) ) {
			m_cellsFootprints.push_back( static_cast< OGRPolygon* >( geometry->clone() ) );
		}
		OGRFeature::DestroyFeature( feature );
	}
	OGRDataSource::DestroyDataSource( datasource );
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::readPositionsFromFile -> Done." << std::endl;
	
}

void pm_trax::ContextDensityMap::createDensityMapFromPositionsFile( const std::string& filename )
{

	readPositionsFromFile( filename );
		
	m_pixelsize = Vec2d( 10.0, 10.0 );

	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::createDensityMapFromPositionsFile -> Creating a density map from positions..." << std::endl;
	m_size = Vec2< long >( ( long )( ( m_extents[2] - m_extents[0] ) / m_pixelsize[0] ), ( long )( ( m_extents[3] - m_extents[1] ) / m_pixelsize[1] ) );
	if( m_valueArray != NULL )
		free( m_valueArray );
	m_valueArray = ( float * )CPLMalloc( sizeof( float )*m_size[0]*m_size[1] );
    long i;
    for( i = 0; i < m_size[1]; i++ ) {
    	long j; 
        for( j = 0; j < m_size[0]; j++ ) {
        	int k;
			m_valueArray[j+i*m_size[0]] = 0;
        	for( k = 0; k < m_iosPositions.size(); k++ ) {
        		Vec3d pixelPosition( j*m_pixelsize[0]+m_extents[0], m_extents[3]-i*m_pixelsize[1], 0 );
        		double distance = ( pixelPosition-m_iosPositions[k] ).length();
//        		// LEAVE CELLS EMPTY
        		//int l = 0;
        		//bool outsideCellsFootprints = true;
        		//while( outsideCellsFootprints && l < m_cellsFootprints.size() ) {
        		//	OGRPoint pp;
        		//	pp.setX( pixelPosition[0] );
        		//	pp.setY( pixelPosition[1] );
        		//	pp.setZ( pixelPosition[2] );
        		//	if( m_cellsFootprints[l]->Intersects( &pp ) )
        		//		outsideCellsFootprints = false;
        		//	l++;
        		//}
        		//if( !outsideCellsFootprints )
        		//	m_densityArray[j+i*size[0]] = 0;
        		//else
        		//	if( distance < m_pixelsize[0] ) distance = m_pixelsize[0];
        		// 
        		if( distance < 1 ) distance = 1;
       			m_valueArray[j+i*m_size[0]] += ( float )1/sqrt(distance);
        		if( m_valueArray[j+i*m_size[0]] > m_max )
        			m_max = m_valueArray[j+i*m_size[0]];
        		if( m_valueArray[j+i*m_size[0]] < m_min )
        			m_min = m_valueArray[j+i*m_size[0]];
        	}
//        	m_densityArray[j+i*size[0]] /= m_iosPositions.size();
            pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::createDensityMapFromPositionsFile -> pixel [" << j << ", " << i << "]: " << m_valueArray[j+i*m_size[0]] << "\r" << std::flush;  
        }
    }

    GDALAllRegister();
    const char *pszFormat = "GTiff";
    GDALDriver *driver;
    driver = GetGDALDriverManager()->GetDriverByName( pszFormat );
    
	GDALDataset *dataset;       
    char **options = NULL;
    dataset = driver->Create( "densitymap.tif", m_size[0], m_size[1], 1, GDT_Float32, options );
    
    double geotransform[6] = { m_extents[0], m_pixelsize[0], 0, m_extents[3], 0, -m_pixelsize[1] };
    dataset->SetGeoTransform( geotransform );
        	
    GDALRasterBand *band;
    band = dataset->GetRasterBand( 1 );
    band->RasterIO( GF_Write, 0, 0, m_size[0], m_size[1], m_valueArray, m_size[0], m_size[1], GDT_Float32, 0, 0 );    

    pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::createDensityMapFromPositionsFile -> Wrote a " << m_size[0] << "x" << m_size[1] << " image with extents: (xmin ymin xmax ymax)=(" << m_extents << ")." << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::createDensityMapFromPositionsFile -> Done." << std::endl;
	
    delete dataset;
}

void pm_trax::ContextDensityMap::adjustPeaksPosition()
{
	double dpos = 10;
	std::map< int, ContextMap::MapPeak* >::iterator peakIt;
	for( peakIt = m_valuePeaks.begin(); peakIt != m_valuePeaks.end(); peakIt++ ) {
		std::vector< OGRPolygon* >::iterator it;
		for( it = m_cellsFootprints.begin(); it != m_cellsFootprints.end(); it++ ) {
			OGRPoint pt( peakIt->second->position()[0], peakIt->second->position()[1] );
			while( ( *it )->Intersects( &pt ) ) {
				peakIt->second->setPosition( peakIt->second->position() + Vec3d( dpos, 0, 0 ) );
				pt.setX( peakIt->second->position()[0] );
				pt.setY( peakIt->second->position()[1] );
			}
		}
	}
}

pm_trax::ContextMap::MapPeak* pm_trax::ContextDensityMap::bestPeak()
{
	double epsilon = 100.0;
	std::map< int, ContextMap::MapPeak* >::iterator it = m_valuePeaks.begin();
	double minDistance = ( std::numeric_limits< double >::max )();
	int minDistancePeakId = ( std::numeric_limits< int >::max )();
	for( it = m_valuePeaks.begin(); it != m_valuePeaks.end(); it++ ) {
		if( !it->second->reached( ( int )m_geom->width() ) ) {
			double distance = ( m_geom->position() - it->second->position() ).length();
			if( distance < minDistance ) {
				minDistance = distance;
				minDistancePeakId = it->first;
			}
			if( distance < epsilon ){
				it->second->setReached( ( int )m_geom->width(), true );
				it = m_valuePeaks.begin();				
			}
		}
	}
	if( minDistancePeakId == ( std::numeric_limits< int >::max )() ){
		return NULL;
	}
	return m_valuePeaks[minDistancePeakId];
}

void pm_trax::ContextDensityMap::analyse( pm_trax::Geom* g ) 
{
	m_geom = g;
	m_direction = bestDirection();
	m_length = bestLength();
}

Vec3d pm_trax::ContextDensityMap::bestPeakDirection( pm_trax::Geom* g ) 
{
	Vec3d geomPosition( g->position() );
	Vec3d geomDirection( g->direction() );
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
				pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> Geom To Peak Direction: " << geomToPeakDirection << std::endl;
				double angle = acos( geomDirection.dot( geomToPeakDirection ) );
				angle = ISNAN( angle ) ? 0 : ( ISINF( angle ) ? 0 : angle );
				pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> ANGLE BETWEEN GEOM AND PEAK: " << angle* Linear::RadToDeg << std::endl;
				if( angle < smallestAngle && angle < m_attributes.m_section2AngleToPeak[g->width()] ) {
					smallestAngle = angle;
					smallestAnglePeakId = it->first;
					setFeatureBestPeak( g->featureId(), it->first );
					smallestAngleDirection = geomToPeakDirection;
				}
			}
		}
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestPeakDirection -> CHOOSED ANGLE, PEAK, DIRECTION: " << smallestAngle*Linear::RadToDeg << ", " << smallestAnglePeakId << ", " << smallestAngleDirection << std::endl; 
	return smallestAngleDirection;
}

int pm_trax::ContextDensityMap::nearestPointIdInFeature( int featureId, const Vec3d& point, int fromId )
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

Vec3d pm_trax::ContextDensityMap::bestFollowingValueDirection( pm_trax::Geom* g )
{
	OGRLinearRing* lr = followingValueRingForFeature( g->featureId() );
	if( lr == NULL ) { // LIGNE DE DIRECTION PAS ENCORE CALCULEE
		float value = valueAtPos( g->position() );
		setFeatureFollowingValue( g->featureId(), value );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> DENSITY AT POS: " << g->position() << " IS: " << value << std::endl;

		std::vector< Vec2d > positions;
		std::vector< double > values;
		long i;
		for( i = 0; i < m_size[1]; i++ ) {
			for( long j = 0; j < m_size[0]; j++ ) {
				positions.push_back( Vec2d( j, m_size[1]-i ) );
				values.push_back( m_valueArray[j+i*m_size[0]] );
			}
		}		
		pm_trax::MarchingSquares ms( m_size, positions, values );
		ms.compute( value );
		std::vector< pm_trax::MarchingSquares::Level > curves = ms.getResult();

		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> MARCHING CURVES: " << curves.size() << std::endl;
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
	}
	// LIGNE DEJA CALCULEE
	Vec3d direction( 0.0, 0.0, 0.0 );
	if( lr != NULL ) {
		int pId = nearestPointIdInFeature( g->featureId(), g->position() );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> POINTS IN LINEAR RING: " << lr->getNumPoints() << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> ID IN LINEAR RING: " << pId << std::endl;
		OGRPoint pt0;
		lr->getPoint( pId, &pt0 );
		if( lr->getNumPoints() > pId+1 ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> POINTS IN LINEAR RING: " << lr->getNumPoints() << std::endl;
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> ID IN LINEAR RING: " << pId << std::endl;
			OGRPoint pt1;
			lr->getPoint( pId+1, &pt1 );
			direction = Vec3d( pt1.getX()-pt0.getX(), pt1.getY()-pt0.getY(), pt1.getZ()-pt0.getZ() );
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestFollowingValueDirection -> DIRECTION: " << direction << std::endl;
		}
		direction.normalize();
	}
	return direction;
}

Vec3d pm_trax::ContextDensityMap::bestDirection()
{
	// prerequisites: analyse( geom ) has been called
	assert( m_geom != NULL );
	
	if( !posInsideBounds( m_geom->position() ) )
		return Vec3d( 0.0, 0.0, 0.0 );

	std::string type = m_attributes.m_section2Type[m_geom->width()];
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextDensityMap::bestDirection -> TYPE: " << type << "." << std::endl;

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
