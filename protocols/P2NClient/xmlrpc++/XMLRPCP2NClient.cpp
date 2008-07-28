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

#include "XMLRPCP2NClient.h"
#include <tinyxml.h>
#include <sstream>

#define LOG(message) if (mLogger != 0) mLogger->logMessage(message);

namespace Solipsis {

const char XMLRPCP2NClient::RETCODE_TAG[] = "retCode";
const char XMLRPCP2NClient::RESPONSE_TAG[] = "response";
const char XMLRPCP2NClient::NODEID_TAG[] = "nodeId";

//-------------------------------------------------------------------------------------
IP2NClient* IP2NClient::createClient(const std::string& host, int port, int verbosity, const std::string& extras)
{
    return new XMLRPCP2NClient(host, port, verbosity, extras);
}

//-------------------------------------------------------------------------------------
bool IP2NClient::destroyClient(IP2NClient* client)
{
    delete client;

    return true;
}

//-------------------------------------------------------------------------------------
XMLRPCP2NClient::XMLRPCP2NClient(const std::string& host, int port, int verbosity, const std::string& extras) :
    mHost(host),
    mPort(port),
    mVerbosity(verbosity),
    mExtras(extras),
    mNbAttempts(1),
    XmlRpcClient(host.c_str(), port, 0),
    mNodeId(""),
    mConnected(false),
    mCallsMutex(PTHREAD_MUTEX_INITIALIZER),
    mLogger(0)
{
    XmlRpc::XmlRpcErrorHandler::setErrorHandler(this);
    XmlRpc::XmlRpcLogHandler::setLogHandler(this);
    XmlRpc::setVerbosity(mVerbosity);

    std::string value;
    if (getExtraInformation(mExtras, "nattempts=", value))
        mNbAttempts = atoi(value.c_str());
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
        retCode = (IP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
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
        retCode = (IP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
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
        retCode = (IP2NClient::RetCode)(int)resultStruct[RETCODE_TAG];
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
    bool success = false;

    if (pthread_mutex_lock(&mCallsMutex) != 0)
        return false;
    for(int attempts=mNbAttempts;attempts>0;attempts--)
    {
        success = this->execute(method, params, result);

        // Fault ?
        if (this->isFault())
        {
            int offset = 0;
            XmlRpc::XmlRpcValue faultStruct(result.toXml(), &offset);
            int faultCode = faultStruct["faultCode"];
            std::string faultString = faultStruct["faultString"];
            LOG("XMLRPCP2NClient::executeThreadSafe() Method=" + (std::string)method + ", Fault:" + convert2string(faultCode) + ", " + faultString);
            success = false;
            // Timeout ? next attempt ...
            if (faultCode == 500)
                continue;
        }
        break;
    }
    if (pthread_mutex_unlock(&mCallsMutex) != 0)
        return false;

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
bool XMLRPCP2NClient::getExtraInformation(const std::string& extras, const std::string& information, std::string& value)
{
    std::string::size_type idx, spc;
    idx = extras.find(information);
    if (idx == std::string::npos)
        return false;
    idx += information.length();
    if (extras.length() < idx)
        return false;
    spc = extras.find(' ', idx);
    if (spc == idx)
        return false;
    if (spc == std::string::npos)
        spc = extras.length();
    value = extras.substr(idx, spc - idx + 1);
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
