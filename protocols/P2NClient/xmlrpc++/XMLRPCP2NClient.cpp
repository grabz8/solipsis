#include "XMLRPCP2NClient.h"
#include "tinyxml.h"
#include <sstream>

#define LOG(message) if (mLogger != 0) mLogger->logMessage(message);

namespace Solipsis {

const char XMLRPCP2NClient::RETCODE_TAG[] = "retCode";
const char XMLRPCP2NClient::RESPONSE_TAG[] = "response";
const char XMLRPCP2NClient::NODEID_TAG[] = "nodeId";

//-------------------------------------------------------------------------------------
IP2NClient* IP2NClient::createClient(const std::string& host, int port, const std::string& extras)
{
    return new XMLRPCP2NClient(host, port, "");
}

//-------------------------------------------------------------------------------------
bool IP2NClient::destroyClient(IP2NClient* client)
{
    delete client;

    return true;
}

//-------------------------------------------------------------------------------------
XMLRPCP2NClient::XMLRPCP2NClient(const std::string& host, int port, const std::string& uri) :
    mHost(host),
    mPort(port),
    mUri(uri),
    XmlRpcClient(host.c_str(), port, uri.empty() ? 0 : uri.c_str()),
    mNodeId(""),
    mConnected(false),
    mCallsMutex(PTHREAD_MUTEX_INITIALIZER),
    mLogger(0)
{
}

//-------------------------------------------------------------------------------------
XMLRPCP2NClient::~XMLRPCP2NClient()
{
}

//-------------------------------------------------------------------------------------
void XMLRPCP2NClient::shareCnx(IP2NClient* P2NClient)
{
    mNodeId = ((XMLRPCP2NClient*)P2NClient)->mNodeId;
    mConnected = ((XMLRPCP2NClient*)P2NClient)->mConnected;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode XMLRPCP2NClient::login(const std::string& xmlParams, std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    const XmlRpc::XmlRpcValue params(xmlParams);
    XmlRpc::XmlRpcValue result;
    mConnected = false;
    bool success = this->executeThreadSafe("Login", params, result);
    if (success)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (IP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == IP2NClient::RCError)
        {
            LOG("XMLRPCP2NClient::login() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        mNodeId = resultStruct[NODEID_TAG];
        xmlResp = resultStruct[RESPONSE_TAG];
        TiXmlDocument xmlDoc;
        xmlDoc.Parse(xmlResp.c_str());
        // check for errors
        if (xmlDoc.Error())
        {
            LOG("XMLRPCP2NClient::login() Unable to parse response !");
            return IP2NClient::RCError;
        }
        mConnected = true;
        LOG("XMLRPCP2NClient::login()\nxmlParams=\n" + xmlParams + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode) + "\nmNodeId=" + mNodeId);
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode XMLRPCP2NClient::logout()
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    if (!mConnected)
        return IP2NClient::RCOk;

    const XmlRpc::XmlRpcValue params(mNodeId);
    XmlRpc::XmlRpcValue result;
    bool success = this->executeThreadSafe("Logout", params, result);
    if (success)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == IP2NClient::RCError)
        {
            LOG("XMLRPCP2NClient::logout() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        mConnected = false;
        LOG("XMLRPCP2NClient::logout() retCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool XMLRPCP2NClient::isConnected()
{
    return mConnected;
}

//-------------------------------------------------------------------------------------
const NodeId& XMLRPCP2NClient::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode XMLRPCP2NClient::handleEvt(std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    const XmlRpc::XmlRpcValue params(mNodeId);
    XmlRpc::XmlRpcValue result;
    bool success = this->executeThreadSafe("HandleEvt", params, result);
    if (success)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == IP2NClient::RCError)
        {
            LOG("XMLRPCP2NClient::handleEvt() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        xmlResp = resultStruct[RESPONSE_TAG];
//        LOG("XMLRPCP2NClient::handleEvt()\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode XMLRPCP2NClient::sendEvt(const std::string& xmlEvt, std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    XmlRpc::XmlRpcValue params;
    params[0] = mNodeId;
    params[1] = xmlEvt;
    XmlRpc::XmlRpcValue result;
    bool success = this->executeThreadSafe("SendEvt", params, result);
    if (success)
    {
        // Parse response
        int offset = 0;
        XmlRpc::XmlRpcValue resultStruct(result.toXml(), &offset);
        retCode = (XMLRPCP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
        if (retCode == IP2NClient::RCError)
        {
            LOG("XMLRPCP2NClient::sendEvt() retCode=" + convert2string(retCode) + ", Error:" + (std::string)resultStruct[RESPONSE_TAG]);
            return retCode;
        }
        xmlResp = resultStruct[RESPONSE_TAG];
//        LOG("XMLRPCP2NClient::sendEvt()\nxmlEvt=\n" + xmlEvt + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool XMLRPCP2NClient::executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result)
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
        LOG("XMLRPCP2NClient::executeThreadSafe() Method=" + (std::string)method + ", Fault:" + convert2string(faultCode) + ", " + faultString);
        success = false;
    }

    return success;
}

//-------------------------------------------------------------------------------------
std::string XMLRPCP2NClient::convert2string(int value)
{
    std::stringstream sstr;
    sstr << value;
    return (std::string)sstr.str();
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
