#include "NodeEventListener.h"
#include "NavigatorXMLRPCClient.h"
#include "OgreHelpers.h"
#include "Platform.h"

NodeEventListener::NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient) :
    BasicThread(),
    mXmlRpcClient(xmlRpcClient),
    mNodeEventsListsMutex(PTHREAD_MUTEX_INITIALIZER),
    mNodeEventsListReceiving(&mNodeEventsList1),
    mNodeEventsListProcessing(&mNodeEventsList2)
{
}

//-------------------------------------------------------------------------------------
NodeEventListener::~NodeEventListener()
{
}

//-------------------------------------------------------------------------------------
void NodeEventListener::run()
{
    while (!isStopRequested())
    {
        // receive new events
        mNodeEventsListReceiving->clear();
        while (mNodeEventsListReceiving->empty() && !isStopRequested())
            mXmlRpcClient->getEvents(*mNodeEventsListReceiving);

        // waiting previous events are processed
        bool isNodeEventsListProcessingEmpty = false;
        while (!isStopRequested() && !isNodeEventsListProcessingEmpty)
        {
            pthread_mutex_lock(&mNodeEventsListsMutex);
            isNodeEventsListProcessingEmpty = mNodeEventsListProcessing->empty();
            Platform::sleep(100);
        }
        if (isStopRequested())
        {
            if (isNodeEventsListProcessingEmpty)
                pthread_mutex_unlock(&mNodeEventsListsMutex);
            break;
        }

        // assign new received events to events to process
        mNodeEventsListProcessing = mNodeEventsListReceiving;
        mNodeEventsListReceiving = (mNodeEventsListReceiving == &mNodeEventsList1) ? &mNodeEventsList2 : &mNodeEventsList1;
        pthread_mutex_unlock(&mNodeEventsListsMutex);

        //processEvents(); is called by the rendering thread to ensure synchronization with the rendering engine
        OGRE_LOG("NodeEventListener::run() new events list in mNodeEventsListProcessing");
    }
}

//-------------------------------------------------------------------------------------
std::list<NodeEvent*>* NodeEventListener::beginProcessEvents()
{
    pthread_mutex_lock(&mNodeEventsListsMutex);
    return mNodeEventsListProcessing;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::endProcessEvents()
{
    mNodeEventsListProcessing->clear();
    pthread_mutex_unlock(&mNodeEventsListsMutex);
}
