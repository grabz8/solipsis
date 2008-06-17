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

#include "OgrePeerManager.h"
#include "Avatar.h"
#include "Scene.h"
#include "OgreOSMScene.h"
#include "Navigator.h"
#include "OgreHelpers.h"
#include "Modeler.h"
#include "AvatarEditor.h"
#include "CharacterManager.h"

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

	static AvatarEditor* avatarEditor = NULL;
	if(!avatarEditor)
	{
		//avatarEditor = new AvatarEditor("..\\..\\..\\..\\media\\cache\\models", mSceneMgr);
		avatarEditor = new AvatarEditor("../../../../media/cache/models", mSceneMgr);
		avatarEditor->buildListSAF();
	}
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
    OgrePeer* newOgrePeer = 0;
    switch (xmlEntity->getType())
    {
    case ETAvatar:
        newOgrePeer = createAvatarNode(xmlEntity);
        break;
    case ETSite:
        newOgrePeer = createSceneNode(xmlEntity);
        break;
    case ETObject:
        newOgrePeer = createObjectNode(xmlEntity);
        break;
    }
    if (newOgrePeer != 0)
        mOgrePeersMap[xmlEntity->getUid()] = newOgrePeer;
    else
        OGRE_LOG("OgrePeerManager::load() Unable to load node type:" + xmlEntity->getTypeRepr() + ", uid:" + xmlEntity->getUidString());

    return true;
}

//-------------------------------------------------------------------------------------
const String& OgrePeerManager::getXmlObjectFilename()
{
    return mXmlObjectFilename;
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
OgrePeer* OgrePeerManager::createAvatarNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
OgrePeer* OgrePeerManager::createAvatarNode(XmlEntity* xmlEntity)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "OgrePeerManager::CreateAvatarNode");

    std::string uidStr = xmlEntity->getUidString();
    bool isLocal = (mMyXmlEntities.find(xmlEntity->getUid()) != mMyXmlEntities.end());

    CharacterInstance* characterInstance = CharacterManager::getSingletonPtr()->loadCharacterInstance(uidStr, "");
    if (characterInstance == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create character instance !", "OgrePeerManager::CreateAvatarNode");
    if (isLocal)
        // set the current user character editable by the avatar editor
        AvatarEditor::getSingletonPtr()->setCharacterInstance(characterInstance);

#ifdef SHADOWS
    entity->setCastShadows(true);
#endif

    Avatar* peerAvatar = new Avatar(xmlEntity, isLocal, characterInstance);
    peerAvatar->setStateAnimName(Avatar::SWalk, "Walk");
    peerAvatar->setStateAnimName(Avatar::SRun, "Run");
    peerAvatar->setStateAnimName(Avatar::SFly, "Fly");
    peerAvatar->setStateAnimName(Avatar::SSwim, "Swim");
    peerAvatar->setState(Avatar::SIdle);

    if (mCallbacks != 0)
        if (!mCallbacks->OnAvatarNodeCreate(peerAvatar))
        {
            delete peerAvatar;
            return 0;
        }

    return peerAvatar;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
OgrePeer* OgrePeerManager::createSceneNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
OgrePeer* OgrePeerManager::createSceneNode(XmlEntity* xmlEntity)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "OgrePeerManager::CreateSceneNode");

	// Open XML file and parse it
	TiXmlDocument xmlDoc;
    String xmlFile = xmlEntity->getUidString() + ".xml";
    mXmlObjectFilename = xmlFile;
    DataStreamPtr stream;
    try {
    	stream = ResourceGroupManager::getSingleton().openResource(xmlFile);
    } catch (Ogre::Exception& e) {
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load XML file " + xmlFile + ", exception: " + e.getFullDescription(), "OgrePeerManager::CreateSceneNode");
    }
    if (!stream->size())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Empty XML file " + xmlFile, "OgrePeerManager::CreateSceneNode");
    size_t size = stream->size();
    char *buf = new char[size + 1];
    memset(buf, 0, size + 1);
    stream->read(buf, size);
    stream.setNull();
    xmlDoc.Parse(buf);
    delete[] buf;
    // Check for errors
    if (xmlDoc.Error())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Failed to load XML file " + xmlFile + ", " + String(xmlDoc.ErrorDesc()), "OgrePeerManager::CreateSceneNode");
    // Process elements
    TiXmlElement* xmlElt = xmlDoc.RootElement()->FirstChildElement("sceneNode");
    if (xmlElt == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Malformed sceneNode XML file " + xmlFile, "OgrePeerManager::CreateSceneNode");
    const char* name = xmlElt->Attribute("name");
    const char* filename = xmlElt->Attribute("filename");
    const char* collision = xmlElt->Attribute("collision");
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(xmlEntity->getUidString() + "Scene");
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

    // Destroy the scene collision mesh
    if (collision != 0)
        mSceneMgr->destroySceneNode(collision);

    bool isLocal = (mMyXmlEntities.find(xmlEntity->getUid()) != mMyXmlEntities.end());
    Scene* peerScene = new Scene(xmlEntity, isLocal, node);

    if (mCallbacks != 0)
        if (!mCallbacks->OnSceneNodeCreate(peerScene))
        {
            delete peerScene;
            return 0;
        }

    return peerScene;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
OgrePeer* OgrePeerManager::createObjectNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
OgrePeer* OgrePeerManager::createObjectNode(XmlEntity* xmlEntity)
#endif
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "OgrePeerManager::CreateObjectNode");

	// Open XML file and parse it
	TiXmlDocument xmlDoc;
    String xmlFile = xmlEntity->getUidString() + ".xml";
    mXmlObjectFilename = xmlFile;
    DataStreamPtr stream;
    try {
    	stream = ResourceGroupManager::getSingleton().openResource(xmlFile);
    } catch (Ogre::Exception& e) {
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load XML file " + xmlFile + ", exception: " + e.getFullDescription(), "OgrePeerManager::CreateSceneNode");
    }
    if (!stream->size())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Empty XML file " + xmlFile, "OgrePeerManager::CreateSceneNode");
    size_t size = stream->size();
    char *buf = new char[size + 1];
    memset(buf, 0, size + 1);
    stream->read(buf, size);
    stream.setNull();
    xmlDoc.Parse(buf);
    delete[] buf;
    // Check for errors
    if (xmlDoc.Error())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Failed to load XML file " + xmlFile + ", " + String(xmlDoc.ErrorDesc()), "OgrePeerManager::CreateSceneNode");
    // Process elements
    TiXmlElement* xmlElt = xmlDoc.FirstChildElement("objectNode");
    if (xmlElt == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Malformed objectNode XML file " + xmlFile, "OgrePeerManager::CreateSceneNode");
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
