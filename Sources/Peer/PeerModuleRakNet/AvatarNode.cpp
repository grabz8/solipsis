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
#include <XmlAction.h>

using namespace RakNet;
using namespace Ogre;
using namespace CommonTools;

namespace Solipsis {

    //-------------------------------------------------------------------------------------
    AvatarNode::AvatarNode() :
mMutex(PTHREAD_MUTEX_INITIALIZER),
mEvtsMutex(PTHREAD_MUTEX_INITIALIZER),
mFrozen(true),
mConnectionLost(false),
Node("avatar"),
mLastSiteUid(""),
mEntity(0)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::AvatarNode()");
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::~AvatarNode()");

    pthread_mutex_lock(&mMutex);

    //    if (!mFrozen)
    // CF there was a crash when emptying the peer time listener list
    Peer::getSingleton().removeTimeListener(this);

    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::loadFromElt(TiXmlElement* nodeElt)
{
    // Load the entity
    TiXmlElement* entityElt = nodeElt->FirstChildElement("entity");
    if (entityElt == 0)
        return false;
    Entity* entity = Peer::getSingletonPtr()->loadEntity(entityElt);
    if (entity == 0)
        return false;
    if (entity->getXmlEntity()->getType() != ETAvatar)
        return false;
    mEntity = entity;

    // Load last site uid where avatar was connected ?
    TiXmlElement* lastSiteUidElt = nodeElt->FirstChildElement("lastSiteUid");
    if (lastSiteUidElt != 0)
        mLastSiteUid = lastSiteUidElt->GetText();

    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* AvatarNode::getSavedElt()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::getSavedElt() saving entity of avatar node with nodeId:%s", mNodeId.c_str());

    if (mEntity == 0)
        return 0;

    // Get root node
    TiXmlElement* nodeElt = Node::getSavedElt();
    // Add avatar entity
    mEntity->getXmlEntity()->toXmlElt(*nodeElt);

    // Add last site uid where avatar was connected
    TiXmlElement* lastSiteUidElt = new TiXmlElement("lastSiteUid");
    TiXmlText* lastSiteUid = new TiXmlText(mLastSiteUid.c_str());
    lastSiteUidElt->LinkEndChild(lastSiteUid);
    nodeElt->LinkEndChild(lastSiteUidElt); 

    return nodeElt;
}

//-------------------------------------------------------------------------------------
void AvatarNode::onNewEntity(Entity* entity)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() new entity uid:%s dirty:%s", entity->getXmlEntity()->getUid().c_str(), LOGHANDLER_LOGBOOL(entity->mDirty));

    if (entity->getXmlEntity()->getOwner() == mNodeId)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() new owned entity uid:%s by me with mNodeId:%s", entity->getXmlEntity()->getUid().c_str(), mNodeId.c_str());
        pthread_mutex_lock(&mMutex);
        mOwnedEntities[entity->getXmlEntity()->getUid()] = entity;
        pthread_mutex_unlock(&mMutex);

