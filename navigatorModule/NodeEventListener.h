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
public:
    typedef std::list<NodeEvent*> EvtsList;

protected:
    // XMLRPC client
    NavigatorXMLRPCClient*& mXmlRpcClient;

private:
    // mutex to secure events lists accesses
    pthread_mutex_t mNodeEventsListsMutex;
    // first events list storage
    EvtsList mNodeEventsList1;
    // second events list storage
    EvtsList mNodeEventsList2;
    // current list used to receive new events
    EvtsList* mNodeEventsListReceiving;
    // current list containing events to process
    EvtsList* mNodeEventsListProcessing;

public:
    NodeEventListener(NavigatorXMLRPCClient*& xmlRpcClient);
    ~NodeEventListener();

protected:
    /** See BasicThread. */
    virtual void run();

    // begin to process events
    virtual EvtsList* beginProcessEvents();
    // end of events processing
    virtual void endProcessEvents();
};

} // namespace Solipsis

#endif // #ifndef __NodeEventListener_h__
