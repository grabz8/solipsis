#include "LoginMethod.h"
#include "IP2NClient.h"
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