        // Entity can now be updated by our avatar node and serialized
        entity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // entity is now owned, next serialize from client will be updates (DAUid by default)
        entity->setLastDeserializedDefinedAttributes(XmlEntity::DAUid);
        // For instance we only auto-update avatar entity
        if (entity->getXmlEntity()->getType() == ETAvatar)
            entity->addAutoSerializeTimer(1000/20); // 20 ups
    }

    // Create physics of my avatar
    if ((entity->getXmlEntity()->getType() == ETAvatar) && (entity->getXmlEntity()->getOwner() == mNodeId))
    {
        pthread_mutex_lock(&mMutex);
        // create physics of the entity
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() creating physics of entity uid:%s", entity->getXmlEntity()->getUid().c_str());
        entity->createPhysics(Peer::getSingleton().getPhysicsScene());
        entity->applyGravity(true);
        pthread_mutex_unlock(&mMutex);
    }
    // Create physics of scene (if content is available)
    if ((entity->getXmlEntity()->getType() == ETSite) &&
        (entity->getXmlEntity()->getDefinedAttributes() & XmlEntity::DAContent) &&
        (entity->getXmlEntity()->getDownloadProgress() >= 1.0f))
    {
        pthread_mutex_lock(&mMutex);
        // create physics of the entity
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() creating physics of entity uid:%s", entity->getXmlEntity()->getUid().c_str());
        entity->createPhysics(Peer::getSingleton().getPhysicsScene());
        entity->applyGravity(true);
        pthread_mutex_unlock(&mMutex);
    }

    if (entity->mDirty)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() sending ETNewEntity for entity uid:%s", entity->getXmlEntity()->getUid().c_str());
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETNewEntity);
        if (entity->getXmlEntity()->getDefinedAttributes() & XmlEntity::DAFlags)
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() ETNewEntity evt sent uid=%s flags=%08x", entity->getXmlEntity()->getUid().c_str(), entity->getXmlEntity()->getFlags());
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(entity->getXmlEntity()));

        pthread_mutex_lock(&mEvtsMutex);
        mEvtsToHandleList.push_back(xmlEvt);
        pthread_mutex_unlock(&mEvtsMutex);
    }

    if ((entity->getXmlEntity()->getType() == ETSite) && (mEntity != 0))
    {
        // Reset avatar orientation and displacement
        RefCntPoolPtr<XmlEntity> xmlEntity = mEntity->getXmlEntity();

        xmlEntity->setDisplacement(Ogre::Vector3::ZERO);
        if (entity->getXmlEntity()->getUid() == mLastSiteUid)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() Resetting avatar: orientation, displacement, position with latest values");
        }
        else
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() Resetting avatar: orientation, displacement, position on entryGate");
            mLastSiteUid = entity->getXmlEntity()->getUid();
            xmlEntity->setOrientation(Ogre::Quaternion::IDENTITY);
            // Move avatar on the entry gate of the scene + gravity
            RefCntPoolPtr<XmlSceneContent> xmlSceneContent = entity->getXmlEntity()->getContent()->getDatas();
            // Randomize position
            int rand = time(NULL)%9;
            xmlEntity->setPosition(xmlSceneContent->getEntryGate().mPosition + Ogre::Vector3(rand/3 - 1, 0, rand%3 - 1));
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() Creating physics, applying gravity, unfreezing");
            if (xmlSceneContent->getEntryGate().mGravity)
                xmlEntity->setFlags(xmlEntity->getFlags() | EFGravity);
            else
                xmlEntity->setFlags(xmlEntity->getFlags() & ~EFGravity);
        }
        mEntity->addLastDeserializedDefinedAttributes(XmlEntity::DAFlags);
        mEntity->addLastDeserializedDefinedAttributes(XmlEntity::DAPosition);
        mEntity->addLastDeserializedDefinedAttributes(XmlEntity::DAOrientation);
        // Here we re-create character physics, we could only reset character position instead of
        ((Entity*)mEntity)->createPhysics(Peer::getSingleton().getPhysicsScene());
        ((Entity*)mEntity)->applyGravity(true);
        onUpdatedEntity((Entity*)mEntity);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onNewEntity() unfreezing");
        freeze(false);
    }
}

//-------------------------------------------------------------------------------------
void AvatarNode::onUpdatedEntity(Entity* entity)
{
    if (entity->getLastDeserializedDefinedAttributes() == XmlEntity::DAUid)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onUpdatedEntity() only DAUid so no ETUpdatedEntity evt sent to navigator !");
        return;
    }

    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETUpdatedEntity);
    RefCntPoolPtr<XmlEntity> xmlEntity;
    xmlEntity->setDefinedAttributes(entity->getLastDeserializedDefinedAttributes());
    XmlEntity::copyEntityDefinedAttributes(entity->getXmlEntity(), xmlEntity);
    if (xmlEntity->getDefinedAttributes() & XmlEntity::DAFlags)
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onUpdatedEntity() ETUpdatedEntity evt sent uid=%s flags=%08x", xmlEntity->getUid().c_str(), xmlEntity->getFlags());
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

    // Create physics of scene (if content is available)
    if ((entity->getXmlEntity()->getType() == ETSite) &&
        (entity->getXmlEntity()->getDefinedAttributes() & XmlEntity::DAContent) &&
        (entity->getXmlEntity()->getDownloadProgress() >= 1.0f) &&
        ((entity->getPhysicsScene() == 0) || !entity->getPhysicsScene()->hasTerrainmesh()))
    {
        pthread_mutex_lock(&mMutex);
        // create physics of the entity
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onUpdatedEntity() creating physics of entity uid:%s", entity->getXmlEntity()->getUid().c_str());
        entity->createPhysics(Peer::getSingleton().getPhysicsScene());
        entity->applyGravity(true);
        pthread_mutex_unlock(&mMutex);
    }

    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);
}

