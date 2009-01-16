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

#include "Navigator.h"
#include "NavigatorFrameListener.h"
#include "OgreHelpers.h"
#include <OgreExternalTextureSourceManager.h>
#include "DebugHelpers.h"
#include <CTSystem.h>
#include <CTStringHelpers.h>
#include <CTLog.h>
#include <CTIO.h>
#include "Navi.h"
#include "NaviLua.h"
#include "FirstPersonCameraSupport.h"
#include "OrbitalCameraSupport.h"
#include <Modeler.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <CharacterInstance.h>
#include <VoiceEngineManager.h>

using namespace Solipsis;
using namespace CommonTools;

Navigator* Navigator::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
Navigator::Navigator(const String name, IApplication* application) :
    Instance(name, application),
    NodeEventListener(mXmlRpcClient),
    mState(SLogin),
    mPeerAddress("localhost:8880"),
    mLocalWorldAddress("localhost:8660"),
    mWorldAddress(""),
    mWorldsServerAddress("localhost:8550"),
    mWorldsServerTimeoutSec(8),
    mLogin("me"),
    mPwd(""),
    mAuthentType(ATSolipsis),
    mFacebookApiKey(""),
    mFacebookSecret("695a02e3645bed085e1802c7e9952d73"), // for api key "8d81e4c64ac0039b209c4a53b21ba220"
    mFacebookServer("api.facebook.com/restserver.php"),
    mFacebookLoginUrl("http://api.facebook.com/login.php"),
    mFixedNodeId(""),
    mNodeId(""),
    mNavigationInterface(NIMouseKeyboard),
    mXmlRpcClient(0),
    mOgrePeerManager(0),
    mNavigatorGUI(0),
    mAvatarEditor(0),
    mMaxNaviPickingDistance(10),
    mMaxVLCPickingDistance(10),
    mMaxVNCPickingDistance(8),
    mMaxSWFPickingDistance(8),
    mMaxAvatarPickingDistance(10),
    mMaxObjectPickingDistance(20),
    mRaySceneQuery(0),
    mPickedMovable(0),
    mUserAvatar(0),
    mNavigatorSound(0),
    mMainCameraSupportMgr(0),
    mModeler(0),
    isOnLeftCTRL(false),
    isOnRightCTRL(false),
	isOnGizmo(false)
{
    ms_singletonPtr = this;

    LogHandler::setLogHandler(&mOgreLogger);
    LogHandler::getLogHandler()->setVerbosityLevel(LogHandler::VL_DEBUG);
    String logFilename = IO::getCWD() + "\\" + LogManager::getSingletonPtr()->getDefaultLog()->getName();
    LogHandler::getLogHandler()->setLogFilename(logFilename);

    // Lua initialization
    mLuaState = lua_open();
    luaL_openlibs(mLuaState);
//with LuaPlus    mLuaPlusState = LuaPlus::LuaState::Create(true);
//with LuaPlus    mLuaState = mLuaPlusState->GetCState();
}

//-------------------------------------------------------------------------------------
Navigator::~Navigator()
{
    // Stop the node events listener thread
    NodeEventListener::stop();
    NodeEventListener::finalize();

    // Destroy XMLRPC client
    delete mXmlRpcClient;

    // Destroy OgrePeer manager
    delete mOgrePeerManager;

    if (mSceneMgr)
    {
        // Destroy the ray scene query
        if (mRaySceneQuery != 0)
            mSceneMgr->destroyQuery(mRaySceneQuery);
    }

    // Destroy the avatar editor
    delete mAvatarEditor;

    // Destroy the modeler
    delete mModeler;

    // Destroy the GUI
    delete mNavigatorGUI;

    // Lua finalization
    lua_close(mLuaState);
//with LuaPlus    LuaPlus::LuaState::Destroy(mLuaPlusState);

    if (mNavigatorSound != 0)
    {
        // Shutdown sound system
        mNavigatorSound->shutdown();
        delete mNavigatorSound;
    }

    // Destroy Main Camera Manager
    if (mMainCameraSupportMgr != 0)
        delete mMainCameraSupportMgr;
}

//-------------------------------------------------------------------------------------
Navigator* Navigator::getSingletonPtr()
{
    return ms_singletonPtr;
}

//-------------------------------------------------------------------------------------
unsigned int Navigator::getVersion()
{
    return NAVIGATOR_VERSION;
}

//-------------------------------------------------------------------------------------
bool Navigator::isConnected()
{
    return ((mXmlRpcClient != 0) && (mXmlRpcClient->isConnected()));
}

//-------------------------------------------------------------------------------------
Navigator::State Navigator::getState()
{
    return mState;
}

