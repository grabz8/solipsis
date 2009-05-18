#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include "GdalImage.h"
#include "ContextMap.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

pm_trax::ContextMap::ContextMap( const std::string& filename )
	: m_valueArray( NULL ), m_min( ( std::numeric_limits< float >::max )() ), m_max( 0 ), m_extents( Vec4d( std::numeric_limits< double >::max(), std::numeric_limits< double >::max(), -std::numeric_limits< double >::max(), -std::numeric_limits< double >::max() ) ), m_length( 0.0 ), m_direction( Vec3d( 0.0, 0.0, 0.0 ) )
{}

pm_trax::ContextMap::~ContextMap()
{
	if( m_valueArray != NULL )
		free( m_valueArray );
}

void pm_trax::ContextMap::load( const std::string& filename )
{
	GDALDataset* dataset = NULL;

    dataset = (GDALDataset *) GDALOpen( filename.c_str(), GA_ReadOnly );
    if( dataset == NULL ) {
		std::cerr << "ContextMap::load -> Could not open image: " << filename << std::endl;
    }
	
	m_size = Vec2< long >( dataset->GetRasterXSize(), dataset->GetRasterYSize() );
	double geotransform[6];
	dataset->GetGeoTransform( geotransform );
	m_pixelsize = Vec2d( geotransform[1], -geotransform[5] );
	m_extents = Vec4d( geotransform[0], geotransform[3]-m_pixelsize[1]*m_size[1], geotransform[0]+m_pixelsize[0]*m_size[0], geotransform[3] );

	assert( dataset->GetRasterCount() == 1 );
	GDALRasterBand* band = dataset->GetRasterBand( 1 );
    m_valueArray = ( float * )CPLMalloc( sizeof( float )*m_size[0]*m_size[1] );
    band->RasterIO( GF_Read, 0, 0, m_size[0], m_size[1], m_valueArray, m_size[0], m_size[1], GDT_Float32, 0, 0 );

	int i;
	for( i = 0; i < m_size[1]; i++ ) {
		int j;
		for( j = 0; j < m_size[0]; j++ ) {
			if( m_valueArray[j+i*m_size[0]] > m_max )
        		m_max = m_valueArray[j+i*m_size[0]];
       		if( m_valueArray[j+i*m_size[0]] < m_min )
       			m_min = m_valueArray[j+i*m_size[0]];
		}
	}

	GDALClose( dataset );
}

void pm_trax::ContextMap::computePeaks()
{
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> Computing density peaks with marching squares algorithm..." << std::endl;

	std::vector< Vec2d > positions;
	std::vector< double > values;
	long i;
	for( i = 0; i < m_size[1]; i++ ) {
		for( long j = 0; j < m_size[0]; j++ ) {
			positions.push_back( Vec2d( j, m_size[1]-i ) );
			values.push_back( m_valueArray[j+i*m_size[0]] );
		}
	}

	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> max : " << m_max << std::endl;
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> min : " << m_min << std::endl;
	
	pm_trax::MarchingSquares ms( m_size, positions, values );
	float value = 4*( m_max - m_min )/5;
	ms.compute( value );
	std::vector< pm_trax::MarchingSquares::Level > curves = ms.getResult();
		
	std::vector< OGRLinearRing* > linearRings;
	for( i = 0; i < curves.size(); i++ ) {
		int j;
		OGRLinearRing* lr = new OGRLinearRing;
		for( j = 0; j < curves[i].size(); j++ ){
			OGRPoint pt;
			pt.setX( m_extents[0] + curves[i][j].pt[0]*m_pixelsize[0] );
			pt.setY( m_extents[1] + curves[i][j].pt[1]*m_pixelsize[1] );
			pt.setZ( 0 );
			lr->addPoint( &pt );
		}
		if( lr->get_IsClosed() && lr->get_Area() > ( m_extents[2] - m_extents[0] ) ) {
			linearRings.push_back( lr );
			OGREnvelope env;
			lr->getEnvelope( &env );
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> envelope: " << env.MinX << " " << env.MinY << " " << env.MaxX << " " << env.MaxY << std::endl;
			Vec3d center( ( env.MinX + env.MaxX )/2, ( env.MinY + env.MaxY )/2, 0.0 );
			double area = lr->get_Area();
			double strength = area / value;
			double radius = ( env.MaxX - env.MinX )/2;
			m_valuePeaks.insert( std::make_pair< int, ContextMap::MapPeak* >( i, new ContextMap::MapPeak( center, strength, radius ) ) );
		}
//		else delete lr;
	}
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> " << linearRings.size() << " peaks found for density value: " << value << std::endl;
	std::map< int, ContextMap::MapPeak* >::iterator it;
	for( it = m_valuePeaks.begin(); it != m_valuePeaks.end(); it++ )
		pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> #" << it->first << " - position: " << it->second->position()[0] << " " << it->second->position()[1] << ", radius: " << it->second->radius() << ", value: " << it->second->value() << std::endl;
	
	pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::computePeaks -> Done." << std::endl;	
}


