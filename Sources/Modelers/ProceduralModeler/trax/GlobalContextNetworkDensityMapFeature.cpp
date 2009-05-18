#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <limits>

#include <ar/arGdalImage.h>

#include "GlobalContextNetworkDensityMapFeature.h"
#include "MarchingSquares.h"

template< typename T >
inline bool ISNAN( T value ) { return value != value; }

template< typename T >
inline bool ISINF( T value ) { return value == std::numeric_limits< T >::infinity(); }

GlobalContextNetworkDensityMapFeature::GlobalContextNetworkDensityMapFeature( const DensityMapAttributes& attributes, const std::string& filename )
	: GlobalContextFeature( GlobalContextFeature::DENSITY ), m_attributes( attributes ), m_densityArray( NULL ), m_min( (std::numeric_limits< double >::max)() ), m_max( 0 )
{
	std::cout << "GLOBAL CONTEXT COMPUTING..." << std::endl;
	// read positions (cells/ios OGR datasource), // get bounding box, fill data strucure
	// create a density image with bounding box size
	createDensityImageFromPositions( filename );

	// compute min & max
	// compute marching squares for an efficient value so find peaks
	computeDensityPeaks();
	
	m_weight2method.insert( std::pair< double, DensityMapMethod >( 2, GlobalContextNetworkDensityMapFeature::DENSITY_PEAK_DIRECTION ) );
	m_weight2method.insert( std::pair< double, DensityMapMethod >( 1, GlobalContextNetworkDensityMapFeature::DENSITY_LINES ) );
	
	std::cout << "DONE." << std::endl;

}

GlobalContextNetworkDensityMapFeature::~GlobalContextNetworkDensityMapFeature()
{}

void GlobalContextNetworkDensityMapFeature::createDensityImageFromPositions( const std::string& filename )
{

	readPositionsFromFile( filename );
		
	m_pixelsize = arVec2d( 10.0, 10.0 );

	std::cout << "-> Creating a density map from positions..." << std::endl;
	arVec2< long > size( ( long )( ( m_extents[2] - m_extents[0] ) / m_pixelsize[0] ), ( long )( ( m_extents[3] - m_extents[1] ) / m_pixelsize[1] ) );
	if( m_densityArray != NULL )
		delete [] m_densityArray;
	m_densityArray = new float[size[0]*size[1]];
    long i;
    for( i = 0; i < size[1]; i++ ) {
    	long j; 
        for( j = 0; j < size[0]; j++ ) {
        	int k;
			m_densityArray[j+i*size[0]] = 0;
        	for( k = 0; k < m_iosPositions.size(); k++ ) {
        		arVec3d pixelPosition( j*m_pixelsize[0]+m_extents[0], m_extents[3]-i*m_pixelsize[1], 0 );
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
       			m_densityArray[j+i*size[0]] += ( double )1/sqrt(distance);
        		if( m_densityArray[j+i*size[0]] > m_max )
        			m_max = m_densityArray[j+i*size[0]];
        		if( m_densityArray[j+i*size[0]] < m_min )
        			m_min = m_densityArray[j+i*size[0]];
        	}
//        	m_densityArray[j+i*size[0]] /= m_iosPositions.size();
            std::cout << "pixel [" << j << ", " << i << "]: " << m_densityArray[j+i*size[0]] << "\r" << std::flush;  
        }
    }

    GDALAllRegister();
    const char *pszFormat = "GTiff";
    GDALDriver *driver;
    driver = GetGDALDriverManager()->GetDriverByName( pszFormat );
    
	GDALDataset *dataset;       
    char **papszOptions = NULL;
    dataset = driver->Create( "map.tif", size[0], size[1], 1, GDT_Float32, papszOptions );
    
    double adfGeoTransform[6] = { m_extents[0], m_pixelsize[0], 0, m_extents[3], 0, -m_pixelsize[1] };
    dataset->SetGeoTransform( adfGeoTransform );
        	
    GDALRasterBand *band;
    band = dataset->GetRasterBand(1);
    band->RasterIO( GF_Write, 0, 0, size[0], size[1], m_densityArray, size[0], size[1], GDT_Float32, 0, 0 );    

    std::cout << "Wrote a " << size[0] << "x" << size[1] << " image with extents: (xmin ymin xmax ymax)=(" << m_extents << ")." << std::endl;
	std::cout << "-> Done." << std::endl;
	
    delete dataset;
}


void GlobalContextNetworkDensityMapFeature::computeDensityPeaks()
{
	std::cout << "-> Computing density peaks with marching squares algorithm..." << std::endl;

	arVec2< long > size( ( long )( ( m_extents[2] - m_extents[0] ) / m_pixelsize[0] ), ( long )( ( m_extents[3] - m_extents[1] ) / m_pixelsize[1] ) );
	std::vector< arVec2d > positions;
	std::vector< double > values;
	long i;
	for( i = 0; i < size[1]; i++ ) {
		for( long j = 0; j < size[0]; j++ ) {
			positions.push_back( arVec2d( j, size[1]-i ) );
			values.push_back( m_densityArray[j+i*size[0]] );
		}
	}

	std::cout << "max : " << m_max << std::endl;
	std::cout << "min : " << m_min << std::endl;
	
	MarchingSquares ms( size, positions, values );
	float value = 4*( m_max - m_min )/5;
	ms.compute( value );
	std::vector< MarchingSquares::Level > curves = ms.getResult();
		
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
			std::cout << "envelope: " << env.MinX << " " << env.MinY << " " << env.MaxX << " " << env.MaxY << std::endl;
			arVec3d center( ( env.MinX + env.MaxX )/2, ( env.MinY + env.MaxY )/2, 0.0 );
			double area = lr->get_Area();
			double strength = area / value;
			double radius = ( env.MaxX - env.MinX )/2;
			m_densityPeaks.insert( std::make_pair< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >( i, new GlobalContextNetworkDensityMapFeature::DensityPeak( center, strength, radius ) ) );
		}
		else delete lr;
	}
	std::cout << linearRings.size() << " peaks found for density value: " << value << std::endl;
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >::iterator it;
	for( it = m_densityPeaks.begin(); it != m_densityPeaks.end(); it++ )
		std::cout << "#" << it->first << " - position: " << it->second->position() << ", radius: " << it->second->radius() << ", density: " << it->second->density() << std::endl;
	
	std::cout << "-> Done." << std::endl;	
}

