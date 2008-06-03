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
NodeEventListener::EvtsList* NodeEventListener::beginProcessEvents()
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

//-------------------------------------------------------------------------------------
