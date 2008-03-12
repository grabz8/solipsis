#include "Navigator.h"
#include "NavigatorFrameListener.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Navi.h"
#include "NaviLua.h"

#include "Modeler.h"

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
    mMaxNaviPickingDistance(10),
    mMaxAvatarPickingDistance(10),
    mRaySceneQuery(0),
    mPickedMovable(0),
    mUserAvatar(0),
    mModeler(0),
    isOnLeftCTRL(false)
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

    // Destroy XMLRPC client
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
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), "http://www.youtube.com/watch?v=066_q4DIeqk", 512, 512);
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
    //http://www.youtube.com/watch?v=ZQcUS4chhc4
    //http://fr.youtube.com/watch?v=u5WIEep8DJg
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1VideoNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(1, 1.5, 1));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoNavi1Text", "demoNavi1Plane");
    txtEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* txtNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(txtEnt->getName(), "local://lgpl-3.0.txt", 512, 512);
    txtNavi->show(true);
    txtNavi->setMaxUPS(8);
    txtEnt->setMaterialName(txtNavi->getMaterialName());
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1TextNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(1, 1.5, 1));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoNavi1WebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* knotNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(knotEnt->getName(), "http://www.google.com", 512, 512);
    knotNavi->show(true);
    knotNavi->setMaxUPS(8);
    std::string googleMtlName = knotNavi->getMaterialName();
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
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(vidEnt->getName(), url2go, 512, 512);
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
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
                if (mState != SModeling)
                    if (it->movable->getQueryFlags() == QFAvatar)
                    {
                        mPickedMovable = it->movable;
                        break;
                    }

				// modeler begin
				if (mState == SModeling )
				{
					String name = it->movable->getName();

					if(name.compare("Avatar") == 0) 
						continue;
					if (it->movable->getQueryFlags() == QFAvatar)
						continue;

					if (it->movable->getMovableType().compare("Entity") == 0)
						if( mModeler != 0 )
							if( !mModeler->isSelectionLocked() )
							{
								// Link mode ?
								if (mModeler->isInLinkMode())
								{
									//String name = it->movable->getName();
									Entity* ent = mSceneMgr->getEntity(name);
									Object3D * obj = mModeler->getSelection()->get3DObject( ent );
									if (obj != NULL)	//if it is a selectionnable object ...
										if( obj != mModeler->getSelection()->getFirstSelectedObject() ) //if it is not itself ...
										{
											if( ! obj->getShowBoundingBox()) //if this object is seleted :
											{
												if(obj->getParent() != NULL)
													obj = obj->getParent();
											}

											mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(false);
											mModeler->getSelection()->getFirstSelectedObject()->linkObject( obj, mSceneMgr);
											mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(true);
											mModeler->lockLinkMode(false);
										}
								}

								// Gizmo ?
								else if (mModeler->isOnGizmo())
								{
									// ...
									//isOnGizmo = false;
									//mModeler->deselectNode();
								}

								// Object3D ?
								//else //if (it->movable->getQueryFlags() == QFObject3D)
								else
								{
									if (!isOnLeftCTRL)
										mModeler->deselectNode();

									mModeler->lockGizmo(false);

									if(name.compare("Avatar") != 0)
									{
										mPickedMovable = it->movable;
										Entity* ent = mSceneMgr->getEntity(name);
										mModeler->selectNode(ent);
									}
								}

							}
				}
				// modeler end

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
    if (mState == SModeling)
        mModeler->lockLinkMode(false);

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
    NaviLibrary::NaviManager::Get().getNavi(naviName)->getExtents(naviWidth, naviHeight);
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
        for (OgrePeerManager::OgrePeersMap::iterator ogrePeer = mOgrePeerManager->getOgrePeersIteratorBegin();ogrePeer != mOgrePeerManager->getOgrePeersIteratorEnd();ogrePeer++)
        {
            if (ogrePeer->second->getXmlEntity()->getType() != ETAvatar) continue;
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//            if (((Avatar*)ogrePeer->second)->getEntity() != static_cast<Entity*>(mPickedMovable)) continue;
            Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
            if (((Avatar*)ogrePeer->second)->getEntity() != pickedEntity) continue;
            avatar = (Avatar*)ogrePeer->second;
            OGRE_LOG("Navigator::is1AvatarHitByMouse() found Avatar movable=" + mPickedMovable->getName() + ", Entity:Uid=" + avatar->getXmlEntity()->getUid() + ", Entity:Name=" + avatar->getEntity()->getName());
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
    mXmlRpcClient = new NavigatorXMLRPCClient(mHost, mPort, "");

    //Try connection
    XmlLogin xmlLogin("user", "demo");
    std::list<EntityUID> myEntities;
    bool nodeResponse = mXmlRpcClient->login(xmlLogin, myEntities);

    if (!nodeResponse)
    {
        //Failed..
        return false;
    }
    else
    {
        // Set my entitys into OgrePeer manager
        mOgrePeerManager->setMyEntities(myEntities);

        // Connected !

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

        return true;
    }
}

