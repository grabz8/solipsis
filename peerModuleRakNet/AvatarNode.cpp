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

#include "AvatarNode.h"
#include "Peer.h"
#include "OgreHelpers.h"

using namespace RakNet;
using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
AvatarNode::AvatarNode() :
    RakNetAvatarNode(),
    Node()
{
    OGRE_LOG("AvatarNode::AvatarNode()");
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
    pthread_mutex_lock(&mMutex);

    if (!mFrozen)
        Peer::getSingleton().removeTimeListener(this);

    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
void AvatarNode::onNewEntity(Entity* entity, bool sendNewEvt)
{
    if (entity->getXmlEntity()->getOwner() == mNodeId)
    {
        OGRE_LOG("AvatarNode::onNewEntity() new owned entity uid:" + entity->getXmlEntity()->getUidString() + " by me with mNodeId:" + mNodeId);
        mOwnedEntities[entity->getXmlEntity()->getUid()] = entity;

        // Entity can now be updated by our avatar node and serialized
        entity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // entity is now owned, next serailize from client will be updates (DAUid by default)
        entity->setLastDeserializedDefinedAttributes(XmlEntity::DAUid);
        // For instance we only auto-update avatar entity
        if (entity->getXmlEntity()->getType() == ETAvatar)
            entity->AddAutoSerializeTimer(1000/20); // 20 ups
    }

    // Create physics of scene + my avatar
    if ((entity->getXmlEntity()->getType() == ETSite) ||
        ((entity->getXmlEntity()->getType() == ETAvatar) && (entity->getXmlEntity()->getOwner() == mNodeId)))
    {
        pthread_mutex_lock(&mMutex);
#ifdef PHYSICSPLUGINS
        // create physics of the entity
        OGRE_LOG("AvatarNode::onNewEntity() creating physics of entity uid:" + entity->getXmlEntity()->getUidString());
        entity->createPhysics(Peer::getSingleton().getPhysicsScene());
        entity->applyGravity(true);
#endif
        pthread_mutex_unlock(&mMutex);
    }

    if (sendNewEvt)
    {
#ifdef POOL
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETNewEntity);
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(entity->getXmlEntity()));
#else
        XmlEvt* xmlEvt = new XmlEvt(ETNewEntity);
        xmlEvt->setDatas(entity->getXmlEntity());
#endif
        pthread_mutex_lock(&mEvtsMutex);
        mEvtsToHandleList.push_back(xmlEvt);
        pthread_mutex_unlock(&mEvtsMutex);
    }
}

//-------------------------------------------------------------------------------------
void AvatarNode::onUpdatedEntity(Entity* entity)
{
    if (entity->getLastDeserializedDefinedAttributes() == XmlEntity::DAUid)
    {
//        OGRE_LOG("AvatarNode::onUpdatedEntity() only DAUid so no ETUpdatedEntity evt sent to navigator !");
        return;
    }
#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETUpdatedEntity);
    RefCntPoolPtr<XmlEntity> xmlEntity;
    xmlEntity->setDefinedAttributes(entity->getLastDeserializedDefinedAttributes());
    XmlEntity::copyEntityDefinedAttributes(entity->getXmlEntity(), xmlEntity);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
    XmlEvt* xmlEvt = new XmlEvt(ETUpdatedEntity);
    XmlEntity* xmlEntity = new XmlEntity();
    xmlEntity->setDefinedAttributes(entity->getLastDeserializedDefinedAttributes());
    XmlEntity::copyEntityDefinedAttributes(entity->getXmlEntity(), xmlEntity);
    xmlEvt->setDatas(entity->getXmlEntity());
#endif
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);
}

//-------------------------------------------------------------------------------------
void AvatarNode::onLostEntity(Entity* entity)
{
#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETLostEntity);
    RefCntPoolPtr<XmlEntity> xmlEntity;
    xmlEntity->setUid(entity->getXmlEntity()->getUid());
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));
#else
    XmlEvt* xmlEvt = new XmlEvt(ETLostEntity);
    XmlEntity* xmlEntity = new XmlEntity();
    xmlEntity->setUid(entity->getXmlEntity()->getUid());
    xmlEvt->setDatas(entity->getXmlEntity());
