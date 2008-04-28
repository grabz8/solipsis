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