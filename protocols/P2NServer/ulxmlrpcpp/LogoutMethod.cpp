#include "LogoutMethod.h"
#include <IP2NClient.h>
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
LogoutMethod::LogoutMethod(P2NServer* s)
    : AbstractP2NMethod("Logout", "Disconnect to Solipsis Peer", s)
{
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &LogoutMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
}

//-------------------------------------------------------------------------------------
ulxr::MethodResponse LogoutMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString nodeIdULXR = calldata.getParam(0);
    std::string nodeId = nodeIdULXR.getString();
    if (nodeId.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->logout(nodeId);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(IP2NClient::RCOk));
    resp.setResult(respStruct);
    return resp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
