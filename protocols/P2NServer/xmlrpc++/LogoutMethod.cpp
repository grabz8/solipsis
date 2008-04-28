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
