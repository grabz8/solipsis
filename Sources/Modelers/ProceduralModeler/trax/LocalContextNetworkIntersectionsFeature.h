#ifndef _LOCAL_CONTEXT_NETWORK_INTERSECTIONS_FEATURE_H_
#define _LOCAL_CONTEXT_NETWORK_INTERSECTIONS_FEATURE_H_

#include <map>

#include <arLinear/arLinear.h>

#include "LocalContextFeature.h"

class LocalContextNetworkIntersectionsFeature : public LocalContextFeature
{
	
public:
		
	/** Constructor. */
	LocalContextNetworkIntersectionsFeature();

	/** Destructor. */
	virtual ~LocalContextNetworkIntersectionsFeature();
		
	void setGeom( Geom* g ) { m_geom = g; updateSearchBounds(); }; 
	
	/** Fill data structures with relevant information */
	virtual void analyse( Geom* g );
	
	virtual arVec3d direction() { return m_direction; };

	virtual double length() { return m_length; };

					
protected:

	void refine( std::vector< Geom* >& geoms );
	
	std::vector< Geom* > geomsInsideSearchBounds();
	
	void updateSearchBounds();
			
	arBox3<double> m_searchBounds;

};

#endif // _LOCAL_CONTEXT_NETWORK_INTERSECTIONS_FEATURE_H_
