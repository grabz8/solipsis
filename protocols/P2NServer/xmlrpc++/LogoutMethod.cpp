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

#include "LogoutMethod.h"
#include <IP2NClient.h>
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
LogoutMethod::LogoutMethod(P2NServer* s)
    : AbstractP2NMethod("Logout", "Disconnect to Solipsis Peer", s)
{
}

//-------------------------------------------------------------------------------------
void LogoutMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    NodeId nodeId = paramsTable[0];
    if (nodeId.empty())
        throw XmlRpc::XmlRpcException("Invalid parameters !");

    // Process
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->logout(nodeId);

    // Format response
    result["retCode"] = IP2NClient::RCOk;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
