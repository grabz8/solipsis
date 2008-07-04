/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
#include "NavigatorSound.h"
#include "AvatarEditor.h"

namespace Solipsis {

class Modeler;
class AvatarEditor;

/** The main class of Navigator application.
 */
class Navigator : public Instance, public NodeEventListener, public IOgrePeerManagerCallbacks
{
public:
    enum State {
        SLogin,         // User is choosing options, ... and finally log on world
        SInWorld,       // GUI displayed when user is in the world
		SModeling,      // GUI displayed when user is modeling an object
		SAvatarEdit     // GUI displayed when user is editing his avatar
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
        QFVLCPanel = QFNaviPanel<<1,
        QFVNCPanel = QFVLCPanel<<1,
        QFAvatar = QFVNCPanel<<1,
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
    String mLogin;
    String mMediaCachePath;

    NavigatorXMLRPCClient* mXmlRpcClient;

    OgrePeerManager* mOgrePeerManager;

    NavigatorGUI* mNavigatorGUI;

    Modeler* mModeler;
    AvatarEditor* mAvatarEditor;

// COLOR PICKING
//    TexturePtr mColorPickingRT;
// COLOR PICKING
    Real mMaxObjectPickingDistance;
    Real mMaxAvatarPickingDistance;
    Real mMaxNaviPickingDistance;
    Real mMaxVLCPickingDistance;
    Real mMaxVNCPickingDistance;
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

    NavigatorSound* mNavigatorSound;

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
    String& getConnectionHost();
    void setConnectionHost(String& host);
    int getConnectionPort();
    void setConnectionPort(int port);
    String& getConnectionLogin();
    void setConnectionLogin(String& login);
    bool setNameValueVariable(const String& varName, const String& varValue);
    const String& getMediaCachePath() { return mMediaCachePath; }

    OgrePeerManager* getOgrePeerManager();
    NavigatorGUI* getNavigatorGUI();
    Modeler* getModeler();
    AvatarEditor* getAvatarEditor();

    lua_State* getLuaState();
    void setNavigatorLua(NavigatorLua* navigatorLua);
    NavigatorLua* getNavigatorLua();

    NavigatorSound* getNavigatorSound() { return mNavigatorSound; }

    Avatar* getUserAvatar();

    // Demonstrators
    void fakeSurroundingArea(int index);
#ifdef DEMO_NAVI1
    void demoNavi1();
#endif
#ifdef DEMO_NAVI2
    class DemoNavi2EventListener : public NaviEventListener
    {
	public:
        virtual void onNaviDataEvent(Navi *caller, const NaviData &naviData) {}
		virtual void onLinkClicked(Navi *caller, const std::string &linkHref) {}
        virtual void onLocationChange(Navi *caller, const std::string &url) { getSingletonPtr()->getNavigatorGUI()->debugRefreshUrl(); }
		virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode) {}
    };
    DemoNavi2EventListener mDemoNavi2EventListener;
    void demoNavi2(const String params);
#endif
#ifdef DEMO_VNC
    void demoVNC(const String params);
#endif
#ifdef DEMO_VLC
    void demoVLC(const String params);
#endif
#ifdef DEMO_VOICE
    void demoVoice(const String params);
#endif
#ifdef DEMO_PHYSICS1
    void demoPhysics1();
#endif

    // Navi 3D panels management
    Entity* getNaviEntity(const String& naviName);

    // Mouse ray picking
    void resetMousePicking();
    bool computeMousePicking(Ray& mouseRay);
    MovableObject* getPickedMovable() { return mPickedMovable; }
    bool is1NaviHitByMouse(String& naviName, int& naviX, int& naviY);
    void computeNaviHit(const String& naviName,
                        Vector2& closestUV,
                        Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                        int& naviX, int& naviY);
    bool is1VLCHitByMouse(MovableObject*& vlcMovableObj);
    bool is1VNCHitByMouse(MovableObject*& vncMovableObj, Vector2& vncXY);
    void computeVncHit(Vector2& closestUV,
                       Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                       Vector2& vncXY);
    bool is1AvatarHitByMouse(Avatar*& avatar);

    bool quit();
    bool connect();
    bool sendMessage(const String& message);
    bool contextItemSelected(const String& item);

    // process events received by node
    void processEvents();
    // send events to node
    void sendEvents();

    /** See IOgrePeerManagerCallbacks. */
    virtual bool OnAvatarNodeCreate(OgrePeer* ogrePeer);
    /** See IOgrePeerManagerCallbacks. */
    virtual bool OnSceneNodeCreate(OgrePeer* ogrePeer);

    // Modeler part

    /** Start modeling mode. */
    bool startModeling();
    /** Stop the modeling mode. */
    bool endModeling();
	/** Create a plane. */
    bool createPlane();
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
    /** Import a mesh file.*/
    bool mdlrXMLImport();
    /** Save to a XML file. */
    bool mdlrXMLSave(bool all = false);

    /** Start editing avatar mode. */
    bool startAvatarEdit();
    /** Stop editing avatar mode. */
    bool endAvatarEdit();
    /** Load from a XML file.*/
    bool avatarXMLLoad();
    /** Save to the current XML file. */
    bool avatarXMLSave();
    /** Save to another XML file. */
    bool avatarXMLSaveAs();

    bool isOnLeftCTRL;

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

#ifdef POOL
    virtual void onPeerNew(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerLost(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerUpdated(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerAction(RefCntPoolPtr<XmlAction>& xmlAction);
#else
    virtual void onPeerNew(XmlEntity* xmlEntity);
    virtual void onPeerLost(XmlEntity* xmlEntity);
    virtual void onPeerUpdated(XmlEntity* xmlEntity);
    virtual void onPeerAction(XmlAction* xmlAction);
#endif
};

} // namespace Solipsis

#endif // #ifndef __Navigator_h__