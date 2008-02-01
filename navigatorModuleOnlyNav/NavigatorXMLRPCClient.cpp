#include "NavigatorXMLRPCClient.h"
#include "tinyxml.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(const std::string& host, int port, const std::string& uri) :
    mNodeClient(host, port, uri),
    mSharedCnx(0)
{
    mNodeClient.setLogger(this);
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(NavigatorXMLRPCClient& sharedCnx) :
    mNodeClient(sharedCnx.mNodeClient.getHost(), sharedCnx.mNodeClient.getPort(), sharedCnx.mNodeClient.getUri()),
    mSharedCnx(&sharedCnx.mNodeClient)
{
    mNodeClient.setLogger(this);
    mNodeClient.shareCnx(mSharedCnx);
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::~NavigatorXMLRPCClient()
{
    if (!mSharedCnx && mNodeClient.isConnected())
        mNodeClient.logout();
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::login(const XmlLogin& xmlLogin, std::list<ObjectUID>& myObjects)
{
    std::string xmlParams;
    std::string xmlResp;

    myObjects.clear();

    xmlParams.append("<solipsis>").append(xmlLogin.toXmlString()).append("</solipsis>");
    XMLRPCNodeClient::RetCode retCode = mNodeClient.login(xmlParams, xmlResp);
    if (retCode != INodeClient::RCOk)
        return false;

    TiXmlDocument xmlDoc;
    TiXmlElement* elt;
    xmlDoc.Parse(xmlResp.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        OGRE_LOG("Invalid response ! xmlResp=\n" + xmlResp);
        return false;
    }
    if ((elt = xmlDoc.RootElement()->FirstChildElement("objects")) == 0)
        return true;
    for (elt = elt->FirstChildElement("object"); elt != 0; elt = elt->NextSiblingElement("object"))
    {
        const char* attr = elt->Attribute("uid");
        if (attr != 0)
            myObjects.push_back(static_cast<ObjectUID>(strtoul(attr, 0, 10)));
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::logout()
{
    return (mNodeClient.logout() == INodeClient::RCOk);
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::isConnected()
{
    return mNodeClient.isConnected();
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::handleEvt(XmlEvt** xmlEvt)
{
    std::string xmlResp;

    *xmlEvt = 0;
    XMLRPCNodeClient::RetCode retCode = mNodeClient.handleEvt(xmlResp);
    if (retCode == INodeClient::RCNoEvt)
        return true;
    if (retCode != INodeClient::RCOk)
        return false;

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlResp.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        OGRE_LOG("Invalid event ! xmlResp=\n" + xmlResp);
        return false;
    }
    (*xmlEvt) = new XmlEvt();
    (*xmlEvt)->fromXmlElt(xmlDoc.RootElement());

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::sendEvt(const XmlEvt& xmlEvt, std::string& xmlResp)
{
    std::string xmlEvtStr;

    xmlEvtStr.append("<solipsis>").append(xmlEvt.toXmlString()).append("</solipsis>");
    XMLRPCNodeClient::RetCode retCode = mNodeClient.sendEvt(xmlEvtStr, xmlResp);
    if (retCode != INodeClient::RCOk)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorXMLRPCClient::logMessage(const std::string& message)
{
    OGRE_LOG(message);
}

//-------------------------------------------------------------------------------------
