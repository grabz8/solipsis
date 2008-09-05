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
#include "Object.h"
#include "OgreOSMScene.h"
#include "Navigator.h"
#include <Modeler.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <CTLog.h>
#include <CTIO.h>

using namespace Solipsis;
using namespace CommonTools;

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
void OgrePeerManager::setNodeId(const NodeId& nodeId)
{
    mNodeId = nodeId;
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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "OgrePeerManager::load() Unable to load node type:%s, uid:%s", xmlEntity->getTypeRepr().c_str(), xmlEntity->getUidString().c_str());
    // update the object now to force loading of content, load is performed after registering into mOgrePeersMap
    // in order the loadTexture can retrieve this object is local (VLC textures)
    // TODO: OgrePeer should register itself in manager on constr then load is performed according its internal state
    if (xmlEntity->getType() == ETObject)
        newOgrePeer->update(xmlEntity);

    return true;
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
void OgrePeerManager::removeAll(bool local)
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
#ifdef POOL
bool OgrePeerManager::action(RefCntPoolPtr<XmlAction>& xmlAction)
#else
bool OgrePeerManager::update(XmlAction* xmlAction)
#endif
{
    if (xmlAction->getType() == ATChat)
    {
        String from = "???";
        OgrePeersMap::iterator it = mOgrePeersMap.find(xmlAction->getSourceEntityUid());
        if (it != mOgrePeersMap.end())
            from = it->second->getXmlEntity()->getName();
        Navigator::getSingletonPtr()->getNavigatorGUI()->addChatText(from + " " + xmlAction->getDesc());
    }

    OgrePeersMap::iterator it = mOgrePeersMap.find(xmlAction->getTargetEntityUid());
    if ((it == mOgrePeersMap.end()) || (it->second == 0))
        return false;

    OgrePeer* ogrePeer = it->second;
    bool result = ogrePeer->action(xmlAction);

#ifdef POOL
#else
    delete xmlAction;
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
EntityUID OgrePeerManager::getNewEntityUID()
{
    static EntityUID nextEntityUID = 0x00000001; // hmhm Entities UID management TODO

    EntityUID objectEntityUid;
    const char *m = mNodeId.c_str();
    sscanf(mNodeId.c_str(), "%08X", &objectEntityUid);
    objectEntityUid <<= 16;
    while (true)
    {
        objectEntityUid |= nextEntityUID;
        OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(objectEntityUid);
        OgrePeersMap::iterator ogrePeerReserved = mReservedOgrePeersMap.find(objectEntityUid);
        if ((ogrePeer == mOgrePeersMap.end()) && (ogrePeerReserved == mReservedOgrePeersMap.end()))
            break;
        objectEntityUid &= 0xFFFF0000;
        nextEntityUID++;
    }

    mReservedOgrePeersMap[objectEntityUid] = 0;
    return objectEntityUid;
}

//-------------------------------------------------------------------------------------
OgrePeer* OgrePeerManager::getOgrePeer(const EntityUID& entityUID)
{
    OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(entityUID);
    if (ogrePeer == mOgrePeersMap.end())
        return 0;
    return ogrePeer->second;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::onObject3DSave(const String& sofFilename, Object3D* object3D)
{
    // Object updated or new object ?
    OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(object3D->getEntityUID());
    if (ogrePeer == mOgrePeersMap.end())
    {
        // New object entity
        mReservedOgrePeersMap.erase(object3D->getEntityUID());

        // Create the Xml entity
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity;
#else
        XmlEntity* xmlEntity = new XmlEntity();
#endif
        xmlEntity->setDefinedAttributes(XmlEntity::DANone);
        xmlEntity->setUid(object3D->getEntityUID());
        xmlEntity->setOwner(mNodeId);
        xmlEntity->setType(ETObject);
        xmlEntity->setName(object3D->getName());
        xmlEntity->setVersion(0);
        xmlEntity->setFlags(EFNone);
        xmlEntity->setPosition(Vector3::ZERO);
        xmlEntity->setOrientation(Quaternion::IDENTITY);
#ifdef POOL
        RefCntPoolPtr<XmlContent> xmlContent;
        RefCntPoolPtr<XmlLodContent> xmlLodContent0;
#else
        XmlContent* xmlContent = new XmlContent();
        XmlLodContent* xmlLodContent0 = new XmlLodContent();
#endif
        xmlLodContent0->setLevel(0);
        LodContentFileStruct lodContent0File;
        lodContent0File.mFilename = sofFilename;
        // here we are maybe creating 1 new object with 1 uid previously assigned to a deleted object
        // so we have to take care files in cache manager are well replaced by new ones
        // ideally we should use the local cacheManager (of navigator) to detect this case, get old version, increment it and purge this entry
        // for instance (no cache manager in navigator) we will simply set the current date into the version
        time_t now;
        time(&now);
        lodContent0File.mVersion = now;
        xmlLodContent0->getLodContentFileList().push_back(lodContent0File);
        xmlContent->getContentLodMap()[xmlLodContent0->getLevel()] = xmlLodContent0;
        xmlEntity->setContent(xmlContent);

        // Create the object
        Object* peerObject = new Object(xmlEntity, true, object3D);

        // Store it
        mOgrePeersMap[xmlEntity->getUid()] = peerObject;

        // Send new entity event
#ifdef POOL
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETNewEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
        XmlEvt xmlEvt(ETNewEntity);
        xmlEvt.setDatas(xmlEntity);
#endif
        mEvtsList.push_back(xmlEvt);
    }
    else
    {
        // Updated object

        Object *object = (Object*)ogrePeer->second;
        object->onObjectSave();

        // Create the Xml entity
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity;
#else
        XmlEntity* xmlEntity = new XmlEntity();
#endif
        xmlEntity->setDefinedAttributes(XmlEntity::DANone);
        xmlEntity->setUid(object->getXmlEntity()->getUid());
        xmlEntity->setType(object->getXmlEntity()->getType());
        xmlEntity->setVersion(object->getXmlEntity()->getVersion());
        xmlEntity->setContent(object->getXmlEntity()->getContent());

        // Send updated entity event
#ifdef POOL
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETUpdatedEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
        XmlEvt xmlEvt(ETUpdatedEntity);
        xmlEvt.setDatas(xmlEntity);
#endif
        mEvtsList.push_back(xmlEvt);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::onObject3DDelete(Object3D* object3D)
{
    // Object exists or not yet saved ?
    OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(object3D->getEntityUID());
    if (ogrePeer == mOgrePeersMap.end())
        return false;

    Object *object = (Object*)ogrePeer->second;

    // Create the Xml entity
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity;
#else
    XmlEntity* xmlEntity = new XmlEntity();
#endif
    xmlEntity->setDefinedAttributes(XmlEntity::DANone);
    xmlEntity->setUid(object->getXmlEntity()->getUid());
    xmlEntity->setType(object->getXmlEntity()->getType());

    // Send lost entity event
#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETLostEntity);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
    XmlEvt xmlEvt(ETLostEntity);
    xmlEvt.setDatas(xmlEntity);
#endif
    mEvtsList.push_back(xmlEvt);

    mOgrePeersMap.erase(ogrePeer);
    delete object;

    return true;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::isObject3DOwned(Object3D* object3D)
{
    // Object exists or not yet saved ?
    OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(object3D->getEntityUID());
    if (ogrePeer == mOgrePeersMap.end())
        return true;

    Object *object = (Object*)ogrePeer->second;
    return (object->getXmlEntity()->getOwner() == mNodeId);
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::onUserAvatarSave()
{
    Avatar *userAvatar = (Avatar*)mUserAvatar;
    userAvatar->onAvatarSave();

    // Create the Xml entity
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity;
#else
    XmlEntity* xmlEntity = new XmlEntity();
#endif
    xmlEntity->setDefinedAttributes(XmlEntity::DANone);
    xmlEntity->setUid(mUserAvatar->getXmlEntity()->getUid());
    xmlEntity->setType(mUserAvatar->getXmlEntity()->getType());
    xmlEntity->setVersion(mUserAvatar->getXmlEntity()->getVersion());
    xmlEntity->setContent(mUserAvatar->getXmlEntity()->getContent());

    // Send updated entity event
#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETUpdatedEntity);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
    XmlEvt xmlEvt(ETUpdatedEntity);
    xmlEvt.setDatas(xmlEntity);
#endif
    mEvtsList.push_back(xmlEvt);

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

    bool isLocal = (xmlEntity->getOwner() == mNodeId);

    String defaultCharacterName = "";
    XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
    for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
        if (it->mFilename.find(".saf") == it->mFilename.length() - 4)
            defaultCharacterName = it->mFilename.substr(0, it->mFilename.length() - 4);
    CharacterInstance* characterInstance = CharacterManager::getSingletonPtr()->loadCharacterInstance(xmlEntity->getUidString(), defaultCharacterName);
    if (characterInstance == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create character instance !", "OgrePeerManager::CreateAvatarNode");
    if (isLocal)
        // set the current user character editable by the avatar editor
        AvatarEditor::getSingletonPtr()->setCharacterInstance(characterInstance);

#ifdef SHADOWS
    entity->setCastShadows(true);
#endif

    Avatar* peerAvatar = new Avatar(xmlEntity, isLocal, characterInstance);
    peerAvatar->setStateAnimName(ASAvatarIdle, "Idle");
    peerAvatar->setStateAnimName(ASAvatarWalk, "Walk");
    peerAvatar->setStateAnimName(ASAvatarRun, "Run");
    peerAvatar->setStateAnimName(ASAvatarFly, "Fly");
    peerAvatar->setStateAnimName(ASAvatarSwim, "Swim");
    peerAvatar->setState(ASAvatarIdle);

    if (isLocal)
    {
        mUserAvatar = peerAvatar;

/*	    characterInstance->saveModified();
        OnUserAvatarSave();*/
    }

    if (mCallbacks != 0)
        if (!mCallbacks->onAvatarNodeCreate(peerAvatar))
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

    // Get the scene content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = xmlEntity->getContent()->getContentLodMap();
    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());

    // Find .ssf file
    XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
    for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
        if (lodContent0File->mFilename.find(".ssf") == lodContent0File->mFilename.length() - 4)
            break;
    if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .ssf scene file found !", "OgrePeerManager::CreateSceneNode");

    // Create the resource group
    String resourceGroup = xmlEntity->getUidString() + "Resources";
    ResourceGroupManager::getSingleton().createResourceGroup(resourceGroup);
    ResourceGroupManager::getSingleton().addResourceLocation(Navigator::getSingletonPtr()->getMediaCachePath() + "\\" + lodContent0File->mFilename, "Zip", resourceGroup);
    ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceGroup);

    // Create the scene node
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(xmlEntity->getUidString() + "Scene");

    // Load from the .osm
    OSMScene osmScene(mSceneMgr);
    OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
    if (!osmScene.initialise(xmlSceneLodContent0->getMainFilename().c_str(), &osmSceneCallbacks))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "OgrePeerManager::createSceneNode");
    osmScene.declareResources();
    if (!osmScene.createScene(node))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "OgrePeerManager::createSceneNode");

#ifdef SHADOWS
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
    mSceneMgr->setShadowTextureSettings(512, 1, PixelFormat::PF_A4R4G4B4);
    Ogre::SharedPtr<LiSPSMShadowCameraSetup> shadowCameraSetup = Ogre::SharedPtr<LiSPSMShadowCameraSetup>(new LiSPSMShadowCameraSetup());
    mSceneMgr->setShadowCameraSetup(shadowCameraSetup);
#endif

    node->setPosition(xmlEntity->getPosition());

    // Destroy the scene collision mesh
    if (!xmlSceneLodContent0->getCollision().empty())
        mSceneMgr->destroySceneNode(xmlSceneLodContent0->getCollision());

    bool isLocal = (xmlEntity->getOwner() == mNodeId);
    Scene* peerScene = new Scene(xmlEntity, isLocal, node);

    if (mCallbacks != 0)
        if (!mCallbacks->onSceneNodeCreate(peerScene))
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

    bool isLocal = (xmlEntity->getOwner() == mNodeId);

    // Create the object
    Object* peerObject = new Object(xmlEntity, isLocal);

    return peerObject;
}

//-------------------------------------------------------------------------------------