//-------------------------------------------------------------------------------------
void AvatarNode::onLostEntity(Entity* entity)
{
    // me ?
    if (entity == mEntity)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::onLostEntity() me ! freezing");
        freeze(true);
    }

    pthread_mutex_lock(&mMutex);
    RakNetEntity::RakNetEntityMap::iterator it = mOwnedEntities.find(entity->getXmlEntity()->getUid());
    if (it != mOwnedEntities.end())
        mOwnedEntities.erase(it);
    pthread_mutex_unlock(&mMutex);

    // me ?
    if (entity == mEntity)
        mEntity = 0;

    if (entity->mDirty)
    {
        RefCntPoolPtr<XmlEvt> xmlEvt;
        xmlEvt->setType(ETLostEntity);
        RefCntPoolPtr<XmlEntity> xmlEntity;
        xmlEntity->setUid(entity->getXmlEntity()->getUid());
        xmlEntity->setType(entity->getXmlEntity()->getType());
        xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlEntity));

        pthread_mutex_lock(&mEvtsMutex);
        mEvtsToHandleList.push_back(xmlEvt);
        pthread_mutex_unlock(&mEvtsMutex);
    }
}

//-------------------------------------------------------------------------------------
void AvatarNode::onActionOnEntity(BitStream *bitStream)
{
    ActionType actionType;
    bitStream->Read(actionType);
    EntityUID sourceEntityUid;
    RakNetConnection::DeserializeString(bitStream, sourceEntityUid);
    EntityUID targetEntityUid;
    RakNetConnection::DeserializeString(bitStream, targetEntityUid);
    bool broadcast;
    bitStream->Read(broadcast);
    std::string desc;
    RakNetConnection::DeserializeString(bitStream, desc);

    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETActionOnEntity);
    RefCntPoolPtr<XmlAction> xmlAction;
    xmlAction->setType(actionType);
    xmlAction->setSourceEntityUid(sourceEntityUid);
    xmlAction->setTargetEntityUid(targetEntityUid);
    xmlAction->setBroadcast(broadcast);
    xmlAction->setDesc(XmlHelpers::convertUTF8ToWString("WCHAR_T", desc));
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlAction));

    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::isOwnedEntity(Entity* entity)
{
    RakNetEntity::RakNetEntityMap::const_iterator it = mOwnedEntities.find(entity->getXmlEntity()->getUid());
    return (it != mOwnedEntities.end());
}

//-------------------------------------------------------------------------------------

