#ifndef __NodeManager_h__
#define __NodeManager_h__

#include "Ogre.h"
#include "IP2NClient.h"
#include "XmlDatas.h"
#include "Node.h"
#include "AvatarNode.h"
#include "SiteNode.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis nodes.
*/
class NodeManager
{
public:
    /// <NodeId, Node*> map
    typedef std::map<NodeId, Node*> NodeMap;
    /// <NodeId> set
    typedef std::set<NodeId> NodeIdSet;

private:
    /// Map of nodes
    NodeMap mNodes;
    /// Set of destroyed nodes Ids
    NodeIdSet mDestroyedNodeIds;

public:
	NodeManager();
	virtual ~NodeManager();

    /** Login. */
	AvatarNode* login(XmlLogin* xmlLogin);

    /** logout. */
    bool logout(const NodeId& nodeId);

    /** update. */
    bool update();

    /** Process an event. */
    bool processEvt(const NodeId& nodeId, XmlEvt& xmlEvt, std::string& xmlRespStr);
    /** Get next event to handle. */
    XmlEvt* getNextEvtToHandle(const NodeId& nodeId);
    /** Free event (handled event). */
    bool freeEvt(const NodeId& nodeId, XmlEvt* evt);
};

} // namespace Solipsis

#endif // #ifndef __NodeManager_h__