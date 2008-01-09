#include "Navigator.h"
#include "NavigatorFrameListener.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "NaviLua.h"

#include "Modeler.h"

#ifdef PHYSICSPLUGINS
#include "IPhysicsBody.h"
#endif

using namespace Solipsis;

Navigator* Navigator::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
Navigator::Navigator(const String name, IApplication* application) :
    Instance(name, application),
    NodeEventListener(mXmlRpcClient),
    mFakeTerrain(false),
    mState(SLogin),
    mConnectionMode(CMExistingNode),
    mNodeStatus(NSUnavailable),
    mUdpPort(6010),
    mHost("localhost"),
    mPort(8550),
    mXmlRpcClient(0),
    mOgrePeerManager(0),
    mNavigatorGUI(0),
    mMaxNaviPickingDistance(500),
    mMaxAvatarPickingDistance(500),
    mRaySceneQuery(0),
    mPickedMovable(0),
    mUserAvatar(0),
	mModeler(0)
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
    delete mOgrePeerManager;

    // Destroy the ray scene query
    if (mSceneMgr)
        mSceneMgr->destroyQuery(mRaySceneQuery);

    // Destroy the GUI
    delete mNavigatorGUI;

    // Lua finalization
    lua_close(mLuaState);
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
#if !defined(PHYSICS) && !defined(PHYSX) && !defined(TOKAMAK) && !defined(PHYSICSPLUGINS)
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
    Plane plane(Vector3::NEGATIVE_UNIT_Z, -1.1);
    MeshManager::getSingleton().createPlane("demoNavi1Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.5, 1.5, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi1Video", "demoNavi1Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    //vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=mAJYQOANDCk", 512, 512, true, 15, true, 0.75f));
    vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=066_q4DIeqk", 512, 512, true, 15, true, 0.75f));
    //http://www.youtube.com/watch?v=ZQcUS4chhc4
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1VideoNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(1, 1.5, 1));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoNavi1Text", "demoNavi1Plane");
    txtEnt->setQueryFlags(QFNaviPanel);
    txtEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(txtEnt->getName(), "local://lgpl-3.0.txt", 512, 512, true, 8, false));
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1TextNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(1, 1.5, 1));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoNavi1WebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFNaviPanel);
    std::string googleMtlName = NaviLibrary::NaviManager::Get().createNaviMaterial(knotEnt->getName(), "http://www.google.com", 512, 512, true, 8, false);
    MaterialPtr googleMtl = (MaterialPtr)MaterialManager::getSingletonPtr()->getByName(googleMtlName);
    googleMtl->setDepthWriteEnabled(true);
    knotEnt->setMaterialName(googleMtlName);
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1WebKnotNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(-3, 1.75, 4));
    node->setScale(Vector3(0.015f, 0.015f, 0.015f));
    node->attachObject(knotEnt);
}
#endif
#ifdef DEMO_NAVI2
//-------------------------------------------------------------------------------------
void Navigator::demoNavi2(const String url)
{
    static bool active = false;
    String url2go(url);
    if (url.length() == 0)
        url2go = "http://www.youtube.com/watch?v=mAJYQOANDCk";
//        "http://www.youtube.com/watch?v=066_q4DIeqk"
    if (active)
    {
        NaviLibrary::NaviManager::Get().navigateNaviTo("demoNavi2Video", url2go);
        return;
    }
    active = true;

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, 0);
    MeshManager::getSingleton().createPlane("demoNavi2Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 4, 4, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi2Video", "demoNavi2Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    vidEnt->setMaterialName(NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), url2go, 512, 512, true, 15, true, 0.75f));
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demo2VideoNode");
    videoNode->attachObject(vidEnt);
    videoNode->setPosition(Vector3(18, -55, 104.5));
    videoNode->yaw(Degree(180), Node::TS_WORLD);
}
#endif
#ifdef DEMO_PHYSICS1
//-------------------------------------------------------------------------------------
void Navigator::demoPhysics1()
{
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
#ifdef PHYSICS
    OgreOde::World* physicsWorld = mOgrePeerManager->getPhysicsWorld();
    if (physicsWorld == 0) return;

    OgreOde::Body* boxBody = 0;
    OgreOde::BoxGeometry* boxGeom = 0;
    std::map<String, SceneNode*>::iterator boxIt = boxes.find(boxName);
    if (boxIt != boxes.end())
    {
        boxNode = mSceneMgr->getSceneNode(boxNodeName);
        boxBody = (OgreOde::Body*)boxNode->getAttachedObject(boxName + "Bod");
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
        boxBody = new OgreOde::Body(physicsWorld, boxName + "Bod");
        boxBody->setMass(OgreOde::BoxMass(1, boxExtents));
        boxBody->setUserData(1);
        boxNode->attachObject(boxBody);
        boxGeom = new OgreOde::BoxGeometry(boxExtents, physicsWorld, physicsWorld->getDefaultSpace());
        boxGeom->setBody(boxBody);
        boxes[boxName] = boxNode;
    }
    boxBody->sleep();
    boxBody->setPosition(boxPos);
    boxBody->setLinearVelocity(Vector3::ZERO);
    boxBody->setAngularVelocity(Vector3::ZERO);
    boxBody->wake();
#elif PHYSX
    static std::map<String, NxActor*> actors;
    NxScene* physicsScene = mOgrePeerManager->getPhysicsScene();
    if (physicsScene == 0) return;

    NxActor* boxActor = 0;
    std::map<String, SceneNode*>::iterator boxIt = boxes.find(boxName);
    if (boxIt != boxes.end())
    {
        boxNode = mSceneMgr->getSceneNode(boxNodeName);
        boxEntity = (Entity*)boxNode->getAttachedObject(boxName + "Ent");
        boxActor = (actors.find(boxName))->second;
    }
    else
    {
        boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(boxNodeName);
        boxNode->setScale(boxScale);
        boxEntity = mSceneMgr->createEntity(boxName + "Ent", "cube.mesh");
        boxEntity->setMaterialName("2 - Default");
        boxNode->attachObject(boxEntity);
        NxBodyDesc boxBodyDesc;
        boxBodyDesc.setToDefault();
        boxBodyDesc.angularDamping = 0.5f;
        NxBoxShapeDesc boxShapeDesc;
        boxShapeDesc.dimensions = NxVec3(boxExtents.x/2, boxExtents.y/2, boxExtents.z/2);
        boxShapeDesc.localPose.t = NxVec3(0, 0, 0);
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&boxShapeDesc);
        actorDesc.body = &boxBodyDesc;
        actorDesc.density = 1.0f;
        boxActor = physicsScene->createActor(actorDesc);
        boxActor->userData = (void*)boxNode;
        physicsScene->setActorPairFlags(*(mOgrePeerManager->getPhysicsWorldActor()), *boxActor, NX_NOTIFY_ON_TOUCH);
        boxes[boxName] = boxNode;
        actors[boxName] = boxActor;
    }
    boxActor->setGlobalPosition(NxVec3(boxPos.x, boxPos.y, boxPos.z));
    boxActor->setLinearVelocity(NxVec3(0, 0, 0));
    boxActor->setAngularVelocity(NxVec3(0, 0, 0));
