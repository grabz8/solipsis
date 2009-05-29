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

#include "MainApplication/Instance.h"
#include <CTLog.h>
#include "NavigatorXMLRPCClient.h"
#include "NodeEventListener.h"
#include "World/Avatar.h"
#include "World/Scene.h"
#include "World/Object.h"
#include "OgrePeerManager.h"
#include "NavigatorGUI/NavigatorGUI.h"
#include "Scripting/LuaBinding.h"
#include "Scripting/NavigatorLua.h"
#include "Selection.h"
#include "Transformations.h"
#include "Tools/Event.h"
#include "NavigatorSound.h"
#include "AvatarEditor.h"
#include "Cameras/CameraSupportManager.h"
#include "Configuration\NavigatorConfiguration.h"
#include "navieventlistener.h"


namespace Solipsis {

// Define Navigator version
#define NAVIGATOR_VERSION_MAJOR 1
#define NAVIGATOR_VERSION_MINOR 1
#define NAVIGATOR_VERSION_PATCH 0
#define NAVIGATOR_VERSION ((NAVIGATOR_VERSION_MAJOR << 16) | (NAVIGATOR_VERSION_MINOR << 8) | NAVIGATOR_VERSION_PATCH)

class Modeler;
class AvatarEditor;


/** The main class of Navigator application.
 */
class Navigator : public Instance, public NodeEventListener, public IOgrePeerManagerCallbacks, public NaviEventListener
{
public:

    enum CameraMode {
        CMDetached,
        CM1stPerson,
        CM1stPersonWithMouse,
        CM3rdPerson,
        CMAroundPerson,
        CMModeling,
        CMAroundObject
    };

    enum State {
        SLogin,         // User is choosing options, ... and finally log on world
        SInWorld,       // GUI displayed when user is in the world
        SModeling,      // GUI displayed when user is modeling an object
        SAvatarEdit     // GUI displayed when user is editing his avatar
    };
    static const uint32 QFNaviPanel = 1;
    static const uint32 QFVLCPanel = QFNaviPanel<<1;
    static const uint32 QFVNCPanel = QFVLCPanel<<1;
    static const uint32 QFSWFPanel = QFVNCPanel<<1;
    static const uint32 QFAvatar = QFSWFPanel<<1;
    static const uint32 QFObject = QFAvatar<<1;
    static const uint32 QFGizmo = QFObject<<1;
    enum NavigationInterface
    {
        NIMouseKeyboard = 0,
        NIWiimoteNunchuk,
        NIWiimoteNunchukIR
    };
   
     void saveConfiguration()
     {
        mConfiguration.saveConfig();
     }

private:
    static Navigator* ms_singletonPtr;

protected:
    State mState;
    String mPeerAddress;
    String mLocalWorldAddress;
    String mWorldAddress;
    String mWorldsServerAddress;
    unsigned short mWorldsServerTimeoutSec;
    String mLogin;
    String mPwd;
    AuthentType mAuthentType;
    String mFacebookApiKey;
    String mFacebookSecret;
    String mFacebookServer;
    String mFacebookLoginUrl;
    NodeId mFixedNodeId;
    NodeId mNodeId;
    String mMediaCachePath;
    String mVoIPServerAddress;
    float mVoIPSilenceLevel;
    bool mCastShadows;
    unsigned int mVoIPSilenceLatency;

    Configuration mConfiguration;

    CameraSupportManager* mMainCameraSupportMgr;

    NavigationInterface mNavigationInterface;

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
    Real mMaxSWFPickingDistance;
    RaySceneQuery* mRaySceneQuery;
    MovableObject* mPickedMovable;
    Real mClosestDistance;
    Vector3 mClosestHitPoint;
    Vector2 mClosestUV;
    Vector2 mClosestTriUV0, mClosestTriUV1, mClosestTriUV2;

    lua_State* mLuaState;
//with LuaPlus    LuaPlus::LuaState* mLuaPlusState;
    NavigatorLua* mNavigatorLua;

    Avatar* mUserAvatar;

//    bool mFakeTerrain;

    NavigatorSound* mNavigatorSound;

    std::map<String, String> mNaviURLUpdatePending;

public:
    Navigator(const String name, IApplication* application);
    ~Navigator();
    static Navigator* getSingletonPtr();
    static unsigned int getVersion();

    bool isConnected();

    // Get and set
    State getState();
	void setState(State newState);
    const String& getPeerAddress();
    void setPeerAddress(const String& address);
    const String& getLocalWorldAddress();
    void setLocalWorldAddress(const String& address);
    const String& getWorldAddress();
    void setWorldAddress(const String& address);
    const String& getWorldsServerAddress();
    void setWorldsServerAddress(const String& address);
    unsigned short getWorldsServerTimeout();
    void setWorldsServerTimeout(unsigned short timeoutSec);
    const String& getLogin();
    void setLogin(const String& login);
    const String& getPwd();
    void setPwd(const String& pwd, bool bSave);
    AuthentType getAuthentType();
    void setAuthentType(AuthentType authentType);
    const String& getFacebookApiKey() { return mFacebookApiKey; }
    void setFacebookApiKey(const String& apiKey) { mFacebookApiKey = apiKey; }
    const String& getFacebookSecret() { return mFacebookSecret; }
    void setFacebookSecret(const String& secret) { mFacebookSecret = secret; }
    const String& getFacebookServer() { return mFacebookServer; }
    void setFacebookServer(const String& server) { mFacebookServer = server; }
    const String& getFacebookLoginUrl() { return mFacebookLoginUrl; }
    void setFacebookLoginUrl(const String& loginUrl) { mFacebookLoginUrl = loginUrl; }
    const NodeId& getFixedNodeId();
    void setFixedNodeId(const NodeId& fixedNodeId);
    const NodeId& getNodeId();
    void setNodeId(const NodeId& nodeId);
    const String& getMediaCachePath();
    void setMediaCachePath(const String& mediaCachePath);
    const String& getVoIPServerAddress();
    void setVoIPServerAddress(const String& address);
    float getVoIPSilenceLevel();
    void setVoIPSilenceLevel(float VoIPSilenceLevel);
    unsigned int getVoIPSilenceLatency();
    void setVoIPSilenceLatency(unsigned int VoIPSilenceLatencySec);
    void setCastShadows(bool castShadows);
    bool getCastShadows();

