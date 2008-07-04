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
#include "OgreHelpers.h"
#include <Modeler.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <CTIO.h>

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
bool OgrePeerManager::OnObject3DListSave(const String& sofPathname, const Object3DPtrList& object3DList)
{
    static EntityUID nextEntityUID = 0x00000001; // hmhm Entities UID management TODO

    // Rename the sof file
    String::size_type dotPos = sofPathname.find_last_of(".");
    String::size_type filenamePos = sofPathname.find_last_of("/\\");
    if (filenamePos == String::npos)
        filenamePos = 0;
    else
        filenamePos++;
    String dstSofFilename = XmlHelpers::convertEntityUIDToHexString(nextEntityUID) + sofPathname.substr(dotPos, sofPathname.length() - dotPos);
    String dstSofPathname = sofPathname.substr(0, filenamePos) + dstSofFilename;
    CommonTools::IO::renameFile(std::string(sofPathname), std::string(dstSofPathname));

    // Create the Xml entity
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity;
#else
    XmlEntity* xmlEntity = new XmlEntity();
#endif
    xmlEntity->setDefinedAttributes(XmlEntity::DANone);
    xmlEntity->setUid(nextEntityUID);
    xmlEntity->setType(ETObject);
    xmlEntity->setName(xmlEntity->getUidString());
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
    XmlLodContent::LodContentFileStruct lodContent0File;
    lodContent0File.filename = dstSofFilename;
    lodContent0File.version = 0;
    xmlLodContent0->getLodContentFileList().push_back(lodContent0File);
    xmlContent->getContentLodMap()[xmlLodContent0->getLevel()] = xmlLodContent0;
    xmlEntity->setContent(xmlContent);

    // Create the object
    Object* peerObject = new Object(xmlEntity, true, object3DList);

    // Store it
    mOgrePeersMap[xmlEntity->getUid()] = peerObject;
    mMyXmlEntities[xmlEntity->getUid()] = 0;

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

    // hmhm next entity UID ??!??
//    nextEntityUID++;

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

    if (isLocal)
        mUserAvatar = peerAvatar;

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

    // Get the scene content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = xmlEntity->getContent()->getContentLodMap();
    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());

    // Find .ssf file
    XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
    for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
        if (lodContent0File->filename.find(".ssf") == lodContent0File->filename.length() - 4)
            break;
    if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .ssf scene file found !", "OgrePeerManager::CreateSceneNode");

    // Create the resource group
    String mediaCacheScenePath = Navigator::getSingletonPtr()->getMediaCachePath() + "\\scenes";
    String resourceGroup = xmlEntity->getUidString() + "Resources";
    ResourceGroupManager::getSingleton().createResourceGroup(resourceGroup);
    ResourceGroupManager::getSingleton().addResourceLocation(mediaCacheScenePath + "\\" + lodContent0File->filename, "Zip", resourceGroup);
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

    bool isLocal = (mMyXmlEntities.find(xmlEntity->getUid()) != mMyXmlEntities.end());

    // Get the scene content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = xmlEntity->getContent()->getContentLodMap();
    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());

    // Find .sof file
    XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
    for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
        if (lodContent0File->filename.find(".sof") == lodContent0File->filename.length() - 4)
            break;
    if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .sof object file found !", "OgrePeerManager::CreateObjectNode");

    String mediaCacheModelsPath = Navigator::getSingletonPtr()->getMediaCachePath() + "\\models";
    String pathname = mediaCacheModelsPath + "\\" + lodContent0File->filename;

    Modeler* modeler = Modeler::getSingletonPtr();
    Object3DPtrList newObjects;
    if (!modeler->XMLLoad(pathname, newObjects))
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load .sof object file !", "OgrePeerManager::CreateObjectNode");

    // Create the object
    Object* peerObject = new Object(xmlEntity, isLocal, newObjects);

    return peerObject;
}

//-------------------------------------------------------------------------------------
