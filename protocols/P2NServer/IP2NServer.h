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

#ifndef __IP2NServer_h__
#define __IP2NServer_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _P2NSERVER_EXPORT __declspec( dllexport )
#define _P2NSERVER_IMPORT

#if defined(P2NSERVER_SELF_BUILD)
#  	define P2NSERVER_EXPORT _P2NSERVER_EXPORT
#else
#  	define P2NSERVER_EXPORT _P2NSERVER_IMPORT
#endif

#  	define P2NSERVER_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _P2NSERVER_EXPORT __attribute__((dllexport))
#define _P2NSERVER_IMPORT __attribute__((dllimport))

#if defined(P2NSERVER_SELF_BUILD)
#  	define P2NSERVER_EXPORT _P2NSERVER_EXPORT
#  	define P2NSERVER_CW_EXPORT
#else
#  	define P2NSERVER_EXPORT
#  	define P2NSERVER_CW_EXPORT _P2NSERVER_IMPORT
#endif

#endif


#define P2NSERVERDIRECT_EXPORT _P2NSERVER_EXPORT

#include <string>
#include <IP2NClient.h>
#include <XmlDatas.h>

namespace Solipsis {

/** This class provide logging capacities interface.
 */
class IP2NServerLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class represents the requests handler interface.
 */
class IP2NServerRequestsHandler
{
public:
    virtual IP2NClient::RetCode login(const std::string& xmlParamsStr, NodeId& nodeId, std::string& xmlRespStr) = 0;
    virtual IP2NClient::RetCode logout(NodeId& nodeId) = 0;
    virtual IP2NClient::RetCode handleEvt(const NodeId& nodeId, std::string& xmlRespStr) = 0;
    virtual IP2NClient::RetCode sendEvt(const NodeId& nodeId, const std::string& xmlEvtStr, std::string& xmlRespStr) = 0;
};

/** This class represents a generic Peer-to-Navigator server interface.
*/
class P2NSERVER_EXPORT IP2NServer
{
public:
    /** Creates a server.
    */
    static IP2NServer* createServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host, int port, int verbosity, const std::string& extras);

    /** Destroys a server.
    */
    static bool destroyServer(IP2NServer* server);

	/** Initialisation
	 */
	virtual bool init() = 0;

	/** Launch the server in a thread
	 */
	virtual bool start() = 0;

	/** Launch the server
	 */
	virtual bool listen() = 0;

	/** Stop the server
	 */
	virtual bool stop() = 0;

    /** setLogger.
    @remarks An implementation must be supplied for this method.
    @param logger The logger instance
    */
    virtual void setLogger(IP2NServerLogger* logger) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IP2NServer_h__