//-------------------------------------------------------------------------------------
bool Navigator::sendMessage(const String& message)
{
    if (mXmlRpcClient == 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send message without XMLRPC client", "Navigator::sendMessage");

    XmlEvt xmlEvt(ETActionOnEntity);
    std::string xmlResp;
    return mXmlRpcClient->sendEvt(xmlEvt, xmlResp);
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
void Navigator::onPeerNew(XmlEntity* xmlEntity)
{
    OGRE_LOG("Navigator::onPeerNew() uid:" + StringConverter::toString(xmlEntity->getUid()));

#ifdef UIDEBUG
    if (mNavigatorGUI != 0)
        mNavigatorGUI->setTreeDirty(true);
#endif
    if (!mOgrePeerManager->load(xmlEntity))
        OGRE_LOG("Navigator::onPeerNew() Unable to load entity !");
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerLost(XmlEntity* xmlEntity)
{
    OGRE_LOG("Navigator::onPeerLost() uid:" + StringConverter::toString(xmlEntity->getUid()));

    if (!mOgrePeerManager->remove(xmlEntity->getUid(), false))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to remove lost peer !", "Navigator::onPeerLost");
}

//-------------------------------------------------------------------------------------
void Navigator::onPeerUpdated(XmlEntity* xmlEntity)
{
//    OGRE_LOG("Navigator::onPeerUpdated()");

    if (!mOgrePeerManager->update(xmlEntity))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to update peer !", "Navigator::onPeerUpdated");
}

//-------------------------------------------------------------------------------------
void Navigator::processEvents()
{
//    OGRE_LOG("Navigator::processEvents()");

    // Process each event
    std::list<XmlEvt*>* nodeEvents = beginProcessEvents();
    for (std::list<XmlEvt*>::iterator evt = nodeEvents->begin();evt != nodeEvents->end();++evt)
    {
        switch ((*evt)->getType())
        {
        case ETNewEntity:
            onPeerNew((XmlEntity*)((*evt)->getDatas()));
            break;
        case ETLostEntity:
            onPeerLost((XmlEntity*)((*evt)->getDatas()));
            break;
        case ETUpdatedEntity:
            onPeerUpdated((XmlEntity*)((*evt)->getDatas()));
            break;
        default: // Caller already check type consistency
            break;
        }
        delete (*evt);
    }
    endProcessEvents();
}

//-------------------------------------------------------------------------------------
void Navigator::sendEvents()
{
//    OGRE_LOG("Navigator::sendEvents()");

    if (mXmlRpcClient == 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send events without XMLRPC client", "Navigator::sendEvents");

    // Send each event
    std::string xmlResp;
    OgrePeerManager::EvtsList& evtsList = mOgrePeerManager->getEvtsToSendList();
    for (OgrePeerManager::EvtsList::iterator it = evtsList.begin(); it != evtsList.end(); ++it)
    {
        if (!mXmlRpcClient->sendEvt(*it, xmlResp))
            OGRE_LOG("Navigator::sendEvents() Unable to send event !");
    }
    evtsList.clear();
}

//-------------------------------------------------------------------------------------
bool Navigator::OnAvatarNodeCreate(TiXmlElement* xmlElt, OgrePeer* ogrePeer)
{
    // User Avatar ?
    if (ogrePeer->isLocal())
    {
        mUserAvatar = (Avatar*)ogrePeer;

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
        camNode->yaw(Radian(-Math::HALF_PI));
        SceneNode* pitchCamNode = camNode->createChildSceneNode("FirstPersonCamPitchNode");

        // Create the Third camera node/pitch node
        camNode = mUserAvatar->getSceneNode()->createChildSceneNode("ThirdPersonCamNode", Vector3(-4, 1.1, 0)*avatarSize.y);
        camNode->yaw(Radian(-Math::HALF_PI));
        pitchCamNode = camNode->createChildSceneNode("ThirdPersonCamPitchNode");

        // set Third person camera
        ((NavigatorFrameListener*)mFrameListener)->setCameraMode(NavigatorFrameListener::CM3rdPerson);
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
		//mModeler = new Modeler(mSceneMgr,mCamera);
		mModeler = Modeler::getSingletonPtr(mSceneMgr,mCamera);
		mModeler->init(mUserAvatar);
	}
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endModeling()
{	
	// Go back in world
	mState = SInWorld;

	if( mModeler )
		mModeler->deselectNode();

// TODO : remove those comments
//	delete mModeler;
//	mModeler = NULL;
    return true;
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

	return mModeler->createBox(plpos + dep);
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

	return mModeler->createCorner(plpos + dep);
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

	return mModeler->createPyramid(plpos + dep);
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

	return mModeler->createPrism(plpos + dep);
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

	return mModeler->createCylinder(plpos + dep);
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

	return mModeler->createHalfCyl(plpos + dep);
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

	return mModeler->createCone(plpos + dep);
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

	return mModeler->createHalfCone(plpos + dep);
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

	return mModeler->createSphere(plpos + dep);
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

	return mModeler->createHalfSphere(plpos + dep);
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

	return mModeler->createTorus(plpos + dep);
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

	return mModeler->createTube(plpos + dep);
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

	return mModeler->createRing(plpos + dep);
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

	return mModeler->createMesh(plpos + dep);
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseMoved(const MouseEvt& evt)
{
	if ( mModeler )
		if (!mModeler->isSelectionEmpty())
		{
			//mModeler->getSelection()->mTransformation->drapNdrop(...);
		}
}

//-------------------------------------------------------------------------------------
void Navigator::onMousePressed(const MouseEvt& evt)
{
	if ( mModeler )
		if (!mModeler->isSelectionEmpty())
		{
			//mModeler->getSelection()->mTransformation->firstClickForTransformation(...);
		}
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseReleased(const MouseEvt& evt)
{
	if ( mModeler )
		if (!mModeler->isSelectionEmpty())
		{
			//mModeler->getSelection()->mTransformation->releasedClickForTransformation();
			//isOnGizmo = false; // a deplacer dans le mousePressed lorsque l'on click sur autre chose qu'un GIZMO
		}
}

//-------------------------------------------------------------------------------------
void Navigator::undo()
{
	if( mModeler )
		if( !mModeler->isSelectionEmpty() )
		{
			mModeler->getSelected()->undo();
			// TODO : reinitialize the deformer sliders
		}
}

//-------------------------------------------------------------------------------------
void Navigator::suppr()
{
	if( mModeler )
		if( !mModeler->isSelectionEmpty() )
		{
			// remove the current selection
			mModeler->removeSelection();

			// hide the gizmos axes
			mModeler->getSelection()->mTransformation->showGizmosMove(false);
			mModeler->getSelection()->mTransformation->showGizmosRotate(false);
			mModeler->getSelection()->mTransformation->showGizmosScale(false);
		}
}

//-------------------------------------------------------------------------------------
void Navigator::modifGizmo(Vector3 dep)
{
	SceneNode* node = mSceneMgr->getSceneNode("NodeSelection");
	static Vector3 scale = Vector3(1,1,1);

	switch(mModeler->getSelection()->mTransformation->getMode())
	{
	case Transformations::MOVE:
		mModeler->updateCommand( Object3D::TRANSLATE, mModeler->getSelected() );
		mModeler->getSelection()->move(dep.x, dep.y, dep.z);
		node->translate(dep);
		break;
	case Transformations::ROTATE:
		mModeler->updateCommand( Object3D::ROTATE, mModeler->getSelected() );
		dep *= 10;
		mModeler->getSelection()->rotate(dep.x, dep.y, dep.z);
		break;
	case Transformations::SCALE:
		mModeler->updateCommand( Object3D::SCALE, mModeler->getSelected() );
		scale += dep;
		mModeler->getSelection()->scale(scale.x, scale.y, scale.z);
		break;
	case Transformations::SELECT:
		break;
	}
}

//-------------------------------------------------------------------------------------
bool Navigator::XMLLoad()
{
	if( mModeler )
	{
		Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
		Radian angle = pldir.getYaw();
		Ogre::Vector3 dep = Vector3(1.5,0,0);

		Real cosY = Math::Cos(angle);
		Real sinY = Math::Sin(angle);

		Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
		//y = point.y;							//		y' = y  
		dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
		dep.x = x;

		return mModeler->XMLLoad( mUserAvatar->getSceneNode()->getPosition() + dep );
	}

	return false;
}
//-------------------------------------------------------------------------------------
bool Navigator::XMLSave(bool all, const char* pathToSave)
{
	if( mModeler )
		if(all || !mModeler->isSelectionEmpty()) 
			return mModeler->XMLSave(all, pathToSave);
		else
#ifdef WIN32
			MessageBox(NULL,"You have to select an object3D","Information",MB_OK | MB_ICONINFORMATION); 
#else
			std::cerr << " You have to select an object3D " << std::endl;
#endif

	return false;
}

//-------------------------------------------------------------------------------------
