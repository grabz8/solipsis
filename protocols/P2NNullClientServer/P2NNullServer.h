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