#ifndef __Navigator_h__
#define __Navigator_h__

#include "OgreApplication.h"
#include "Peer.h"
#include "NavigatorXMLRPCClient.h"
#include "NodeEventListener.h"
#include "NavigatorGUI.h"
#include "Avatar.h"
#include "Scene.h"

#ifdef PHYSICS
#include "OgreOde_Core.h"
#endif

class Navigator : public OgreApplication, public NodeEventListener
{
public:
    enum ConnectionMode {
        CMExistingNode, // Use an existing node
        CMStartNewNode  // Start a new node for this session
    };
    enum Status {
        SReady,         // Node is connected to the Solipsis world
        SBusy,          // Node is an unstable state w.r.t. the Solipsis network: it is either trying to connect or to repair its connectivity
        SUnavailable    // Node is not connected to the Solipsis world
    };

protected:
    ConnectionMode mConnectionMode;
    Status mStatus;
    int mUdpPort;
    String mHost;
    int mPort;

    NavigatorXMLRPCClient* mXmlRpcClient;

    std::list<Peer*> mPeersList;
    std::map<String,OgrePeer*> mOgrePeersMap;

    NavigatorGUI* mNavigatorGUI;

    Avatar* mUserAvatar;

#ifdef PHYSICS
    OgreOde::World* mPhysicsWorld;
    OgreOde::StepHandler* mPhysicsStepHandler;
    OgreOde::TriangleMeshGeometry* mPhysicsWorldGeometry;
#endif

    bool mFakeTerrain;

public:
    Navigator();
    ~Navigator();

    bool isConnected();

    // Get and set
    ConnectionMode getConnectionMode();
    void setConnectionMode(ConnectionMode connectionMode);
    Status getStatus();
    int getConnectionUdpPort();
    void setConnectionUdpPort(int udpPort);
    String getConnectionHost();
    void setConnectionHost(String host);
    int getConnectionPort();
    void setConnectionPort(int port);

    std::map<String,OgrePeer*>::iterator getOgrePeersIteratorBegin();
    std::map<String,OgrePeer*>::iterator getOgrePeersIteratorEnd();

    NavigatorGUI* getNavigatorGUI();

    Avatar* getUserAvatar();

#ifdef PHYSICS
    OgreOde::World* getPhysicsWorld();
    OgreOde::StepHandler* getPhysicsStepHandler();
    OgreOde::TriangleMeshGeometry* getPhysicsWorldGeometry();
#endif

    // Demonstrators
    void fakeSurroundingArea(int index);
#ifdef DEMO_NAVI1
    void demoNavi1();
#endif
#ifdef DEMO_NAVI2
    void demoNavi2();
#endif
#ifdef DEMO_PHYSICS1
    void demoPhysics1();
#endif

    bool quit();
    bool connect();
    bool sendMessage(const String& message);

    void processEvents();

protected:
    virtual void createSceneManager(); 
    virtual void createFrameListener();

    virtual void createScene();

    virtual void createGUI();

    void setStatus(String& statusString);

    void cleanUpPeers(bool cleanUpLocalPeers);

    Avatar* generateAvatarFromPeer(Peer* peer);
    Scene* generateSceneFromPeer(Peer* peer);

    virtual void onPeerNew(NodeEvent::DatasPeerNew& evtDatas);
    virtual void onPeerLost(NodeEvent::DatasPeerLost& evtDatas);
    virtual void onStatusChanged(NodeEvent::DatasStatusChanged& evtDatas);
};

#endif // #ifndef __Navigator_h__