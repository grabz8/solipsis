#ifndef _LOCAL_CONTEXT_MODULE_H_
#define _LOCAL_CONTEXT_MODULE_H_

#include "ContextModule.h"
#include "LocalContextFeature.h"

class LocalContextModule : public ContextModule
{
	
public:
		
	/** Constructor. */
	LocalContextModule() {};

	/** Destructor. */
	virtual ~LocalContextModule() {};
	
	/** Analyse context - Fill data structures with relevant information */
	virtual void analyse() = 0;
	
	void addFeature( LocalContextFeature::LocalContextFeatureType ft, const LocalContextFeature& f ) { m_features[ft] = f; };
	
	feature( LocalContextFeature::LocalContextFeatureType ft ) { return m_features[ft]; };
 	
protected:
	
	std::map< LocalContextFeature::LocalContextFeatureType, LocalContextFeature > m_features;
	
};

#endif // _LOCAL_CONTEXT_MODULE_H_
