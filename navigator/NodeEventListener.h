#ifndef __NodeEventListener_h__
#define __NodeEventListener_h__

#include <pthread.h>
#include <list>
#include "NodeEvent.h"

class NavigatorXMLRPCClient;

class NodeEventListener
{
public:
    enum State {
        SInit,      // Listener is initialized
        SRunning,   // Listener is running
        SStopped    // Listener is stopped
    };

protected:
    NavigatorXMLRPCClient*& mXmlRpcClient;

    std::list<NodeEvent*> mNodeEventsList1;
    std::list<NodeEvent*> mNodeEventsList2;
    std::list<NodeEvent*>* mNodeEventsListReceiving;
    std::list<NodeEvent*>* mNodeEventsListProcessing;

    pthread_t mThread;
    State mState;
    bool mStop;

public:
    NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient);
    ~NodeEventListener();

    bool start();
    virtual void run();
    void stop(unsigned int timeoutSec = 5);

protected:
    virtual void processEvents() = 0; // pure virtual

private:
    static void *start_routine(void* args);
};

#endif // #ifndef __NodeEventListener_h__