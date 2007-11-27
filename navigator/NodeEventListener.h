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
protected:
    // XMLRPC client
    NavigatorXMLRPCClient*& mXmlRpcClient;

private:
    // mutex to secure events lists accesses
    pthread_mutex_t mNodeEventsListsMutex;
    // first events list storage
    std::list<NodeEvent*> mNodeEventsList1;
    // second events list storage
    std::list<NodeEvent*> mNodeEventsList2;
    // current list used to receive new events
    std::list<NodeEvent*>* mNodeEventsListReceiving;
    // current list containing events to process
    std::list<NodeEvent*>* mNodeEventsListProcessing;

public:
    NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient);
    ~NodeEventListener();

protected:
    // implements the BasicThread methods
    virtual void run();

    // begin to process events
    virtual std::list<NodeEvent*>* beginProcessEvents();
    // end of events processing
    virtual void endProcessEvents();
};

} // namespace Solipsis

#endif // #ifndef __NodeEventListener_h__