#elif TOKAMAK
    std::map<String, neRigidBody*>& bodies = mOgrePeerManager->getPhysicsBodies();
    neSimulator* physicsSim = mOgrePeerManager->getPhysicsSim();
    if (physicsSim == 0) return;

    neRigidBody* boxBody = 0;
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
        boxBody = physicsSim->CreateRigidBody();
        boxBody->SetMass(1.0f);
        neV3 boxExtentsTok;
        boxExtentsTok.Set(boxExtents.x, boxExtents.y, boxExtents.z);
        boxBody->SetInertiaTensor(neBoxInertiaTensor(boxExtentsTok, boxBody->GetMass()));
        neGeometry* boxGeom = boxBody->AddGeometry();
        boxGeom->SetBoxSize(boxExtentsTok);
        /*neT3 xform;
        xform.SetIdentity();
        xform.pos.Set(0, -boxExtents.y/2, 0);
        boxGeom->SetTransform(xform);*/
        boxGeom->SetMaterialIndex(0);
        boxBody->GravityEnable(true);
        boxBody->SetCollisionID(0);
        boxBody->SetUserData((u32)boxNode);
        boxes[boxName] = boxNode;
        bodies[boxName] = boxBody;
    }
    neV3 boxPosTok;
    boxPosTok.Set(boxPos.x, boxPos.y, boxPos.z);
    boxBody->SetPos(boxPosTok);
    neV3 velocity;
    velocity.SetZero();
    boxBody->SetVelocity(velocity);
