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

#ifndef __NavigatorXMLRPCClient_h__
#define __NavigatorXMLRPCClient_h__

#define UNPLUG_MODE

#include "XmlRpc.h"
#include "Ogre.h"
#include "Peer.h"
#include "NodeEvent.h"
#include <pthread.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages XMLRPC requests and responses processed by the Navigator.
 */
class NavigatorXMLRPCClient : public XmlRpc::XmlRpcClient
{
protected:
    String mConnectionId;
    bool mConnected;

private:
    pthread_mutex_t mCallsMutex;

public:
    NavigatorXMLRPCClient(const char *host, int port, const char *uri=0);
    ~NavigatorXMLRPCClient();

    bool connect();
    bool disconnect();
    bool isConnected();
    bool getAllPeers(std::list<Peer*> &peersList);
    bool getEvents(std::list<NodeEvent*> &nodeEventsList);
    bool getStatus(String &status);
    bool getDesc(const Peer &peer);
    bool move(Peer &peer);
    bool sendMessage(const String& message, std::list<Peer*> &peersList);

private:
    Peer* createPeerFromXml(std::string& peerXml);
    String removeFirstLevelOfXmlAnchor(String& xml);
    bool executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorXMLRPCClient_h__