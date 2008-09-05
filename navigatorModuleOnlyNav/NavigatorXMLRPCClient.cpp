/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "NavigatorXMLRPCClient.h"
#include "tinyxml.h"
#include <CTLog.h>
#include "DebugHelpers.h"
#include "Platform.h"

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(const std::string& host, int port, int verbosity, const std::string& extras)
{
    mP2NClient = IP2NClient::createClient(host, port, verbosity, extras);
    mP2NClient->setLogger(this);
    mSharedCnx = 0;
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(NavigatorXMLRPCClient& sharedCnx)
{
    mP2NClient = IP2NClient::createClient(sharedCnx.mP2NClient->getHost(), sharedCnx.mP2NClient->getPort(), sharedCnx.mP2NClient->getVerbosity(), sharedCnx.mP2NClient->getExtras());
    mP2NClient->setLogger(this);
    mSharedCnx = sharedCnx.mP2NClient;
    mP2NClient->shareCnx(mSharedCnx);
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::~NavigatorXMLRPCClient()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorXMLRPCClient::~NavigatorXMLRPCClient() mSharedCnx=%s mP2NClient->isConnected()=%s", LOGHANDLER_LOGBOOL(mSharedCnx), LOGHANDLER_LOGBOOL(mP2NClient->isConnected()));
    if ((mSharedCnx == 0) && mP2NClient->isConnected())
        mP2NClient->logout();
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::login(const XmlLogin& xmlLogin, NodeId& nodeId)
{
    std::string xmlParams;
    std::string xmlResp;

    xmlParams.append("<solipsis>").append(xmlLogin.toXmlString()).append("</solipsis>");
    IP2NClient::RetCode retCode = mP2NClient->login(xmlParams, xmlResp);
    if (retCode != IP2NClient::RCOk)
        return false;

    nodeId = mP2NClient->getNodeId();

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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Invalid event ! xmlResp=\n%s", xmlResp.c_str());
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
    LOGHANDLER_LOG(LogHandler::VL_DEBUG, message.c_str());
}

//-------------------------------------------------------------------------------------
