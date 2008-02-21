#include "Node.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
Node::Node(const NodeId& nodeId, const String& type) :
    mNodeId(nodeId),
    mType(type),
    mEvtsMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
Node::~Node()
{
}

//-------------------------------------------------------------------------------------
const NodeId& Node::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
const String& Node::getType()
{
    return mType;
}

//-------------------------------------------------------------------------------------
bool Node::processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr)
{
    return true;
}

//-------------------------------------------------------------------------------------
XmlEvt* Node::getNextEvtToHandle()
{
    XmlEvt* evt = 0;

    pthread_mutex_lock(&mEvtsMutex);
    if (!mEvtsToHandleList.empty())
    {
        evt = mEvtsToHandleList.front();
        mEvtsToHandleList.pop_front();
    }
    pthread_mutex_unlock(&mEvtsMutex);

    return evt;
}

//-------------------------------------------------------------------------------------
bool Node::freeEvt(XmlEvt* evt)
{
    delete evt;

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
