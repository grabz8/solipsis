#ifndef __NodeEventListener_h__
#define __NodeEventListener_h__

#include <list>
#include "BasicThread.h"
#include "NodeEvent.h"

namespace Solipsis {

class NavigatorXMLRPCClient;

/** This class listen/manages events received from Solipsis nodes.
*/
class NodeEventListener : public BasicThread
{
public:
    typedef std::list<NodeEvent*> EvtsList;

protected:
    // XMLRPC client
    NavigatorXMLRPCClient*& mXmlRpcClient;

private:
    // mutex to secure events lists accesses
    pthread_mutex_t mNodeEventsListsMutex;
    // first events list storage
    EvtsList mNodeEventsList1;
    // second events list storage
    EvtsList mNodeEventsList2;
    // current list used to receive new events
    EvtsList* mNodeEventsListReceiving;
    // current list containing events to process
    EvtsList* mNodeEventsListProcessing;

public:
    NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient);
    ~NodeEventListener();

protected:
    /** See BasicThread. */
    virtual void run();

    // begin to process events
    virtual EvtsList* beginProcessEvents();
    // end of events processing
    virtual void endProcessEvents();
};

} // namespace Solipsis

#endif // #ifndef __NodeEventListener_h__
