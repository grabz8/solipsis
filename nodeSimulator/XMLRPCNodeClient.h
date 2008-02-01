#ifndef __XMLRPCNodeClient_h__
#define __XMLRPCNodeClient_h__

#include "INodeClient.h"
#include "XmlRpc.h"
#include <string>
#include <pthread.h>

namespace Solipsis {

/** This class provide logging capacities interface.
 */
class XMLRPCNodeClientLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class manages XMLRPC Node client interface.
 */
class XMLRPCNodeClient : public INodeClient, public XmlRpc::XmlRpcClient
{
protected:
    static const char RETCODE_TAG[];
    static const char RESPONSE_TAG[];
    static const char NODEID_TAG[];

protected:
    std::string mHost;
    int mPort;
    std::string mUri;
    NodeId mNodeId;
    bool mConnected;

private:
    pthread_mutex_t mCallsMutex;
    XMLRPCNodeClientLogger* mLogger;

public:
    XMLRPCNodeClient(const std::string& host, int port, const std::string& uri);
    ~XMLRPCNodeClient();

    const std::string& getHost() { return mHost; }
    int getPort() { return mPort; }
    const std::string& getUri() { return mUri; }
    void shareCnx(XMLRPCNodeClient* nodeClient);

    /// @copydoc INodeClient::login
    virtual RetCode login(const std::string& xmlParams, std::string& xmlResp);
    /// @copydoc INodeClient::logout
    virtual RetCode logout();
    /// @copydoc INodeClient::isConnected
    virtual bool isConnected();
    /// @copydoc INodeClient::getNodeId
    virtual const NodeId& getNodeId();

    /// @copydoc INodeClient::handleEvt
    virtual RetCode handleEvt(std::string& xmlResp);
    /// @copydoc INodeClient::sendEvt
    virtual RetCode sendEvt(const std::string& xmlEvt, std::string& xmlResp);

    /** logger.
    @param logger The logger instance
    */
    void setLogger(XMLRPCNodeClientLogger* logger) { mLogger = logger; }

private:
    bool executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result);
    std::string convert2string(int value);
};

} // namespace Solipsis

#endif // #ifndef __XMLRPCNodeClient_h__