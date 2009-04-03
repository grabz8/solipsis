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

#include "Prerequisites.h"

#include "OgrePeerManager.h"
#include "World/Avatar.h"
#include "World/Scene.h"
#include "World/Object.h"
#include "MainApplication/Navigator.h"
#include "OgreTools/OgreHelpers.h"
#include "World/Modeler.h"
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <CTLog.h>
#include <CTIO.h>
#include <CTStringHelpers.h>
#include <time.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------
OgrePeerManager::OgrePeerManager(SceneManager* sceneMgr, IOgrePeerManagerCallbacks* callbacks) :
    mNodeId(""),
    mUserAvatar(0),
    mSceneMgr(sceneMgr),
    mCallbacks(callbacks)
{
    mOgrePeersMap.clear();
    mReservedOgrePeersMap.clear();
}

//-------------------------------------------------------------------------------------
OgrePeerManager::~OgrePeerManager()
{
}

//-------------------------------------------------------------------------------------
const NodeId& OgrePeerManager::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
void OgrePeerManager::setNodeId(const NodeId& nodeId)
{
    mNodeId = nodeId;
}

//-------------------------------------------------------------------------------------

bool OgrePeerManager::load(RefCntPoolPtr<XmlEntity>& xmlEntity)
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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "OgrePeerManager::load() Unable to load node type:%s, uid:%s", xmlEntity->getTypeRepr().c_str(), xmlEntity->getUid().c_str());
    // update the object now to force loading of content, load is performed after registering into mOgrePeersMap
    // in order the loadTexture can retrieve this object is local (VLC textures)
    // TODO: OgrePeer should register itself in manager on constr then load is performed according its internal state
    if (xmlEntity->getType() != ETAvatar)
    {
        if (!newOgrePeer->updateEntity(xmlEntity))
        {
            // delete the object on error
            remove(xmlEntity->getUid());
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::remove(const EntityUID& entity)
{
    for (OgrePeersMap::iterator ogrePeer = mOgrePeersMap.begin(); ogrePeer != mOgrePeersMap.end(); ++ogrePeer)
    {
        if (ogrePeer->second->getXmlEntity()->getUid() != entity)
            continue;

        switch (ogrePeer->second->getXmlEntity()->getType())
        {
        case ETSite:
            if (mCallbacks != 0)
                mCallbacks->onSceneNodeDestroy(ogrePeer->second);
            break;
        }

        delete ogrePeer->second;
        mOgrePeersMap.erase(ogrePeer);
        return true;
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

            delete ogrePeer->second;
            mOgrePeersMap.erase(ogrePeer);
            loopAgain = true;
            break;
        }
    }
}

//-------------------------------------------------------------------------------------
void OgrePeerManager::cleanUp()
{
    removeAll(false);
    removeAll(true);

    mOgrePeersMap.clear();
    mReservedOgrePeersMap.clear();

    mNodeId = "";
    mUserAvatar = 0;

    mEvtsList.clear();
}

//-------------------------------------------------------------------------------------

bool OgrePeerManager::updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    OgrePeersMap::iterator it = mOgrePeersMap.find(xmlEntity->getUid());
    if ((it == mOgrePeersMap.end()) || (it->second == 0))
        return false;

    OgrePeer* ogrePeer = it->second;
    bool result = ogrePeer->updateEntity(xmlEntity);

    return result;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::action(RefCntPoolPtr<XmlAction>& xmlAction)
{
    if (xmlAction->getType() == ATChat)
    {
        String from = "???";
        OgrePeersMap::iterator it = mOgrePeersMap.find(xmlAction->getSourceEntityUid());
        if (it != mOgrePeersMap.end())
            from = it->second->getXmlEntity()->getName();
        Navigator::getSingletonPtr()->getNavigatorGUI()->addChatText(StringHelpers::convertStringToWString(from) + L" " + xmlAction->getDesc());
    }

    OgrePeersMap::iterator it = mOgrePeersMap.find(xmlAction->getTargetEntityUid());
    if ((it == mOgrePeersMap.end()) || (it->second == 0))
        return false;

    OgrePeer* ogrePeer = it->second;
    bool result = ogrePeer->action(xmlAction);

    return result;
}

//-------------------------------------------------------------------------------------
bool OgrePeerManager::frameStarted(const FrameEvent& evt)
{
    // Animate
    for (OgrePeersMap::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
        it->second->update(evt.timeSinceLastFrame);

    // Send updated entities events to node


    for (OgrePeersMap::iterator it = mOgrePeersMap.begin();it != mOgrePeersMap.end();++it)
    {

        RefCntPoolPtr<XmlEntity>& updatedXmlEntity = it->second->getUpdatedXmlEntity();
        if (updatedXmlEntity.isNull()) continue;
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETUpdatedEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(updatedXmlEntity));

        mEvtsList.push_back(xmlEvt);
    }

    return true;
}

//-------------------------------------------------------------------------------------
EntityUID OgrePeerManager::getNewEntityUID()
{
    EntityUID objectEntityUid;
    unsigned short objectId = 0x0001;
    unsigned short subObjectId = 0x0000;
    while (objectId != 0x0000)
    {
        objectEntityUid = mNodeId + "_" + XmlHelpers::convertUIntToHexString(((unsigned int)objectId) << 16 | (unsigned int)subObjectId);
        OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(objectEntityUid);
        OgrePeersMap::iterator ogrePeerReserved = mReservedOgrePeersMap.find(objectEntityUid);
        if ((ogrePeer == mOgrePeersMap.end()) && (ogrePeerReserved == mReservedOgrePeersMap.end()))
            break;
        objectId++;
    }
    if (objectId == 0x0000)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No free object id found !", "OgrePeerManager::getNewEntityUID");

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
bool OgrePeerManager::isOgrePeerOwned(OgrePeer* peer)
{
    // Object exists or not yet saved ?
    OgrePeersMap::iterator ogrePeer = mOgrePeersMap.find(peer->getXmlEntity()->getUid());
    if (ogrePeer == mOgrePeersMap.end())
        return true;

    return (peer->getXmlEntity()->getOwner() == mNodeId);
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
        RefCntPoolPtr<XmlEntity> xmlEntity;

        xmlEntity->setDefinedAttributes(XmlEntity::DANone);
        xmlEntity->setUid(object3D->getEntityUID());
        xmlEntity->setOwner(mNodeId);
        xmlEntity->setType(ETObject);
        xmlEntity->setName(object3D->getName());
        xmlEntity->setVersion(0);
        AxisAlignedBox box;
        object3D->getAABoundingBox(box);
        xmlEntity->setAABoundingBox(box);
        xmlEntity->setFlags(EFNone);
        xmlEntity->setPosition(Vector3::ZERO);
        xmlEntity->setOrientation(Quaternion::IDENTITY);

        RefCntPoolPtr<XmlContent> xmlContent;
        RefCntPoolPtr<XmlLodContent> xmlLodContent0;

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

        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETNewEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

        mEvtsList.push_back(xmlEvt);
    }
    else
    {
        // Updated object

        Object *object = (Object*)ogrePeer->second;
        object->onObjectSave();

        // Create the Xml entity

        RefCntPoolPtr<XmlEntity> xmlEntity;

        xmlEntity->setDefinedAttributes(XmlEntity::DANone);
        xmlEntity->setUid(object->getXmlEntity()->getUid());
        xmlEntity->setType(object->getXmlEntity()->getType());
        xmlEntity->setVersion(object->getXmlEntity()->getVersion());
        xmlEntity->setContent(object->getXmlEntity()->getContent());

        // Send updated entity event

        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETUpdatedEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

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
    {
        mReservedOgrePeersMap.erase(object3D->getEntityUID());
        return false;
    }

    Object *object = (Object*)ogrePeer->second;

    // Create the Xml entity
    RefCntPoolPtr<XmlEntity> xmlEntity;

    xmlEntity->setDefinedAttributes(XmlEntity::DANone);
    xmlEntity->setUid(object->getXmlEntity()->getUid());
    xmlEntity->setType(object->getXmlEntity()->getType());

    // Send lost entity event

    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETLostEntity);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

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

    RefCntPoolPtr<XmlEntity> xmlEntity;

    xmlEntity->setDefinedAttributes(XmlEntity::DANone);
    xmlEntity->setUid(mUserAvatar->getXmlEntity()->getUid());
    xmlEntity->setType(mUserAvatar->getXmlEntity()->getType());
    xmlEntity->setVersion(mUserAvatar->getXmlEntity()->getVersion());
    xmlEntity->setContent(mUserAvatar->getXmlEntity()->getContent());

    // Send updated entity event
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETUpdatedEntity);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

    mEvtsList.push_back(xmlEvt);

    return true;
}

