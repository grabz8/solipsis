#ifndef __NodeEventListener_h__
#define __NodeEventListener_h__

#include <list>
#include "BasicThread.h"
#include "XmlDatas.h"

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
    std::list<XmlEvt*> mNodeEventsList1;
    // second events list storage
    std::list<XmlEvt*> mNodeEventsList2;
    // current list used to receive new events
    std::list<XmlEvt*>* mNodeEventsListReceiving;
    // current list containing events to process
    std::list<XmlEvt*>* mNodeEventsListProcessing;

public:
    NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient);
    ~NodeEventListener();

protected:
    /** See BasicThread. */
    virtual void run();

    // begin to process events
    virtual std::list<XmlEvt*>* beginProcessEvents();
    // end of events processing
    virtual void endProcessEvents();
};

} // namespace Solipsis

#endif // #ifndef __NodeEventListener_h__
