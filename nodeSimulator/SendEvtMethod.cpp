#include "SendEvtMethod.h"
#include "XmlDatas.h"
#include <sstream>
/////////////////////////////////////
#include "Node.h"
/////////////////////////////////////

using namespace Solipsis;

SendEvtMethod::SendEvtMethod(NodeServer* s)
    : AbstractNodeMethod("SendEvt", "Send/post an event to Solipsis Peer", s)
{
#ifdef ULXR
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &SendEvtMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString()
                                                              << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
#endif
}

#ifdef ULXR
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
    INodeClient::RetCode retCode = _execute(nodeId, xmlEvt, xmlResp);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(retCode))
               << ulxr::make_member(ULXR_PCHAR("response"), ulxr::RpcString(xmlResp.c_str()));
    resp.setResult(respStruct);
    return resp;
}
#else
void SendEvtMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    std::string nodeId = paramsTable[0];
    std::string xmlEvt = paramsTable[1];
    if (nodeId.empty() || xmlEvt.empty())
    {
        throw XmlRpc::XmlRpcException("Invalid parameters !");
    }

    // Process
    std::string xmlResp;
    INodeClient::RetCode retCode = _execute(nodeId, xmlEvt, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["response"] = xmlResp;
}
#endif

INodeClient::RetCode SendEvtMethod::_execute(const INodeClient::NodeId& nodeId, const std::string& xmlEvtStr, std::string& xmlRespStr)
{
    xmlRespStr.clear();

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlEvtStr.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        std::stringstream errorStr;
        errorStr << "Invalid parameters ! xmlDoc error(" << xmlDoc.ErrorId() << ", " << xmlDoc.ErrorDesc() << ")";
        xmlRespStr = errorStr.str();
        return INodeClient::RCError;
    }

    /////////////////////////////////////
    Node& node = Node::getSingleton();
    XmlEvt xmlEvt;
    if (!xmlEvt.fromXmlElt(xmlDoc.RootElement()))
    {
        xmlRespStr = "Invalid parameters !";
        return INodeClient::RCError;
    }
    if (xmlEvt.getType() == ETUpdatedObject)
    {
        XmlObject* xmlObject = (XmlObject*)xmlEvt.getDatas();
        if (xmlObject == 0)
        {
            xmlRespStr = "No object found in event !";
            return INodeClient::RCError;
        }
        if (xmlObject->getDefinedAttributes().Test(XmlObject::DAPosition))
        {
            node.mAvatarPosition = xmlObject->getPosition();
        }
        if (xmlObject->getDefinedAttributes().Test(XmlObject::DAOrientation))
        {
            node.mAvatarOrientation = xmlObject->getOrientation();
        }
        node.mAvatarDirty = true;
    }
    /////////////////////////////////////

    return INodeClient::RCOk;
}