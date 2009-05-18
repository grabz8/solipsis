#ifndef _GLOBAL_CONTEXT_NETWORK_PATTERN_FEATURE_H_
#define _GLOBAL_CONTEXT_NETWORK_PATTERN_FEATURE_H_

#include <arLinear/arLinear.h>

#include "ContextDensityMap.h"
#include "ContextReliefMap.h"
#include "GlobalContextFeature.h"

class GlobalContextNetworkPatternFeature : public GlobalContextFeature
{
	
public:
			
	/** Constructor. */
	GlobalContextNetworkPatternFeature();

	/** Destructor. */
	virtual ~GlobalContextNetworkPatternFeature();
			
	/** Fill data structures with relevant information */
	virtual void analyse( Geom* g );
	
	virtual arVec3d direction() { return m_direction; };
	
	virtual double length() { return m_length; }; 
	
protected:
				
	void addContextMap( ContextMap* m, std::map< double, double >& strengths ) { m_contextMaps.push_back( std::make_pair< ContextMap*, std::map< double, double > >( m, strengths ) ); };
	
	std::vector< std::pair< ContextMap*, std::map< double, double > > > m_contextMaps; 
				
};

#endif // _GLOBAL_CONTEXT_NETWORK_PATTERN_FEATURE_H_
