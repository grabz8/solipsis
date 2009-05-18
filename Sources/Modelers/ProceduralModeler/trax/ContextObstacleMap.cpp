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

#include "ContextObstacleMap.h"
#include "GdalImage.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::ContextObstacleMap::ContextObstacleMap( const pm_trax::ContextObstacleMap::MapAttributes& attributes, const std::string& filename )
	: pm_trax::ContextMap( filename ), m_attributes( attributes )
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::ContextObstacleMap -> CONTEXT OBSTACLE MAP COMPUTING..." << std::endl;
	createObstacleMapFromObstaclesFile( filename );
}

pm_trax::ContextObstacleMap::~ContextObstacleMap()
{}

void pm_trax::ContextObstacleMap::readObstaclesFromFile( const std::string& filename, bool forceReading )
{
	if( !forceReading && m_extents[0] < std::numeric_limits< double >::max() )
		return;
	
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::readObstaclesFromFile -> Reading obstacles polygons from file: " << filename << std::endl;
	OGRRegisterAll();

	OGRDataSource* datasource = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
	if( datasource == NULL ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::readObstaclesFromFile -> Open failed" << std::endl;
		exit( 1 );
	}
	
	OGRLayer* obstaclesLayer = datasource->GetLayerByName( "obstacles" );
	assert( obstaclesLayer != NULL );
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::readObstaclesFromFile -> " << obstaclesLayer->GetFeatureCount() << " obstacles footprints." << std::endl;
	   
	OGREnvelope oExt;	
	if( obstaclesLayer->GetExtent( &oExt, TRUE ) == OGRERR_NONE ) {
		m_extents[0] = oExt.MinX;
		m_extents[1] = oExt.MinY;
		m_extents[2] = oExt.MaxX;
		m_extents[3] = oExt.MaxY;
	}

	for( int i = 0; i < m_obstacles.size(); i++ )
		delete m_obstacles[i];
	m_obstacles.clear();

	OGRFeature* feature = NULL;
	obstaclesLayer->ResetReading();
	while( ( feature = obstaclesLayer->GetNextFeature() ) != NULL ) {
		
		OGRGeometry *geometry;
		geometry = feature->GetGeometryRef();
		if( geometry != NULL  && ( geometry->getGeometryType() == wkbPolygon || geometry->getGeometryType() == wkbPolygon25D ) ) {
			OGRPolygon* p = static_cast< OGRPolygon* >( geometry->clone() );
			dilate( p, 10.0 );
			m_obstacles.push_back( p );
		}
		OGRFeature::DestroyFeature( feature );
	}
	OGRDataSource::DestroyDataSource( datasource );
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::readObstaclesFromFile -> Done." << std::endl;
	
}

void pm_trax::ContextObstacleMap::createObstacleMapFromObstaclesFile( const std::string& filename )
{

	readObstaclesFromFile( filename );
		
	m_pixelsize = Vec2d( 10.0, 10.0 );

	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::createObstacleMapFromObstaclesFile -> Creating a obstacles map from obstacles..." << std::endl;
	Vec2< long > size( ( long )( ( m_extents[2] - m_extents[0] ) / m_pixelsize[0] ), ( long )( ( m_extents[3] - m_extents[1] ) / m_pixelsize[1] ) );
	if( m_valueArray != NULL )
		delete [] m_valueArray;
	m_valueArray = new float[size[0]*size[1]];
    long i;
    for( i = 0; i < size[0]*size[1]; i++ )
		m_valueArray[i] = 0;

	for( i = 0; i < m_obstacles.size(); i++ ){
		OGREnvelope env;
		m_obstacles[i]->getEnvelope( &env );
		int pxmin = ( env.MinX-m_extents[0] )/m_pixelsize[0];
		int pxmax = ( env.MaxX-m_extents[0] )/m_pixelsize[0];
		int pymax = ( m_extents[3]-env.MinY )/m_pixelsize[1];
		int pymin = ( m_extents[3]-env.MaxX )/m_pixelsize[1];
		for( int j = pxmin; j < pxmax; j++ ){
			for( int k = pymin; k < pymax; k++ ){
				Vec3d pixelPosition( j*m_pixelsize[0]+m_extents[0], m_extents[3]-k*m_pixelsize[1], 0 );
        		OGRPoint pp;
				pp.setX( pixelPosition[0] );
        		pp.setY( pixelPosition[1] );
        		pp.setZ( pixelPosition[2] );
        		if( m_obstacles[i]->Intersects( &pp ) )
					m_valueArray[j+k*size[0]] = 1;
			}
		}
    }

    GDALAllRegister();
    const char *pszFormat = "GTiff";
    GDALDriver *driver;
    driver = GetGDALDriverManager()->GetDriverByName( pszFormat );
    
	GDALDataset *dataset;       
    char **papszOptions = NULL;
    dataset = driver->Create( "obstaclemap.tif", size[0], size[1], 1, GDT_Float32, papszOptions );
    
    double adfGeoTransform[6] = { m_extents[0], m_pixelsize[0], 0, m_extents[3], 0, -m_pixelsize[1] };
    dataset->SetGeoTransform( adfGeoTransform );
        	
    GDALRasterBand *band;
    band = dataset->GetRasterBand(1);
    band->RasterIO( GF_Write, 0, 0, size[0], size[1], m_valueArray, size[0], size[1], GDT_Float32, 0, 0 );    

    pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::createObstacleMapFromObstaclesFile -> Wrote a " << size[0] << "x" << size[1] << " image with extents: (xmin ymin xmax ymax)=(" << m_extents << ")." << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::createObstacleMapFromObstaclesFile -> Done." << std::endl;
	
    delete dataset;
}