bool AvatarNode::processEvt(RefCntPoolPtr<XmlEvt>& xmlEvt, std::string& xmlRespStr)
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
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt->getDatas().get();
        if (xmlEntity == 0)
        {
            xmlRespStr = "No entity found in event !";
            return false;
        }
        if (xmlEntity->getType() == ETAvatar)
        {
            Entity* entity = (Entity*)mEntity;
            if (entity == 0)
                return true;
            pthread_mutex_lock(&mMutex);
            XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
            if (definedAttributes & XmlEntity::DAFlags)
            {
                EntityFlags diff = entity->getXmlEntity()->getFlags() ^ xmlEntity->getFlags();
                if (diff & EFGravity)
                    entity->setGravity(xmlEntity->getFlags() & EFGravity);
                if (diff & EFGravity) { LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RCV uid:%s setGravity(%s)", xmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(xmlEntity->getFlags() & EFGravity)); }
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
            String log = "RCV uid:" + xmlEntity->getUid();
            if (definedAttributes & XmlEntity::DADisplacement) log += " d:" + StringConverter::toString(xmlEntity->getDisplacement());
            LOGHANDLER_LOG(LogHandler::VL_DEBUG, log.c_str());
#endif
            pthread_mutex_unlock(&mMutex);
        }
        else if (xmlEntity->getType() == ETObject)
        {
            RakNetEntity::RakNetEntityMap& entities = Entity::getEntities();
            Entity* entity = (Entity*)entities[xmlEntity->getUid()];
            if (entity == 0)
                return true;
            XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
            if (definedAttributes & XmlEntity::DAContent)
            {
                entity->getXmlEntity()->setContent(xmlEntity->getContent());
                entity->addLastDeserializedDefinedAttributes(XmlEntity::DAContent);
                // add new files into the cache manager
                entity->addFilesInCacheManager();
                entity->broadcastSerialize();
            }
        }
    }
    else if (xmlEvt->getType() == ETNewEntity)
    {

        RefCntPoolPtr<XmlEntity> xmlEntity = (RefCntPoolPtr<XmlEntity>)xmlEvt->getDatas();
        if (xmlEntity.isNull())
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
        entity->setSystemAddress(RakNetConnection::getSingletonPtr()->getMySystemAddress());
        entity->setXmlEntity(xmlEntity);
        // add new files into the cache manager
        entity->addFilesInCacheManager();
        entity->mDirty = false;
        // Entity is ready, owner client can construct/serialize it
        entity->addReplicaFlags(RakNetEntity::RFReady | RakNetEntity::RFConstructionAuthorized | RakNetEntity::RFSerializationAuthorized);
        // Send out this new entity to all systems
        RakNetConnection::getSingletonPtr()->getReplicationManager()->addReplica(entity);
        Entity::addEntity(entity);
    }
    else if (xmlEvt->getType() == ETLostEntity)
    {
        // Entity was deleted
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt->getDatas().get();
        if (xmlEntity == 0)
        {
            xmlRespStr = "No entity found in event !";
            return false;
        }
        if (xmlEntity->getType() == ETObject)
        {
            RakNetEntity::RakNetEntityMap& entities = Entity::getEntities();
            Entity* entity = (Entity*)entities[xmlEntity->getUid()];
            if (entity == 0)
                return true;
            entity->mDirty = false;
            Entity::removeEntity(entity);
            pthread_mutex_lock(&mMutex);
            mOwnedEntities.erase(entity->getXmlEntity()->getUid());
            pthread_mutex_unlock(&mMutex);
            RakNetConnection::getSingletonPtr()->getReplicationManager()->removeReplica(entity);
            delete entity;
        }
    }
    else if (xmlEvt->getType() == ETActionOnEntity)
    {

        XmlAction* xmlAction = (XmlAction*)xmlEvt->getDatas().get();
        if (xmlAction == 0)
        {
            xmlRespStr = "No action found in event !";
            return false;
        }
        BitStream bitStream;
        bitStream.Write((MessageID)RakNetConnection::ID_ACTION_ON_ENTITY);
        bitStream.Write(xmlAction->getType());
        RakNetConnection::SerializeString(&bitStream, xmlAction->getSourceEntityUid());
        RakNetConnection::SerializeString(&bitStream, xmlAction->getTargetEntityUid());
        bitStream.Write(xmlAction->getBroadcast());
        RakNetConnection::SerializeString(&bitStream, XmlHelpers::convertWStringToUTF8("WCHAR_T", xmlAction->getDesc()));
        // Send the action to the server, it will look at source/target to broadcast/send to target(s)
        RakNetConnection::getSingletonPtr()->getRakPeer()->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, RakNetConnection::getSingletonPtr()->getServerSystemAddress(), false);
    }

    return true;
}