#endif
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);
}

//-------------------------------------------------------------------------------------
void AvatarNode::onActionOnEntity(BitStream *bitStream)
{
    ActionType actionType;
    bitStream->Read(actionType);
    EntityUID sourceEntityUid;
    bitStream->Read(sourceEntityUid);
    EntityUID targetEntityUid;
    bitStream->Read(targetEntityUid);
    std::string desc;
    RakNetConnection::DeserializeString(bitStream, desc);
#ifdef POOL
    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETActionOnEntity);
    RefCntPoolPtr<XmlAction> xmlAction;
    xmlAction->setType(actionType);
    xmlAction->setSourceEntityUid(sourceEntityUid);
    xmlAction->setTargetEntityUid(targetEntityUid);
    xmlAction->setDesc(desc);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlAction));
#else
    XmlEvt* xmlEvt = new XmlEvt();
    xmlEvt->setType(xmlEvt->getType());
    XmlEvt* xmlAction = new XmlAction();
    xmlAction->setType(actionType);
    xmlAction->setSourceEntityUid(sourceEntityUid);
    xmlAction->setTargetEntityUid(targetEntityUid);
    xmlAction->setDesc(desc);
    xmlEvt->setDatas(xmlAction);
#endif
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::isOwnedEntity(Entity* entity)
{
    Entity::EntityMap::const_iterator it = mOwnedEntities.find(entity->getXmlEntity()->getUid());
    return (it != mOwnedEntities.end());
}

//-------------------------------------------------------------------------------------
void AvatarNode::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
    RakNetAvatarNode::Deserialize(bitStream, serializationType, sender, timestamp);

    if (mIsLocal && mNodeId.empty())
    {
        OGRE_LOG("AvatarNode::Deserialize() assigning nodeId and name");
        mNodeId = Peer::getSingleton().getNodeId();
        mName = Peer::getSingleton().getName();
        // Setup FileListTransfer by allowing the server to send us files
        mFileListTransferSetID = RakNetConnection::getSingleton()->mFileListTransfer.SetupReceive(&mFileListTransferCallback, false, sender);
        Peer::getSingleton().getNodeManager()->addNode(mNodeId, this);
        BroadcastSerialize();

        // Call onNewEntity on entities received previously
        Entity::EntityMap &entities = Entity::getEntities();
        for (Entity::EntityMap::const_iterator it = entities.begin(); it != entities.end(); ++it)
        {
            Entity *entity = it->second;
            onNewEntity(entity, true);
        }
     }

    // Unfreeze avatar node
    if (mIsLocal && (mEntity != 0))
        freeze(false);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::QueryIsSerializationAuthority(void) const
{
	// Client can update its local avatar node, otherwise only server can do it.
    bool authorized = mIsLocal; // Uid set and has serialization authority
#ifdef LOGRAKNET
    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "AvatarNode::QueryIsSerializationAuthority() mNodeId:%s, returning %s", mNodeId.c_str(), authorized ? "true" : "false");
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
#endif
	return authorized;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::FileListTransferCallback::OnFile(OnFileStruct *onFileStruct)
{
    return true;
}