//-------------------------------------------------------------------------------------

OgrePeer* OgrePeerManager::createAvatarNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    if (mSceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "OgrePeerManager::CreateAvatarNode");

    bool isLocal = (xmlEntity->getOwner() == mNodeId);

    String defaultCharacterName = "";
    XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
    for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
        if (it->mFilename.find(".saf") == it->mFilename.length() - 4)
            defaultCharacterName = it->mFilename.substr(0, it->mFilename.length() - 4);
    CharacterInstance* characterInstance = CharacterManager::getSingletonPtr()->loadCharacterInstance(xmlEntity->getUid(), defaultCharacterName);
    if (characterInstance == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create character instance !", "OgrePeerManager::CreateAvatarNode");
    if (isLocal)
        // set the current user character editable by the avatar editor
        AvatarEditor::getSingletonPtr()->setCharacterInstance(characterInstance);

    characterInstance->getEntity()->setCastShadows(Navigator::getSingletonPtr()->getCastShadows());

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
        mCallbacks->onAvatarNodeCreate(peerAvatar);

    return peerAvatar;
}

//-------------------------------------------------------------------------------------

OgrePeer* OgrePeerManager::createSceneNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    bool isLocal = (xmlEntity->getOwner() == mNodeId);
    Scene* peerScene = new Scene(xmlEntity, isLocal);

    if (mCallbacks != 0)
        mCallbacks->onSceneNodeCreate(peerScene);

    return peerScene;
}

//-------------------------------------------------------------------------------------

OgrePeer* OgrePeerManager::createObjectNode(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    bool isLocal = (xmlEntity->getOwner() == mNodeId);

    // Create the object
    Object* peerObject = new Object(xmlEntity, isLocal);

    return peerObject;
}

//-------------------------------------------------------------------------------------
