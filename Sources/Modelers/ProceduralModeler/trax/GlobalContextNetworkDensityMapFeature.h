#ifndef _GLOBAL_CONTEXT_NETWORK_DENSITY_MAP_FEATURE_H_
#define _NETWORK_DENSITY_MAP_GLOBAL_CONTEXT_H_

#include <map>

#include <gdal.h>
#include <gdal_priv.h>

//#include <ogrsf_frmts.h>
//#include <cpl_conv.h>
//#include <cpl_string.h>
//#include <cpl_multiproc.h>

#include <arLinear/arLinear.h>

#include "GlobalContextFeature.h"

class GlobalContextNetworkDensityMapFeature : public GlobalContextFeature
{
	
public:
	
	typedef enum DensityMapMethod {
		DENSITY_PEAK_DIRECTION,
		DENSITY_LINES
	};

	class DensityMapAttributes {
		
	public:
		
		DensityMapAttributes( const std::map< double, double >& sectionWeightAngleToPeak, 
							  const std::map< double, double >& sectionWeightDistanceToPeak,
							  const std::map< double, double >& sectionWeightMinDensity ) : m_sectionWeightAngleToPeak( sectionWeightAngleToPeak ), m_sectionWeightDistanceToPeak( sectionWeightDistanceToPeak ), m_sectionWeightMinDensity( sectionWeightMinDensity) {};
		
		virtual ~DensityMapAttributes() {};
		
		std::map< double, double > m_sectionWeightAngleToPeak;
		
		std::map< double, double > m_sectionWeightDistanceToPeak;
		
		std::map< double, double > m_sectionWeightMinDensity;
		
	};
	
	class DensityPeak {
		
	public:
		
		DensityPeak( const arVec3d& pos, double d, double r ) : m_position( pos ), m_density( d ), m_radius( r ) {};
		
		virtual ~DensityPeak() {};
		
		void setPosition( const arVec3d& pos ) { m_position = pos; };
		
		arVec3d position() { return m_position; };
		
		void setDensity( double d ) { m_density = d; };
		
		double density() { return m_density; };
		
		void setRadius( double r ) { m_radius = r; };
		
		double radius() { return m_radius; };
		
		void setReached( double weight, bool r ) { m_reached[weight] = r; };
		
		bool reached( double weight ) { return m_reached[weight]; };
		
		double densityAt( const arVec3d& pos ) { 		
			double distance = ( pos - m_position ).length();
			if( distance < m_radius )
				return( ( m_radius - distance ) / m_radius * m_density );
			return 0.0;
		}
		
	protected:
				
		arVec3d m_position;
		
		double m_density;
		
		double m_radius;
		
		std::map< int, bool > m_reached;
	
	}; 
		
	/** Constructor. */
	GlobalContextNetworkDensityMapFeature( const DensityMapAttributes& dma, const std::string& filename = "" );

	/** Destructor. */
	virtual ~GlobalContextNetworkDensityMapFeature();
			
	/** Fill data structures with relevant information */
	virtual void analyse( Geom* g );
	
	virtual arVec3d direction() { return m_direction; };
	
	virtual double length() { return m_length; }; 
	
		
protected:
			
	void createDensityImageFromPositions( const std::vector< arVec3d >& positions );
 
	void createDensityImageFromPositions( const std::string& filename );
	
	void computeDensityPeaks();
	
	arVec3d bestDirection( DensityMapMethod& method );
	
	double bestLength() { return m_geom->length(); };
	
	void computeDensityField();
	
	double densityAtPos( const arVec3d& pos );
	
	void update();

	std::vector< int > availablePeaks( double weight );

	GlobalContextNetworkDensityMapFeature::DensityPeak* bestDensityPeak();
		
	double m_angle;
		
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* > m_positionDependantDensityPeaks;
	
	std::map< int, GlobalContextNetworkDensityMapFeature::DensityPeak* > m_densityPeaks; // map of (density strength, position) peaks
	
	std::vector< arVec4d > m_densityField;
	
	DensityMapAttributes m_attributes;
	
	std::map< double, GlobalContextNetworkDensityMapFeature::DensityMapMethod > m_weight2method;
			
	double m_min;
	
	double m_max;
		
	arVec2d m_pixelsize;
	
	float* m_densityArray;
	
};

#endif // _GLOBAL_CONTEXT_NETWORK_DENSITY_MAP_FEATURE_H_
