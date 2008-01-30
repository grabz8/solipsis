#include "XMLRPCNodeClient.h"
#include "tinyxml.h"
#include <sstream>

#define LOG(message) if (mLogger != 0) mLogger->logMessage(message);

using namespace Solipsis;

const char XMLRPCNodeClient::RETCODE_TAG[] = "retCode";
const char XMLRPCNodeClient::RESPONSE_TAG[] = "response";
const char XMLRPCNodeClient::NODEID_TAG[] = "nodeId";

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::XMLRPCNodeClient(const char *host, int port, const char *uri) :
    XmlRpcClient(host, port, uri),
    mNodeId(""),
    mConnected(false),
    mCallsMutex(PTHREAD_MUTEX_INITIALIZER),
    mLogger(0)
{
}

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::~XMLRPCNodeClient()
{
    if (isConnected())
    {
        LOG("XMLRPCNodeClient::~XMLRPCNodeClient() last chance disconnection");
        logout();
    }
}

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::RetCode XMLRPCNodeClient::login(const std::string& xmlParams, std::string& xmlResp)
{
    INodeClient::RetCode retCode = INodeClient::RCError;

    xmlResp.clear();
    const XmlRpc::XmlRpcValue params(xmlParams);
    XmlRpc::XmlRpcValue result;
    mConnected = this->executeThreadSafe("Login", params, result);
    if (mConnected)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (INodeClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == INodeClient::RCError)
        {
            LOG("XMLRPCNodeClient::login() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            mConnected = false;
            return retCode;
        }
        mNodeId = resultStruct[NODEID_TAG];
        xmlResp = resultStruct[RESPONSE_TAG];
        TiXmlDocument xmlDoc;
        xmlDoc.Parse(xmlResp.c_str());
        // check for errors
        if (xmlDoc.Error())
        {
            LOG("XMLRPCNodeClient::login() Unable to parse response !");
            mConnected = false;
            return INodeClient::RCError;
        }
        LOG("XMLRPCNodeClient::login()\nxmlParams=\n" + xmlParams + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode) + "\nmNodeId=" + mNodeId);
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::RetCode XMLRPCNodeClient::logout()
{
    INodeClient::RetCode retCode = INodeClient::RCError;

    if (!mConnected)
        return INodeClient::RCOk;

    const XmlRpc::XmlRpcValue params(mNodeId);
    XmlRpc::XmlRpcValue result;
    mConnected = this->executeThreadSafe("Logout", params, result);
    if (mConnected)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCNodeClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == INodeClient::RCError)
        {
            LOG("XMLRPCNodeClient::logout() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        mConnected = false;
        LOG("XMLRPCNodeClient::logout() retCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool XMLRPCNodeClient::isConnected()
{
    return mConnected;
}

//-------------------------------------------------------------------------------------
const XMLRPCNodeClient::NodeId& XMLRPCNodeClient::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::RetCode XMLRPCNodeClient::handleEvt(std::string& xmlResp)
{
    INodeClient::RetCode retCode = INodeClient::RCError;

    xmlResp.clear();
    const XmlRpc::XmlRpcValue params(mNodeId);
    XmlRpc::XmlRpcValue result;
    mConnected = this->executeThreadSafe("HandleEvt", params, result);
    if (mConnected)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCNodeClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == INodeClient::RCError)
        {
            LOG("XMLRPCNodeClient::handleEvt() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        xmlResp = resultStruct[RESPONSE_TAG];
//        LOG("XMLRPCNodeClient::handleEvt()\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
XMLRPCNodeClient::RetCode XMLRPCNodeClient::sendEvt(const std::string& xmlEvt, std::string& xmlResp)
{
    INodeClient::RetCode retCode = INodeClient::RCError;

    xmlResp.clear();
    XmlRpc::XmlRpcValue params;
    params[0] = mNodeId;
    params[1] = xmlEvt;
    XmlRpc::XmlRpcValue result;
    mConnected = this->executeThreadSafe("SendEvt", params, result);
    if (mConnected)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCNodeClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == INodeClient::RCError)
        {
            LOG("XMLRPCNodeClient::sendEvt() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        xmlResp = resultStruct[RESPONSE_TAG];
//        LOG("XMLRPCNodeClient::sendEvt()\nxmlEvt=\n" + xmlEvt + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool XMLRPCNodeClient::executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result)
{
    if (pthread_mutex_lock(&mCallsMutex) != 0)
        return false;
    bool success = this->execute(method, params, result);
    if (pthread_mutex_unlock(&mCallsMutex) != 0)
        return false;

    // Fault ?
    if (this->isFault())
    {
        int offset = 0;
        XmlRpc::XmlRpcValue faultStruct(result.toXml(), &offset);
        int faultCode = faultStruct["faultCode"];
        std::string faultString = faultStruct["faultString"];
        LOG("XMLRPCNodeClient::executeThreadSafe() Method=" + (std::string)method + ", Fault:" + convert2string(faultCode) + ", " + faultString);
        success = false;
    }

    return success;
}

//-------------------------------------------------------------------------------------
std::string XMLRPCNodeClient::convert2string(int value)
{
    std::stringstream sstr;
    sstr << value;
    return (std::string)sstr.str();
}

//-------------------------------------------------------------------------------------
