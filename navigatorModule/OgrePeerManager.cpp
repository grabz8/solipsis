#include "OgrePeerManager.h"
#include "Avatar.h"
#include "Scene.h"
#include "OgreOSMScene.h"
#include "Navigator.h"
#include "OgreHelpers.h"

#include "Modeler.h"

using namespace Solipsis;

// this internal OSM-loader callbacks class is used to force OFF shadows casting of entities
class OgrePeerManagerOSMSceneCallbacks : public OSMSceneCallbacks
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

//-------------------------------------------------------------------------------------
OgrePeerManager::OgrePeerManager(SceneManager* sceneMgr, IOgrePeerManagerCallbacks* callbacks) :
    mSceneMgr(sceneMgr),
    mCallbacks(callbacks)
#ifdef PHYSICS
    ,mPhysicsWorld(0),
    mPhysicsStepHandler(0),
    mPhysicsWorldGeometry(0)
#elif PHYSX
    ,mPhysicsScene(0),
    mControllerManager(0),
    mPhysicsWorldGeometry(0),
    mPhysicsWorldActor(0)
#elif TOKAMAK
    ,mPhysicsSim(0)
#elif PHYSICSPLUGINS
    ,mPhysicsScene(0)
#endif
{
}

//-------------------------------------------------------------------------------------
OgrePeerManager::~OgrePeerManager()
{
#ifdef PHYSICS
    // Destroy the physical world
    delete mPhysicsWorldGeometry;
    delete mPhysicsStepHandler;
    delete mPhysicsWorld;
#elif PHYSX
    if (PhysXHelpers::getPhysicsSDK() != 0)
    {
        delete mControllerManager;
        if (mPhysicsScene != 0)
        {
            NxActor** actors = mPhysicsScene->getActors();
            for (NxU32 a=0; a<mPhysicsScene->getNbActors(); ++a)
            {
                NxActor* actor = actors[a];
                mPhysicsScene->releaseActor(*actor);
            }
            PhysXHelpers::getPhysicsSDK()->releaseScene(*mPhysicsScene);
        }
        PhysXHelpers::shutdown();
    }
#elif TOKAMAK
    if (mPhysicsSim != 0)
        neSimulator::DestroySimulator(mPhysicsSim);
#elif PHYSICSPLUGINS
    if (mPhysicsScene != 0)
        PhysicsEngineManager::getSingleton().getSelectedEngine()->destroyScene(mPhysicsScene);
#endif
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::load(Peer* peer, const String xmlFile)
{
    class TiXmlDocumentPtr : public Ogre::SharedPtr<TiXmlDocument> {
    public:
	    TiXmlDocumentPtr() : Ogre::SharedPtr<TiXmlDocument>() {}	
	    explicit TiXmlDocumentPtr(TiXmlDocument* rep) : Ogre::SharedPtr<TiXmlDocument>(rep) {}
	    TiXmlDocumentPtr(const TiXmlDocumentPtr& r) : Ogre::SharedPtr<TiXmlDocument>(r) {} 
    };

	// Create new XML document
    TiXmlDocumentPtr xmlDoc = TiXmlDocumentPtr(new TiXmlDocument());

    // Open XML file and parse it
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(xmlFile);
    if (!stream->size())
    {
        OGRE_LOG("OgrePeerManager::load() Empty peer XML file " + xmlFile);
        return false;
    }
    size_t size = stream->size();
    char *buf = new char[size + 1];
    memset(buf, 0, size + 1);
    stream->read(buf, size);
    stream.setNull();
    xmlDoc->Parse(buf);
    delete[] buf;

    // Check for errors
    if (xmlDoc->Error())
    {
        OGRE_LOG("OgrePeerManager::load() Failed to load peer XML file " + xmlFile + ", " + String(xmlDoc->ErrorDesc()));
        return false;
    }

    // Process elements
    TiXmlElement* peerDatasElt = xmlDoc->FirstChildElement("peerDatas");
    if (peerDatasElt == 0)
    {
        OGRE_LOG("OgrePeerManager::load() Malformed peer XML file " + xmlFile);
        return false;
    }
    for (TiXmlElement* elt = peerDatasElt->FirstChildElement(); elt != 0; elt = elt->NextSiblingElement())
    {
        OgrePeer* newOgrePeer = 0;
        if (String(elt->Value()).compare("avatarNode") == 0)
        {
            newOgrePeer = createAvatarNode(peer, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load avatarNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        else if (String(elt->Value()).compare("sceneNode") == 0)
        {
			xmlObjectFilename = xmlFile;
            newOgrePeer = createSceneNode(peer, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load sceneNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
		else if (String(elt->Value()).compare("objectNode") == 0)
        {
			//xmlObjectFilename = xmlFile;
            newOgrePeer = createObjectNode(peer, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load objectNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        if (newOgrePeer != 0)
            mOgrePeersMap[peer->getNetworkId()] = newOgrePeer;
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string OgrePeerManager::getXmlObjectFilename()
{
	return xmlObjectFilename;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::remove(String& peerId, bool local)
{
    bool peerFound = false;
    for (std::map<String,OgrePeer*>::iterator ogrePeer=mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
    {
        if (ogrePeer->second->getPeer()->isLocal() != local) continue;
        if (ogrePeer->second->getPeer()->getNetworkId().compare(peerId) == 0)
        {
            delete ogrePeer->second->getPeer();
            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::removeAll(bool local)
{
    bool loopAgain;

    loopAgain = true;
    while (loopAgain)
    {
        loopAgain = false;
        for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
        {
            if (ogrePeer->second->getPeer()->isLocal() != local) continue;
            delete ogrePeer->second->getPeer();
            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            loopAgain = true;
            break;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::frameStarted(const FrameEvent& evt)
{
    for (std::map<String,OgrePeer*>::iterator ogrePeer = mOgrePeersMap.begin();ogrePeer != mOgrePeersMap.end();ogrePeer++)
    {
        if ((ogrePeer->second->getType().compare("avatar") == 0) && ogrePeer->second->getPeer()->isLocal())
        {
            Avatar* avatar = (Avatar*)ogrePeer->second;
#ifdef PHYSICS
            // Create avatar's world ray
            // Simulate entry of local avatars into the physical world of the current scene
            if ((mPhysicsWorld != 0) && (avatar->getPhysicsWorld() == 0))
            {
                avatar->createPhysics(mPhysicsWorld, mPhysicsWorldGeometry);
                avatar->setMaxUpdateTimeStep(1.0f/60.0f);
                avatar->setGravity(true);
            }
#elif PHYSX
            // Simulate entry of local avatars into the physical world of the current scene
            if ((mPhysicsScene != 0) && (avatar->getPhysicsScene() == 0))
            {
                avatar->createPhysics(mPhysicsScene, mControllerManager);
                avatar->setGravity(true);
            }
#elif PHYSICSPLUGINS
            // Simulate entry of local avatars into the physics scene
            if ((mPhysicsScene != 0) && (avatar->getPhysicsScene() == 0))
            {
                avatar->createPhysics(mPhysicsScene);
                avatar->setGravity(true);
            }
#endif
        }
    }

#ifdef PHYSICS
    // Step physics
    if (mPhysicsStepHandler != 0)
        mPhysicsStepHandler->step(evt.timeSinceLastFrame);
    if (mPhysicsWorld != 0)
        mPhysicsWorld->synchronise();
#elif PHYSX
    // Step physics part 1
    if (mPhysicsScene != 0)
    {
//        mPhysicsScene->simulate(evt.timeSinceLastFrame);
//        mPhysicsScene->flushStream();
//        mPhysicsScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
        // To avoid blocking on fetchResults(), we will shift actions
        // in order to let simulation threading during the rendering
        mPhysicsScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
        NxReal maxTimestep;
        NxTimeStepMethod method;
        NxU32 maxIter;
        NxU32 numSubSteps;
        mPhysicsScene->getTiming(maxTimestep, maxIter, method, &numSubSteps);
        if (numSubSteps) mControllerManager->updateControllers();
        NxActor** actors = mPhysicsScene->getActors();
        for (NxU32 a=0; a<mPhysicsScene->getNbActors(); ++a)
        {
            NxActor* actor = actors[a];
            if (actor->userData == 0) continue;
            SceneNode* node = (SceneNode*)actor->userData;
            NxVec3 pos = actor->getGlobalPosition();
            node->setPosition(pos.x, pos.y, pos.z);
            NxQuat orientation = actor->getGlobalOrientationQuat();
            node->setOrientation(orientation.w, orientation.x, orientation.y, orientation.z);
        }
    }
#elif TOKAMAK
    // Step physics
    if (mPhysicsSim != 0)
    {
        Real maxUpdateTimeStep = 1.0f/60.0f;
        Real timeSinceLastFrame = evt.timeSinceLastFrame;
        Real totalTime = 0.0;
        for (;totalTime < timeSinceLastFrame - maxUpdateTimeStep; totalTime += maxUpdateTimeStep)
            mPhysicsSim->Advance(maxUpdateTimeStep);
        // last step
        timeSinceLastFrame -= totalTime;
        mPhysicsSim->Advance(timeSinceLastFrame);
        for (std::map<String, neRigidBody*>::iterator it = mPhysicsBodies.begin();it != mPhysicsBodies.end(); ++it)
        {
            neRigidBody* body = it->second;
            if (body->GetUserData() == 0) continue;
            SceneNode* node = (SceneNode*)body->GetUserData();
            neV3 pos = body->GetPos();
            node->setPosition(pos.X(), pos.Y(), pos.Z());
            neQ orientation = body->GetRotationQ();
            node->setOrientation(orientation.W, orientation.X, orientation.Y, orientation.Z);
        }
    }
#elif PHYSICSPLUGINS
    // Step physics part 1
    if (mPhysicsScene != 0)
        mPhysicsScene->preStep(evt.timeSinceLastFrame);
#endif

    // Animate
    // Peers' avatars
    for (std::map<String,OgrePeer*>::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
        it->second->update(evt.timeSinceLastFrame);

#ifdef PHYSX
    // Step physics part 2
    if (mPhysicsScene != 0)
    {
        mPhysicsScene->simulate(evt.timeSinceLastFrame);
        mPhysicsScene->flushStream();
    }
#elif PHYSICSPLUGINS
    // Step physics part 2
    if (mPhysicsScene != 0)
        mPhysicsScene->postStep();
#endif

    return true;
}

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
OgreOde::World* OgrePeerManager::getPhysicsWorld()
{
    return mPhysicsWorld;
}

//-------------------------------------------------------------------------------------
OgreOde::StepHandler* OgrePeerManager::getPhysicsStepHandler()
{
    return mPhysicsStepHandler;
}

//-------------------------------------------------------------------------------------
OgreOde::TriangleMeshGeometry* OgrePeerManager::getPhysicsWorldGeometry()
{
    return mPhysicsWorldGeometry;
}
#elif PHYSX
//-------------------------------------------------------------------------------------
NxScene* OgrePeerManager::getPhysicsScene()
{
    return mPhysicsScene;
}

//-------------------------------------------------------------------------------------
::ControllerManager* OgrePeerManager::getControllerManager()
{
    return mControllerManager;
}

//-------------------------------------------------------------------------------------
NxTriangleMesh* OgrePeerManager::getPhysicsWorldGeometry()
{
    return mPhysicsWorldGeometry;
}

//-------------------------------------------------------------------------------------
NxActor* OgrePeerManager::getPhysicsWorldActor()
{
    return mPhysicsWorldActor;
}
#elif TOKAMAK
//-------------------------------------------------------------------------------------
neSimulator* OgrePeerManager::getPhysicsSim()
{
    return mPhysicsSim;
}

//-------------------------------------------------------------------------------------
neTriangleMesh& OgrePeerManager::getPhysicsWorldGeometry()
{
    return mPhysicsWorldGeometry;
}

//-------------------------------------------------------------------------------------
std::map<String, neRigidBody*>& OgrePeerManager::getPhysicsBodies()
{
    return mPhysicsBodies;
}
#elif PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* OgrePeerManager::getPhysicsScene()
{
    return mPhysicsScene;
}
#endif

//-------------------------------------------------------------------------------------
OgrePeer* OgrePeerManager::createAvatarNode(Peer* peer, TiXmlElement* xmlElt)
{
    if (mSceneMgr == 0)
        Exception(Exception::ERR_INTERNAL_ERROR,
            "No scene manager !",
            "OgrePeerManager::CreateAvatarNode");

    const char* name = xmlElt->Attribute("name");
    const char* meshFilename = xmlElt->Attribute("meshFilename");
    const char* skeletonFilename = xmlElt->Attribute("skeletonFilename");
    if ((name == 0) || (meshFilename == 0) || (skeletonFilename == 0))
        return false;
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(peer->getNetworkId() + "Avatar");
    Entity* entity = mSceneMgr->createEntity(peer->getNetworkId() + "Avatar", meshFilename);
/* simple test about color picking, bind 1 unique color to each pickable entity, set 1 flag when
   picking is expected, switch material of pickable entities, render into 1 picking texture, switch
   back materials and finally get the entity according to the picked color value */
/*    entity->setMaterialName("Solipsis/ColorPicking");
    SubEntity* subEntity = entity->getSubEntity(0);
    subEntity->setCustomParameter(1, Vector4(0.0f, 1.0f, 0.0f, 0.0f));*/
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//    entity->setQueryFlags(Navigator::QFAvatar);
    ManualObject* selectionObject = new ManualObject(peer->getNetworkId() + "Sel");
    AxisAlignedBox entityBbox = entity->getBoundingBox();
    AxisAlignedBox selectionBbox;
    selectionBbox.setExtents(entityBbox.getCenter() - entityBbox.getHalfSize()*0.5f, entityBbox.getCenter() + entityBbox.getHalfSize()*0.5f);
    selectionObject->setBoundingBox(selectionBbox);
    selectionObject->setQueryFlags(Navigator::QFAvatar);
    node->attachObject(selectionObject);
#ifdef SHADOWS
    entity->setCastShadows(true);
#endif

    //TODO : need to set correct position         
    node->setPosition(peer->getFakeX(),peer->getFakeZ(),peer->getFakeY()); //careful to switch y and z !
#ifdef LEXI
    if (peer->getLogin().find("salamandra") != String::npos)
        node->setPosition(0, 0.67f, 7.14f);
#else
        node->setPosition(17, -56.9f, 120);
#endif

    Avatar* peerAvatar = new Avatar(peer, node, entity);
    peerAvatar->setGravity(false);
    peerAvatar->setStateAnimName(Avatar::SWalk, "Walk");
    peerAvatar->setStateAnimName(Avatar::SRun, "Run");
    peerAvatar->setStateAnimName(Avatar::SFly, "Fly");
    peerAvatar->setStateAnimName(Avatar::SSwim, "Swim");
    peerAvatar->setState(Avatar::SIdle);

    if (mCallbacks != 0)
        if (!mCallbacks->OnAvatarNodeCreate(xmlElt, peerAvatar))
        {
            delete peerAvatar;
            return 0;
        }

    return peerAvatar;
}

//-------------------------------------------------------------------------------------
OgrePeer* OgrePeerManager::createSceneNode(Peer* peer, TiXmlElement* xmlElt)
{
    if (mSceneMgr == 0)
        Exception(Exception::ERR_INTERNAL_ERROR,
            "No scene manager !",
            "OgrePeerManager::CreateSceneNode");

    const char* name = xmlElt->Attribute("name");
    const char* filename = xmlElt->Attribute("filename");
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(peer->getNetworkId() + "Scene");
    OSMScene osmScene(mSceneMgr);
    OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
    if (!osmScene.initialise(filename, &osmSceneCallbacks))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(filename), "OgrePeerManager::createSceneNode");
    osmScene.declareResources();
    if (!osmScene.createScene(node))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(filename), "OgrePeerManager::createSceneNode");

#ifdef SHADOWS
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
    mSceneMgr->setShadowTextureSettings(512, 1, PixelFormat::PF_A4R4G4B4);
    Ogre::SharedPtr<LiSPSMShadowCameraSetup> shadowCameraSetup = Ogre::SharedPtr<LiSPSMShadowCameraSetup>(new LiSPSMShadowCameraSetup());
    mSceneMgr->setShadowCameraSetup(shadowCameraSetup);
#endif

    //TODO : need to set correct position         
//    node->setPosition(peer->getFakeX(),peer->getFakeZ(),peer->getFakeY()); //careful to switch y and z !
    node->setPosition(18,-58,133);

#ifdef PHYSICS
    // Create the physical world
    mPhysicsWorld = new OgreOde::World(mSceneMgr);
    mPhysicsWorld->setGravity(Vector3(0, -9.80665f, 0));
    mPhysicsWorld->setCFM(10e-5f);
    mPhysicsWorld->setERP(0.8f);
    mPhysicsWorld->setAutoSleep(true);
    mPhysicsWorld->setContactCorrectionVelocity(0.1f);
    mPhysicsWorld->setContactSurfaceLayer(0.01f);
    mPhysicsWorld->setAutoSleepLinearThreshold(1.0f);
    mPhysicsWorld->setAutoSleepAngularThreshold(1.0f);
    mPhysicsWorld->setCollisionListener(dynamic_cast<OgreOde::CollisionListener*>(this));
    // Create something that will step the world, but don't do it automatically
    mPhysicsStepHandler = new OgreOde::ForwardFixedStepHandler(mPhysicsWorld, OgreOde::StepHandler::QuickStep, Real(1.0/60.0), Real(1000.0), Real(1.0));
    mPhysicsStepHandler->setAutomatic(OgreOde::StepHandler::AutoMode_NotAutomatic, Root::getSingletonPtr());
    // Create the world collision mesh
    Entity* worldCollisionEntity = mSceneMgr->getEntity("MC_station");
    SceneNode* worldCollisionSceneNode = mSceneMgr->getSceneNode("MC_station");
    OgreOde::EntityInformer entityInformer(worldCollisionEntity, worldCollisionSceneNode->_getFullTransform());
    mPhysicsWorldGeometry = entityInformer.createStaticTriangleMesh(mPhysicsWorld, mPhysicsWorld->getDefaultSpace());
    worldCollisionSceneNode->setVisible(false);
#elif PHYSX
    // Init the SDK
    PhysXHelpers::init();
    // Create the physical world
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = NxVec3(0, -9.80665f, 0);
    mPhysicsScene = PhysXHelpers::getPhysicsSDK()->createScene(sceneDesc);
    if (mPhysicsScene == 0)
    {
        Exception(Exception::ERR_INTERNAL_ERROR,
            "Unable to create the PhysX scene !",
            "OgrePeerManager::CreateSceneNode");
    }
    mPhysicsScene->setTiming(1.0f/60.0f, 8, NX_TIMESTEP_FIXED);
    // Set the default material 0
	NxMaterial* defaultMaterial = mPhysicsScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
    // Create the world collision mesh
    Entity* worldCollisionEntity = mSceneMgr->getEntity("MC_station");
    SceneNode* worldCollisionSceneNode = mSceneMgr->getSceneNode("MC_station");
    mPhysicsWorldGeometry = PhysXHelpers::cookMesh(worldCollisionEntity->getMesh(),
                                                   worldCollisionEntity->getParentNode()->getWorldPosition(),
                                                   worldCollisionEntity->getParentNode()->getWorldOrientation(),
                                                   worldCollisionEntity->getParentNode()->getScale());
    NxTriangleMeshShapeDesc triangleMeshShapeDesc;
    NxActorDesc actorDesc;
    triangleMeshShapeDesc.meshData = mPhysicsWorldGeometry;
    triangleMeshShapeDesc.group = PhysXHelpers::CG_COLLIDABLE_NON_PUSHABLE;
    actorDesc.shapes.pushBack(&triangleMeshShapeDesc);
    mPhysicsWorldActor = mPhysicsScene->createActor(actorDesc);
    mPhysicsWorldActor->userData = (void*)0;
    worldCollisionSceneNode->setVisible(false);
    mControllerManager = new ::ControllerManager();
#elif TOKAMAK
    // Create the physical world
    neSimulatorSizeInfo simSizeInfo;
    neV3 gravity;
    gravity.Set(0, -9.80665f, 0);
    mPhysicsSim = neSimulator::CreateSimulator(simSizeInfo, NULL, &gravity);
    if (mPhysicsSim == 0)
    {
        Exception(Exception::ERR_INTERNAL_ERROR,
            "Unable to create the Tokamak simulation !",
            "OgrePeerManager::CreateSceneNode");
    }
    // Set the default material 0
    mPhysicsSim->SetMaterial(0, 0.5f, 0.1f);
    // Create the world collision mesh
    Entity* worldCollisionEntity = mSceneMgr->getEntity("MC_station");
    SceneNode* worldCollisionSceneNode = mSceneMgr->getSceneNode("MC_station");
    mPhysicsWorldGeometry = TokamakHelpers::convertMesh(worldCollisionEntity->getMesh(),
                                                        worldCollisionEntity->getParentNode()->getWorldPosition(),
                                                        worldCollisionEntity->getParentNode()->getWorldOrientation(),
                                                        worldCollisionEntity->getParentNode()->getScale());
    mPhysicsSim->SetTerrainMesh(&mPhysicsWorldGeometry);
    worldCollisionSceneNode->setVisible(false);
#elif PHYSICSPLUGINS
    IPhysicsEngine* engine = PhysicsEngineManager::getSingleton().getSelectedEngine();
    // Create the physical scene
    mPhysicsScene = engine->createScene();
    if (!mPhysicsScene->create(mSceneMgr))
        Exception(Exception::ERR_INTERNAL_ERROR,
        "Unable to create the PhysX scene !",
        "PhysXScene::PhysXScene");
    // Create the scene collision mesh
    Entity* worldCollisionEntity = mSceneMgr->getEntity("MC_station");
    mPhysicsScene->setTerrainMesh(*worldCollisionEntity);
    worldCollisionEntity->getParentSceneNode()->setVisible(false);
#else
    // Destroy collision mesh
    mSceneMgr->destroySceneNode("MC_station");
#endif

    Scene* peerScene = new Scene(peer, node);

    if (mCallbacks != 0)
        if (!mCallbacks->OnSceneNodeCreate(xmlElt, peerScene))
        {
            delete peerScene;
            return 0;
        }

    return peerScene;
}

//-------------------------------------------------------------------------------------
OgrePeer* OgrePeerManager::createObjectNode(Peer* peer, TiXmlElement* xmlElt)
{
	if (mSceneMgr == 0)
		Exception(Exception::ERR_INTERNAL_ERROR,
		"No scene manager !",
		"OgrePeerManager::CreateObjectNode");

	const char* name = xmlElt->Attribute("name");
	const char* filename = xmlElt->Attribute("filename");

	Modeler* modeler = Modeler::getSingletonPtr( mSceneMgr, NULL );
	if(modeler)
	{
		modeler->init( NULL );
		modeler->XMLLoad( Vector3::ZERO, filename );
	}

	return NULL;
}

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
bool OgrePeerManager::collision(OgreOde::Contact* contact)
{
    /*
    we have 2 collidable objects from our object system, if one of the Collide function returns false, we return false in this method, too,
    else we return true, so ode computes a normal collision.
    true means ode will treat this like a normal collison => rigid body behavior
    false means ode will not treat this collision at all => objects ignore each other
    */
    // Check for collisions between things that are connected and ignore them
/*    OgreOde::Geometry * const g1 = contact->getFirstGeometry();
    OgreOde::Geometry * const g2 = contact->getSecondGeometry();
    if (g1 && g2)
    {
        const OgreOde::Body * const b1 = g1->getBody();
        const OgreOde::Body * const b2 = g2->getBody();
        if (b1 && b2 && OgreOde::Joint::areConnected(b1, b2)) 
            return false; 
    }*/

    contact->setCoulombFriction(0.9f);
    contact->setBouncyness(0.25f);
    contact->setSoftness(0.8f, 10e-5f);

    OgreOde::Geometry * const g1 = contact->getFirstGeometry();
    OgreOde::Geometry * const g2 = contact->getSecondGeometry();
    if ((g1 != mPhysicsWorldGeometry) && (g2 != mPhysicsWorldGeometry))
    {
        OgreOde::Body * const b1 = g1->getBody();
        OgreOde::Body * const b2 = g2->getBody();
        if ((b1->getUserData() == 1) && (b2->getUserData() == 2))
            b1->addForceAt(contact->getNormal()*contact->getPenetrationDepth()*981, contact->getPosition());
        if ((b1->getUserData() == 2) && (b2->getUserData() == 1))
            b2->addForceAt(contact->getNormal()*contact->getPenetrationDepth()*981, contact->getPosition());
    }
/*    OgreOde::Body * const b1 = g1->getBody();
    OgreOde::Body * const b2 = g2->getBody();
    if (b1 != 0) b1->wake();
    if (b2 != 0) b2->wake();*/

    return true;
}
#endif

//-------------------------------------------------------------------------------------