#elif PHYSICSPLUGINS
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
#endif
}
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
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//            if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
            if ((it->movable != 0) &&
                ((it->movable->getMovableType().compare("Entity") == 0) ||
                (it->movable->getMovableType().compare("ManualObject") == 0)))
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
        for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeerManager->getOgrePeersIteratorBegin();ogrePeer != mOgrePeerManager->getOgrePeersIteratorEnd();ogrePeer++)
        {
            if (ogrePeer->second->getType().compare("avatar") != 0) continue;
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//            if (((Avatar*)ogrePeer->second)->getEntity() != static_cast<Entity*>(mPickedMovable)) continue;
            Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
            if (((Avatar*)ogrePeer->second)->getEntity() != pickedEntity) continue;
            avatar = (Avatar*)ogrePeer->second;
            OGRE_LOG("Navigator::is1AvatarHitByMouse() found Avatar movable=" + mPickedMovable->getName() + ", Peer:NetworkId=" + avatar->getPeer()->getNetworkId() + ", Peer:Login=" + avatar->getPeer()->getLogin());
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

    if (!mNaviSupported)
    {
        // Set avatar name
        getUserAvatar()->setName("mee");

        DebugHelpers::debugCommands["testEvt"] = "peerNew";

        // Call connect
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
        OGRE_LOG("Navigator::initPostOgreCore() Unable to load boot.lua, error: " + String(lua_tostring(mLuaState, -1)));
        return false;
    }
    if (lua_pcall(mLuaState, 0, LUA_MULTRET, 0))
    {
        OGRE_LOG("Navigator::initPostOgreCore() Unable to run boot.lua, error: " + String(lua_tostring(mLuaState, -1)));
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

    // Create OgrePeer manager
    mOgrePeerManager = new OgrePeerManager(mSceneMgr, this);

    // Create user avatar
    Peer* userLocalPeer = new Peer(String("User"), 0, 0, 0);
    if (!generateFromPeer(userLocalPeer))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load User avatar", "Navigator::createScene");

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
    SceneNode* camNode = mUserAvatar->getSceneNode()->createChildSceneNode("FirstPersonCamNode", Vector3(0, 0.95, 0)*avatarSize);
#ifdef LEXI
    if (entity->getMesh()->getName().find("salamandra") != String::npos)
        camNode->setPosition(Vector3(0, 1, 0)*avatarHalfSize);
#endif
    camNode->yaw(Radian(-Math::HALF_PI));
    SceneNode* pitchCamNode = camNode->createChildSceneNode("FirstPersonCamPitchNode");

    // Create the Third camera node/pitch node
    camNode = mUserAvatar->getSceneNode()->createChildSceneNode("ThirdPersonCamNode", Vector3(-4, 1.1, 0)*avatarSize.y);
#ifdef LEXI
    if (entity->getMesh()->getName().find("salamandra") != String::npos)
        camNode->setPosition(Vector3(0, -0.67, 0) + Vector3(-4, 1.1, 0)*avatarSize);
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
        std::list<Peer*> peersList;
        nodeResponse = mXmlRpcClient->getAllPeers(peersList);

        if (nodeResponse)
        {
            //Create avatars, set on the scene and store them
            for (std::list<Peer*>::iterator peer = peersList.begin();peer != peersList.end();++peer)
            {
                OGRE_LOG("create avatar for " + (*peer)->getLogin());
                if (!generateFromPeer(*peer))
                    return false;
            }
        }

        if (mNavigatorGUI != 0)
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
    std::list<Peer*> peersList;
    for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeerManager->getOgrePeersIteratorBegin();ogrePeer != mOgrePeerManager->getOgrePeersIteratorEnd();ogrePeer++)
    {
        if (ogrePeer->second->getType().compare("avatar") != 0) continue;
        if (ogrePeer->second->getPeer()->isLocal()) continue;
        peersList.push_back(ogrePeer->second->getPeer());
    }
    return mXmlRpcClient->sendMessage(message, peersList);
}

