#ifndef __NavigatorXMLRPCClient_h__
#define __NavigatorXMLRPCClient_h__

#define UNPLUG_MODE

#include "XmlRpc.h"
#include "Peer.h"
#include "NodeEvent.h"
#include <pthread.h>

class NavigatorXMLRPCClient : public XmlRpc::XmlRpcClient
{
protected:
    String mConnectionId;
    bool mConnected;

private:
    pthread_mutex_t mCallsMutex;

public:
    NavigatorXMLRPCClient(const char *host, int port, const char *uri=0);
    ~NavigatorXMLRPCClient();

    bool connect();
    bool disconnect();
    bool isConnected();
    bool getAllPeers(std::list<Peer*> &peersList);
    bool getEvents(std::list<NodeEvent*> &nodeEventsList);
    bool getStatus(Ogre::String &status);
    bool getDesc(const Peer &peer);
    bool move(const Peer &peer);
    bool sendMessage(const Ogre::String& message, std::list<Peer*> &peersList);

private:
    Peer* createPeerFromXml(std::string& peerXml);
    Ogre::String removeFirstLevelOfXmlAnchor(Ogre::String& xml);
    bool executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result);
};

#endif // #ifndef __NavigatorXMLRPCClient_h__