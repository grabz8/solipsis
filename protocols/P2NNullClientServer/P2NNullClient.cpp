#include "P2NNullClient.h"
#include "P2NNullServer.h"

#define LOG(message) if (mLogger != 0) mLogger->logMessage(message);

namespace Solipsis {

//-------------------------------------------------------------------------------------
IP2NClient* IP2NClient::createClient(const std::string& host, int port, int verbosity, const std::string& extras)
{
    return new P2NNullClient(host, port, verbosity, "");
}

//-------------------------------------------------------------------------------------
bool IP2NClient::destroyClient(IP2NClient* client)
{
    delete client;

    return true;
}

//-------------------------------------------------------------------------------------
P2NNullClient::P2NNullClient(const std::string& host, int port, int verbosity, const std::string& extras) :
    mHost(host),
    mPort(port),
    mVerbosity(verbosity),
    mExtras(extras),
    mNodeId(""),
    mLogger(0)
{
}

//-------------------------------------------------------------------------------------
P2NNullClient::~P2NNullClient()
{
}

//-------------------------------------------------------------------------------------
void P2NNullClient::shareCnx(IP2NClient* P2NClient)
{
    mNodeId = ((P2NNullClient*)P2NClient)->mNodeId;
    mConnected = ((P2NNullClient*)P2NClient)->mConnected;
}

//-------------------------------------------------------------------------------------
P2NNullClient::RetCode P2NNullClient::login(const std::string& xmlParams, std::string& xmlResp)
{
    P2NNullServer* server = P2NNullServer::getSingletonPtr();
    IP2NServerRequestsHandler* serverRequestsHandler = server->getRequestsHandler();
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    NodeId localNodeId;
    std::string localXmlResp;
    retCode = serverRequestsHandler->login(xmlParams, localNodeId, localXmlResp);
    if (retCode == IP2NClient::RCError)
    {
        LOG("P2NNullClient::login() retCode=" + convert2string(retCode) + ", Error:" + localXmlResp);
        return retCode;
    }
    mNodeId = localNodeId;
    xmlResp = localXmlResp;
    mConnected = true;
    LOG("P2NNullClient::login()\nxmlParams=\n" + xmlParams + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode) + "\nmNodeId=" + mNodeId);

    return retCode;
}

//-------------------------------------------------------------------------------------
P2NNullClient::RetCode P2NNullClient::logout()
{
    P2NNullServer* server = P2NNullServer::getSingletonPtr();
    IP2NServerRequestsHandler* serverRequestsHandler = server->getRequestsHandler();
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    if (!mConnected)
        return IP2NClient::RCOk;

    retCode = serverRequestsHandler->logout(mNodeId);
    if (retCode == IP2NClient::RCError)
    {
        LOG("P2NNullClient::logout() retCode=" + convert2string(retCode) + ", Error detected");
        return retCode;
    }
    mConnected = false;
    LOG("P2NNullClient::logout() retCode=" + convert2string(retCode));

    return retCode;
}

//-------------------------------------------------------------------------------------
bool P2NNullClient::isConnected()
{
    return mConnected;
}

//-------------------------------------------------------------------------------------
const NodeId& P2NNullClient::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
P2NNullClient::RetCode P2NNullClient::handleEvt(std::string& xmlResp)
{
    P2NNullServer* server = P2NNullServer::getSingletonPtr();
    IP2NServerRequestsHandler* serverRequestsHandler = server->getRequestsHandler();
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    std::string localXmlResp;
    retCode = serverRequestsHandler->handleEvt(mNodeId, localXmlResp);
    if (retCode == IP2NClient::RCError)
    {
        LOG("P2NNullClient::handleEvt() retCode=" + convert2string(retCode) + ", Error:" + localXmlResp);
        return retCode;
    }
    xmlResp = localXmlResp;
//    LOG("P2NNullClient::handleEvt()\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));

    return retCode;
}

//-------------------------------------------------------------------------------------
P2NNullClient::RetCode P2NNullClient::sendEvt(const std::string& xmlEvt, std::string& xmlResp)
{
    P2NNullServer* server = P2NNullServer::getSingletonPtr();
    IP2NServerRequestsHandler* serverRequestsHandler = server->getRequestsHandler();
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    std::string localXmlResp;
    retCode = serverRequestsHandler->sendEvt(mNodeId, xmlEvt, localXmlResp);
    if (retCode == IP2NClient::RCError)
    {
        LOG("P2NNullClient::sendEvt() retCode=" + convert2string(retCode) + ", Error:" + localXmlResp);
        return retCode;
    }
    xmlResp = localXmlResp;
//    LOG("P2NNullClient::sendEvt()\nxmlEvt=\n" + xmlEvt + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));

    return retCode;
}

//-------------------------------------------------------------------------------------
std::string P2NNullClient::convert2string(int value)
{
    std::stringstream sstr;
    sstr << value;
    return (std::string)sstr.str();
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
