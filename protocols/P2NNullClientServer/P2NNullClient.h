#ifndef __P2NNullClient_h__
#define __P2NNullClient_h__

#include "IP2NClient.h"
#include <string>

namespace Solipsis {

/** This class manages null Peer-to-Navigator client interface.
 */
class P2NNullClient : public IP2NClient
{
protected:
    /// Host
    std::string mHost;
    /// Port
    int mPort;
    /// URI
    std::string mUri;
    /// Node identifier
    NodeId mNodeId;
    /// Connection state
    bool mConnected;

private:
    /// Logging instance
    IP2NClientLogger* mLogger;

public:
    P2NNullClient(const std::string& host, int port, const std::string& uri);
    virtual ~P2NNullClient();

    /// @copydoc IP2NClient::getHost
    const std::string& getHost() { return mHost; }
    /// @copydoc IP2NClient::getPort
    int getPort() { return mPort; }
    /// @copydoc IP2NClient::getExtras
    const std::string& getExtras() { return mUri; }
    /// @copydoc IP2NClient::shareCnx
    void shareCnx(IP2NClient* P2NClient);

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
    void setLogger(IP2NClientLogger* logger) { mLogger = logger; }

private:
    std::string convert2string(int value);
};

} // namespace Solipsis

#endif // #ifndef __P2NNullClient_h__