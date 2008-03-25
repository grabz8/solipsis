#ifndef __Navigator_h__
#define __Navigator_h__

#include "Instance.h"
#include "NavigatorXMLRPCClient.h"
#include "NodeEventListener.h"
#include "Avatar.h"
#include "Scene.h"
#include "OgrePeerManager.h"
#include "NavigatorGUI.h"
#include "LuaBinding.h"
#include "NavigatorLua.h"
#include "Selection.h"
#include "Transformations.h"
#include "Event.h"

namespace Solipsis {

class Modeler;

/** The main class of Navigator application.
 */
class Navigator : public Instance, public NodeEventListener, public IOgrePeerManagerCallbacks
{
public:
    enum State {
        SLogin,         // User is choosing options, ... and finally log on world
        SInWorld,       // GUI displayed when user is in the world
		SModeling       // GUI displayed when user is modeling an object
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
        QFAvatar = QFNaviPanel<<1,
        QFObject = QFAvatar<<1,
        QFGizmo = QFObject<<1
	};

private:
    static Navigator* ms_singletonPtr;

protected:
    State mState;
    ConnectionMode mConnectionMode;
    NodeStatus mNodeStatus;
    int mUdpPort;
    String mHost;
    int mPort;

    NavigatorXMLRPCClient* mXmlRpcClient;

    OgrePeerManager* mOgrePeerManager;

    NavigatorGUI* mNavigatorGUI;

    Real mMaxObjectPickingDistance;
    Real mMaxAvatarPickingDistance;
    Real mMaxNaviPickingDistance;
    RaySceneQuery* mRaySceneQuery;
    MovableObject* mPickedMovable;
    Real closestDistance;
    Vector3 closestHitPoint;
    Vector2 closestUV;
    Vector2 closestTriUV0, closestTriUV1, closestTriUV2;

    lua_State* mLuaState;
    NavigatorLua* mNavigatorLua;

    Avatar* mUserAvatar;

    bool mFakeTerrain;

public:
    Navigator(const String name, IApplication* application);
    ~Navigator();
    static Navigator* getSingletonPtr();

    bool isConnected();

    // Get and set
    State getState();
	void setState(State newState) {mState = newState;};
    ConnectionMode getConnectionMode();
    void setConnectionMode(ConnectionMode connectionMode);
    NodeStatus getNodeStatus();
    int getConnectionUdpPort();
    void setConnectionUdpPort(int udpPort);
    String getConnectionHost();
    void setConnectionHost(String host);
    int getConnectionPort();
    void setConnectionPort(int port);

    OgrePeerManager* getOgrePeerManager();
    NavigatorGUI* getNavigatorGUI();

    lua_State* getLuaState();
    void setNavigatorLua(NavigatorLua* navigatorLua);
    NavigatorLua* getNavigatorLua();

    Avatar* getUserAvatar();

    // Demonstrators
    void fakeSurroundingArea(int index);
#ifdef DEMO_NAVI1
    void demoNavi1();
#endif
#ifdef DEMO_NAVI2
    void demoNavi2(const String url);
#endif
#ifdef DEMO_PHYSICS1
    void demoPhysics1();
#endif

    // Navi 3D panels management
    Entity* getNaviEntity(const String& naviName);

    // Mouse ray picking
    void resetMousePicking();
    bool computeMousePicking(Ray& mouseRay);
    bool is1NaviHitByMouse(String& naviName, int& naviX, int& naviY);
    void computeNaviHit(const String& naviName,
                        Vector2& closestUV,
                        Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                        int& naviX, int& naviY);
    bool is1AvatarHitByMouse(Avatar*& avatar);

    bool quit();
    bool connect();
    bool sendMessage(const String& message);
    bool contextItemSelected(const String& message);

    // process events received by node
    void processEvents();

    /** See IOgrePeerManagerCallbacks. */
    virtual bool OnAvatarNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer);
    /** See IOgrePeerManagerCallbacks. */
    virtual bool OnSceneNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer);

    // Modeler part

    /** Start modeling mode. */
    bool startModeling();
    /** Stop the modeling mode. */
    bool endModeling();
    /** Create a box. */
    bool createBox();
    /** Create a corner. */
    bool createCorner();
    /** Create a pyramid. */
    bool createPyramid();
    /** Create a prism. */
    bool createPrism();
    /** Create a cylinder. */
    bool createCylinder();
    /** Create a half cylinder. */
    bool createHalfCyl();
    /** Create a cone. */
    bool createCone();
    /** Create a Halfcone. */
    bool createHalfCone();
    /** Create a sphere. */
    bool createSphere();
    /** Create a half sphere. */
    bool createHalfSphere();
    /** Create a torus. */
    bool createTorus();
    /** Create a tube. */
    bool createTube();
    /** Create a ring. */
    bool createRing();
    /** Create a mesh. */
    bool createMesh();
    /** Load from a XML file.*/
    bool XMLLoad();
    /** Save to a XML file. */
    bool XMLSave(bool all = false, const char* pathToSave = NULL);

    /// The modeler object
    Modeler		*mModeler;
    bool		isOnLeftCTRL;

    void onMouseMoved(const MouseEvt& evt);
    void onMousePressed(const MouseEvt& evt);
    void onMouseReleased(const MouseEvt& evt);

    void MdlrModifGizmo(Vector3 dep);

protected:
    /** These methods implement Instance
    */
    virtual bool initialize();

    virtual bool initPostOgreCore();

    virtual void createSceneManager(); 
    virtual void createFrameListener();

    virtual void createScene();

    virtual bool createGUI();

    virtual bool setWindow(IWindow* w);

    // Locals
    void setNodeStatus(String& nodeStatusString);

    void cleanUpPeers(bool cleanUpLocalPeers);

    bool generateFromPeer(Peer* peer);

    virtual void onPeerNew(NodeEvent::DatasPeerNew& evtDatas);
    virtual void onPeerLost(NodeEvent::DatasPeerLost& evtDatas);
    virtual void onStatusChanged(NodeEvent::DatasStatusChanged& evtDatas);
};

} // namespace Solipsis

#endif // #ifndef __Navigator_h__