#ifndef _CONTEXT_MAP_H_
#define _CONTEXT_MAP_H_

#include <map>

#include <gdal.h>
#include <gdal_priv.h>

#include <Linear/Linear.h>

#include <utils/Utils.h>

#include "GeomBuilder.h"

namespace pm_trax {

class ContextMap
{
	
public:
		
	class MapPeak {
		
	public:
		
		MapPeak( const Vec3d& pos, float v, double r ) : m_position( pos ), m_value( v ), m_radius( r ) {};
		
		virtual ~MapPeak() {};
		
		void setPosition( const Vec3d& pos ) { m_position = pos; };
		
		Vec3d position() { return m_position; };
		
		void setValue( float v ) { m_value = v; };
		
		float value() { return m_value; };
		
		void setRadius( double r ) { m_radius = r; };
		
		double radius() { return m_radius; };
		
		void setReached( double weight, bool r ) { m_reached[weight] = r; };
		
		bool reached( double weight ) { return m_reached[weight]; };
		
		float valueAt( const Vec3d& pos ) { 		
			double distance = ( pos - m_position ).length();
			if( distance < m_radius )
				return( float( ( m_radius - distance ) / m_radius * m_value ) );
			return 0.0;
		}
		
	protected:
				
		Vec3d m_position;
		
		float m_value;
		
		double m_radius;
		
		std::map< double, bool > m_reached;
	
	}; // MapPeak
		
	/** Constructor. */
	ContextMap( const std::string& filename = "" );

	/** Destructor. */
	virtual ~ContextMap();

	float mapmin() { return m_min; };

	float mapmax() { return m_max; };

	void load( const std::string& mapname );	

	float peakValueAtPos( const Vec3d& pos );

	float valueAtPos( const Vec3d& pos );

	Vec4d extents() { return m_extents; };

	virtual void analyse( pm_trax::Geom* g ) = 0;

	Vec3d direction() { return m_direction; }

	double length() { return m_length; };

	std::vector< int > availablePeaks( double w );

	Vec2< long > pos2pix( const Vec3d& pos );

	Vec3d pix2pos( const Vec2< long >& pix );

	void setFeatureBestPeak( int featureId, int peakId ) { m_feature2peak[featureId] = peakId; };

	int bestPeakForFeature( int featureId ) { return( m_feature2peak.count( featureId ) > 0 ? m_feature2peak[featureId] : -1 ); };

	void setFeatureFollowingValue( int featureId, float value ) { m_feature2value[featureId] = value; };
	
	float followingValueForFeature( int featureId ) { return( m_feature2value.count( featureId ) > 0 ? m_feature2value[featureId] : -1 ); }; 

	void setFeatureFollowingValueRing( int featureId, OGRLinearRing* lr ) { m_feature2ring[featureId] = lr; };

	OGRLinearRing* followingValueRingForFeature( int featureId ) { return( m_feature2ring.count( featureId ) > 0 ? m_feature2ring[featureId] : NULL ); };

	void setFeatureFollowingIdInFeature( int featureId, int id ) { m_feature2id[featureId] = id; };
	
	int followingIdInFeature( int featureId ) { return( m_feature2id.count( featureId ) > 0 ? m_feature2id[featureId] : -1 ); }; 

	bool posInsideBounds( const Vec3d& p );

	/**
	 * Makes a dilatation on this contour
	 * @param distance The distance to dilate
	 */
	void dilate( OGRPolygon* p, const double& distance);

protected:
				
	void computePeaks();

	pm_trax::Geom* m_geom;
	
	std::map< int, MapPeak* > m_valuePeaks; // map of index, (value strength, position) peaks
					
	float m_min;
	
	float m_max;

	Vec2< long > m_size;

	Vec4d m_extents;
	
	Vec2d m_pixelsize;
	
	float* m_valueArray;

	Vec3d m_direction;

	double m_length;

	std::map< int, int > m_feature2peak;
			
	std::map< int, float > m_feature2value;

	std::map< int, OGRLinearRing* > m_feature2ring;

	std::map< int, int > m_feature2id;


}; // ContextMap

}; //pm_trax

#endif // _CONTEXT_MAP_H_
