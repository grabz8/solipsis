#ifndef __ULXMLRPCPPP2NClient_h__
#define __ULXMLRPCPPP2NClient_h__

#include <IP2NClient.h>
#include <ulxmlrpcpp/ulxr_tcpip_connection.h> 
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_except.h>
#include <ulxmlrpcpp/ulxr_call.h>
#include <ulxmlrpcpp/ulxr_response.h>
#include <ulxmlrpcpp/ulxr_requester.h>
#include <ulxmlrpcpp/ulxr_http_client.h>
#include <string>
#include <pthread.h>

namespace Solipsis {

/** This class manages ULXMLRPCPP Peer-to-Navigator client interface.
 */
class ULXMLRPCPPP2NClient : public IP2NClient
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
    /// TCP/IP Connection
    ulxr::TcpIpConnection* mTcpIpConnection;
    /// HTTP Protocol
    ulxr::HttpProtocol* mHttpProtocol;
    /// HTTP Client
    ulxr::HttpClient* mHttpClient;
    /// Requester
    ulxr::Requester* mRpcClient;

private:
    /// Mutex on XMLRPC methods calls
    pthread_mutex_t mCallsMutex;
    /// Logging instance
    IP2NClientLogger* mLogger;

public:
    ULXMLRPCPPP2NClient(const std::string& host, int port, int verbosity, const std::string& extras = "");
    virtual ~ULXMLRPCPPP2NClient();

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

private:
    /** Execute (thread-safe) 1 XMLRPC method */
    bool executeThreadSafe(ulxr::MethodCall const& method, ulxr::MethodResponse& result);

    /** Convert 1 integer to 1 string */
    std::string convert2string(int value);

    /** Retrieve 1 extra information */
    bool getExtraInformation(const std::string& extras, const std::string& information, std::string& value);
};

} // namespace Solipsis

#endif // #ifndef __ULXMLRPCPPP2NClient_h__