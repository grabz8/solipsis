#ifndef __SiteNode_h__
#define __SiteNode_h__

#include <map>
#include "Ogre.h"
#include "Node.h"
#include "Site.h"

namespace Solipsis {

/** This class manages 1 site node.
*/
class SiteNode : public Node
{
protected:
    /// Site entity
    Site mSite;
    /// Map of stored entities
    Entity::EntityMap mStoredEntities;

public:
    /** Constructor. */
    SiteNode(const NodeId& nodeId, XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~SiteNode();

	/** Get associated site entity. */
    Site& getEntity();
};

} // namespace Solipsis

#endif // #ifndef __SiteNode_h__