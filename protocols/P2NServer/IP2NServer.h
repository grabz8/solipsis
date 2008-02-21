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
#include "IP2NClient.h"
#include "XmlDatas.h"

namespace Solipsis {

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
};

} // namespace Solipsis

#endif // #ifndef __IP2NServer_h__