//-------------------------------------------------------------------------------------
bool Navigator::contextItemSelected(const String& item)
{
    OGRE_LOG("Navigator::contextItemSelected()");

    if (mNavigatorGUI == 0) return true;

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
    mOgrePeerManager->removeAll(false);
    if (cleanUpLocalPeers)
        mOgrePeerManager->removeAll(true);
}

//-------------------------------------------------------------------------------------
bool Navigator::generateFromPeer(Peer* peer)
{
#ifdef UIDEBUG
    if (mNavigatorGUI != 0)
        mNavigatorGUI->setTreeDirty(true);
#endif
    return mOgrePeerManager->load(peer, peer->getLogin() + ".xml");
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerNew(NodeEvent::DatasPeerNew& evtDatas)
{
    OGRE_LOG("Navigator::onPeerNew()");

    generateFromPeer(evtDatas.mPeer);
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerLost(NodeEvent::DatasPeerLost& evtDatas)
{
    OGRE_LOG("Navigator::onPeerLost()");

    if (!mOgrePeerManager->remove(evtDatas.mNetworkId, false))
        Exception(Exception::ERR_INTERNAL_ERROR, "Lost peer not found", "Navigator::onPeerLost");
}

//-------------------------------------------------------------------------------------
void Navigator::onStatusChanged(NodeEvent::DatasStatusChanged& evtDatas)
{
    OGRE_LOG("Navigator::onStatusChanged()");

    setNodeStatus(evtDatas.mStatus);
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

//-------------------------------------------------------------------------------------
bool Navigator::OnAvatarNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer)
{
    // User Avatar ?
    if (ogrePeer->getPeer()->isLocal())
    {
        mUserAvatar = (Avatar*)ogrePeer;
#ifdef LEXI
        if (mUserAvatar->getEntity()->getMesh()->getName().find("salamandra") != String::npos)
            mUserAvatar->getSceneNode()->setPosition(0, 0.67f, 0);
#else
        mUserAvatar->getSceneNode()->setPosition(17, -57, 115);
#endif
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::OnSceneNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer)
{

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::startModeling()
{
	mState = SModeling;
	// Init a new Modeler 
	if (!mModeler)
	{
		mModeler  = new Modeler(mSceneMgr,mCamera);
		mModeler->init(mUserAvatar);
	}
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endModeling()
{	
	// Go back in world
	mState = SInWorld;

	delete mModeler;
	mModeler = NULL;
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::createBox()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	//Quaternion plQuat = mUserAvatar->getSceneNode()->getOrientation();
	//Degree plangleDegree;
	//Vector3 plAxis;
	//plQuat.ToAngleAxis(plangleDegree,plAxis);
	//Vector3 plDir = /*plangleDegree.valueDegrees() * */ plAxis;

	//plDir.normalise();
	//
	//Vector3 boxPos = plpos + 10*plDir;
	return mModeler->createBox(plpos);
}


//-------------------------------------------------------------------------------------
bool Navigator::createCorner()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createCorner(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createPyramid()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createPyramid(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createPrism()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createPrism(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createCylinder()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createCylinder(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createHalfCyl()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createHalfCyl(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createCone()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createCone(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createHalfCone()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createHalfCone(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createSphere()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createSphere(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createHalfSphere()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createHalfSphere(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createTorus()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createTorus(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createTube()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createTube(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createRing()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createRing(plpos);
}
//-------------------------------------------------------------------------------------
bool Navigator::createMesh()
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	return mModeler->createMesh(plpos);
}
//-------------------------------------------------------------------------------------
