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

#ifndef __XMLRPCP2NClient_h__
#define __XMLRPCP2NClient_h__

#include <IP2NClient.h>
#include <XmlRpc.h>
#include <string>
#include <pthread.h>

namespace Solipsis {

/** This class manages XMLRPC++ Peer-to-Navigator client interface.
 */
class XMLRPCP2NClient : public IP2NClient, public XmlRpc::XmlRpcClient, public XmlRpc::XmlRpcErrorHandler, public XmlRpc::XmlRpcLogHandler
{
protected:
    static const char RETCODE_TAG[];
    static const char RESPONSE_TAG[];
    static const char NODEID_TAG[];

protected:
    /// Host
    std::string mHost;
    /// Port
    int mPort;
    /// Verbosity
    int mVerbosity;
    /// Additional informations
    std::string mExtras;
    /// Number of attempts
    unsigned int mNbAttempts;
    /// Node identifier
    NodeId mNodeId;
    /// Connection state
    bool mConnected;

private:
    /// Mutex on XMLRPC methods calls
    pthread_mutex_t mCallsMutex;
    /// Logging instance
    IP2NClientLogger* mLogger;

public:
    XMLRPCP2NClient(const std::string& host, int port, int verbosity, const std::string& extras = "");
    virtual ~XMLRPCP2NClient();

    /// @copydoc IP2NClient::getHost
    virtual const std::string& getHost() { return mHost; }
    /// @copydoc IP2NClient::getPort
    virtual int getPort() { return mPort; }
    /// @copydoc IP2NClient::getVerbosity
    virtual int getVerbosity() { return mVerbosity; }
    /// @copydoc IP2NClient::getExtras
    virtual const std::string& getExtras() { return mExtras; }
    /// @copydoc IP2NClient::shareCnx
    virtual void shareCnx(IP2NClient* P2NClient);

    /// @copydoc IP2NClient::login
    virtual RetCode login(const std::string& xmlParams, std::string& xmlResp);
    /// @copydoc IP2NClient::logout
    virtual RetCode logout();
    /// @copydoc IP2NClient::isConnected
    virtual bool isConnected();
    /// @copydoc IP2NClient::getNodeId
    virtual const NodeId& getNodeId();

    /// @copydoc IP2NClient::handleEvt
    virtual RetCode handleEvt(std::string& xmlResp);
    /// @copydoc IP2NClient::sendEvt
    virtual RetCode sendEvt(const std::string& xmlEvt, std::string& xmlResp);

    /// @copydoc IP2NClient::setLogger
    virtual void setLogger(IP2NClientLogger* logger) { mLogger = logger; }

    /// See XmlRpc::XmlRpcErrorHandler
    virtual void error(const char* msg) { if (mLogger != 0) mLogger->logMessage("XMLRPCP2NClient::error() " + std::string(msg)); }
    /// See XmlRpc::XmlRpcLogHandler
    virtual void log(int level, const char* msg) { if (mLogger != 0) mLogger->logMessage("XMLRPCP2NClient::log() " + std::string(msg)); }

private:
    /** Execute (thread-safe) 1 XMLRPC method */
    bool executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result);

    /** Convert 1 integer to 1 string */
    std::string convert2string(int value);

    /** Retrieve 1 extra information */
    bool getExtraInformation(const std::string& extras, const std::string& information, std::string& value);
};

} // namespace Solipsis

#endif // #ifndef __XMLRPCP2NClient_h__