//-------------------------------------------------------------------------------------

RefCntPoolPtr<XmlEvt> AvatarNode::getNextEvtToHandle()
{
    RefCntPoolPtr<XmlEvt> xmlEvt(RefCntPoolPtr<XmlEvt>::nullPtr);

    pthread_mutex_lock(&mEvtsMutex);
    if (!mEvtsToHandleList.empty())
    {
        xmlEvt = mEvtsToHandleList.front();
        mEvtsToHandleList.pop_front();
    }
    pthread_mutex_unlock(&mEvtsMutex);

    return xmlEvt;
}

//-------------------------------------------------------------------------------------

bool AvatarNode::freeEvt(RefCntPoolPtr<XmlEvt>& xmlEvt)
{
    pthread_mutex_lock(&mEvtsMutex);
    Entity* entity = (Entity*)mEntity;
    if (entity != 0)
    {
        if (xmlEvt->getDatas() == entity->mUpdatedXmlEntity)
            entity->mUpdatedXmlEntity->setDefinedAttributes(XmlEntity::DANone);
    }
    pthread_mutex_unlock(&mEvtsMutex);

    return true;
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

    mFrozen = frozen;

    return true;
}

//-------------------------------------------------------------------------------------
void AvatarNode::setConnectionLost(bool lost)
{
    if (mConnectionLost == lost) return;
    mConnectionLost = lost;

    // Send connection lost event to navigator

    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(lost ? ETConnectionLost : ETConnectionRestored);

    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(xmlEvt);
    pthread_mutex_unlock(&mEvtsMutex);

    if (!lost) return;

    // Destroy other avatar entities
    RakNetEntity::RakNetEntityMap entities = RakNetEntity::getEntities();
    std::list<RakNetEntity*> avatars;
    for (RakNetEntity::RakNetEntityMap::iterator it = entities.begin(); it != entities.end(); ++it)
        if ((it->second->getXmlEntity()->getType() == ETAvatar) &&
            (mEntity != 0) && (it->second->getXmlEntity()->getUid() != mEntity->getXmlEntity()->getUid()))
            avatars.push_back(it->second);
    for (std::list<RakNetEntity*>::iterator a = avatars.begin(); a != avatars.end(); ++a)
        RakNetEntity::removeEntity(*a);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::tick(Real timeSinceLastTick)
{
    pthread_mutex_lock(&mMutex);

    // Update entities
    for (RakNetEntity::RakNetEntityMap::iterator obj = mOwnedEntities.begin();obj != mOwnedEntities.end();++obj)
    {
        Entity* entity = (Entity*)obj->second;
        entity->update(timeSinceLastTick);
    }

    Entity* entity = (Entity*)mEntity;
    static int c=0;
    c++;
    if ((entity != 0) && entity->mDirty && ((c % 3) == 0)) // 20 ups
        //    if ((entity != 0) && entity->mDirty)
    {
        pthread_mutex_lock(&mEvtsMutex);

        if (!entity->mUpdatedXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        {
            RefCntPoolPtr<XmlEvt> xmlEvt;
            xmlEvt->setType(ETUpdatedEntity);
            entity->mUpdatedXmlEntity->setUid(entity->getXmlEntity()->getUid());
            entity->mUpdatedXmlEntity->setPosition(entity->getXmlEntity()->getPosition());
            if (entity->mUpdatedXmlEntity->getDefinedAttributes() & XmlEntity::DAFlags)
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::tick() ETUpdatedEntity evt sent uid=%s flags=%08x", entity->mUpdatedXmlEntity->getUid().c_str(), entity->mUpdatedXmlEntity->getFlags());
#ifdef LOGSNDRCV
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SND uid:%s p:%s", entity->getXmlEntity()->getUid().c_str(), StringConverter::toString(entity->getXmlEntity()->getPosition()).c_str());
#endif
            xmlEvt->setDatas(RefCntPoolPtr<XmlData>(entity->mUpdatedXmlEntity));

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
