#include "OgrePeerManager.h"
#include "Avatar.h"
#include "Scene.h"
#include "OgreOSMScene.h"
#include "Navigator.h"
#include "OgreHelpers.h"

using namespace Ogre;

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

OgrePeerManager::OgrePeerManager(SceneManager* sceneMgr, IOgrePeerManagerCallbacks* callbacks) :
    mSceneMgr(sceneMgr),
    mCallbacks(callbacks)
#ifdef PHYSICS
    ,mPhysicsWorld(0),
    mPhysicsStepHandler(0),
    mPhysicsWorldGeometry(0)
#endif
{
}

OgrePeerManager::~OgrePeerManager()
{
#ifdef PHYSICS
    // Destroy the physical world
    delete mPhysicsWorldGeometry;
    delete mPhysicsStepHandler;
    delete mPhysicsWorld;
#endif
}

bool OgrePeerManager::load(Peer* peer, const Ogre::String xmlFile)
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
            newOgrePeer = createSceneNode(peer, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load sceneNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        if (newOgrePeer != 0)
            mOgrePeersMap[peer->getNetworkId()] = newOgrePeer;
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::remove(Ogre::String& peerId, bool local)
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
#ifdef CAPSULEGEOM
                avatar->setMaxUpdateTimeStep(1.0/60.0);
#endif
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
#endif

    // Animate
    // Peers' avatars
    for (std::map<String,OgrePeer*>::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
        it->second->update(evt.timeSinceLastFrame);

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
    entity->setQueryFlags(Navigator::QFAvatar);
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
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(filename), "Navigator::generateSceneFromPeer");
    osmScene.declareResources();
    if (!osmScene.createScene(node))
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(filename), "Navigator::generateSceneFromPeer");

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
    mPhysicsWorld->setCollisionListener(dynamic_cast<OgreOde::CollisionListener*>(this));
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

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
bool OgrePeerManager::collision(OgreOde::Contact* contact)
{
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

    contact->setCoulombFriction(0.9);
    contact->setBouncyness(0.2);
    contact->setSoftness(0.8, 10e-5);

    return true;
}
#endif
