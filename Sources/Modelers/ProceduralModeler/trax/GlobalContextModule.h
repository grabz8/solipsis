#ifndef _GLOBAL_CONTEXT_MODULE_H_
#define _GLOBAL_CONTEXT_MODULE_H_

#include "ContextModule.h"
#include "GlobalContextFeature.h"

namespace trax {

class GlobalContextModule : public ContextModule
{
	
public:
		
	/** Constructor. */
	GlobalContextModule() {};

	/** Destructor. */
	virtual ~GlobalContextModule() {};
	
	/** Analyse context - Fill data structures with relevant information */
	virtual void analyse() = 0;
	
	void addFeature( GlobalContextFeature::GlobalContextFeatureType ft, const GlobalContextFeature& f ) { m_features[ft] = f; };
	
	feature( GlobalContextFeature::GlobalContextFeatureType ft ) { return m_features[ft]; };
 	
protected:
	
	std::map< GlobalContextFeature::GlobalContextFeatureType, GlobalContextFeature > m_features;
	
}; 

#endif // _GLOBAL_CONTEXT_MODULE_H_
