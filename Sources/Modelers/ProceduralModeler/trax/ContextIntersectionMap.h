#ifndef _CONTEXT_INTERSECTION_MAP_H_
#define _CONTEXT_INTERSECTION_MAP_H_

#include "ContextMap.h"

namespace pm_trax {

class ContextIntersectionMap : public ContextMap
{
	
public:
	
	class MapAttributes {
		
	public:
		
		MapAttributes( const std::map< double, std::string >& sectionWeight2Type ) : m_sectionWeight2Type( sectionWeight2Type ) {};
		
		virtual ~MapAttributes() {};

		std::map< double, std::string > m_sectionWeight2Type;
		
	};

	/** Constructor. */
	ContextIntersectionMap( const MapAttributes& dma );

	/** Destructor. */
	virtual ~ContextIntersectionMap();
		
	virtual void analyse( pm_trax::Geom* g );

protected:
			
	Vec3d bestDirection();

	double bestLength();

	MapAttributes m_attributes;
		
}; // ContextIntersectionMap

}; // pm_trax

#endif // _CONTEXT_INTERSECTION_MAP_H_
