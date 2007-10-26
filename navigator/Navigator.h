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
    enum State {
        SLogin,         // User is choosing options, ... and finally log on world
        SInWorld        // GUI displayed when user is in the world
    };
    enum ConnectionMode {
        CMExistingNode, // Use an existing node
        CMStartNewNode  // Start a new node for this session
    };
    enum NodeStatus {
        NSReady,         // Node is connected to the Solipsis world
        NSBusy,          // Node is an unstable state w.r.t. the Solipsis network: it is either trying to connect or to repair its connectivity
        NSUnavailable    // Node is not connected to the Solipsis world
    };
    enum QueryFlags
    {
        QFNaviPanel = 1<<0,
        QFAvatars = QFNaviPanel<<1
    };

protected:
    State mState;
    ConnectionMode mConnectionMode;
    NodeStatus mNodeStatus;
    int mUdpPort;
    String mHost;
    int mPort;
    Real mMaxNaviPickingDistance;

    NavigatorXMLRPCClient* mXmlRpcClient;

    std::list<Peer*> mPeersList;
    std::map<String,OgrePeer*> mOgrePeersMap;

    NavigatorGUI* mNavigatorGUI;
    RaySceneQuery* mNaviRaySceneQuery;

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
    State getState();
    ConnectionMode getConnectionMode();
    void setConnectionMode(ConnectionMode connectionMode);
    NodeStatus getNodeStatus();
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

    // Navi 3D panels management
    Entity* getNaviEntity(const String& naviName);
    bool isNaviHitByMouse(Ray& mouseRay, Entity* naviEntity,
                          Real& closestDistance,
                          Vector2& closestUV,
                          Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2);
    void computeNaviHit(const String& naviName,
                        Vector2& closestUV,
                        Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                        int& naviX, int& naviY);
    bool is1NaviHitByMouse(Ray& mouseRay, String& naviName, int& naviX, int& naviY);

    bool quit();
    bool connect();
    bool sendMessage(const String& message);

    // process events received by node
    void processEvents();

protected:
    virtual void createSceneManager(); 
    virtual void createFrameListener();

    virtual void createScene();

    virtual void createGUI();

    void setNodeStatus(String& nodeStatusString);

    void cleanUpPeers(bool cleanUpLocalPeers);

    Avatar* generateAvatarFromPeer(Peer* peer);
    Scene* generateSceneFromPeer(Peer* peer);

    virtual void onPeerNew(NodeEvent::DatasPeerNew& evtDatas);
    virtual void onPeerLost(NodeEvent::DatasPeerLost& evtDatas);
    virtual void onStatusChanged(NodeEvent::DatasStatusChanged& evtDatas);
};

#endif // #ifndef __Navigator_h__