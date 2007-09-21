#include "NodeEventListener.h"
#include "NavigatorXMLRPCClient.h"
#include "Platform.h"

NodeEventListener::NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient) :
    mXmlRpcClient(xmlRpcClient),
    mNodeEventsListReceiving(&mNodeEventsList1),
    mNodeEventsListProcessing(&mNodeEventsList2),
    mState(SInit),
    mStop(false)
{
}

//-------------------------------------------------------------------------------------
NodeEventListener::~NodeEventListener()
{
}

//-------------------------------------------------------------------------------------
bool NodeEventListener::start()
{
    if (pthread_create(&mThread, NULL, start_routine, this) != 0)
        return false;
    if (pthread_detach(mThread) != 0)
        return false;

    mState = SRunning;
    mStop = false;

    return true;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::run()
{
    while (!mStop)
    {
        mNodeEventsListReceiving->clear();
        while (mNodeEventsListReceiving->empty() && !mStop)
            mXmlRpcClient->getEvents(*mNodeEventsListReceiving);
        while (!mNodeEventsListProcessing->empty() && !mStop)
            Platform::sleep(100);
        if (mStop) break;
        mNodeEventsListProcessing = mNodeEventsListReceiving;
        mNodeEventsListReceiving = (mNodeEventsListReceiving == &mNodeEventsList1) ? &mNodeEventsList2 : &mNodeEventsList1;
        //processEvents(); is called by the rendering thread to ensure synchronization with the rendering engine
        LogManager::getSingletonPtr()->logMessage("NodeEventListener::run() new events list in mNodeEventsListProcessing");
    }
    mState = SStopped;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::stop(unsigned int timeoutSec)
{
    unsigned long elapsedMs = 0;

    mStop = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)timeoutSec*1000))
    {
        Platform::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
    }
}

//-------------------------------------------------------------------------------------
void* NodeEventListener::start_routine(void* args)
{
    NodeEventListener* nodeEventListener = (NodeEventListener*)args;
    if (nodeEventListener != 0)
        nodeEventListener->run();
    return NULL;
}