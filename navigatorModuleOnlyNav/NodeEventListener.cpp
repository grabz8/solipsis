#include "NodeEventListener.h"
#include "NavigatorXMLRPCClient.h"
#include "OgreHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NodeEventListener::NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient) :
    BasicThread("NodeEventListener"),
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
    // receive new events
    // Events processing is performed by the rendering thread to ensure synchronization with the rendering engine
    while (!isStopRequested())
    {
        XmlEvt* xmlEvt = 0;
        if (mXmlRpcClient->NavigatorXMLRPCClient::handleEvt(&xmlEvt) && (xmlEvt != 0))
        {
            pthread_mutex_lock(&mNodeEventsListsMutex);
            mNodeEventsListReceiving->push_back(xmlEvt);
            pthread_mutex_unlock(&mNodeEventsListsMutex);
        }
        // here we will sleeping 10ms if no evt was returned,
        // best thing should be to get 1 different XMLRPC client to call blocking-method handleEvt()
        // then we would only receive evt (no more NOEVT response)
        // It is only possible if the XMLRPC server is multi-threaded (XMLRPC++ lib is not)
        else
            Platform::sleep(10);
    }
}

//-------------------------------------------------------------------------------------
std::list<XmlEvt*>* NodeEventListener::beginProcessEvents()
{
    // assign new received events to events to process
    pthread_mutex_lock(&mNodeEventsListsMutex);
    mNodeEventsListProcessing = mNodeEventsListReceiving;
    mNodeEventsListReceiving = (mNodeEventsListReceiving == &mNodeEventsList1) ? &mNodeEventsList2 : &mNodeEventsList1;
//    if (!mNodeEventsListProcessing->empty())
//        OGRE_LOG("NodeEventListener::beginProcessEvents() new events list in mNodeEventsListProcessing");
    pthread_mutex_unlock(&mNodeEventsListsMutex);
    return mNodeEventsListProcessing;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::endProcessEvents()
{
    pthread_mutex_lock(&mNodeEventsListsMutex);
    mNodeEventsListProcessing->clear();
    pthread_mutex_unlock(&mNodeEventsListsMutex);
}

//-------------------------------------------------------------------------------------