    void setNavigationInterface(NavigationInterface ni);
    NavigationInterface getNavigationInterface(); 

    void loadConfigurationValues();

    OgrePeerManager* getOgrePeerManager();
    NavigatorGUI* getNavigatorGUI();
    Modeler* getModeler();
    AvatarEditor* getAvatarEditor();

    lua_State* getLuaState();
    void setNavigatorLua(NavigatorLua* navigatorLua);
    NavigatorLua* getNavigatorLua();

    NavigatorSound* getNavigatorSound() { return mNavigatorSound; };

    CameraSupportManager* getMainCameraSupportManager() { return mMainCameraSupportMgr; };
    void setCameraMode(int mode);
    int getCameraMode() { return mMainCameraSupportMgr->getActiveCameraSupportIndex(); };
    int getLastCameraMode() { return mMainCameraSupportMgr->getLastCameraSupportIndex(); };

    void toggleVoIP();

    Avatar* getUserAvatar();

    // Demonstrators
    void fakeSurroundingArea(int index);
#ifdef DEMO_NAVI1
    void demoNavi1();
#endif
#ifdef DEMO_NAVI2
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

    // Navi 3D panels management
    String getEntityNaviName(const Entity& entity);
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
    bool is1SWFHitByMouse(MovableObject*& swfMovableObj, Vector2& swfXY);
    void computeSwfHit(Vector2& closestUV,
                       Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                       Vector2& swfXY);
    bool is1VLCHitByMouse(MovableObject*& vlcMovableObj);
    bool is1VNCHitByMouse(MovableObject*& vncMovableObj, Vector2& vncXY);
    void computeVncHit(Vector2& closestUV,
                       Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                       Vector2& vncXY);
    bool is1AvatarHitByMouse(Avatar*& avatar);
    bool is1ObjectHitByMouse(Object*& object);

    bool quit();
    bool connect();
    bool disconnect();
    bool contextItemSelected(const String& item);
    bool sendMessage(const String& message);
    bool addNaviURLUpdatePending(const String& naviName, const String& url);
    bool sendURLUpdate(const EntityUID& entityUID, const String& naviName, const String& url);

    // process events received by node
    void processEvents();
    // send events to node
    void sendEvents();

    /** See IOgrePeerManagerCallbacks. */
    virtual void onAvatarNodeCreate(OgrePeer* ogrePeer);
    /** See IOgrePeerManagerCallbacks. */
    virtual void onSceneNodeCreate(OgrePeer* ogrePeer);
    /** See IOgrePeerManagerCallbacks. */
    virtual void onSceneNodeDestroy(OgrePeer* ogrePeer);

    // Modeler part

    /** Start modeling mode. */
    bool startModeling();
    /** Stop the modeling mode. */
    bool endModeling();


    /** the user creates a 3D primitive for the modeler **/
    bool createPrimitive(Object3D::Type type);

#ifdef DECLARATIVE_MODELER
    /** Create a Scene from text. */
	bool createSceneFromText( const std::string&, std::string& errMsg, std::string& warnMsg );
#endif
#ifdef TERRAIN_MODELER
	/** Create a terrain. */
    bool createTerrain(double steepness,double noiseScale,double granularity);
#endif
    /** Create a mesh. */
    bool createMesh();
    /** Import a mesh file.*/
    bool mdlrXMLImport();
    /** Save to a XML file. */
    bool mdlrXMLSave(bool all = false);
    /** save the selected object3D and save it to the specifdied file (.SOF) */
    bool mdlrXMLSaveAs(const String& pDestination);

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
    bool isOnRightCTRL;
	bool isOnGizmo;

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
    virtual void createCamera(); 
    virtual void createFrameListener();

    virtual void createScene();

    virtual bool createGUI();

    virtual bool setWindow(IWindow* w);

    class OgreLogger : public CommonTools::LogHandler
    {
    public:
        /** See CommonTools::LogHandler. */
        void log(VerbosityLevel level, const char* msg);

    };
    OgreLogger mOgreLogger;

    virtual void onPeerNew(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerLost(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerUpdated(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual void onPeerAction(RefCntPoolPtr<XmlAction>& xmlAction);

    /** See NaviEventListener. */
    virtual void onCallback(const std::string& name, const Awesomium::JSArguments& args);
    virtual void onLocationChange(Navi *caller, const std::string &url);
    virtual void onNavigateComplete(Navi *caller, const std::string &url, int responseCode);

};

} // namespace Solipsis

#endif // #ifndef __Navigator_h__