//-------------------------------------------------------------------------------------
void Navigator::setState(State newState)
{
    mState = newState;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getPeerAddress()
{
    return mPeerAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setPeerAddress(const String& address)
{
    mPeerAddress = address;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getLocalWorldAddress()
{
    return mLocalWorldAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setLocalWorldAddress(const String& address)
{
    mLocalWorldAddress = address;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getWorldAddress()
{
    return mWorldAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldAddress(const String& address)
{
    mWorldAddress = address;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getWorldsServerAddress()
{
    return mWorldsServerAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldsServerAddress(const String& address)
{
    mWorldsServerAddress = address;
}

//-------------------------------------------------------------------------------------
unsigned short Navigator::getWorldsServerTimeout()
{
    return mWorldsServerTimeoutSec;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldsServerTimeout(unsigned short timeoutSec)
{
    mWorldsServerTimeoutSec = timeoutSec;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getLogin()
{
    return mLogin;
}

//-------------------------------------------------------------------------------------
void Navigator::setLogin(const String& login)
{
    mLogin = login;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getPwd()
{
    return mPwd;
}

//-------------------------------------------------------------------------------------
void Navigator::setPwd(const String& pwd)
{
    mPwd = pwd;
}

//-------------------------------------------------------------------------------------
AuthentType Navigator::getAuthentType()
{
    return mAuthentType;
}

//-------------------------------------------------------------------------------------
void Navigator::setAuthentType(AuthentType authentType)
{
    mAuthentType = authentType;
}

//-------------------------------------------------------------------------------------
const NodeId& Navigator::getFixedNodeId()
{
    return mFixedNodeId;
}

//-------------------------------------------------------------------------------------
void Navigator::setFixedNodeId(const NodeId& nodeId)
{
    mFixedNodeId = nodeId;
}

//-------------------------------------------------------------------------------------
const NodeId& Navigator::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
void Navigator::setNodeId(const NodeId& nodeId)
{
    mNodeId = nodeId;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getMediaCachePath()
{
    return mMediaCachePath;
}

//-------------------------------------------------------------------------------------
void Navigator::setMediaCachePath(const String& mediaCachePath)
{
    mMediaCachePath = mediaCachePath;
}

//-------------------------------------------------------------------------------------
bool Navigator::setNameValueVariable(const String& varName, const String& varValue)
{
    if (varName == "PeerAddress")
    {
        mPeerAddress = varValue;
        return true;
    }
    if (varName == "LocalWorldAddress")
    {
        mLocalWorldAddress = varValue;
        if (mWorldAddress.empty())
            mWorldAddress = mLocalWorldAddress;
        return true;
    }
    if (varName == "WorldsServerAddress")
    {
        mWorldsServerAddress = varValue;
        return true;
    }
    if (varName == "WorldsServerTimeout")
    {
        mWorldsServerTimeoutSec = StringConverter::parseInt(varValue);
        return true;
    }
    if (varName == "Login")
    {
        mLogin = varValue;
        return true;
    }
    if (varName == "FixedNodeId")
    {
        mFixedNodeId = varValue;
        if (!mFixedNodeId.empty())
            mAuthentType = ATFixed;
        return true;
    }
    if (varName == "FacebookApiKey")
    {
        mFacebookApiKey = varValue;
        return true;
    }
    if (varName == "FacebookSecret")
    {
        mFacebookSecret = varValue;
        return true;
    }
    if (varName == "FacebookServer")
    {
        mFacebookServer = varValue;
        return true;
    }
    if (varName == "FacebookLoginUrl")
    {
        mFacebookLoginUrl = varValue;
        return true;
    }
    if (varName == "MediaCachePath")
    {
        mMediaCachePath = varValue;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------
OgrePeerManager* Navigator::getOgrePeerManager()
{
    return mOgrePeerManager;
}

//-------------------------------------------------------------------------------------
NavigatorGUI* Navigator::getNavigatorGUI()
{
    return mNavigatorGUI;
}

//-------------------------------------------------------------------------------------
Modeler* Navigator::getModeler()
{
    return mModeler;
}

//-------------------------------------------------------------------------------------
AvatarEditor* Navigator::getAvatarEditor()
{
    return mAvatarEditor;
}

//-------------------------------------------------------------------------------------
lua_State* Navigator::getLuaState()
{
    return mLuaState;
}

//-------------------------------------------------------------------------------------
void Navigator::setNavigatorLua(NavigatorLua* navigatorLua)
{
    mNavigatorLua = navigatorLua;
}

//-------------------------------------------------------------------------------------
NavigatorLua* Navigator::getNavigatorLua()
{
    return mNavigatorLua;
}

//-------------------------------------------------------------------------------------
Avatar* Navigator::getUserAvatar()
{
    return mUserAvatar;
}

//-------------------------------------------------------------------------------------
void Navigator::fakeSurroundingArea(int index)
{
    if (mState != SInWorld) return;
    mCamera->getViewport()->setBackgroundColour(ColourValue::Black);
    switch (index)
    {
        case 1:
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox1", 1000);
           break;
        case 2:
           mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 100, true);
           break;
        case 3:
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox2", 10, true);
           break;
        case 4:
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox3", 10, true);
           break;
        case 0:
           mSceneMgr->setSkyBox(false, "");
           break;
   }
}

#ifdef DEMO_NAVI1
//-------------------------------------------------------------------------------------
void Navigator::demoNavi1()
{
    static bool active = false;
    if (active) return;
    active = true;

    SceneNode* userAvatarSceneNode = mUserAvatar->getSceneNode();

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, -1.1);
    MeshManager::getSingleton().createPlane("demoNavi1Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.5, 1.5, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi1Video", "demoNavi1Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial("WWW_" + vidEnt->getName(), "http://www.youtube.com/watch?v=066_q4DIeqk", 512, 512);
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
    //http://www.youtube.com/watch?v=ZQcUS4chhc4
    //http://fr.youtube.com/watch?v=u5WIEep8DJg
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1VideoNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoNavi1Text", "demoNavi1Plane");
    txtEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* txtNavi = NaviLibrary::NaviManager::Get().createNaviMaterial("WWW_" + txtEnt->getName(), "local://lgpl-3.0.txt", 512, 512);
    txtNavi->show(true);
    txtNavi->setMaxUPS(8);
    txtEnt->setMaterialName(txtNavi->getMaterialName());
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1TextNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoNavi1WebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* knotNavi = NaviLibrary::NaviManager::Get().createNaviMaterial("WWW_" + knotEnt->getName(), "http://www.google.com", 512, 512);
    knotNavi->show(true);
    knotNavi->setMaxUPS(8);
    std::string googleMtlName = knotNavi->getMaterialName();
    MaterialPtr googleMtl = (MaterialPtr)MaterialManager::getSingletonPtr()->getByName(googleMtlName);
    googleMtl->setDepthWriteEnabled(true);
    knotEnt->setMaterialName(googleMtlName);
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1WebKnotNode", userAvatarSceneNode->getPosition() + Vector3(-3, 1.75, 4));
    node->setScale(Vector3(0.015f, 0.015f, 0.015f));
    node->attachObject(knotEnt);
}
#endif
#ifdef DEMO_NAVI2
//-------------------------------------------------------------------------------------
void Navigator::demoNavi2(const String params)
{
    static bool active = false;
    String url2go("");
    String strPosition("(17, -108, 96.5)");
    Vector3 position;
    std::string::size_type strPos;
    strPos = params.find_first_of(";");
    if (strPos == std::string::npos)
        url2go = params;
    else
    {
        url2go = params.substr(0, strPos);
        if (strPos + 1 < params.length())
            strPosition = params.substr(strPos + 1, params.length() - (strPos + 1));
    }
    if (!OgreHelpers::convertString2Vector3(strPosition, position))
        position = Vector3(17, -108, 96.5);
    if (url2go.length() == 0)
        url2go = "http://fr.youtube.com/watch?v=u5WIEep8DJg";
    if (active)
    {
        NaviLibrary::NaviManager::Get().getNavi("demoNavi2Video")->navigateTo(url2go);
        return;
    }
    active = true;

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, 0);
    MeshManager::getSingleton().createPlane("demoNavi2Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 4, 4, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi2Video", "demoNavi2Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial("WWW_" + vidEnt->getName(), "", 512, 512);
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demo2VideoNode");
    videoNode->attachObject(vidEnt);
    videoNode->setPosition(position);
    videoNode->yaw(Degree(180), Node::TS_WORLD);
    // Add 1 listener to follow URL changes
    vidNavi->addEventListener(&mDemoNavi2EventListener);
    vidNavi->navigateTo(url2go);
}
#endif
#ifdef DEMO_VNC
//-------------------------------------------------------------------------------------
void Navigator::demoVNC(const String params)
{
    static bool active = false;

    if (active)
    {
        SceneNode* vncNode = mSceneMgr->getSceneNode("demoVNCNode");
        MovableObject* vncEnt = vncNode->getAttachedObject("demoVNC");
        vncNode->detachObject(vncEnt);
        vncNode->getCreator()->destroyMovableObject(vncEnt);
        vncNode->getCreator()->destroySceneNode(vncNode->getName());
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSource* vncExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc");
        vncExtTextSrc->destroyAdvancedTexture("demoVNCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        MaterialManager::getSingleton().remove("demoVNCMaterial");
    }
    else
    {
        String url2go("");
        String pwd("");
        std::string::size_type strPos;
        strPos = params.find_first_of(";");
        if (strPos == std::string::npos)
            url2go = params;
        else
        {
            url2go = params.substr(0, strPos);
            if (strPos + 1 < params.length())
                pwd = params.substr(strPos + 1, params.length() - (strPos + 1));
        }

        // Creates the VNC Plane and subsequent NaviMaterial
        Entity* vncEnt = mSceneMgr->createEntity("demoVNC", "demoVNCPlane.mesh");
        vncEnt->setQueryFlags(QFVNCPanel);
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSource* vncExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc");
        vncExtTextSrc->setParameter("address", "vnc://" + url2go);
        vncExtTextSrc->setParameter("password", "vncpwd:" + pwd);
        MaterialManager::getSingleton().create("demoVNCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vncExtTextSrc->createDefinedTexture("demoVNCMaterial");
        vncEnt->setMaterialName("demoVNCMaterial");
        SceneNode* vncNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoVNCNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(2.5, 1.5, 0));
        vncNode->attachObject(vncEnt);
        vncNode->yaw(Degree(90), Node::TS_WORLD);
    }

    active = !active;
}
#endif
#ifdef DEMO_VLC
//-------------------------------------------------------------------------------------
void Navigator::demoVLC(const String params)
{
    static bool active = false;

    if (active)
    {
        SceneNode* vlcNode = mSceneMgr->getSceneNode("demoVLCNode");
        MovableObject* vlcEnt = vlcNode->getAttachedObject("demoVLC");
        vlcNode->detachObject(vlcEnt);
        vlcNode->getCreator()->destroyMovableObject(vlcEnt);
        vlcNode->getCreator()->destroySceneNode(vlcNode->getName());
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vlc");
        ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
        vlcExtTextSrc->destroyAdvancedTexture("demoVLCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        MaterialManager::getSingleton().remove("demoVLCMaterial");
    }
    else
    {
        String mrl(params);
        if (mrl.empty()) return;

        // Creates the VLC Plane and subsequent NaviMaterial
        Entity* vlcEnt = mSceneMgr->createEntity("demoVLC", "demoVNCPlane.mesh");
        vlcEnt->setQueryFlags(QFVLCPanel);
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vlc");
        ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
        vlcExtTextSrc->setParameter("mrl", mrl);
        vlcExtTextSrc->setParameter("width", "256");
        vlcExtTextSrc->setParameter("height", "256");
        vlcExtTextSrc->setParameter("frames_per_second", "25");
        MaterialManager::getSingleton().create("demoVLCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vlcExtTextSrc->createDefinedTexture("demoVLCMaterial");
        vlcEnt->setMaterialName("demoVLCMaterial");
        SceneNode* vlcNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoVLCNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(2.5, 1.5, 0));
        vlcNode->attachObject(vlcEnt);
        vlcNode->yaw(Degree(90), Node::TS_WORLD);
    }
    active = !active;
}
#endif
#ifdef DEMO_VOICE
//-------------------------------------------------------------------------------------
void Navigator::demoVoice(const String params)
{
    // get voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
        return;

    if (params.compare("toggleVoiceRecording") == 0)
    {
        // start/stop speaking
        if (voiceEngine->isRecording())
            voiceEngine->stopRecording();
        else
            voiceEngine->startRecording();
    }
    else
    {
        // connection to the voice server
        String voiceServerHost("localhost");
        unsigned short voiceServerPort = 30000;
        std::string::size_type strPos;
        strPos = params.find_first_of(":");
        if (strPos == std::string::npos)
            voiceServerHost = params;
        else
        {
            voiceServerHost = params.substr(0, strPos);
            if (strPos + 1 < params.length())
                voiceServerPort = atoi(params.substr(strPos + 1, params.length() - (strPos + 1)).c_str());
        }
        // stop recording
        if (voiceEngine->isRecording())
            voiceEngine->stopRecording();
        // connect to voice server
		EntityUID avatarUid = this->getUserAvatar()->getCharacterInstance()->getUid();
        bool connectionSuccess = voiceEngine->connect(voiceServerHost.c_str(), voiceServerPort, avatarUid);
		assert( connectionSuccess );
    }
}
#endif
#ifdef DEMO_PHYSICS1
//-------------------------------------------------------------------------------------
void Navigator::demoPhysics1()
{
/*
#define MAX_BOXES 10
    static int nextBox = 0;
    static std::map<String, SceneNode*> boxes;

    // Create a box
    int boxNum = nextBox;
    nextBox = (nextBox + 1)%MAX_BOXES;
    String boxName = "demoPhysics1box" + StringConverter::toString(boxNum);
    String boxNodeName = boxName + "Node";
    Vector3 boxGeomSize = Vector3(100, 100, 100);
    Vector3 boxScale = Vector3(0.01 - boxNum*0.0005, 0.01 - boxNum*0.0005, 0.01 - boxNum*0.0005);
    Vector3 boxExtents = boxGeomSize*boxScale;
    Vector3 userAvatarPos = mUserAvatar->getSceneNode()->getWorldPosition();
    Vector3 userAvatarVpn = mUserAvatar->getSceneNode()->getWorldOrientation()*Vector3::UNIT_X;
    Vector3 userAvatarVup = mUserAvatar->getSceneNode()->getWorldOrientation()*Vector3::UNIT_Y;
    Vector3 boxPos = userAvatarPos + userAvatarVpn*4.5 + userAvatarVup*4.5;
    SceneNode* boxNode = 0;
    Entity* boxEntity = 0;

    static std::map<String, IPhysicsBody*> bodies;
    IPhysicsScene* physicsScene = mOgrePeerManager->getPhysicsScene();
    if (physicsScene == 0) return;

    IPhysicsBody* boxBody = 0;
    std::map<String, SceneNode*>::iterator boxIt = boxes.find(boxName);
    if (boxIt != boxes.end())
    {
        boxNode = mSceneMgr->getSceneNode(boxNodeName);
        boxEntity = (Entity*)boxNode->getAttachedObject(boxName + "Ent");
        boxBody = (bodies.find(boxName))->second;
    }
    else
    {
        boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(boxNodeName);
        boxNode->setScale(boxScale);
        boxEntity = mSceneMgr->createEntity(boxName + "Ent", "cube.mesh");
        boxEntity->setMaterialName("2 - Default");
        boxNode->attachObject(boxEntity);
        boxBody = physicsScene->createBody();
        boxBody->createBox(boxNode, boxExtents);
        boxes[boxName] = boxNode;
        bodies[boxName] = boxBody;
    }
    boxBody->setPosition(boxPos);
    boxBody->setLinearVelocity(Vector3::ZERO);
    boxBody->setAngularVelocity(Vector3::ZERO);
*/
}
#endif

//-------------------------------------------------------------------------------------
Entity* Navigator::getNaviEntity(const String& naviName)
{
    if (naviName.find("WWW_") != 0)
        return 0;
    String entityName = naviName.substr(4, naviName.length() - 4);
    if (mSceneMgr->hasEntity(entityName))
        return mSceneMgr->getEntity(entityName);
    return 0;
}

//-------------------------------------------------------------------------------------
void Navigator::resetMousePicking()
{
    mRaySceneQuery->clearResults();
    mClosestDistance = -1.0f;
    mPickedMovable = 0;
}

//-------------------------------------------------------------------------------------
bool Navigator::computeMousePicking(Ray& mouseRay)
{
    mRaySceneQuery->clearResults();
    mRaySceneQuery->setRay(mouseRay);
    mRaySceneQuery->setSortByDistance(true);
    mRaySceneQuery->setQueryTypeMask(SceneManager::ENTITY_TYPE_MASK);
    RaySceneQueryResult &queryResult = mRaySceneQuery->execute();
    mClosestDistance = -1.0f;
    mPickedMovable = 0;
    String movablesList;
	isOnGizmo = false;

    for (RaySceneQueryResult::iterator it = queryResult.begin(); it != queryResult.end(); ++it )
    {
        if (it->movable && (it->distance > 0))
        {
            if (movablesList.length() > 0) movablesList += ", ";
            movablesList += it->movable->getName() + ":" + StringConverter::toString(it->distance) + ":" + StringConverter::toString(it->movable->getQueryFlags());
            // stop checking if we have found a raycast hit that is closer
            // than all remaining entities
            if ((mClosestDistance >= 0.0f) && (mClosestDistance < it->distance))
                break;
            if (mState == SModeling)
            {
                // stop checking if we are too far
                if (it->distance >= mMaxObjectPickingDistance)
                    break;
                // only check this result if its a hit against an entity
                /* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
                //          if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
                if ((it->movable != 0) &&
                    ((it->movable->getMovableType().compare("Entity") == 0) ||
                    (it->movable->getMovableType().compare("ManualObject") == 0)))
                {
					if (it->movable->getQueryFlags() & QFAvatar)
                        continue;

					// Gizmo ?
					else if (mModeler->isOnGizmo())
					{
						mModeler->getSelection()->mTransformation->releasedClickForTransformation();
//						mModeler->getSelection()->mTransformation->mDragNDrop = Vector3::ZERO;

						//mModeler->getSelection()->mTransformation->getMode();
						//mModeler->deselectNode();

						string name = it->movable->getName();
						if( name.substr(0,4) == "move")
						{
							isOnGizmo = true;
							mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "moveX","moveY","moveZ" );
							break;
						}
						else if( name.substr(0,6) == "rotate")
						{
							isOnGizmo = true;
							mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "rotateX","rotateY","rotateZ" );
							break;
						}
						else if( name.substr(0,5) == "scale")
						{
							isOnGizmo = true;
							mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "scaleX","scaleY","scaleZ" );
							break;
						}
					}
					// Object3D
                    else if (it->movable->getQueryFlags() & QFObject)
                    {
                        if( mModeler && !mModeler->isSelectionLocked() )
                        {
                            String name = it->movable->getName();

                            // Link mode ?
                            if (mModeler->isInLinkMode())
                            {
                                Entity* ent = mSceneMgr->getEntity(name);
                                Object3D * obj = mModeler->getSelection()->get3DObject( ent );
                                //if it is a selectionnable object ...
                                if( obj && obj != mModeler->getSelection()->getFirstSelectedObject() ) //if it is not itself ...
                                {
                                    if( ! obj->getShowBoundingBox()) //if this object is seleted :
                                        if(obj->getParent())
                                            obj = obj->getParent();

                                    mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(false);
                                    mModeler->getSelection()->getFirstSelectedObject()->linkObject( obj, mSceneMgr);
                                    mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(true);
                                    mModeler->lockLinkMode(false);
                                }
                            }

                            // Object3D ?
                            else
                            {
                                if (!isOnLeftCTRL)
                                    mModeler->deselectNode();

                                mModeler->lockGizmo(false);
                                if (it->movable->getQueryFlags() & QFObject)
                                {
                                    mPickedMovable = it->movable;
                                    Entity* ent = mSceneMgr->getEntity(name);
                                    mModeler->selectNode(ent);
                                }
                            }
                        }

                        // if we found a new closest raycast for this object, update the
                        // mPickedMovable before moving on to the next object.
                        if (OgreHelpers::isEntityHitByMouse(mouseRay, static_cast<Entity*>(it->movable),
                            mClosestDistance,
                            mClosestUV,
                            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2))
                        {
                            if ((it->movable->getQueryFlags() & QFNaviPanel) && (it->distance < mMaxNaviPickingDistance))
                                mPickedMovable = it->movable;
                        }
                    }
                    else
                        mModeler->deselectNode();
                }
                mModeler->lockLinkMode(false);            
            }

            else // SInWorld
            {
                // stop checking if we are too far
                if (it->distance >= std::max(std::max(std::max(mMaxNaviPickingDistance, mMaxVLCPickingDistance), mMaxVNCPickingDistance), mMaxAvatarPickingDistance))
                    break;
                // only check this result if its a hit against an entity
                /* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
                //if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
                if ((it->movable != 0) &&
                    ((it->movable->getMovableType().compare("Entity") == 0) ||
                    (it->movable->getMovableType().compare("ManualObject") == 0)))
                {
                    if (it->movable->getQueryFlags() & QFAvatar)
                    {
                        mPickedMovable = it->movable;
                        break;
                    }                    

                    // if we found a new closest raycast for this object, update the
                    // mPickedMovable before moving on to the next object.
                    if (OgreHelpers::isEntityHitByMouse(mouseRay, static_cast<Entity*>(it->movable),
                        mClosestDistance,
                        mClosestUV,
                        mClosestTriUV0, mClosestTriUV1, mClosestTriUV2))
                    {
                        if (((it->movable->getQueryFlags() & QFNaviPanel) && (it->distance < mMaxNaviPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFVNCPanel) && (it->distance < mMaxVNCPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFSWFPanel) && (it->distance < mMaxSWFPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFVLCPanel) && (it->distance < mMaxVLCPickingDistance)))
                            mPickedMovable = it->movable;
                    }
                }
            }
        }
    }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeMousePicking() found movables %s", movablesList.c_str());
    // if 1 entity hit
    if ((mClosestDistance >= 0.0f) && (mPickedMovable != 0))
    {
        mClosestHitPoint = mouseRay.getPoint(mClosestDistance);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeMousePicking() found movable=%s, mClosestDistance=%.2f, mClosestHitPoint=%s", mPickedMovable->getName().c_str(), mClosestDistance, StringConverter::toString(mClosestHitPoint).c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::is1NaviHitByMouse(String& naviName, int& naviX, int& naviY)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFNaviPanel))
    {
        Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        naviName = NaviLibrary::NaviManager::Get().getNaviFromMtlName(mtlName)->getName();
        // compute texture coordinates of the hit
        computeNaviHit(naviName,
                       mClosestUV,
                       mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
                       naviX, naviY);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1NaviHitByMouse() found Navi movable=%s, naviName=%s, (naviX, naviY)=(%d, %d)", mPickedMovable->getName().c_str(), naviName.c_str(), naviX, naviY);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeNaviHit(const String& naviName,
                               Vector2& closestUV,
                               Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                               int& naviX, int& naviY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;
    Vector2 closestResultUV;
    closestResultUV.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    closestResultUV.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;
    // Navi textures are repeated not clamped so bound results to [0..1]
    unsigned short naviWidth, naviHeight;
    NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().getNavi(naviName);
    if (navi == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeNaviHit() naviName=%s not found !", naviName.c_str());
        return;
    }
    navi->getExtents(naviWidth, naviHeight);
    naviX = ((int)(closestResultUV.x*naviWidth))%naviWidth;
    naviY = ((int)(closestResultUV.y*naviHeight))%naviHeight;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeNaviHit() uv=%s, dt1=%s, dt2=%s, closestResultUV=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(closestResultUV).c_str());
}

//-------------------------------------------------------------------------------------
bool Navigator::is1SWFHitByMouse(MovableObject*& swfMovableObj, Vector2& swfXY)
{
    // if 1 SWF entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFSWFPanel))
    {
        swfMovableObj = mPickedMovable;

        Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        // compute texture coordinates of the hit
        computeSwfHit(mClosestUV,
                      mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
                      swfXY);

        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1SWFHitByMouse() found SWF movable=%s, swfXY=%s", mPickedMovable->getName().c_str(), StringConverter::toString(swfXY).c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeSwfHit(Vector2& closestUV,
                              Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                              Vector2& swfXY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;

    swfXY.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    swfXY.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeSwfHit() uv=%s, dt1=%s, dt2=%s, swfXY=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(swfXY).c_str());
}

//-------------------------------------------------------------------------------------
bool Navigator::is1VLCHitByMouse(MovableObject*& vlcMovableObj)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFVLCPanel))
    {
        vlcMovableObj = mPickedMovable;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1VLCHitByMouse() found VLC movable=%s", mPickedMovable->getName().c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::is1VNCHitByMouse(MovableObject*& vncMovableObj, Vector2& vncXY)
{
    // if 1 VNC entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFVNCPanel))
    {
        vncMovableObj = mPickedMovable;
        // compute texture coordinates of the hit
        computeVncHit(mClosestUV,
                      mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
                      vncXY);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1VNCHitByMouse() found VNC movable=%s, vncXY=%s", mPickedMovable->getName().c_str(), StringConverter::toString(vncXY).c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeVncHit(Vector2& closestUV,
                              Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                              Vector2& vncXY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;
    vncXY.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    vncXY.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeVncHit() uv=%s, dt1=%s, dt2=%s, vncXY=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(vncXY).c_str());
}

//-------------------------------------------------------------------------------------
bool Navigator::is1AvatarHitByMouse(Avatar*& avatar)
{
    // if 1 Avatar entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFAvatar))
    {
        // retrieve avatar instance
        for (OgrePeerManager::OgrePeersMap::iterator ogrePeer = mOgrePeerManager->getOgrePeersIteratorBegin();ogrePeer != mOgrePeerManager->getOgrePeersIteratorEnd();ogrePeer++)
        {
            if (ogrePeer->second->getXmlEntity()->getType() != ETAvatar) continue;
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//            if (((Avatar*)ogrePeer->second)->getEntity() != static_cast<Entity*>(mPickedMovable)) continue;
            Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
            if (((Avatar*)ogrePeer->second)->getEntity() != pickedEntity) continue;
            avatar = (Avatar*)ogrePeer->second;
            if ((avatar == mUserAvatar) && (getCameraMode() == CM1stPersonWithMouse)) continue;
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1AvatarHitByMouse() found Avatar movable=%s, Entity:Uid=%s, Entity:Name=%s", mPickedMovable->getName().c_str(), avatar->getXmlEntity()->getUid().c_str(), avatar->getEntity()->getName().c_str());
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::initialize()
{
    // call inherited
    if (!Instance::initialize())
        return false;

// COLOR PICKING
/*
    mColorPickingRT = TextureManager::getSingleton().createManual("ColorPickingRT", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TextureType::TEX_TYPE_2D, 256, 256, 0, PixelFormat::PF_B8G8R8, TU_RENDERTARGET, NULL);
    /// Set up viewport over entire texture
    RenderTexture *rtt = mColorPickingRT->getBuffer()->getRenderTarget();
    rtt->setAutoUpdated(false);
//    rtt->addListener(&mColorPickingRTListener);
    Camera* camera = mWindow->getViewport(0)->getCamera();
    // Save last viewport and current aspect ratio
    Viewport* oldViewport = camera->getViewport();
    Real aspectRatio = camera->getAspectRatio();
    Viewport* v = rtt->addViewport(camera);
    v->setClearEveryFrame(false);
    v->setOverlaysEnabled(false);
    v->setBackgroundColour(ColourValue(0, 0, 0, 0));
    // Should restore aspect ratio, in case of auto aspect ratio
    // enabled, it'll changed when add new viewport.
    camera->setAspectRatio(aspectRatio);
    // Should restore last viewport, i.e. never disturb user code
    // which might based on that.
    camera->_notifyViewport(oldViewport);
*/
// COLOR PICKING

    // Create Main Camera Support Manager
    mMainCameraSupportMgr=new CameraSupportManager(mCamera);

    if (!mNaviSupported)
    {
        // Call connect
        mNodeId = XmlHelpers::convertAuthentTypeToRepr(ATFixed) + mFixedNodeId;
        return connect();
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::initPostOgreCore()
{
    // call inherited
    if (!Instance::initPostOgreCore())
        return false;

    // boot Lua
    DebugHelpers::initLua(mLuaState);
    initNaviLua(mLuaState);
    Lunar<NavigatorLua>::Register(mLuaState);
    if (luaL_loadfile(mLuaState, "lua\\boot.lua") != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to load boot.lua, error: %s", lua_tostring(mLuaState, -1));
        return false;
    }
    if (lua_pcall(mLuaState, 0, LUA_MULTRET, 0))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to run boot.lua, error: %s", lua_tostring(mLuaState, -1));
        return false;
    }

    // Initialize sound system
    mNavigatorSound = new NavigatorSound();
    if (!mNavigatorSound->initialize())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to initialize sound");
        return false;
    }
    // Register our sound system as sound handler for "vlc" external texture source plugin
    ExternalTextureSource *vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
    if (vlcExtTextSrc != 0)
    {
        ExternalTextureSourceEx *vlcExtTextSrcEx = dynamic_cast<ExternalTextureSourceEx*>(vlcExtTextSrc);
        vlcExtTextSrcEx->setSoundHandler(mNavigatorSound);
    }

    // Retrieve Media/Cache path (either set by lua either found from cwd)
    if (mMediaCachePath.empty())
        mMediaCachePath = IO::getCWD() + "\\" + IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::createSceneManager()
{
    // Use the terrain scene manager.
    mSceneMgr = Root::getSingletonPtr()->createSceneManager(ST_EXTERIOR_CLOSE);

    // Create the ray scene query
    mRaySceneQuery = mSceneMgr->createRayQuery(Ray());
}

//-------------------------------------------------------------------------------------
void Navigator::createCamera()
{
    // call inherited
    Instance::createCamera();

    // Set the sound listener camera
    if (mNavigatorSound != 0)
        mNavigatorSound->setSoundListenerCamera(mCamera);
}

//-------------------------------------------------------------------------------------
void Navigator::createFrameListener()
{
    // Navigator frame listener
    mFrameListener = new NavigatorFrameListener(this);
}

//-------------------------------------------------------------------------------------
void Navigator::createScene()
{
    mSceneMgr->setAmbientLight(ColourValue(0.20, 0.20, 0.20));

    // Create OgrePeer manager
    mOgrePeerManager = new OgrePeerManager(mSceneMgr, this);

	// Create the Modeler 
    mModeler = new Modeler(mSceneMgr, mCamera, mOgrePeerManager);
	mModeler->init(mMediaCachePath);

    // Create the avatar editor
    mAvatarEditor = new AvatarEditor(mMediaCachePath, mSceneMgr);
    mAvatarEditor->buildListSAF();
}

//-------------------------------------------------------------------------------------
bool Navigator::createGUI()
{
    if (!Instance::createGUI())
        return false;

    if (!mNaviSupported)
        return true;

    // Create the GUI
    mNavigatorGUI = new NavigatorGUI(this);
    if (!mNavigatorGUI->startup())
        return false;

    mNavigatorGUI->login();

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::setWindow(IWindow* w)
{
    if (!Instance::setWindow(w))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::OgreLogger::log(int level, const char* msg)
{ 
    if (level > mVerbosity) return;
    OGRE_LOG(std::string(msg));
}

//-------------------------------------------------------------------------------------
bool Navigator::quit()
{
    mFrameListener->requestShutDown();
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::connect()
{
    if (mLogin.empty() || mNodeId.empty() || mWorldAddress.empty())
        return false;

    std::string peerHost;
    unsigned short peerPort;
    StringHelpers::getURLHostPort(mPeerAddress, peerHost, peerPort);
    mXmlRpcClient = new NavigatorXMLRPCClient(peerHost, peerPort, 0, "nattempts=2");

    //Try connection
#ifdef POOL
    RefCntPoolPtr<XmlLogin> xmlLogin;
    xmlLogin->setUsername(mLogin);
    std::string worldHost;
    unsigned short worldPort;
    StringHelpers::getURLHostPort(mWorldAddress, worldHost, worldPort);
    xmlLogin->setWorldHost(worldHost);
    xmlLogin->setWorldPort(worldPort);
    xmlLogin->setNodeId(mNodeId);
#else
    XmlLogin xmlLogin(mLogin, mWorldHost, mWorldPort, mNodeId);
#endif
#ifdef POOL
    bool nodeResponse = mXmlRpcClient->login(*xmlLogin, mNodeId);
#else
    bool nodeResponse = mXmlRpcClient->login(xmlLogin, mNodeId);
#endif

    if (!nodeResponse)
    {
        //Failed..
        return false;
    }
    else
    {
        // Connected !

        // Set my node identifier
        mOgrePeerManager->setNodeId(mNodeId);

        // Stop the node events listener thread
        NodeEventListener::stop();
        NodeEventListener::finalize();
        // Start listener
        if (!NodeEventListener::start())
        {
            mXmlRpcClient->logout();
            return false;
        }

        if (mNavigatorGUI != 0)
            mNavigatorGUI->inWorld();

        mState = SInWorld;

        // Create and add all needed Camera Supports to the Main Camera Support Manager
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("DetachedCameraSupport", CMDetached, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("FirstPersonCameraSupport", CM1stPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("FirstPersonWithMouseCameraSupport", CM1stPersonWithMouse, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("ThirdPersonCameraSupport", CM3rdPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("AroundPersonCameraSupport", CMAroundPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("ModelingCameraSupport", CMModeling, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("AroundObjectCameraSupport", CMAroundObject, mSceneMgr));
        return true;
    }
}

//-------------------------------------------------------------------------------------
bool Navigator::disconnect()
{
    if (mXmlRpcClient == 0)
        return false;

    // Unload avatar/modeler panels
    NavigatorFrameListener* navigatorFrameListener = (NavigatorFrameListener*)mFrameListener;
    if (mState == SAvatarEdit)
    {
        mNavigatorGUI->avatarMainUnload();
        setCameraMode(getLastCameraMode());
    }
    else if (mState == SModeling)
    {
        mNavigatorGUI->modelerMainUnload();
        setCameraMode(getLastCameraMode());
    }

    // Stop the node events listener thread
    NodeEventListener::stop();
    NodeEventListener::finalize();

    // Destroy XMLRPC client
    delete mXmlRpcClient;
    mXmlRpcClient = 0;

    // reset the camera mode
    setCameraMode(CMDetached);

    // Suppress all camera supports
    mMainCameraSupportMgr->suppressAllCameraSupports();

    // reset mouse picking
    resetMousePicking();

    // Clean up allocated peers datas
    mOgrePeerManager->cleanUp();
    mUserAvatar = 0;

    // Clean up modeler
    mModeler->cleanUp();

    if (mSceneMgr)
    {
        // destroy the sun light
        if (mSceneMgr->hasLight("SunLight"))
            mSceneMgr->destroyLight("SunLight");
        // destroy the skybox
        fakeSurroundingArea(0);
    }

    mState = SLogin;
    mNavigatorGUI->login();

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::mainMenuClick(const String& item)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::mainMenuClick() item=%s", item.c_str());

    if (mNavigatorGUI == 0) return true;

    NavigatorFrameListener* navigatorFrameListener = (NavigatorFrameListener*)mFrameListener;

    // Perform action associated to item selected
    // Submenu File
    if (item == "Exit")
        disconnect();
    // Submenu View
    else if (item == "1stPerson")
        setCameraMode(CM1stPerson);
    else if (item == "1stPersonMouse")
        setCameraMode(CM1stPersonWithMouse);
    else if (item == "3rdPerson")
        setCameraMode(CM3rdPerson);
    else if (item == "Orbit")
        setCameraMode(CMAroundPerson);
    // Submenu Panels
    else if (item == "Chat")
        mNavigatorGUI->switchLuaNavi(NavigatorGUI::NAVI_CHAT);
    else if (item == "Avatar")
    {
        if (mState == SInWorld)
        {
            setCameraMode(CMAroundPerson);
            mNavigatorGUI->avatarMainShow();
        }
        else if (mState == SAvatarEdit)
        {
            mNavigatorGUI->avatarMainUnload();
            setCameraMode(getLastCameraMode());
        }
    }
    else if (item == "Modeler")
    {
        if (mState == SInWorld)
        {
            setCameraMode(CMModeling);
            mNavigatorGUI->modelerMainShow();
        }
        else if (mState == SModeling)
        {
            mNavigatorGUI->modelerMainUnload();
            setCameraMode(getLastCameraMode());
        }
    }
    // Submenu Help
    else if (item == "About")
        mNavigatorGUI->switchLuaNavi(NavigatorGUI::NAVI_ABOUT, true);
    else if (item == "Commands")
        mNavigatorGUI->switchLuaNavi(NavigatorGUI::NAVI_COMMANDS, true);

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::contextItemSelected(const String& item)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::contextItemSelected() item=%s", item.c_str());

    if (mNavigatorGUI == 0) return true;

    mNavigatorGUI->contextHide();

    // Perform action associated to item selected
    if (item == "config")
    {
        if (mState == SInWorld)
        {
            setCameraMode(CMAroundPerson);
            mNavigatorGUI->avatarMainShow();
        }
    }
    else if (item == "create")
    {
        if (mState == SInWorld)
        {
            setCameraMode(CMModeling);
            mNavigatorGUI->modelerMainShow();
        }
    }
    else if (item == "chat")
    {
        mNavigatorGUI->switchLuaNavi(NavigatorGUI::NAVI_CHAT);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::sendMessage(const String& message)
{
    if (mXmlRpcClient == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send message without XMLRPC client", "Navigator::sendMessage");

    // decode URI encoded string into a wide-char string
    std::wstring messageWStr = NaviUtilities::decodeURIComponent(message);
    // log with locale string
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::sendMessage(%s)", StringHelpers::convertWStringToString(messageWStr).c_str());

#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETActionOnEntity);
    RefCntPoolPtr<XmlAction> xmlAction;
    xmlAction->setSourceEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setTargetEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setType(ATChat);
    xmlAction->setDesc(messageWStr);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlAction));
    std::string xmlResp;
    return mXmlRpcClient->sendEvt(*xmlEvt, xmlResp);
#else
    XmlEvt xmlEvt(ETActionOnEntity);
    XmlAction xmlAction(ETActionOnEntity);
    xmlAction->setTargetEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setType(ATChat);
    xmlAction->setDesc(messageWStr);
    std::string xmlResp;
    return mXmlRpcClient->sendEvt(xmlEvt, xmlResp);
#endif
}

//-------------------------------------------------------------------------------------
#ifdef POOL
void Navigator::onPeerNew(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
void Navigator::onPeerNew(XmlEntity* xmlEntity)
#endif
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerNew() uid:%s", xmlEntity->getUid().c_str());

#ifdef UIDEBUG
    if (mNavigatorGUI != 0)
        mNavigatorGUI->setTreeDirty(true);
#endif
    if (!mOgrePeerManager->load(xmlEntity))
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::onPeerNew() Unable to load entity !");
}

//-------------------------------------------------------------------------------------
#ifdef POOL
void Navigator::onPeerLost(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
void Navigator::onPeerLost(XmlEntity* xmlEntity)
#endif
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerLost() uid:%s", xmlEntity->getUid().c_str());

    if (!mOgrePeerManager->remove(xmlEntity->getUid(), false))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to remove lost peer !", "Navigator::onPeerLost");
}

//-------------------------------------------------------------------------------------
#ifdef POOL
void Navigator::onPeerUpdated(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
void Navigator::onPeerUpdated(XmlEntity* xmlEntity)
#endif
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerUpdated()");

    if (!mOgrePeerManager->update(xmlEntity))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to update peer !", "Navigator::onPeerUpdated");
}

//-------------------------------------------------------------------------------------
#ifdef POOL
void Navigator::onPeerAction(RefCntPoolPtr<XmlAction>& xmlAction)
#else
void Navigator::onPeerAction(XmlAction* xmlAction)
#endif
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerAction()");

    if (!mOgrePeerManager->action(xmlAction))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to process action on peer !", "Navigator::onPeerAction");
}

//-------------------------------------------------------------------------------------
void Navigator::processEvents()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::processEvents()");

    // Process each event
    NodeEventListener::EvtsList* nodeEvents = beginProcessEvents();
    for (NodeEventListener::EvtsList::iterator xmlEvt = nodeEvents->begin();xmlEvt != nodeEvents->end();++xmlEvt)
    {
        switch ((*xmlEvt)->getType())
        {
        case ETNewEntity:
#ifdef POOL
            onPeerNew(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
#else
            onPeerNew((XmlEntity*)((*xmlEvt)->getDatas()));
#endif
            break;
        case ETLostEntity:
#ifdef POOL
            onPeerLost(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
#else
            onPeerLost((XmlEntity*)((*xmlEvt)->getDatas()));
#endif
            break;
        case ETUpdatedEntity:
#ifdef POOL
            onPeerUpdated(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
#else
            onPeerUpdated((XmlEntity*)((*xmlEvt)->getDatas()));
#endif
            break;
        case ETActionOnEntity:
#ifdef POOL
            onPeerAction(RefCntPoolPtr<XmlAction>((*xmlEvt)->getDatas()));
#else
            onPeerAction((XmlAction*)((*xmlEvt)->getDatas()));
#endif
            break;
        default: // Caller already check type consistency
            break;
        }
#ifdef POOL
#else
        delete (*xmlEvt);
#endif
    }
    endProcessEvents();
}

//-------------------------------------------------------------------------------------
void Navigator::sendEvents()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::sendEvents()");

    if (mXmlRpcClient == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send events without XMLRPC client", "Navigator::sendEvents");

    // Send each event
    std::string xmlResp;
    OgrePeerManager::EvtsList& evtsList = mOgrePeerManager->getEvtsToSendList();
    for (OgrePeerManager::EvtsList::iterator xmlEvt = evtsList.begin(); xmlEvt != evtsList.end(); ++xmlEvt)
    {
#ifdef POOL
        if (!mXmlRpcClient->sendEvt(*(*xmlEvt), xmlResp))
#else
        if (!mXmlRpcClient->sendEvt((*xmlEvt), xmlResp))
#endif
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::sendEvents() Unable to send event !");
    }
    evtsList.clear();
}

//-------------------------------------------------------------------------------------
void Navigator::onAvatarNodeCreate(OgrePeer* ogrePeer)
{
    // User Avatar ?
    if (ogrePeer->isLocal())
    {
        mUserAvatar = (Avatar*)ogrePeer;
        
        // Attach all camera supports to the new avatar 
        mMainCameraSupportMgr->attachAllCameraSupportsToNode(mUserAvatar->getSceneNode());
 
        // Set the ThirdPersonCam as active
        setCameraMode(CM3rdPerson);
    }
}

//-------------------------------------------------------------------------------------
void Navigator::onSceneNodeCreate(OgrePeer* ogrePeer)
{
    // create the sun light
    Light *sunLight = mSceneMgr->createLight("SunLight");
    sunLight->setType(Light::LT_DIRECTIONAL);
    Vector3 sunLightDir = Vector3(0.5f, -1.0f, 0.25f);
    sunLightDir.normalise();
    sunLight->setDirection(sunLightDir);
    sunLight->setDiffuseColour(ColourValue::White);
    sunLight->setSpecularColour(ColourValue::White);
#ifdef SHADOWS
    sunLight->setCastShadows(true);
#endif
}

//-------------------------------------------------------------------------------------
bool Navigator::startModeling()
{
	mState = SModeling;

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endModeling()
{	
	// Go back in world
	mState = SInWorld;

    // Save all owned objects
	mdlrXMLSave(true);

	if (mModeler)
		mModeler->deselectNode();

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::createPlane()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createPlane(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createBox()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createBox(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createCorner()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createCorner(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createPyramid()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createPyramid(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createPrism()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createPrism(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createCylinder()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createCylinder(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createHalfCyl()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createHalfCyl(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createCone()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createCone(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createHalfCone()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createHalfCone(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createSphere()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createSphere(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createHalfSphere()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createHalfSphere(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createTorus()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createTorus(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createTube()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createTube(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createRing()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createRing(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::createMesh()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
	return mModeler->createMesh(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::startAvatarEdit()
{
    mState = SAvatarEdit;
  
	return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endAvatarEdit()
{	
	// Go back in world
	mState = SInWorld;

    // Save avatar
	avatarXMLSave();

    // Attach all camera supports to the new avatar 
    mMainCameraSupportMgr->attachAllCameraSupportsToNode(mUserAvatar->getSceneNode());
    
    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseMoved(const MouseEvt& evt)
{
}

//-------------------------------------------------------------------------------------
void Navigator::onMousePressed(const MouseEvt& evt)
{
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseReleased(const MouseEvt& evt)
{
	if (mModeler && !mModeler->isSelectionEmpty())
	{
		if (mModeler->isOnGizmo())
		{
			if(isOnGizmo) 
				isOnGizmo = false;
		}
	}
}

//-------------------------------------------------------------------------------------
void Navigator::MdlrModifGizmo(Vector3 dep)
{
	SceneNode* node = mSceneMgr->getSceneNode("NodeSelection");
	Vector3 vec;

	switch (mModeler->getSelection()->mTransformation->getMode())
	{
	case Transformations::MOVE:
		mModeler->updateCommand( Object3D::TRANSLATE, mModeler->getSelected() );
		mModeler->getSelection()->move(dep.x, dep.y, dep.z);
		node->translate(dep);
		break;
	case Transformations::ROTATE:
		mModeler->updateCommand( Object3D::ROTATE, mModeler->getSelected() );
		dep *= 5.;
		//vec = mModeler->getSelected()->getRotate();
		//vec += dep;
		//mModeler->getSelection()->rotate(vec.x, vec.y, vec.z);
        mModeler->getSelection()->rotate(dep.x, dep.y, dep.z);
		break;
	case Transformations::SCALE:
		mModeler->updateCommand( Object3D::SCALE, mModeler->getSelected() );

		vec = mModeler->getSelected()->getScale();
		//vec = Vector3::UNIT_SCALE;

		vec += dep;
		//mModeler->getSelection()->scaleTo(vec.x, vec.y, vec.z);
		mModeler->getSelection()->scale(vec.x, vec.y, vec.z);
		break;
	case Transformations::SELECT:
		break;
	}
}

//-------------------------------------------------------------------------------------
bool Navigator::mdlrXMLImport()
{
	if (mModeler)
	{
		Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
		Radian angle = pldir.getYaw();
		Ogre::Vector3 dep = Vector3(2.0,0,0);

		Real cosY = Math::Cos(angle);
		Real sinY = Math::Sin(angle);

		Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
		//y = point.y;							//		y' = y  
		dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
		dep.x = x;

        EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
		return mModeler->XMLImport(entityUID, entityUID, "", mUserAvatar->getSceneNode()->getPosition() + dep );
	}

	return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::mdlrXMLSave(bool all)
{
	if (mModeler)
		if (all || !mModeler->isSelectionEmpty())
            return mModeler->XMLSave(all);
		else
            mNavigatorGUI->showMessageBox("Modeler information", NavigatorGUI::ms_ModelerErrors[NavigatorGUI::ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);

	return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLLoad()
{
    if (mAvatarEditor)
		//if( mAvatarEditor->XMLLoad() )
        {
            // 0. store the old avatar' parameters (camera mode ...)
            Quaternion dir = mUserAvatar->getSceneNode()->getOrientation();
            Vector3 pos = mUserAvatar->getSceneNode()->getPosition();

            // 1. remove the current avatar' mesh + skeleton
			// -> it's not necessary to unload because this avatar can be used by another user in the current scene !
			// -> so just remove the Entity / Mesh from the SceneNode

            // 2. update the avatar' mesh + skeleton

            // 3. update his position & orientation

            // 4. set the old parameters to the new avatar (camera mode ...)

            return true;
        }

	return false;
}
//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLSave()
{
	if (mAvatarEditor)
    {
		mAvatarEditor->getManager()->getCurrentInstance()->saveModified();
        mOgrePeerManager->onUserAvatarSave();
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLSaveAs()
{
	if (mAvatarEditor)
	{}

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::setCameraMode(int mode)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorFrameListener::setCameraMode(%d)", mode);

    if (mode == mMainCameraSupportMgr->getActiveCameraSupportIndex() && (mode != CMAroundPerson && mode != CMModeling && mode != CM3rdPerson)) 
        return;

    if (((mState == SAvatarEdit) && (mMainCameraSupportMgr->getActiveCameraSupportIndex() == CMAroundPerson)) ||
        ((mState == SModeling) && (mMainCameraSupportMgr->getActiveCameraSupportIndex() == CMModeling)))
        return;

    if (mUserAvatar == 0) return;
	Vector3 pos;
	Quaternion orientation;
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
     pos = mUserAvatar->getSceneNode()->getWorldPosition();
	 orientation = mUserAvatar->getSceneNode()->getWorldOrientation();
#else
	 pos = mUserAvatar->getSceneNode()->_getDerivedPosition();
	 orientation = mUserAvatar->getSceneNode()->_getDerivedOrientation();
#endif
    Vector3 size = mUserAvatar->getEntity()->getBoundingBox().getSize();

    switch (mode)
    {
    case CMDetached:
        {
            mMainCameraSupportMgr->activeCameraSupport(CMDetached);
            break;
        }
    case CM1stPerson:
        {
            mMainCameraSupportMgr->activeCameraSupport(CM1stPerson);
            FPCameraSupport* FPSupportCam = (FPCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM1stPerson);
            FPSupportCam->resetCameraSupport();
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            FPSupportCam->yaw(Radian(-Math::PI/2));
            // Translate the origin of the camera support along the y axis to the eyes of the avatar (85% of the bbox)
            FPSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.85*size.y, 0));
            mUserAvatar->setMvtType(Avatar::MT1stPerson);
            break;
        }
    case CM1stPersonWithMouse:
        {
            mMainCameraSupportMgr->activeCameraSupport(CM1stPersonWithMouse);
            FPCameraSupport* FPWMSupportCam = (FPCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM1stPersonWithMouse);
            FPWMSupportCam->resetCameraSupport();
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            FPWMSupportCam->yaw(Radian(-Math::PI/2));
            // Translate the origin of the camera support along the y axis to the eyes of the avatar (85% of the bbox)
            FPWMSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.85*size.y, 0));
            mUserAvatar->setMvtType(Avatar::MT1stPerson);
            break;
        }
    case CM3rdPerson:
        {
            mMainCameraSupportMgr->activeCameraSupport(CM3rdPerson);
            OrbitalCameraSupport* TPSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM3rdPerson);
            TPSupportCam->resetCameraSupport();
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            TPSupportCam->yaw(Radian(-Math::PI/2));
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox
            TPSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.5*size.y, 0));
            TPSupportCam->pitch(Degree(-15.0));
            TPSupportCam->setDistanceFromTarget(2.0*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
        }
        
    case CMAroundPerson:
		{
            mMainCameraSupportMgr->activeCameraSupport(CMAroundPerson);
            OrbitalCameraSupport* APSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CMAroundPerson);
            APSupportCam->resetCameraSupport();
            mCamera->setOrientation(Quaternion::IDENTITY);
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            APSupportCam->yaw(Radian(-Math::PI/2));
            // Put camera on the front of the avatar 
            APSupportCam->yaw(Radian(Math::PI));
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox
            APSupportCam->translateCameraSupport(0.0, 0.5*size.y, 0.0);
            APSupportCam->pitch(Degree(-15.));
            APSupportCam->setDistanceFromTarget(2*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
        }
		
	case CMModeling:
		{
			mMainCameraSupportMgr->activeCameraSupport(CMModeling);
            OrbitalCameraSupport* MSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CMModeling);
            MSupportCam->resetCameraSupport();
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox, and 1,5m ahead
            MSupportCam->translateCameraSupport(DIST_AVATAR_OBJECT, 0.5*size.y, 0.0);
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            MSupportCam->yaw(Radian(-Math::PI/2));
            // Put camera on the front of the avatar (45°)
            MSupportCam->yaw(Degree(45.0));
            MSupportCam->pitch(Degree(-35.));
            MSupportCam->setDistanceFromTarget(2.5*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
		}
	case CMAroundObject:
		{
    		break;
		}
    }
    mUserAvatar->getEntity()->setVisible(mode == CM3rdPerson || mode == CMAroundPerson || mode == CMModeling);
    mUserAvatar->setNameVisibility(mode == CM3rdPerson || mode == CMAroundPerson || mode == CMModeling);
    if (mNavigatorGUI != 0)
    {
        // Hide mouse only on 1st person camera mode
        mNavigatorGUI->SetMouseVisibility(mode != CM1stPerson);
        // Set mouse exclusive mode in windowed mode (exclusive only on 1st person camera mode)
        if (!mIWindow->isFullscreen())
            mIWindow->setMouseExclusive(mode == CM1stPerson);
        mNavigatorGUI->setNaviVisibility(mNavigatorGUI->getNaviName(NavigatorGUI::NAVI_MAINMENU), mode != CM1stPerson);
        NaviManager::Get().deFocusAllNavis();
        if (mode == CM1stPerson)
            mNavigatorGUI->setStatusBarText("Press 2,3 or 4 to return to a view with mouse ...");
        else if (mode == CMAroundPerson || mode == CMModeling)
            mNavigatorGUI->setStatusBarText("Click/Drag middle button to rotate ...");
    }
}
