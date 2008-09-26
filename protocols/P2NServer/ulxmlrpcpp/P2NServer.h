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
#include <ulxmlrpcpp/ulxmlrpcpp.h>
#include <ulxmlrpcpp/ulxr_tcpip_connection.h>
#include <ulxmlrpcpp/ulxr_ssl_connection.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_except.h>
#include <ulxmlrpcpp/ulxr_signature.h>
#include <ulxmlrpcpp/ulxr_mtrpc_server.h>
#include <IP2NServer.h>

namespace Solipsis {

class AbstractP2NMethod;

/** ULXMLRPCPP Peer-to-Navigator server for a Solipsis host
 */
class P2NServer : public IP2NServer
{
private:
    /** Requests handler */
    IP2NServerRequestsHandler* mRequestsHandler;

    /** Host */
    std::string mHost;
    /** Port Number */
    unsigned short mPort;
    /** Additional informations */
    std::string mExtras;

    /** Server ULXMLRPCPP */
    ulxr::MultiThreadRpcServer *mMTServer;

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
    /** Constructor
     */
    P2NServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host = "localhost", unsigned short port = 8550, int verbosity = 0, const std::string& extras = "");

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

    /** Retrieve the requests handler */
    IP2NServerRequestsHandler* getRequestsHandler() { return mRequestsHandler; }

    /** Retrieve the internal server instance */
    ulxr::MultiThreadRpcServer* getInternalServer() { return mMTServer; }

private:
    /** Register all method you want */
    bool registerAllMethods();

    /** Start routine of the thread */
    static void *startThread(void* ptr);

    /** Retrieve the requests handler */
    bool getExtraInformation(const std::string& extras, const std::string& information, std::string& value);
};

} // namespace Solipsis

#endif // #ifndef __P2NServer_h__