//-------------------------------------------------------------------------------------
void AvatarNode::FileListTransferCallback::OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength)
{
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool AvatarNode::processEvt(RefCntPoolPtr<XmlEvt>& xmlEvt, std::string& xmlRespStr)
#else
bool AvatarNode::processEvt(XmlEvt* xmlEvt, std::string& xmlRespStr)
#endif
{
    static int c;
    static unsigned long l = (unsigned long)-1;
    unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
    if (l == (unsigned long)-1) { l = n; c = 0; }
    c++;
    if (n - l > 10000)
    {
        Real fr = (Real)c/10.0f;
        OGRE_LOG("AvatarNode::processEvt() fr=" + StringConverter::toString(fr));
        l = n; c = 0;
    }

    if (xmlEvt->getType() == ETUpdatedEntity)
    {
#ifdef POOL
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt->getDatas().get();
#else
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt->getDatas();
#endif
        if (xmlEntity == 0)
        {
            xmlRespStr = "No entity found in event !";
            return false;
        }
        Entity* entity = (Entity*)mEntity;
        if (entity == 0)
            return true;
        pthread_mutex_lock(&mMutex);
        if (entity->getXmlEntity()->getType() == ETAvatar)
        {
            XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
            if (definedAttributes & XmlEntity::DAFlags)
            {
                EntityFlags diff = entity->getXmlEntity()->getFlags() ^ xmlEntity->getFlags();
                if (diff & EFGravity)
                    entity->setGravity(xmlEntity->getFlags() & EFGravity);
                entity->getXmlEntity()->setFlags(xmlEntity->getFlags());
            }
            if (definedAttributes & XmlEntity::DADisplacement)
            {
                entity->getXmlEntity()->setDisplacement(xmlEntity->getDisplacement());
            }
            if (definedAttributes & XmlEntity::DAOrientation)
            {
                entity->getXmlEntity()->setOrientation(xmlEntity->getOrientation());
                entity->addLastDeserializedDefinedAttributes(XmlEntity::DAOrientation);
            }
            if (definedAttributes & XmlEntity::DAAnimation)
            {
                entity->getXmlEntity()->setAnimation(xmlEntity->getAnimation());
                entity->addLastDeserializedDefinedAttributes(XmlEntity::DAAnimation);
            }
            if (definedAttributes & XmlEntity::DAContent)
            {
                entity->getXmlEntity()->setContent(xmlEntity->getContent());
                entity->addLastDeserializedDefinedAttributes(XmlEntity::DAContent);
            }
#ifdef LOGSNDRCV
            String log = "RCV uid:" + xmlEntity->getUidString();
            if (definedAttributes & XmlEntity::DADisplacement) log += " d:" + StringConverter::toString(xmlEntity->getDisplacement());
            OGRE_LOG(log);
#endif
        }
        pthread_mutex_unlock(&mMutex);
    }
    else if (xmlEvt->getType() == ETNewEntity)
    {
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity = (RefCntPoolPtr<XmlEntity>)xmlEvt->getDatas();
        if (xmlEntity.isNull())
#else
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt->getDatas();
        if (xmlEntity == 0)
#endif
        {
            xmlRespStr = "No entity found in event !";
            return false;
        }
        if (xmlEntity->getType() != ETObject)
        {
            xmlRespStr = "Only object entity can be added !";
            return false;
        }
        // Create object node
        xmlEntity->setOwner(mNodeId);
//        ObjectNode* objectNode = Peer::getSingleton().getNodeManager()->createObjectNode(xmlEntity);
        Entity* entity = new Entity();
        entity->setXmlEntity(xmlEntity);
        Entity::addEntity(entity, false);
        // In order to use any networked member functions of Replica2, you must first call SetReplicaManager
        entity->SetReplicaManager(&RakNetConnection::getSingleton()->mReplicaManager);
        // Tell the user to automatically serialize our data members every 100 milliseconds (if changed)
        // This way if we change the system address or the Soldier* we don't have to call user->BroadcastSerialize();
        //    entity->AddAutoSerializeTimer(1000);
        // Send out this new user to all systems. Unlike the old system (ReplicaManager) all sends are done immediately.
        entity->BroadcastConstruction();
    }
    else if (xmlEvt->getType() == ETActionOnEntity)
    {
#ifdef POOL
        XmlAction* xmlAction = (XmlAction*)xmlEvt->getDatas().get();
#else
        XmlAction* xmlAction = (XmlAction*)xmlEvt->getDatas();
#endif
        if (xmlAction == 0)
        {
            xmlRespStr = "No action found in event !";
            return false;
        }
        BitStream bitStream;
        bitStream.Write((MessageID)RakNetConnection::ID_ACTION_ON_ENTITY);
        bitStream.Write(xmlAction->getType());
        bitStream.Write(xmlAction->getSourceEntityUid());
        bitStream.Write(xmlAction->getTargetEntityUid());
        RakNetConnection::SerializeString(&bitStream, xmlAction->getDesc());
        // Send the action to the server, it will look at source/target to broadcast/send to target(s)
        RakNetConnection::getSingleton()->mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, RakNetConnection::getSingleton()->mServerSystemAddress, false);
    }

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool AvatarNode::freeEvt(RefCntPoolPtr<XmlEvt>& xmlEvt)
#else
bool AvatarNode::freeEvt(XmlEvt* xmlEvt)
#endif
{
    pthread_mutex_lock(&mEvtsMutex);
    Entity* entity = (Entity*)mEntity;
    if (entity != 0)
    {
#ifdef POOL
        if (xmlEvt->getDatas() == entity->mUpdatedXmlEntity)
            entity->mUpdatedXmlEntity->setDefinedAttributes(XmlEntity::DANone);
#else
        if (xmlEvt->getDatas() == &entity->mUpdatedXmlEntity)
            entity->mUpdatedXmlEntity.setDefinedAttributes(XmlEntity::DANone);
#endif
    }
    pthread_mutex_unlock(&mEvtsMutex);

    return Node::freeEvt(xmlEvt);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::freeze(bool frozen)
{
    pthread_mutex_lock(&mMutex);
    if (frozen != mFrozen)
    {
        OGRE_LOG("AvatarNode::freeze() " + String(frozen ? "freezing" : "unfreezing"));
        if (frozen)
            Peer::getSingleton().removeTimeListener(this);
        else
            Peer::getSingleton().addTimeListener(this);
        mFrozen = frozen;
    }
    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::tick(Real timeSinceLastTick)
{
    pthread_mutex_lock(&mMutex);

    // Update entities
    for (Entity::EntityMap::iterator obj = mOwnedEntities.begin();obj != mOwnedEntities.end();++obj)
        obj->second->update(timeSinceLastTick);

//    pthread_mutex_unlock(&mMutex);

    Entity* entity = (Entity*)mEntity;
static int c=0;
c++;
    if (entity->mDirty && ((c % 3) == 0)) // 20 ups
//    if ((entity != 0) && entity->mDirty)
    {
        pthread_mutex_lock(&mEvtsMutex);
#ifdef POOL
        if (!entity->mUpdatedXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        {
            RefCntPoolPtr<XmlEvt> xmlEvt;
            xmlEvt->setType(ETUpdatedEntity);
            entity->mUpdatedXmlEntity->setUid(entity->getXmlEntity()->getUid());
            entity->mUpdatedXmlEntity->setPosition(entity->getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            OGRE_LOG("SND uid:" + entity->getXmlEntity()->getUidString() + " p:" + StringConverter::toString(entity->getXmlEntity()->getPosition()));
#endif
            xmlEvt->setDatas(RefCntPoolPtr<XmlData>(entity->mUpdatedXmlEntity));
#else
        if (!entity->mUpdatedXmlEntity.getDefinedAttributes() & XmlEntity::DAUid)
        {
            XmlEvt* xmlEvt = new XmlEvt(ETUpdatedEntity);
            entity->mUpdatedXmlEntity.setUid(entity->getXmlEntity()->getUid());
            entity->mUpdatedXmlEntity.setPosition(entity->getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            OGRE_LOG("SND uid:" + entity->getXmlEntity()->getUidString() + " p:" + StringConverter::toString(entity->getXmlEntity()->getPosition()));
#endif
            xmlEvt->setDatas(&entity->mUpdatedXmlEntity);
#endif
            mEvtsToHandleList.push_back(xmlEvt);
            entity->addLastDeserializedDefinedAttributes(XmlEntity::DAPosition);
            entity->mDirty = false;
        }
        pthread_mutex_unlock(&mEvtsMutex);
    }

    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
