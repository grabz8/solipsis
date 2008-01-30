#include "LoginMethod.h"
#include "XmlDatas.h"
#include <sstream>
/////////////////////////////////////
#include "Node.h"
/////////////////////////////////////

using namespace Solipsis;

LoginMethod::LoginMethod(NodeServer* s)
    : AbstractNodeMethod("Login", "Connect to Solipsis Peer", s)
{
#ifdef ULXR
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &LoginMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
#endif
}

#ifdef ULXR
ulxr::MethodResponse LoginMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString xmlParamsULXR = calldata.getParam(0);
    std::string xmlParams = xmlParamsULXR.getString();
    if (xmlParams.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    INodeClient::NodeId nodeId;
    std::string xmlResp;
    INodeClient::RetCode retCode = _execute(xmlParams, nodeId, xmlResp);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(retCode))
               << ulxr::make_member(ULXR_PCHAR("nodeId"), ulxr::RpcString(nodeId))
               << ulxr::make_member(ULXR_PCHAR("response"), ulxr::RpcString(xmlResp.c_str()));
    resp.setResult(respStruct);
    return resp;
}
#else
void LoginMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    std::string xmlParams = paramsTable[0];
    if (xmlParams.empty())
        throw XmlRpc::XmlRpcException("Invalid parameters !");

    // Process
    INodeClient::NodeId nodeId;
    std::string xmlResp;
    INodeClient::RetCode retCode = _execute(xmlParams, nodeId, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["nodeId"] = nodeId;
    result["response"] = xmlResp.c_str();
}
#endif

INodeClient::RetCode LoginMethod::_execute(const std::string& xmlParamsStr, INodeClient::NodeId& nodeId, std::string& xmlRespStr)
{
    nodeId.clear();
    xmlRespStr.clear();

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlParamsStr.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        std::stringstream errorStr;
        errorStr << "Invalid parameters ! xmlDoc error(" << xmlDoc.ErrorId() << ", " << xmlDoc.ErrorDesc() << ")";
        xmlRespStr = errorStr.str();
        return INodeClient::RCError;
    }
    XmlLogin xmlLogin;
    if (!xmlLogin.fromXmlElt(xmlDoc.RootElement()))
    {
        xmlRespStr = "Invalid parameters !";
        return INodeClient::RCError;
    }

    /////////////////////////////////////
    Node& node = Node::getSingleton();
    node.mConnectionsCount++;
    if (node.mConnectionsCount == 1)
    {
        node.mState = -1;
    }
    char nodeIdChar[9];
    _snprintf(nodeIdChar, 9, "%08x", node.mConnectionsCount);
    nodeId = nodeIdChar;

    if (xmlLogin.getPwd().compare("demo") == 0)
    {
        xmlRespStr = "\
<solipsis>\
 <objects>\
  <object uid=11111111 />\
  <object uid=11112222 />\
 </objects>\
</solipsis>\
";
    }
    /////////////////////////////////////

    return INodeClient::RCOk;
}