std::vector< Vec3d > pm_trax::ContextObstacleMap::exteriorRing( OGRPolygon* pol ) 
{
	std::vector< Vec3d > vertices;
	OGRLinearRing* extRing = pol->getExteriorRing();
	int i;
	for( i = 0; i < extRing->getNumPoints(); i++ ) {
		OGRPoint pt;
		extRing->getPoint( i, &pt );
		vertices.push_back( Vec3d( pt.getX(), pt.getY(), pt.getZ() ) );
	}
	return vertices;
}

void pm_trax::ContextObstacleMap::analyse( pm_trax::Geom* g ) 
{
	m_geom = g;
	m_direction = bestDirection();
	m_length = bestLength();
}

Vec3d pm_trax::ContextObstacleMap::bestDirection()
{
	Vec3d direction = m_geom->direction();

	if( m_extents[0] == std::numeric_limits< double >::max() )
		return Vec3d( 0.0, 0.0, 0.0 );


	// checks that geom does not intersect any cell in the world
	Vec3d pos0 = m_geom->previousPosition();

	if( !posInsideBounds( pos0 ) )
		return Vec3d( 0.0, 0.0, 0.0 );

	OGRPoint p0;
	p0.setX( pos0[0] ); p0.setY( pos0[1] ); p0.setZ( pos0[2] );

	Vec3d pos1 = m_geom->position();
	OGRPoint p1;
	p1.setX( pos1[0] ); p1.setY( pos1[1] );	p1.setZ( pos1[2] );

	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> POS0: " << pos0[0] << " " << pos0[1] << " " << pos0[2] << std::endl;
	
	int i = 0;
	bool hasIntersection = false;
	while( i < m_obstacles.size() && !hasIntersection ) {
		if( m_obstacles[i]->Intersects( &p0 ) ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> POS0: " << pos0[0] << " " << pos0[1] << " " << pos0[2] << std::endl;
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> SHOULD NOT INTERSECT. RETURNS." << std::endl;
			return Vec3d( 0.0, 0.0, 0.0 );
		}
		//if( m_obstacles[i]->Intersects( &p1 ) ) {
		//	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> GEOM P1 HAS AN INTERSECTION WITH OBTACLE" << std::endl;
		//	hasIntersection = true;
		//}
		OGRLineString ls;
		ls.addPoint( &p0 );
		ls.addPoint( &p1 );
		if( m_obstacles[i]->Intersects( &ls ) ) {
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> GEOM LS HAS AN INTERSECTION WITH OBTACLE" << std::endl;
			hasIntersection = true;
		}
		i++;
	}
	if( i == m_obstacles.size() && !hasIntersection ) {
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> POSITION OK" << std::endl;
		return direction;
	}
	else {
		i--;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> INTERSECTION AT POSITION: " << p1.getX() << " " << p1.getY() << " WITH OBSTACLE #" << i << std::endl;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> MOVING POINT TO A VALID POSITION" << std::endl;

		// determine on which side going round obstacle
		OGRPoint obstacleCenter;
		m_obstacles[i]->Centroid( &obstacleCenter );
		Vec3d currentDir = pos1 - pos0;
		currentDir.normalize();
		Vec3d obstacleDir = Vec3d( obstacleCenter.getX(), obstacleCenter.getY(), obstacleCenter.getZ() )-pos0;
		obstacleDir.normalize();
		int sign = ( ( currentDir.cross( obstacleDir ) )[2] > 0 ) ? -1 : 1;

		// angle step
		double dAngle = 0;
		OGRLineString ls;
		ls.addPoint( &p0 );
		ls.addPoint( &p1 );
		while ( m_obstacles[i]->Intersects( &ls ) ) {
			dAngle += sign*5*Linear::DegToRad;
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> POS1: " << pos1[0] << " " << pos1[1] << " " << pos1[2] << std::endl;
			pos1 = pos0+m_geom->length()*Vec3d( cos( m_geom->heading()+dAngle )*cos( m_geom->pitch() ),
												  sin( m_geom->heading()+dAngle )*cos( m_geom->pitch() ),
												  sin( m_geom->pitch() ) );
			p1.setX( pos1[0] );
			p1.setY( pos1[1] );
			p1.setZ( pos1[2] );
			ls.setPoint( 1, &p1 );
		}
		pos1 = pos0+m_geom->length()*Vec3d( cos( m_geom->heading()+dAngle )*cos( m_geom->pitch() ),
											  sin( m_geom->heading()+dAngle )*cos( m_geom->pitch() ),
											  sin( m_geom->pitch() ) );
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection MOVE ANGLE: " << dAngle*Linear::RadToDeg << std::endl;
		direction = pos1-pos0;
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> FINAL LENGTH = " << direction.length() << std::endl; 
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> FINAL POS0 = " << pos0 << std::endl; 
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> FINAL POS1 = " << pos1 << std::endl; 
		direction.normalize();
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextObstacleMap::bestDirection -> OBSTACLE MAP DIRECTION: " << direction << std::endl;	
	return direction;
}

double pm_trax::ContextObstacleMap::bestLength()
{
	return m_geom->length();
}
