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

#ifndef __IP2NClient_h__
#define __IP2NClient_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _P2NCLIENT_EXPORT __declspec( dllexport )
#define _P2NCLIENT_IMPORT

#if defined(P2NCLIENT_SELF_BUILD)
#  	define P2NCLIENT_EXPORT _P2NCLIENT_EXPORT
#else
#  	define P2NCLIENT_EXPORT _P2NCLIENT_IMPORT
#endif

#  	define P2NCLIENT_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _P2NCLIENT_EXPORT __attribute__((dllexport))
#define _P2NCLIENT_IMPORT __attribute__((dllimport))

#if defined(P2NCLIENT_SELF_BUILD)
#  	define P2NCLIENT_EXPORT _P2NCLIENT_EXPORT
#  	define P2NCLIENT_CW_EXPORT
#else
#  	define P2NCLIENT_EXPORT
#  	define P2NCLIENT_CW_EXPORT _P2NCLIENT_IMPORT
#endif

#endif


#define P2NCLIENTDIRECT_EXPORT _P2NCLIENT_EXPORT

#include <string>
#include <XmlDatas.h>

namespace Solipsis {

/** This class provide logging capacities interface.
 */
class IP2NClientLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class represents a generic Peer-to-Navigator client interface.
*/
class P2NCLIENT_EXPORT IP2NClient
{
public:
    /** Enumeration of possible return codes */
    enum RetCode {
        RCOk,
        RCError,
        RCNoEvt
    };

public:
    /** Creates a client.
    */
    static IP2NClient* createClient(const std::string& host, int port, int verbosity, const std::string& extras);

    /** Destroys a client.
    */
    static bool destroyClient(IP2NClient* client);

    /** Return host used.
    @remarks An implementation must be supplied for this method.
    */
    virtual const std::string& getHost() = 0;
    /** Return port used.
    @remarks An implementation must be supplied for this method.
    */
    virtual int getPort() = 0;
    /** Return verbosity used.
    @remarks An implementation must be supplied for this method.
    */
    virtual int getVerbosity() = 0;
    /** Return extras used.
    @remarks An implementation must be supplied for this method.
    */
    virtual const std::string& getExtras() = 0;
    /** Share an already existing connection (nodeId).
    @remarks An implementation must be supplied for this method.
    @param P2NClient The client connection to share with
    */
    virtual void shareCnx(IP2NClient* P2NClient) = 0;

    /** login request.
    @remarks An implementation must be supplied for this method.
    @param xmlParams The xml-formatted string containing login parameters
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded
    */
    virtual RetCode login(const std::string& xmlParams, std::string& xmlResp) = 0;
    /** logout request.
    @remarks An implementation must be supplied for this method.
    @returns OK if call suceeded
    */
    virtual RetCode logout() = 0;
    /** isConnected.
    @remarks An implementation must be supplied for this method.
    @returns True if client is connected
    */
    virtual bool isConnected() = 0;
    /** getNodeId.
    @remarks An implementation must be supplied for this method.
    @returns The nodeId if client is connected
    */
    virtual const NodeId& getNodeId() = 0;

    /** handleEvt request.
    @remarks An implementation must be supplied for this method.
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded, NOEVT if no event to handle
    */
    virtual RetCode handleEvt(std::string& xmlResp) = 0;
    /** sendEvt request.
    @remarks An implementation must be supplied for this method.
    @param xmlEvt The string containing the xml-formatted event
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded
    */
    virtual RetCode sendEvt(const std::string& xmlEvt, std::string& xmlResp) = 0;

    /** setLogger.
    @remarks An implementation must be supplied for this method.
    @param logger The logger instance
    */
    virtual void setLogger(IP2NClientLogger* logger) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IP2NClient_h__