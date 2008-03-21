#ifndef __P2NNullServer_h__
#define __P2NNullServer_h__

#include "IP2NServer.h"

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

    /** Server singleton pointer */
    static P2NNullServer* ms_singletonPtr;

public:
    /** Constructor */
    P2NNullServer(IP2NServerRequestsHandler* requestsHandler, const std::string& host = "localhost", int port = 8550, int verbosity = 0, const std::string& extras = "");

    /** Destructor */
    virtual ~P2NNullServer();

    /** Initialisation */
    virtual bool init();

    /** Launch the server in a thread */
    virtual bool start();

    /** Launch the server */
    virtual bool listen();

    /** Stop the server */
    virtual bool stop();

    /** Retrieve the requests handler */
    IP2NServerRequestsHandler* getRequestsHandler() { return mRequestsHandler; }

    static P2NNullServer* getSingletonPtr() { return ms_singletonPtr; }
};

} // namespace Solipsis

#endif // #ifndef __P2NNullServer_h__