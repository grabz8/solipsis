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
#ifdef POOL
RefCntPoolPtr<XmlEvt> Node::getNextEvtToHandle()
#else
XmlEvt* Node::getNextEvtToHandle()
#endif
{
#ifdef POOL
    RefCntPoolPtr<XmlEvt> evt(RefCntPoolPtr<XmlEvt>::nullPtr);
#else
    XmlEvt* evt = 0;
#endif

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
#ifdef POOL
bool Node::freeEvt(RefCntPoolPtr<XmlEvt>& evt)
{
    return true;
}
#else
bool Node::freeEvt(XmlEvt* evt)
{
    delete evt;

    return true;
}
#endif

//-------------------------------------------------------------------------------------

} // namespace Solipsis
