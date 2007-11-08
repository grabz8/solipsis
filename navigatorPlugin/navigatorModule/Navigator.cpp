#include "Navigator.h"
#include "NavigatorFrameListener.h"
#include "OgreOSMScene.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "NaviLua.h"

using namespace NavigatorModule;

Navigator* Navigator::ms_singletonPtr = 0;

// this internal OSM-loader callbacks class is used to force OFF shadows casting of entities
class navigatorOSMSceneCallbacks : public OSMSceneCallbacks
{
    virtual void OnLightCreate(Light *pLight, TiXmlElement* pLightDesc)
    {
        pLight->setCastShadows(false);
    }
    virtual void OnEntityCreate(Entity *pEntity, TiXmlElement* pEntityDesc)
    {
        pEntity->setCastShadows(false);
    }
};

Navigator::Navigator() :
    Instance(),
    NodeEventListener(mXmlRpcClient),
    mFakeTerrain(false),
    mState(SLogin),
    mConnectionMode(CMExistingNode),
    mNodeStatus(NSUnavailable),
    mUdpPort(6010),
    mHost("localhost"),
    mPort(8550),
    mXmlRpcClient(0),
    mNavigatorGUI(0),
    mMaxNaviPickingDistance(500),
    mMaxAvatarPickingDistance(500),
    mRaySceneQuery(0),
    mPickedMovable(0),
    mUserAvatar(0)
#ifdef PHYSICS
    ,mPhysicsWorld(0),
    mPhysicsStepHandler(0),
    mPhysicsWorldGeometry(0)
#endif
{
    ms_singletonPtr = this;

    // Lua initialization
    mLuaState = lua_open();
    luaL_openlibs(mLuaState);
}

//-------------------------------------------------------------------------------------
Navigator::~Navigator()
{
    // Stop the node events listener thread
    NodeEventListener::stop();
    NodeEventListener::finalize();

    // Clean up allocated peers datas
    delete mXmlRpcClient;

    // Clean up allocated peers datas
    cleanUpPeers(true);

    // Destroy the ray scene query
    if (mSceneMgr)
        mSceneMgr->destroyQuery(mRaySceneQuery);

    // Destroy the GUI
    delete mNavigatorGUI;

    // Lua finalization
    lua_close(mLuaState);

#ifdef PHYSICS
    // Destroy the physical world
    delete mPhysicsWorldGeometry;
    delete mPhysicsStepHandler;
    delete mPhysicsWorld;
#endif
}

