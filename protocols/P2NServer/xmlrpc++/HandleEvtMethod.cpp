#include "HandleEvtMethod.h"
#include "IP2NClient.h"
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
HandleEvtMethod::HandleEvtMethod(P2NServer* s)
    : AbstractP2NMethod("HandleEvt", "Handle an event post by Solipsis Peer", s)
{
}

//-------------------------------------------------------------------------------------
void HandleEvtMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    NodeId nodeId = paramsTable[0];
    if (nodeId.empty())
    {
        throw XmlRpc::XmlRpcException("Invalid parameters !");
    }

    // Process
    std::string xmlResp;
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->handleEvt(nodeId, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["response"] = xmlResp.c_str();
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
