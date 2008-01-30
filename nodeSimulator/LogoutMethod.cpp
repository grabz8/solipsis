#include "LogoutMethod.h"
/////////////////////////////////////
#include "Node.h"
/////////////////////////////////////

using namespace Solipsis;

LogoutMethod::LogoutMethod(NodeServer* s)
    : AbstractNodeMethod("Logout", "Disconnect to Solipsis Peer", s)
{
#ifdef ULXR
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &LogoutMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
#endif
}

#ifdef ULXR
ulxr::MethodResponse LogoutMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString nodeIdULXR = calldata.getParam(0);
    std::string nodeId = nodeIdULXR.getString();
    if (nodeId.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    /////////////////////////////////////
    Node& node = Node::getSingleton();
    node.mConnectionsCount--;
    /////////////////////////////////////

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(INodeClient::RCOk));
    resp.setResult(respStruct);
    return resp;
}
#else
void LogoutMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    std::string nodeId = paramsTable[0];
    if (nodeId.empty())
        throw XmlRpc::XmlRpcException("Invalid parameters !");

    // Process
    /////////////////////////////////////
    Node& node = Node::getSingleton();
    node.mConnectionsCount--;
    /////////////////////////////////////

    // Format response
    result["retCode"] = INodeClient::RCOk;
}
#endif
