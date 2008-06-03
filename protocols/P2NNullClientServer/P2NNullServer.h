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

#ifndef __P2NNullServer_h__
#define __P2NNullServer_h__

#include <IP2NServer.h>

namespace Solipsis {

/** Null Peer-to-Navigator server for a Solipsis host
 */
class P2NNullServer : public IP2NServer
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

    /** Logging instance */
    IP2NServerLogger* mLogger;

    /** Server singleton pointer */
    static P2NNullServer* ms_singletonPtr;

public:
    /** Constructor */
    P2NNullServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host = "localhost", int port = 8550, int verbosity = 0, const std::string& extras = "");

    /** Destructor */
    virtual ~P2NNullServer();

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

    static P2NNullServer* getSingletonPtr() { return ms_singletonPtr; }
};

} // namespace Solipsis

#endif // #ifndef __P2NNullServer_h__