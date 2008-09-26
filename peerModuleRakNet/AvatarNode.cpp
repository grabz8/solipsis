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
#include <CTLog.h>

using namespace RakNet;
using namespace Ogre;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
AvatarNode::AvatarNode() :
    RakNetAvatarNode(),
    Node(),
    mAvatarEntity(0)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::AvatarNode()");
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::~AvatarNode()");

    pthread_mutex_lock(&mMutex);

    if (!mFrozen)
        Peer::getSingleton().removeTimeListener(this);

    pthread_mutex_unlock(&mMutex);

    if (mAvatarEntity != 0)
        delete mAvatarEntity;
}

//-------------------------------------------------------------------------------------
void AvatarNode::onNewEntity(Entity* entity, bool sendNewEvt)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() new entity uid:%s sendNewEvt:%s", entity->getXmlEntity()->getUid().c_str(), LOGHANDLER_LOGBOOL(sendNewEvt));

    if (entity->getXmlEntity()->getOwner() == mNodeId)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() new owned entity uid:%s by me with mNodeId:%s", entity->getXmlEntity()->getUid().c_str(), mNodeId.c_str());
        pthread_mutex_lock(&mMutex);
        mOwnedEntities[entity->getXmlEntity()->getUid()] = entity;
        pthread_mutex_unlock(&mMutex);

        // My avatar entity ?
        if (entity->getXmlEntity()->getType() == ETAvatar)
            mAvatarEntity = entity;

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
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() creating physics of entity uid:%s", entity->getXmlEntity()->getUid().c_str());
        entity->createPhysics(Peer::getSingleton().getPhysicsScene());
        entity->applyGravity(true);
#endif
        pthread_mutex_unlock(&mMutex);
    }

    if (sendNewEvt)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() sending ETNewEntity for entity uid:%s", entity->getXmlEntity()->getUid().c_str());
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

    // Unfreeze avatar node
    if (mIsLocal && (mAvatarEntity != 0))
        freeze(false);
}

//-------------------------------------------------------------------------------------
void AvatarNode::onUpdatedEntity(Entity* entity)
{
    if (entity->getLastDeserializedDefinedAttributes() == XmlEntity::DAUid)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onUpdatedEntity() only DAUid so no ETUpdatedEntity evt sent to navigator !");
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
void AvatarNode::onLostEntity(Entity* entity, bool sendLostEvt)
{
    if (!sendLostEvt) return;
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
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::Deserialize() assigning nodeId and name");
        mNodeId = Peer::getSingleton().getNodeId();
        mName = Peer::getSingleton().getName();
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
}

//-------------------------------------------------------------------------------------
bool AvatarNode::QueryIsSerializationAuthority(void) const
{
	// Client can update its local avatar node, otherwise only server can do it.
    bool authorized = mIsLocal; // Uid set and has serialization authority
#ifdef LOGRAKNET
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "AvatarNode::QueryIsSerializationAuthority() mNodeId:%s, returning %s",
        mNodeId.c_str(),
        LOGHANDLER_LOGBOOL(authorized));
#endif
	return authorized;
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
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::processEvt() fr=%.2f", fr);
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
        if (xmlEntity->getType() == ETAvatar)
        {
            Entity* entity = (Entity*)mAvatarEntity;
            if (entity == 0)
                return true;
            pthread_mutex_lock(&mMutex);
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
                // add new files into the cache manager
                entity->addFilesInCacheManager();
            }
#ifdef LOGSNDRCV
            String log = "RCV uid:" + xmlEntity->getUidString();
            if (definedAttributes & XmlEntity::DADisplacement) log += " d:" + StringConverter::toString(xmlEntity->getDisplacement());
            LOGHANDLER_LOG(LogHandler::VL_DEBUG, log.c_str());
#endif
            pthread_mutex_unlock(&mMutex);
        }
        else if (xmlEntity->getType() == ETObject)
        {
            Entity::EntityMap& entities = Entity::getEntities();
            Entity* entity = entities[xmlEntity->getUid()];
            if (entity == 0)
                return true;
            XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
            if (definedAttributes & XmlEntity::DAContent)
            {
                entity->getXmlEntity()->setContent(xmlEntity->getContent());
                entity->addLastDeserializedDefinedAttributes(XmlEntity::DAContent);
                // add new files into the cache manager
                entity->addFilesInCacheManager();
                entity->BroadcastSerialize();
            }
        }
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
        // Create object entity
//        xmlEntity->setOwner(mNodeId);
        Entity* entity = new Entity();
        entity->setXmlEntity(xmlEntity);
        // add new files into the cache manager
        entity->addFilesInCacheManager();
        Entity::addEntity(entity, false);
        // In order to use any networked member functions of Replica2, you must first call SetReplicaManager
        entity->SetReplicaManager(&RakNetConnection::getSingletonPtr()->mReplicaManager);
        // Send out this new user to all systems. Unlike the old system (ReplicaManager) all sends are done immediately.
        entity->BroadcastConstruction();
    }
    else if (xmlEvt->getType() == ETLostEntity)
    {
        // Entity was deleted
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
        if (xmlEntity->getType() == ETObject)
        {
            Entity::EntityMap& entities = Entity::getEntities();
            Entity* entity = entities[xmlEntity->getUid()];
            if (entity == 0)
                return true;
            Entity::removeEntity(entity, false);
            pthread_mutex_lock(&mMutex);
            mOwnedEntities.erase(entity->getXmlEntity()->getUid());
            pthread_mutex_unlock(&mMutex);
            // Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
            entity->BroadcastDestruction();
            delete entity;
        }
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
        RakNetConnection::getSingletonPtr()->mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, RakNetConnection::getSingletonPtr()->mServerSystemAddress, false);
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
    Entity* entity = (Entity*)mAvatarEntity;
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
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::freeze() frozen:%s", LOGHANDLER_LOGBOOL(frozen));
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

    Entity* entity = (Entity*)mAvatarEntity;
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
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SND uid:%s p:%s", entity->getXmlEntity()->getUidString().c_str(), StringConverter::toString(entity->getXmlEntity()->getPosition()).c_str());
#endif
            xmlEvt->setDatas(RefCntPoolPtr<XmlData>(entity->mUpdatedXmlEntity));
#else
        if (!entity->mUpdatedXmlEntity.getDefinedAttributes() & XmlEntity::DAUid)
        {
            XmlEvt* xmlEvt = new XmlEvt(ETUpdatedEntity);
            entity->mUpdatedXmlEntity.setUid(entity->getXmlEntity()->getUid());
            entity->mUpdatedXmlEntity.setPosition(entity->getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SND uid:%s p:", entity->getXmlEntity()->getUidString().c_str(), StringConverter::toString(entity->getXmlEntity()->getPosition()).c_str());
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
