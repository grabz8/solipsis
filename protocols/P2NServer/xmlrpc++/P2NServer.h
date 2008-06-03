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

#ifndef __P2NServer_h__
#define __P2NServer_h__

#include <vector>
#include <pthread.h>
#include <XmlRpc.h>
#include <IP2NServer.h>

namespace Solipsis {

class AbstractP2NMethod;

/** XMLRPC++ Peer-to-Navigator server for a Solipsis host
 */
class P2NServer : public IP2NServer, public XmlRpc::XmlRpcErrorHandler, public XmlRpc::XmlRpcLogHandler
{
private:
    /** Requests handler */
    IP2NServerRequestsHandler* mRequestsHandler;

    /** Host */
    std::string mHost;
    /** Port Number */
    int mPort;
    /** Additional informations */
    std::string mExtras;

    /** Server XMLRPC++ */
    XmlRpc::XmlRpcServer mServer;

    /** Logging instance */
    IP2NServerLogger* mLogger;

    /** List of XMLRPCMethod declared.
     * Note on XMLRPC++ library : 
     * The default XmlRpcServer is not in charge of the destruction of his method objects
     * --> so we have to do it here(in the destructor) with a reference to the methods
     */
    std::vector<AbstractP2NMethod*> mMethods;

    /** Listenning Thread */
    pthread_t mThread;

public:
    /** Constructor */
    P2NServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host = "localhost", int port = 8550, int verbosity = 0, const std::string& extras = "");

    /** Destructor */
    virtual ~P2NServer();

    /// @copydoc IP2NServer::init
    virtual bool init();

    /// @copydoc IP2NServer::start
    virtual bool start();

    /// @copydoc IP2NServer::listen
    virtual bool listen();

    /// @copydoc IP2NServer::stop
    virtual bool stop();

    /// @copydoc IP2NServer::setLogger
    virtual void setLogger(IP2NServerLogger* logger) { mLogger = logger; }

    /// See XmlRpc::XmlRpcErrorHandler
    virtual void error(const char* msg) { if (mLogger != 0) mLogger->logMessage("P2NServer::error() " + std::string(msg)); }
    /// See XmlRpc::XmlRpcLogHandler
    virtual void log(int level, const char* msg) { if (mLogger != 0) mLogger->logMessage("P2NServer::log() " + std::string(msg)); }

    /** Retrieve the requests handler */
    IP2NServerRequestsHandler* getRequestsHandler() { return mRequestsHandler; }

    /** Retrieve the internal server instance */
    XmlRpc::XmlRpcServer* getInternalServer() { return &mServer; }

private:
    /** Register all method you want */
    bool registerAllMethods();

    /** Start routine of the thread */
    static void *startThread(void* ptr);

    /** Retrieve 1 extra information */
    bool getExtraInformation(const std::string& extras, const std::string& information, std::string& value);
};

} // namespace Solipsis

#endif // #ifndef __P2NServer_h__