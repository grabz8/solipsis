#include "NodeEventListener.h"
#include "NavigatorXMLRPCClient.h"
#include "OgreHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NodeEventListener::NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient) :
    BasicThread("NodeEventListener"),
    mXmlRpcClient(xmlRpcClient),
    mXmlRpcClientAsync(0),
    mNodeEventsListsMutex(PTHREAD_MUTEX_INITIALIZER),
    mNodeEventsListReceiving(&mNodeEventsList1),
    mNodeEventsListProcessing(&mNodeEventsList2)
{
}

//-------------------------------------------------------------------------------------
NodeEventListener::~NodeEventListener()
{
    delete mXmlRpcClientAsync;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::run()
{
    delete mXmlRpcClientAsync;
    mXmlRpcClientAsync = new NavigatorXMLRPCClient(*mXmlRpcClient);

    // receive new events
    // Events processing is performed by the rendering thread to ensure synchronization with the rendering engine
    while (!isStopRequested())
    {
#ifdef POOL
        RefCntPoolPtr<XmlEvt> xmlEvt(RefCntPoolPtr<XmlEvt>::nullPtr);
        if (mXmlRpcClientAsync->handleEvt(xmlEvt) && !xmlEvt.isNull())
#else
        XmlEvt* xmlEvt = 0;
        if (mXmlRpcClientAsync->handleEvt(&xmlEvt) && (xmlEvt != 0))
#endif
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
NodeEventListener::EvtsList* NodeEventListener::beginProcessEvents()
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
