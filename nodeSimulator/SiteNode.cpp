#include "SiteNode.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
SiteNode::SiteNode(const NodeId& nodeId, XmlEntity* xmlEntity) :
    Node(nodeId, "site"),
    mSite(xmlEntity)
{
}

//-------------------------------------------------------------------------------------
SiteNode::~SiteNode()
{
}

//-------------------------------------------------------------------------------------
Site& SiteNode::getEntity()
{
    return mSite;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
