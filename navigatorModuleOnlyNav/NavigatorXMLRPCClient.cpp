#include "NavigatorXMLRPCClient.h"
#include "tinyxml.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(const std::string& host, int port, const std::string& extras)
{
    mP2NClient = IP2NClient::createClient(host, port, extras);
    mP2NClient->setLogger(this);
    mSharedCnx = 0;
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(NavigatorXMLRPCClient& sharedCnx)
{
    mP2NClient = IP2NClient::createClient(sharedCnx.mP2NClient->getHost(), sharedCnx.mP2NClient->getPort(), sharedCnx.mP2NClient->getExtras());
    mP2NClient->setLogger(this);
    mSharedCnx = sharedCnx.mP2NClient;
    mP2NClient->shareCnx(mSharedCnx);
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::~NavigatorXMLRPCClient()
{
    OGRE_LOG("NavigatorXMLRPCClient::~NavigatorXMLRPCClient() mSharedCnx=" + StringConverter::toString(mSharedCnx) + " mP2NClient->isConnected()=" + StringConverter::toString(mP2NClient->isConnected()));
    if ((mSharedCnx == 0) && mP2NClient->isConnected())
        mP2NClient->logout();
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::login(const XmlLogin& xmlLogin, std::list<EntityUID>& myXmlEntities)
{
    std::string xmlParams;
    std::string xmlResp;

    myXmlEntities.clear();

    xmlParams.append("<solipsis>").append(xmlLogin.toXmlString()).append("</solipsis>");
    IP2NClient::RetCode retCode = mP2NClient->login(xmlParams, xmlResp);
    if (retCode != IP2NClient::RCOk)
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
    if ((elt = xmlDoc.RootElement()->FirstChildElement("entities")) == 0)
        return true;
    for (elt = elt->FirstChildElement("entity"); elt != 0; elt = elt->NextSiblingElement("entity"))
    {
        const char* attr = elt->Attribute("uid");
        if (attr != 0)
        {
            EntityUID uid;
            convertStringToEntityUID(attr, uid);
            myXmlEntities.push_back(uid);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::logout()
{
    return (mP2NClient->logout() == IP2NClient::RCOk);
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::isConnected()
{
    return mP2NClient->isConnected();
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool NavigatorXMLRPCClient::handleEvt(RefCntPoolPtr<XmlEvt>& xmlEvt)
#else
bool NavigatorXMLRPCClient::handleEvt(XmlEvt** xmlEvt)
#endif
{
    std::string xmlResp;

#ifdef POOL
    xmlEvt = RefCntPoolPtr<XmlEvt>::nullPtr;
#else
    *xmlEvt = 0;
#endif
    IP2NClient::RetCode retCode = mP2NClient->handleEvt(xmlResp);
    if (retCode == IP2NClient::RCNoEvt)
        return true;
    if (retCode != IP2NClient::RCOk)
        return false;

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(xmlResp.c_str());
    // check for errors
    if (xmlDoc.Error() || (strcmp(xmlDoc.RootElement()->Value(), "solipsis") != 0))
    {
        OGRE_LOG("Invalid event ! xmlResp=\n" + xmlResp);
        return false;
    }
#ifdef POOL
    xmlEvt.allocate();
    xmlEvt->fromXmlElt(xmlDoc.RootElement());
#else
    (*xmlEvt) = new XmlEvt();
    (*xmlEvt)->fromXmlElt(xmlDoc.RootElement());
#endif

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::sendEvt(const XmlEvt& xmlEvt, std::string& xmlResp)
{
    std::string xmlEvtStr;

    xmlEvtStr.append("<solipsis>").append(xmlEvt.toXmlString()).append("</solipsis>");
    IP2NClient::RetCode retCode = mP2NClient->sendEvt(xmlEvtStr, xmlResp);
    if (retCode != IP2NClient::RCOk)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorXMLRPCClient::logMessage(const std::string& message)
{
    OGRE_LOG(message);
}

//-------------------------------------------------------------------------------------
