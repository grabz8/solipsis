#include "SendEvtMethod.h"
#include "IP2NClient.h"
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
SendEvtMethod::SendEvtMethod(P2NServer* s)
    : AbstractP2NMethod("SendEvt", "Send/post an event to Solipsis Peer", s)
{
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &SendEvtMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString()
                                                              << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
}

//-------------------------------------------------------------------------------------
ulxr::MethodResponse SendEvtMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString nodeIdULXR = calldata.getParam(0);
    std::string nodeId = nodeIdULXR.getString();
    ulxr::RpcString xmlEvtULXR = calldata.getParam(1);
    std::string xmlEvt = xmlEvtULXR.getString();
    if (nodeId.empty() || xmlEvt.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    std::string xmlResp;
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->sendEvt(nodeId, xmlEvt, xmlResp);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(retCode))
               << ulxr::make_member(ULXR_PCHAR("response"), ulxr::RpcString(xmlResp.c_str()));
    resp.setResult(respStruct);
    return resp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