void GlobalContextNetworkDensityMapFeature::analyse( Geom* g )
{
	m_geom = g;
	m_direction = bestDirection( m_weight2method[g->width()] );
	m_length = bestLength();
}

GlobalContextNetworkDensityMapFeature::DensityPeak* GlobalContextNetworkDensityMapFeature::bestDensityPeak()
{
	double epsilon = 100.0;
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >::iterator it = m_densityPeaks.begin();
	double minDistance = ( std::numeric_limits< double >::max )();
	int minDistancePeakId = ( std::numeric_limits< int >::max )();
	for( it = m_densityPeaks.begin(); it != m_densityPeaks.end(); it++ ) {
		if( !it->second->reached( ( int )m_geom->width() ) ) {
			double distance = ( m_geom->position() - it->second->position() ).length();
			if( distance < minDistance ) {
				minDistance = distance;
				minDistancePeakId = it->first;
			}
			if( distance < epsilon ){
				it->second->setReached( ( int )m_geom->width(), true );
				it = m_densityPeaks.begin();				
			}
		}
	}
	if( minDistancePeakId == ( std::numeric_limits< int >::max )() ){
		return NULL;
	}
	return m_densityPeaks[minDistancePeakId];
}

arVec3d GlobalContextNetworkDensityMapFeature::bestDirection( DensityMapMethod& m )
{
	// prerequisites: analyse( geom ) has been called
	assert( m_geom != NULL );
	
	switch( m ) {
	case GlobalContextNetworkDensityMapFeature::DENSITY_PEAK_DIRECTION: 
	{	
		arVec3d geomPosition( m_geom->position() );
		arVec3d geomDirection( m_geom->length()*cos( m_geom->heading() )*cos( m_geom->pitch() ),
							   m_geom->length()*sin( m_geom->heading() )*cos( m_geom->pitch() ),
							   m_geom->length()*sin( m_geom->pitch() ) );
		geomDirection.normalize();
		std::cout << "Geom Direction: " << geomDirection << std::endl;
	
		std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >::iterator it = m_densityPeaks.begin();
		double smallestAngle = ( std::numeric_limits<double>::max )();
		int smallestAnglePeakId = ( std::numeric_limits<int>::max )();
		arVec3d smallestAngleDirection = arVec3d( 0, 0, 0 );
		for( it = m_densityPeaks.begin(); it != m_densityPeaks.end(); it++ ) {
			std::vector< int > availables = availablePeaks( ( int )m_geom->width() );
			if( std::find( availables.begin(), availables.end(), it->first ) != availables.end() ) {
				arVec3d geomToPeakDirection = it->second->position() - geomPosition;
				geomToPeakDirection.normalize();
				std::cout << "Geom To Peak Direction: " << geomToPeakDirection << std::endl;
				double angle = acos( geomDirection.dot( geomToPeakDirection ) );
				angle = ISNAN( angle ) ? 0 : ( ISINF( angle ) ? 0 : angle );
				std::cout << "ANGLE BETWEEN GEOM AND PEAK: " << angle* arLinear::RadToDeg << std::endl;
				if( angle < smallestAngle && angle < m_attributes.m_sectionWeightAngleToPeak[m_geom->width()] ) {
					smallestAngle = angle;
					smallestAnglePeakId = it->first;
					smallestAngleDirection = geomToPeakDirection;
				}
			}
		}
		std::cout << "CHOOSED ANGLE, PEAK, DIRECTION: " << smallestAngle*arLinear::RadToDeg << ", " << smallestAnglePeakId << ", " << smallestAngleDirection << std::endl; 
		return smallestAngleDirection;
		break;
	}
	case GlobalContextNetworkDensityMapFeature::DENSITY_LINES:
	{
		break;
	}
	}
	return arVec3d( 0, 0, 0 );
}

std::vector< int > GlobalContextNetworkDensityMapFeature::availablePeaks( double weight )
{
	double epsilon = m_geom->length();
	std::vector< int > availables;
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >::iterator it = m_densityPeaks.begin();
	for( it = m_densityPeaks.begin(); it != m_densityPeaks.end(); it++ ) {
		if( ( it->second->position() - m_geom->position() ).length() < epsilon ) {
			it->second->setReached( weight, true );
			std::cout << "PICK#" <<  it->first << " POS: " << it->second->position() << " REACHED."<< std::endl;
		}
		if( !it->second->reached( weight ) )
			availables.push_back( it->first );
	}
	return availables;
}

double GlobalContextNetworkDensityMapFeature::densityAtPos( const arVec3d& pos )
{
	double density = 0.0;
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* >::iterator it = m_densityPeaks.begin();
	for( it = m_densityPeaks.begin(); it != m_densityPeaks.end(); it++ ) {
		density += it->second->densityAt( pos ); 
	}
	return density;
}
