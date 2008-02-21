#include "LoginMethod.h"
#include "IP2NClient.h"
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
LoginMethod::LoginMethod(P2NServer* s)
    : AbstractP2NMethod("Login", "Connect to Solipsis Peer", s)
{
}

//-------------------------------------------------------------------------------------
void LoginMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    std::string xmlParams = paramsTable[0];
    if (xmlParams.empty())
        throw XmlRpc::XmlRpcException("Invalid parameters !");

    // Process
    NodeId nodeId;
    std::string xmlResp;
    IP2NClient::RetCode retCode = mServer->getRequestsHandler()->login(xmlParams, nodeId, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["nodeId"] = nodeId;
    result["response"] = xmlResp.c_str();
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