std::vector< int > pm_trax::ContextMap::availablePeaks( double weight )
{
	double epsilon = m_geom->length();
	std::vector< int > availables;
	std::map< int, ContextMap::MapPeak* >::iterator it;
	for( it = m_valuePeaks.begin(); it != m_valuePeaks.end(); it++ ) {
		if( ( it->second->position() - m_geom->position() ).length() < epsilon ) {
			it->second->setReached( weight, true );
			pm_utils::notify( pm_utils::PM_DEBUG ) << "ContextMap::availablePeaks -> PICK#" <<  it->first << " POS: " << it->second->position() << " REACHED."<< std::endl;
		}
		if( !it->second->reached( weight ) )
			availables.push_back( it->first );
	}
	return availables;
}

float pm_trax::ContextMap::peakValueAtPos( const Vec3d& pos )
{
	float value = 0.0;
	std::map< int, ContextMap::MapPeak* >::iterator it;
	for( it = m_valuePeaks.begin(); it != m_valuePeaks.end(); it++ ) {
		value += it->second->valueAt( pos ); 
	}
	return value;
}

Vec3d pm_trax::ContextMap::pix2pos( const Vec2< long >& pix )
{
	Vec3d pos( pix[0]*m_pixelsize[0]+m_extents[0],  m_extents[3]-pix[1]*m_pixelsize[1], valueAtPos( Vec3d( pix[0]*m_pixelsize[0]+m_extents[0],  m_extents[3]-pix[1]*m_pixelsize[1], 0.0 ) ) );
	pm_utils::notify( pm_utils::PM_VERBOSE_DEBUG ) << "ContextMap::pix2pos -> PIX: " << pix << ", POS: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
	return pos;
}

Vec2< long > pm_trax::ContextMap::pos2pix( const Vec3d& pos )
{
	Vec2< long > pix( ( pos[0]-m_extents[0] )/m_pixelsize[0], ( m_extents[3]-pos[1] )/m_pixelsize[1] );
	pm_utils::notify( pm_utils::PM_VERBOSE_DEBUG ) << "ContextMap::pos2pix -> POS: " << pos[0] << " " << pos[1] << " " << pos[2] << ", PIXEL: " << pix << std::endl;
	return pix;
}

float pm_trax::ContextMap::valueAtPos( const Vec3d& pos )
{
	Vec2< long > pix = pos2pix( pos );
	return m_valueArray[pix[0]+pix[1]*m_size[0]];
}

bool pm_trax::ContextMap::posInsideBounds( const Vec3d& p )
{
	if( p[0] < m_extents[0] || p[0] > m_extents[2] || p[1] < m_extents[1] || p[1] > m_extents[3] )
		return false;
	return true;
}

void pm_trax::ContextMap::dilate( OGRPolygon*pol, const double& distance )
{
	OGRLinearRing *lr = pol->getExteriorRing();
	unsigned int size = lr->getNumPoints();
	unsigned int i;
    for( i = 0; i < size-1; i++ ) { // dans une linear ring, premier et dernier points sont identiques.
		OGRPoint pt;
		lr->getPoint( ((size-1)+i-1)%(size-1), &pt );
		Vec3d a( pt.getX(), pt.getY(), pt.getZ() );
		lr->getPoint( i, &pt );
		Vec3d b( pt.getX(), pt.getY(), pt.getZ() );
		lr->getPoint( (i+1)%(size-1), &pt );
		Vec3d c( pt.getX(), pt.getY(), pt.getZ() );
	    Vec3d ba = a-b;
	    ba.normalize();
	    Vec3d bc = c-b;
	    bc.normalize();
	    Vec3d dilateVector = bc+ba;
	    dilateVector[2] = 0.0;
	    dilateVector.normalize(); 
		OGRPoint* p = new OGRPoint;
	    if( Linear::aligned( a, b, c, Linear::DegToRad ) ) {
			Vec3d dilatedPoint( b-Vec3d( bc[1], -bc[0], 0 )*distance );
			p->setX( dilatedPoint[0] ); p->setY( dilatedPoint[1] ); p->setZ( dilatedPoint[2] );
			lr->setPoint( i, p );
		}
	    else{
			if( ba.cross(bc)[2] > 0 ) {
				Vec3d dilatedPoint( b-dilateVector*distance );
				p->setX( dilatedPoint[0] ); p->setY( dilatedPoint[1] ); p->setZ( dilatedPoint[2] );
				lr->setPoint( i, p );
			}
			else if( ba.cross( bc )[2] < 0 ) {
				Vec3d dilatedPoint( b+dilateVector*distance );
				p->setX( dilatedPoint[0] ); p->setY( dilatedPoint[1] ); p->setZ( dilatedPoint[2] );
				lr->setPoint( i, p );
			}
			else {
				Vec3d dilatedPoint( b-Vec3d( bc[1], -bc[0], 0 )*distance );
				p->setX( dilatedPoint[0] ); p->setY( dilatedPoint[1] ); p->setZ( dilatedPoint[2] );
				lr->setPoint( i, p );
			}
		}
    }
	OGRPoint pt;
	lr->getPoint( 0, &pt );
	lr->setPoint( size-1, &pt ); // on remplace le dernier point par le premier dilaté

}
