#ifndef __Node_h__
#define __Node_h__

#include <list>
#include <pthread.h>
#include "Ogre.h"
#include "IP2NClient.h"
#include "XmlDatas.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Node.
*/
class Node
{
public:
#ifdef POOL
    typedef std::list<RefCntPoolPtr<XmlEvt>> XmlEvtToHandleList;
#else
    typedef std::list<XmlEvt*> XmlEvtToHandleList;
#endif

protected:
    /// Node unique identifier
    NodeId mNodeId;
    /// Type
    String mType;
    /// Mutex
    pthread_mutex_t mEvtsMutex;
    /// List of events to handle
    XmlEvtToHandleList mEvtsToHandleList;
    /// Frozen state
    bool mFrozen;

public:
    /** Constructor. */
    Node(const NodeId& nodeId, const String& type);
    /** Destructor. */
    virtual ~Node();

    /** Gets the identifier. */
    const NodeId& getNodeId();
    /** Gets the type. */
    const String& getType();

    /** Process an event. */
    virtual bool processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr);
#ifdef POOL
    /** Get next event to handle. */
    virtual RefCntPoolPtr<XmlEvt> getNextEvtToHandle();
    /** Free event (handled event). */
    virtual bool freeEvt(RefCntPoolPtr<XmlEvt>& evt);
#else
    /** Get next event to handle. */
    virtual XmlEvt* getNextEvtToHandle();
    /** Free event (handled event). */
    virtual bool freeEvt(XmlEvt* evt);
#endif
    /** Freeze. */
    virtual bool freeze(bool frozen) { mFrozen = frozen; return true; }
};

} // namespace Solipsis

#endif // #ifndef __Node_h__