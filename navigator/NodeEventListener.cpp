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
    int rc;

    rc = pthread_create(&mThread, NULL, start_routine, this);
    if (rc != 0)
    {
        LogManager::getSingletonPtr()->logMessage("NodeEventListener::start() pthread_create returned " + StringConverter::toString(rc));
        return false;
    }
    rc = pthread_detach(mThread);
    if (rc != 0)
    {
        LogManager::getSingletonPtr()->logMessage("NodeEventListener::start() pthread_detach returned " + StringConverter::toString(rc));
        return false;
    }

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

    LogManager::getSingletonPtr()->logMessage("NodeEventListener::stop() stop requested");

    mStop = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)timeoutSec*1000))
    {
        Platform::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
    }

    LogManager::getSingletonPtr()->logMessage("NodeEventListener::stop() end");
}

//-------------------------------------------------------------------------------------
void* NodeEventListener::start_routine(void* args)
{
    NodeEventListener* nodeEventListener = (NodeEventListener*)args;
    if (nodeEventListener != 0)
        nodeEventListener->run();

    LogManager::getSingletonPtr()->logMessage("NodeEventListener::start_routine() end");

    return NULL;
}