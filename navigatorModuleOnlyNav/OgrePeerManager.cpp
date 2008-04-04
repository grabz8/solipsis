#include "OgrePeerManager.h"
#include "Avatar.h"
#include "Scene.h"
#include "OgreOSMScene.h"
#include "Navigator.h"
#include "OgreHelpers.h"
#include "Modeler.h"
#include "AvatarEditor.h"

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
{
    mOgrePeersMap.clear();
}

//-------------------------------------------------------------------------------------
OgrePeerManager::~OgrePeerManager()
{
}

//-------------------------------------------------------------------------------------
void OgrePeerManager::setMyEntities(std::list<EntityUID> myEntities)
{
    mMyXmlEntities.clear();
    for (std::list<EntityUID>::iterator it=myEntities.begin(); it != myEntities.end(); ++it)
        mMyXmlEntities[(*it)] = 0;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool OgrePeerManager::load(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool OgrePeerManager::load(XmlEntity* xmlEntity)
#endif
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
    String xmlFile = xmlEntity->getName() + ".xml";
    if (xmlEntity->getType() == ETAvatar)
        xmlFile = "User.xml";
    DataStreamPtr stream;
    try
    {
    	stream = ResourceGroupManager::getSingleton().openResource(xmlFile);
    }
    catch (Ogre::Exception& e)
    {
        OGRE_LOG("OgrePeerManager::load() Unable to load peer XML file " + xmlFile);
        return false;
    }
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
            newOgrePeer = createAvatarNode(xmlEntity, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load avatarNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        else if (String(elt->Value()).compare("sceneNode") == 0)
        {
            xmlObjectFilename = xmlFile;
            newOgrePeer = createSceneNode(xmlEntity, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load sceneNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        else if (String(elt->Value()).compare("objectNode") == 0)
        {
            //xmlObjectFilename = xmlFile;
            newOgrePeer = createObjectNode(xmlEntity, elt);
            if (newOgrePeer == 0)
                OGRE_LOG("OgrePeerManager::load() Unable to load objectNode in peer XML file " + xmlFile + ", raw " + StringConverter::toString(elt->Row()));
        }
        if (newOgrePeer != 0)
            mOgrePeersMap[xmlEntity->getUid()] = newOgrePeer;
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string OgrePeerManager::getXmlObjectFilename()
{
    return xmlObjectFilename;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::remove(const EntityUID& entity, bool local)
{
    bool peerFound = false;
    for (OgrePeersMap::iterator ogrePeer = mOgrePeersMap.begin(); ogrePeer != mOgrePeersMap.end(); ++ogrePeer)
    {
        if (ogrePeer->second->isLocal() != local) continue;
        if (ogrePeer->second->getXmlEntity()->getUid() == entity)
        {
#ifdef POOL
#else
            delete ogrePeer->second->getXmlEntity();
#endif
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
        for (OgrePeersMap::iterator ogrePeer = mOgrePeersMap.begin(); ogrePeer != mOgrePeersMap.end(); ++ogrePeer)
        {
            if (ogrePeer->second->isLocal() != local) continue;
#ifdef POOL
#else
            delete ogrePeer->second->getXmlEntity();
#endif
            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            loopAgain = true;
            break;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool OgrePeerManager::update(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool OgrePeerManager::update(XmlEntity* xmlEntity)
#endif
{
    OgrePeersMap::iterator it = mOgrePeersMap.find(xmlEntity->getUid());
    if ((it == mOgrePeersMap.end()) || (it->second == 0))
        return false;

    OgrePeer* ogrePeer = it->second;
    bool result = ogrePeer->update(xmlEntity);

#ifdef POOL
#else
    delete xmlEntity;
#endif

    return result;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::frameStarted(const FrameEvent& evt)
{
    // Animate
    for (OgrePeersMap::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
        it->second->update(evt.timeSinceLastFrame);

    // Send updated entities events to node
#ifdef POOL
#else
    XmlEvt xmlEvt(ETUpdatedEntity);
#endif
    for (OgrePeersMap::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
    {
#ifdef POOL
        RefCntPoolPtr<XmlEntity>& updatedXmlEntity = it->second->getUpdatedXmlEntity();
        if (updatedXmlEntity.isNull()) continue;
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETUpdatedEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(updatedXmlEntity));
#else
        XmlEntity* updatedXmlEntity = it->second->getUpdatedXmlEntity();
        if (updatedXmlEntity == 0) continue;
        xmlEvt.setDatas(updatedXmlEntity);
#endif
        mEvtsList.push_back(xmlEvt);
    }

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
OgrePeer* OgrePeerManager::createAvatarNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt)
#else
OgrePeer* OgrePeerManager::createAvatarNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR,
            "No scene manager !",
            "OgrePeerManager::CreateAvatarNode");

// GILLES begin
    const char* name = xmlElt->Attribute("name");
#if 0
    const char* meshFilename = xmlElt->Attribute("meshFilename");
    const char* skeletonFilename = xmlElt->Attribute("skeletonFilename");
#else
    const char* filename = xmlElt->Attribute("filename");

    /*
    static AvatarEditor* avatarEditor = 0;
    if(!avatarEditor)
        avatarEditor = new AvatarEditor();
    if (!avatarEditor->XMLLoad(filename))
        return false;
    */

    AvatarEditor* avatarEditor = AvatarEditor::getSingletonPtr();
    if(!avatarEditor)
        avatarEditor = new AvatarEditor();
    if (!avatarEditor->XMLLoad("..\\..\\..\\..\\media\\cache\\models\\", filename))
        return false;

    std::string meshName = avatarEditor->getMeshFilename();
    std::string skeletonName = avatarEditor->getSkeletonFilename();
    const char* meshFilename = meshName.c_str();
    const char* skeletonFilename = skeletonName.c_str();
#endif
// GILLES end
	
	if ((name == 0) || (meshFilename == 0) || (skeletonFilename == 0))
        return false;
    String uidString = StringConverter::toString(xmlEntity->getUid());
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(uidString + "Avatar");
    Entity* entity = mSceneMgr->createEntity(uidString + "Avatar", meshFilename);
#ifdef SHADOWS
    entity->setCastShadows(true);
#endif

    bool isLocal = (mMyXmlEntities.find(xmlEntity->getUid()) != mMyXmlEntities.end());
    Avatar* peerAvatar = new Avatar(xmlEntity, isLocal, node, entity);
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
#ifdef POOL
OgrePeer* OgrePeerManager::createSceneNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt)
#else
OgrePeer* OgrePeerManager::createSceneNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR,
            "No scene manager !",
            "OgrePeerManager::CreateSceneNode");

    const char* name = xmlElt->Attribute("name");
    const char* filename = xmlElt->Attribute("filename");
    const char* collision = xmlElt->Attribute("collision");
    String uidString = StringConverter::toString(xmlEntity->getUid());
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(uidString + "Scene");
    OSMScene osmScene(mSceneMgr);
    OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
    if (!osmScene.initialise(filename, &osmSceneCallbacks))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(filename), "OgrePeerManager::createSceneNode");
    osmScene.declareResources();
    if (!osmScene.createScene(node))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(filename), "OgrePeerManager::createSceneNode");

#ifdef SHADOWS
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
    mSceneMgr->setShadowTextureSettings(512, 1, PixelFormat::PF_A4R4G4B4);
    Ogre::SharedPtr<LiSPSMShadowCameraSetup> shadowCameraSetup = Ogre::SharedPtr<LiSPSMShadowCameraSetup>(new LiSPSMShadowCameraSetup());
    mSceneMgr->setShadowCameraSetup(shadowCameraSetup);
#endif

    node->setPosition(xmlEntity->getPosition());
    node->setPosition(18,-58,133);

    // Destroy the scene collision mesh
    mSceneMgr->destroySceneNode(collision);

    bool isLocal = (mMyXmlEntities.find(xmlEntity->getUid()) != mMyXmlEntities.end());
    Scene* peerScene = new Scene(xmlEntity, isLocal, node);

    if (mCallbacks != 0)
        if (!mCallbacks->OnSceneNodeCreate(xmlElt, peerScene))
        {
            delete peerScene;
            return 0;
        }

    return peerScene;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
OgrePeer* OgrePeerManager::createObjectNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt)
#else
OgrePeer* OgrePeerManager::createObjectNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR,
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

//-------------------------------------------------------------------------------------
