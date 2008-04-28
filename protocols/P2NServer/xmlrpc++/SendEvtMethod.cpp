#include "SendEvtMethod.h"
#include <IP2NClient.h>
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
SendEvtMethod::SendEvtMethod(P2NServer* s)
    : AbstractP2NMethod("SendEvt", "Send/post an event to Solipsis Peer", s)
{
}

//-------------------------------------------------------------------------------------
void SendEvtMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    NodeId nodeId = paramsTable[0];
    std::string xmlEvt = paramsTable[1];
    if (nodeId.empty() || xmlEvt.empty())
    {
        throw XmlRpc::XmlRpcException("Invalid parameters !");
    }

    // Process
    std::string xmlResp;
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->sendEvt(nodeId, xmlEvt, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["response"] = xmlResp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
