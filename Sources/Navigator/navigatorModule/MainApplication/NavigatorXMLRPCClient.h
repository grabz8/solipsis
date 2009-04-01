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

#include <list>
#include "IP2NClient.h"
#include "XmlDatas.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages XMLRPC requests and responses processed by the Navigator.
 */
class NavigatorXMLRPCClient : public IP2NClientLogger
{
protected:
    IP2NClient* mP2NClient;
    IP2NClient* mSharedCnx;

public:
    NavigatorXMLRPCClient(const std::string& host, unsigned short port, int verbosity, const std::string& extras);
    NavigatorXMLRPCClient(NavigatorXMLRPCClient& sharedCnx);
    ~NavigatorXMLRPCClient();

    /** log on Solipsis host.
    @param xmlLogin Login parameters
    @param nodeId Node identifier
    @returns True if login succeeded
    */
    bool login(const XmlLogin& xmlLogin, NodeId& nodeId);
    /** log off Solipsis host.
    @returns True if logout succeeded
    */
    bool logout();
    /** test if client is connected to Solipsis host.
    @returns True if client is connected
    */
    bool isConnected();

    /** handle an event.
    @param xmlEvt If exists, the allocated event to handle
    @returns True if handle operation succeded (even if no event returned)
    */

    bool handleEvt(RefCntPoolPtr<XmlEvt>& xmlEvt);

    /** send an event.
    @param xmlEvt The event to send
    @param xmlResp The response
    @returns True if the event was correctly sent
    */
    bool sendEvt(const XmlEvt& xmlEvt, std::string& xmlResp);

protected:
    /// @copydoc IP2NClientLogger::logMessage
    virtual void logMessage(const std::string& message);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorXMLRPCClient_h__