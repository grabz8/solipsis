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

#include "LoginMethod.h"
#include <IP2NClient.h>
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
LoginMethod::LoginMethod(P2NServer* s)
    : AbstractP2NMethod("Login", "Connect to Solipsis Peer", s)
{
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &LoginMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
}

//-------------------------------------------------------------------------------------
ulxr::MethodResponse LoginMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString xmlParamsULXR = calldata.getParam(0);
    std::string xmlParams = xmlParamsULXR.getString();
    if (xmlParams.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    NodeId nodeId;
    std::string xmlResp;
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->login(xmlParams, nodeId, xmlResp);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(retCode))
               << ulxr::make_member(ULXR_PCHAR("nodeId"), ulxr::RpcString(nodeId))
               << ulxr::make_member(ULXR_PCHAR("response"), ulxr::RpcString(xmlResp.c_str()));
    resp.setResult(respStruct);
    return resp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