//-------------------------------------------------------------------------------------
Navigator* Navigator::getSingletonPtr()
{
    return ms_singletonPtr;
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
Navigator::ConnectionMode Navigator::getConnectionMode()
{
    return mConnectionMode;
}

//-------------------------------------------------------------------------------------
void Navigator::setConnectionMode(Navigator::ConnectionMode connectionMode)
{
    mConnectionMode = connectionMode;
}

//-------------------------------------------------------------------------------------
Navigator::NodeStatus Navigator::getNodeStatus()
{
    return mNodeStatus;
}

//-------------------------------------------------------------------------------------
int Navigator::getConnectionUdpPort()
{
    return mUdpPort;
}

//-------------------------------------------------------------------------------------
void Navigator::setConnectionUdpPort(int udpPort)
{
    mUdpPort = udpPort;
}

//-------------------------------------------------------------------------------------
String Navigator::getConnectionHost()
{
    return mHost;
}

//-------------------------------------------------------------------------------------
void Navigator::setConnectionHost(String host)
{
    mHost = host;
}

//-------------------------------------------------------------------------------------
int Navigator::getConnectionPort()
{
    return mPort;
}

//-------------------------------------------------------------------------------------
void Navigator::setConnectionPort(int port)
{
    mPort = port;
}

//-------------------------------------------------------------------------------------
std::map<String,OgrePeer*>::iterator Navigator::getOgrePeersIteratorBegin()
{
    return mOgrePeersMap.begin();
}

//-------------------------------------------------------------------------------------
std::map<String,OgrePeer*>::iterator Navigator::getOgrePeersIteratorEnd() {
    return mOgrePeersMap.end();
}

//-------------------------------------------------------------------------------------
NavigatorGUI* Navigator::getNavigatorGUI()
{
    return mNavigatorGUI;
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

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
OgreOde::World* Navigator::getPhysicsWorld()
{
    return mPhysicsWorld;
}

//-------------------------------------------------------------------------------------
OgreOde::StepHandler* Navigator::getPhysicsStepHandler()
{
    return mPhysicsStepHandler;
}

//-------------------------------------------------------------------------------------
OgreOde::TriangleMeshGeometry* Navigator::getPhysicsWorldGeometry()
{
    return mPhysicsWorldGeometry;
}
#endif

//-------------------------------------------------------------------------------------
void Navigator::fakeSurroundingArea(int index)
{
    switch (index)
    {
        case 1:
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox1", 1000);
           break;
        case 2:
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 100, true);
           break;
        case 3:
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox2", 10, true);
           break;
        case 4:
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyBox(true, "Solipsis/SkyBox3", 10, true);
           break;
#ifndef PHYSICS
        case 5: 
           mFakeTerrain = true;
           mSceneMgr->setWorldGeometry("SolipsisFakeTerrain.cfg"); //Add fake terrain (fake because local and not shared with other peers)
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyBox(true, "SkyBox", 1000);
           break;
#endif
        case 0:
           mUserAvatar->setGravity(mFakeTerrain);
           mSceneMgr->setSkyDome(false, "Solipsis/SkyBox1");
           mSceneMgr->setSkyBox(false, "Solipsis/SkyBox2");
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

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, -60);
    MeshManager::getSingleton().createPlane("demoNavi1Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 85, 85, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi1Video", "demoNavi1Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    //vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=mAJYQOANDCk", 512, 512, true, 15, true, 0.75f));
    vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=066_q4DIeqk", 512, 512, true, 15, true, 0.75f));
    //http://www.youtube.com/watch?v=ZQcUS4chhc4
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1VideoNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(85, 100, 85));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoNavi1Text", "demoNavi1Plane");
    txtEnt->setQueryFlags(QFNaviPanel);
    txtEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(txtEnt->getName(), "local://lgpl-3.0.txt", 512, 512, true, 8, false));
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1TextNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(85, 100, 85));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoNavi1WebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFNaviPanel);
    std::string googleMtlName = NaviLibrary::NaviManager::Get().createNaviMaterial(knotEnt->getName(), "http://www.google.com", 512, 512, true, 8, false);
    MaterialPtr googleMtl = (MaterialPtr)MaterialManager::getSingletonPtr()->getByName(googleMtlName);
    googleMtl->setDepthWriteEnabled(true);
    knotEnt->setMaterialName(googleMtlName);
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1WebKnotNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(0, 150, 250));
    node->attachObject(knotEnt);
}
#endif
#ifdef DEMO_NAVI2
//-------------------------------------------------------------------------------------
void Navigator::demoNavi2()
{
    static bool active = false;
    if (active) return;
    active = true;

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, 0);
#ifdef CAPSULEGEOM
    MeshManager::getSingleton().createPlane("demoNavi2Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 200, 200, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
#else
    MeshManager::getSingleton().createPlane("demoNavi2Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 85, 85, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
#endif

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi2Video", "demoNavi2Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=mAJYQOANDCk", 512, 512, true, 15, true, 0.75f));
//    vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=066_q4DIeqk", 512, 512, true, 15, true, 0.75f));
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demo2VideoNode");
    videoNode->attachObject(vidEnt);
#ifdef CAPSULEGEOM
    videoNode->setPosition(Vector3(10, 100, -1300));
    videoNode->yaw(Degree(180), Node::TS_WORLD);
#else
    videoNode->setPosition(Vector3(280, 100, -100));
    videoNode->yaw(Degree(180), Node::TS_WORLD);
#endif
}
#endif
#ifdef PHYSICS
#ifdef DEMO_PHYSICS1
//-------------------------------------------------------------------------------------
void Navigator::demoPhysics1()
{
#define MAX_BOXES 10
    static int nextBox = 0;
    static std::map<String, SceneNode*> boxes;
    if (mPhysicsWorld == 0) return;

    // Create a box
    int boxNum = nextBox;
    nextBox = (nextBox + 1)%MAX_BOXES;
    String boxName = "demoPhysics1box" + StringConverter::toString(boxNum);
    String boxNodeName = boxName + "Node";
    Vector3 boxGeomSize = Vector3(100, 100, 100);
    Vector3 boxScale = Vector3(0.5 - boxNum*0.02, 0.5 - boxNum*0.02, 0.5 - boxNum*0.02);
    Vector3 userAvatarPos = mUserAvatar->getSceneNode()->getWorldPosition();
    Vector3 userAvatarVpn = mUserAvatar->getSceneNode()->getWorldOrientation()*Vector3::UNIT_X;
    Vector3 userAvatarVup = mUserAvatar->getSceneNode()->getWorldOrientation()*Vector3::UNIT_Y;
    Vector3 boxPos = userAvatarPos + userAvatarVpn*300 + userAvatarVup*300;
    SceneNode* boxNode = 0;
    OgreOde::Body* boxBody = 0;
    Entity* boxEntity = 0;
    OgreOde::BoxGeometry* boxGeom = 0;
    std::map<String, SceneNode*>::iterator boxIt = boxes.find(boxName);
    if (boxIt != boxes.end())
    {
        boxNode = mSceneMgr->getSceneNode(boxNodeName);
        boxBody = (OgreOde::Body*)boxNode->getAttachedObject(boxName + "Body");
        boxEntity = (Entity*)boxNode->getAttachedObject(boxName + "Ent");
        boxGeom = (OgreOde::BoxGeometry*)boxBody->getGeometry(0);
    }
    else
    {
        boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(boxNodeName);
        boxNode->setScale(boxScale);
        boxEntity = mSceneMgr->createEntity(boxName + "Ent", "cube.mesh");
        boxEntity->setMaterialName("2 - Default");
        boxNode->attachObject(boxEntity);
        boxBody = new OgreOde::Body(mPhysicsWorld, boxName + "Body");
        boxBody->setMass(OgreOde::BoxMass(1, boxGeomSize*boxScale));
        boxNode->attachObject(boxBody);
        boxGeom = new OgreOde::BoxGeometry(boxGeomSize*boxScale, mPhysicsWorld, mPhysicsWorld->getDefaultSpace());
        boxGeom->setBody(boxBody);
        boxes[boxName] = boxNode;
    }
    boxBody->sleep();
    boxBody->setPosition(boxPos);
    boxBody->setLinearVelocity(Vector3::ZERO);
    boxBody->setAngularVelocity(Vector3::ZERO);
    boxBody->wake();
}
#endif
#endif

//-------------------------------------------------------------------------------------
Entity* Navigator::getNaviEntity(const String& naviName)
{
    try {
        return mSceneMgr->getEntity(naviName);
    }
    catch (...) {
        return 0;
    }
}

//-------------------------------------------------------------------------------------
void Navigator::resetMousePicking()
{
    mRaySceneQuery->clearResults();
    closestDistance = -1.0f;
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
    closestDistance = -1.0f;
    mPickedMovable = 0;
    String movablesList;
    for (RaySceneQueryResult::iterator it = queryResult.begin(); it != queryResult.end(); ++it )
    {
        if (it->movable && (it->distance > 0))
        {
            if (movablesList.length() > 0) movablesList += ", ";
            movablesList += it->movable->getName() + ":" + StringConverter::toString(it->distance);
            // stop checking if we have found a raycast hit that is closer
            // than all remaining entities
            if ((closestDistance >= 0.0f) && (closestDistance < it->distance))
                break;
            // ... or if we are too far
            if (it->distance >= std::max(mMaxNaviPickingDistance, mMaxAvatarPickingDistance))
                break;
            // only check this result if its a hit against an entity
            if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
            {
                // avatar ?
                if (it->movable->getQueryFlags() == QFAvatar)
                {
                    mPickedMovable = it->movable;
                    break;
                }
                // if we found a new closest raycast for this object, update the
                // mPickedMovable before moving on to the next object.
                if (OgreHelpers::isEntityHitByMouse(mouseRay, static_cast<Entity*>(it->movable),
                                                    closestDistance,
                                                    closestUV,
                                                    closestTriUV0, closestTriUV1, closestTriUV2))
                {
                    if ((it->movable->getQueryFlags() == QFNaviPanel) && (it->distance < mMaxNaviPickingDistance))
                        mPickedMovable = it->movable;
                }
            }       
        }
    }
    OGRE_LOG("Navigator::computeMousePicking() found movables " + movablesList);
    // if 1 entity hit
    if ((closestDistance >= 0.0f) && (mPickedMovable != 0))
    {
        closestHitPoint = mouseRay.getPoint(closestDistance);
        OGRE_LOG("Navigator::computeMousePicking() found movable=" + mPickedMovable->getName() + ", closestDistance=" + StringConverter::toString(closestDistance) + ", closestHitPoint=" + StringConverter::toString(closestHitPoint));
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
    NaviLibrary::NaviManager::Get().getNaviExtents(naviName, naviWidth, naviHeight);
    naviX = ((int)(closestResultUV.x*naviWidth))%naviWidth;
    naviY = ((int)(closestResultUV.y*naviHeight))%naviHeight;
    OGRE_LOG("Navigator::computeNaviHit() uv=" + StringConverter::toString(Vector2(closestUV.x, closestUV.y)) + ", dt1=" + StringConverter::toString(dt1) + ", dt2=" + StringConverter::toString(dt2) + ", closestResultUV=" + StringConverter::toString(closestResultUV));
}

//-------------------------------------------------------------------------------------
bool Navigator::is1NaviHitByMouse(String& naviName, int& naviX, int& naviY)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() == QFNaviPanel))
    {
        naviName = mPickedMovable->getName();
        // compute texture coordinates of the hit
        computeNaviHit(naviName,
                       closestUV,
                       closestTriUV0, closestTriUV1, closestTriUV2,
                       naviX, naviY);
        OGRE_LOG("Navigator::is1NaviHitByMouse() found Navi movable=" + mPickedMovable->getName() + ", naviName=" + naviName + ", (naviX, naviY)=(" + StringConverter::toString(naviX) + ", " + StringConverter::toString(naviY) + ")");
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::is1AvatarHitByMouse(Avatar*& avatar)
{
    // if 1 Avatar entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() == QFAvatar))
    {
        // retrieve avatar instance
        for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
        {
            if (ogrePeer->second->getType().compare("avatar") != 0) continue;
            if (((Avatar*)ogrePeer->second)->getEntity() != static_cast<Entity*>(mPickedMovable)) continue;
            avatar = (Avatar*)ogrePeer->second;
            OGRE_LOG("Navigator::is1AvatarHitByMouse() found Avatar movable=" + mPickedMovable->getName() + ", Peer:NetworkId=" + avatar->getPeer()->getNetworkId());
            return true;
        }
    }

    return false;
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
        OGRE_LOG("Navigator::initialise() Unable to load boot.lua, error: " + String(lua_tostring(mLuaState, -1)));
        return false;
    }
    if (lua_pcall(mLuaState, 0, LUA_MULTRET, 0))
    {
        OGRE_LOG("Navigator::initialise() Unable to run boot.lua, error: " + String(lua_tostring(mLuaState, -1)));
        return false;
    }

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
void Navigator::createFrameListener()
{
    // Navigator frame listener
    mFrameListener = new NavigatorFrameListener(this);
}

//-------------------------------------------------------------------------------------
void Navigator::createScene()
{
    mSceneMgr->setAmbientLight(ColourValue(0.20, 0.20, 0.20));

    // Create user avatar
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("UserAvatarNode");
    Entity* entity = mSceneMgr->createEntity("UserAvatar", "salamandra.mesh");//robot.mesh
    entity->setQueryFlags(QFAvatar);
#ifdef SHADOWS
    entity->setCastShadows(true);
#endif
    Peer* userLocalPeer = new Peer(String("UserAvatarPeer"), 0, 0, 0);
    mUserAvatar = new Avatar(userLocalPeer, node, entity);
    mOgrePeersMap[userLocalPeer->getNetworkId()] = mUserAvatar;
    mUserAvatar->setGravity(false);
    mUserAvatar->setStateAnimName(Avatar::SWalk, "Walk");
    mUserAvatar->setStateAnimName(Avatar::SRun, "Run");
    if (entity->getMesh()->getName().find("robot") != String::npos)
        mUserAvatar->setStateAnimName(Avatar::SRun, "Walk");
    mUserAvatar->setStateAnimName(Avatar::SFly, "Walk");
    mUserAvatar->setStateAnimName(Avatar::SSwim, "Walk");
    mUserAvatar->setState(Avatar::SIdle);

#ifdef LEXI
    if (entity->getMesh()->getName().find("salamandra") != String::npos)
        node->setPosition(0, 47, 0);
#else
    node->setPosition(0, 0, 0);
#ifdef CAPSULEGEOM
    node->setPosition(0, 0, -650);
#endif
#endif

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

    // Create camera node/pitch nodes
    Vector3 avatarSize = mUserAvatar->getEntity()->getBoundingBox().getSize();
    Vector3 avatarHalfSize = mUserAvatar->getEntity()->getBoundingBox().getHalfSize();

    // Create First person camera node/pitch node
    SceneNode* camNode = node->createChildSceneNode("FirstPersonCamNode", Vector3(0, 0.95, 0)*avatarSize);
#ifdef LEXI
    if (entity->getMesh()->getName().find("salamandra") != String::npos)
        camNode->setPosition(Vector3(0, 1, 0)*avatarHalfSize);
#endif
    camNode->yaw(Radian(-Math::HALF_PI));
    SceneNode* pitchCamNode = camNode->createChildSceneNode("FirstPersonCamPitchNode");

    // Create the Third camera node/pitch node
    camNode = node->createChildSceneNode("ThirdPersonCamNode", Vector3(-4, 1.1, 0)*avatarSize.y);
#ifdef LEXI
    if (entity->getMesh()->getName().find("salamandra") != String::npos)
        camNode->setPosition(Vector3(0, -47, 0) + Vector3(-4, 1.1, 0)*avatarSize);
#endif
    camNode->yaw(Radian(-Math::HALF_PI));
    pitchCamNode = camNode->createChildSceneNode("ThirdPersonCamPitchNode");

    // attach camera on Third person
    pitchCamNode->attachObject(mCamera);

    // Waiting connection..
    mUserAvatar->getEntity()->setVisible(false);
    mUserAvatar->setNameVisibility(false);
}

//-------------------------------------------------------------------------------------
bool Navigator::createGUI()
{
    if (!Instance::createGUI())
        return false;

    // Create the GUI
    mNavigatorGUI = new NavigatorGUI(this);
    if (!mNavigatorGUI->startup())
        return false;

    mNavigatorGUI->login();

    return true;
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
    mXmlRpcClient = new NavigatorXMLRPCClient(mHost.c_str(),mPort);

    //Try connection
    bool nodeResponse = mXmlRpcClient->connect();

    if (!nodeResponse)
    {
        //Failed..
        return false;
    }
    else
    {
        // Stop the node events listener thread
        NodeEventListener::stop();
        NodeEventListener::finalize();
        // Start listener
        if (!NodeEventListener::start())
        {
            mXmlRpcClient->disconnect();
            return false;
        }

        // Connected !
        // Get node status
        String nodeStatus;
        nodeResponse = mXmlRpcClient->getStatus(nodeStatus);
        if (nodeResponse)
            setNodeStatus(nodeStatus);

        // Get all peers
        cleanUpPeers(false);
        nodeResponse = mXmlRpcClient->getAllPeers(mPeersList);

        if (nodeResponse)
        {
            //You've got peers!
//            CEGUI::Editbox* infoTxt = (CEGUI::Editbox*)CEGUI::System::getSingleton().getGUISheet()->getChild("InfoTxt");
//            char nb[128]; sprintf(nb, "%d", mPeersList.size());
//            infoTxt->setText("Node has found " + String(nb) + " peer(s)");
            
            //Create avatars, set on the scene and store them
            for (std::list<Peer*>::iterator peer = mPeersList.begin();peer != mPeersList.end();++peer)
            {
                OGRE_LOG("create avatar for " + (*peer)->getLogin());
                Avatar* avatar = generateAvatarFromPeer(*peer);
                mOgrePeersMap[(*peer)->getNetworkId()] = avatar;
            }
        }

        mNavigatorGUI->inWorld();

        // set Third person camera
        ((NavigatorFrameListener*)mFrameListener)->setCameraMode(NavigatorFrameListener::CM3rdPerson);

        mState = SInWorld;

        return true;
    }
}

//-------------------------------------------------------------------------------------
bool Navigator::sendMessage(const String& message)
{
    if (mXmlRpcClient == 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send message without XMLRPC client", "Navigator::sendMessage");
    return mXmlRpcClient->sendMessage(message, mPeersList);
}

//-------------------------------------------------------------------------------------
bool Navigator::contextItemSelected(const String& item)
{
    OGRE_LOG("Navigator::contextItemSelected()");

    mNavigatorGUI->contextHide();
    OGRE_LOG("Navigator::contextItemSelected() item=" + item);

    // Perform action associated to item selected
    // TODO
    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::setNodeStatus(String& nodeStatusString)
{
    if (nodeStatusString.compare("READY") == 0)
        mNodeStatus = NSReady;
    else if (nodeStatusString.compare("BUSY") == 0)
        mNodeStatus = NSBusy;
    else if (nodeStatusString.compare("UNAVAILABLE") == 0)
        mNodeStatus = NSUnavailable;
    else
        Exception(Exception::ERR_INTERNAL_ERROR, "Unknown node status : " + nodeStatusString, "Navigator::setNodeStatus");
}

//-------------------------------------------------------------------------------------
void Navigator::cleanUpPeers(bool cleanUpLocalPeers)
{
    bool loopAgain;

    loopAgain = true;
    while (loopAgain)
    {
        loopAgain = false;
        for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
        {
            if (!cleanUpLocalPeers && ogrePeer->second->getPeer()->isLocal()) continue;
            mPeersList.remove(ogrePeer->second->getPeer());
            delete ogrePeer->second->getPeer();
            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            loopAgain = true;
            break;
        }
    }
    loopAgain = true;
    while (loopAgain)
    {
        loopAgain = false;
        for (std::list<Peer*>::iterator peer = mPeersList.begin();peer != mPeersList.end();++peer)
        {
            if (!cleanUpLocalPeers && (*peer)->isLocal()) continue;
            OGRE_LOG("deleting orphan peer instance " + (*peer)->getLogin());
            delete (*peer);
            mPeersList.erase(peer);
            loopAgain = true;
            break;
        }
    }
}

//-------------------------------------------------------------------------------------
Avatar* Navigator::generateAvatarFromPeer(Peer* peer)
{
    if (mSceneMgr == 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Navigator::generateAvatarFromPeer");

    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(peer->getNetworkId() + "Avatar");
    Entity* entity = mSceneMgr->createEntity(peer->getNetworkId() + "Avatar", "salamandra.mesh");//"salamandra.mesh");
    entity->setQueryFlags(QFAvatar);
#ifdef SHADOWS
    entity->setCastShadows(true);
#endif

    //TODO : need to set correct position         
    node->setPosition(peer->getFakeX(),peer->getFakeZ(),peer->getFakeY()); //careful to switch y and z !
#ifdef LEXI
    if (peer->getLogin().find("salamandra") != String::npos)
        node->setPosition(0, 47, 500);
#else
    node->setPosition(0, 3, 500);
#ifdef CAPSULEGEOM
    node->setPosition(-50, 3, -500);
#endif
#endif

    Avatar* peerAvatar = new Avatar(peer, node, entity);
    
    peerAvatar->setGravity(false);
    peerAvatar->setStateAnimName(Avatar::SWalk, "Walk");
    peerAvatar->setStateAnimName(Avatar::SRun, "Run");
    peerAvatar->setStateAnimName(Avatar::SFly, "Fly");
    peerAvatar->setStateAnimName(Avatar::SSwim, "Swim");
    peerAvatar->setState(Avatar::SIdle);

    return peerAvatar;
}

//-------------------------------------------------------------------------------------
Scene* Navigator::generateSceneFromPeer(Peer* peer)
{
    if (mSceneMgr == 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Navigator::generateSceneFromPeer");

    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(peer->getNetworkId() + "Scene");
    OSMScene osmScene(mSceneMgr);
    //OSMSceneCallbacks* osmSceneCallbacks = 0;
    navigatorOSMSceneCallbacks osmSceneCallbacks;
    if (!osmScene.initialise("Deltastation1.osm", &osmSceneCallbacks))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene !", "Navigator::generateSceneFromPeer");
    osmScene.declareResources();
    if (!osmScene.createScene(node))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene !", "Navigator::generateSceneFromPeer");

#ifdef SHADOWS
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
    mSceneMgr->setShadowTextureSettings(512, 2, PixelFormat::PF_A4R4G4B4);
#endif

    //TODO : need to set correct position         
//    node->setPosition(peer->getFakeX(),peer->getFakeZ(),peer->getFakeY()); //careful to switch y and z !
    node->setPosition(18,-58,133);

#ifdef PHYSICS
    // Create the physical world
    mPhysicsWorld = new OgreOde::World(mSceneMgr);
    mPhysicsWorld->setGravity(Vector3(0,-9.80665,0));
    mPhysicsWorld->setCFM(10e-5/50);
    mPhysicsWorld->setERP(0.8/50);
    mPhysicsWorld->setAutoSleep(true);
    mPhysicsWorld->setContactCorrectionVelocity(0.1*50);
    mPhysicsWorld->setContactSurfaceLayer(0.2*50);
//    mPhysicsWorld->setAutoSleepLinearThreshold(10.0);
//    mPhysicsWorld->setAutoSleepAngularThreshold(10.0);
    mPhysicsWorld->setCollisionListener(dynamic_cast<OgreOde::CollisionListener*>(mFrameListener));
    // Create something that will step the world, but don't do it automatically
//    mPhysicsStepHandler = new OgreOde::ForwardFixedStepHandler(mPhysicsWorld, OgreOde::StepHandler::BasicStep, Real(0.001), Real(1000.0), Real(1.0));
    mPhysicsStepHandler = new OgreOde::ForwardFixedStepHandler(mPhysicsWorld, OgreOde::StepHandler::QuickStep, Real(1.0/60.0), Real(1000.0), Real(5.0));
    mPhysicsStepHandler->setAutomatic(OgreOde::StepHandler::AutoMode_NotAutomatic, Root::getSingletonPtr());
    // Create the world collision mesh
    Entity* worldCollisionEntity = mSceneMgr->getEntity("MC_station");
    SceneNode* worldCollisionSceneNode = mSceneMgr->getSceneNode("MC_station");
    OgreOde::EntityInformer entityInformer(worldCollisionEntity, worldCollisionSceneNode->_getFullTransform());
    mPhysicsWorldGeometry = entityInformer.createStaticTriangleMesh(mPhysicsWorld, mPhysicsWorld->getDefaultSpace());
    worldCollisionSceneNode->setVisible(false);
    // Create user avatar's world ray
    mUserAvatar->createPhysicsRayGeometry(mPhysicsWorld, mPhysicsWorldGeometry);
#ifdef CAPSULEGEOM
    mUserAvatar->setMaxUpdateTimeStep(1.0/60.0);
#endif
#else
    // Destroy collision mesh
    mSceneMgr->destroySceneNode("MC_station");
#endif
    mUserAvatar->setGravity(true);

    Scene* peerScene = new Scene(peer, node);

    return peerScene;
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerNew(NodeEvent::DatasPeerNew& evtDatas)
{
    OGRE_LOG("Navigator::onPeerNew()");

    mPeersList.push_back(evtDatas.mPeer);
    Scene* scene = generateSceneFromPeer(evtDatas.mPeer);
    mOgrePeersMap[evtDatas.mPeer->getNetworkId()] = scene;
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerLost(NodeEvent::DatasPeerLost& evtDatas)
{
    OGRE_LOG("Navigator::onPeerLost()");

    bool peerFound = false;
    for (std::map<String,OgrePeer*>::iterator ogrePeer=mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
    {
        if (ogrePeer->second->getPeer()->getNetworkId().compare(evtDatas.mNetworkId) == 0)
        {
            if (ogrePeer->second->getPeer()->isLocal())
                Exception(Exception::ERR_INTERNAL_ERROR, "We cannot loose one local peer", "Navigator::onPeerLost");
            mPeersList.remove(ogrePeer->second->getPeer());
            delete ogrePeer->second->getPeer();
            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            peerFound = true;
            break;
        }
    }
    if (!peerFound)
        Exception(Exception::ERR_INTERNAL_ERROR, "Lost peer not found", "Navigator::onPeerLost");
}

//-------------------------------------------------------------------------------------
void Navigator::onStatusChanged(NodeEvent::DatasStatusChanged& evtDatas)
{
    OGRE_LOG("Navigator::onStatusChanged()");

    this->setNodeStatus(evtDatas.mStatus);
}

//-------------------------------------------------------------------------------------
void Navigator::processEvents()
{
//    OGRE_LOG("Navigator::processEvents()");

    // Process each event
    std::list<NodeEvent*>* nodeEvents = beginProcessEvents();
    for (std::list<NodeEvent*>::iterator evt = nodeEvents->begin();evt != nodeEvents->end();++evt)
    {
        switch ((*evt)->getType())
        {
        case NodeEvent::TNew:
            onPeerNew(*((NodeEvent::DatasPeerNew*)(*evt)->getDatas()));
            break;
        case NodeEvent::TLost:
            onPeerLost(*((NodeEvent::DatasPeerLost*)(*evt)->getDatas()));
            break;
        case NodeEvent::TStatusChanged:
            onStatusChanged(*((NodeEvent::DatasStatusChanged*)(*evt)->getDatas()));
            break;
        default: // Caller already check type consistency
            break;
        }
    }
    endProcessEvents();
}
