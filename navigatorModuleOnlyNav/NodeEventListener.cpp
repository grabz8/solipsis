/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "NodeEventListener.h"
#include "NavigatorXMLRPCClient.h"
#include "OgreHelpers.h"
#include "CTSystem.h"

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
    if (mXmlRpcClientAsync != 0)
        delete mXmlRpcClientAsync;
}

//-------------------------------------------------------------------------------------
void NodeEventListener::run()
{
    assert(mXmlRpcClientAsync == 0);
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
            CommonTools::System::sleep(10);
    }

    delete mXmlRpcClientAsync;
    mXmlRpcClientAsync = 0;

    mNodeEventsList1.clear();
    mNodeEventsList2.clear();
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
