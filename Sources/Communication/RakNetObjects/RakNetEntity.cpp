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

#include "RakNetEntity.h"
#include "RakNetConnection.h"
#include <StringTable.h>
#include <CTLog.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

// Map of entities
RakNetEntity::RakNetEntityMap RakNetEntity::ms_Entities;

//-------------------------------------------------------------------------------------
RakNetEntity::RakNetEntity() :
    mReplicaFlags(RFNone),
    mLastDeserializedDefinedAttributes(XmlEntity::DANone),
    mTotalNbfiles(0)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::RakNetEntity()");
}

//-------------------------------------------------------------------------------------
RakNetEntity::~RakNetEntity()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::~RakNetEntity()");
}

//-------------------------------------------------------------------------------------
void RakNetEntity::addEntity(RakNetEntity* entity)
{
    RakNetEntityMap::const_iterator it = ms_Entities.find(entity->getXmlEntity()->getUid());
    if (it != ms_Entities.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "RakNetEntity::addEntity() Entity already present, ignoring ...");
        return;
    }

    ms_Entities[entity->getXmlEntity()->getUid()] = entity;
    entity->onNewEntity();
}

//-------------------------------------------------------------------------------------
void RakNetEntity::removeEntity(RakNetEntity* entity)
{
    entity->onLostEntity();
    ms_Entities.erase(entity->getXmlEntity()->getUid());
}

//-------------------------------------------------------------------------------------
void RakNetEntity::cleanUpEntities()
{
    for(RakNetEntityMap::const_iterator it = ms_Entities.begin(); it != ms_Entities.end(); it = ms_Entities.begin())
    {
        RakNetEntity *entity = it->second;
        delete entity;
    }
}

//-------------------------------------------------------------------------------------
RakNetEntity* RakNetEntity::findByAddressAndType(SystemAddress& systemAddress, EntityType type)
{
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddressAndType() systemAddress:%s, type:%s", systemAddress.ToString(), XmlHelpers::convertEntityTypeToRepr(type).c_str());
/*    std::string entitiesListStr;
    for (RakNetEntityMap::iterator it = ms_Entities.begin(); it != ms_Entities.end(); ++it)
    {
        entitiesListStr += it->second->mXmlEntity->getUid() + "(" + it->second->getSystemAddress().ToString() + ")";
        if (it != ms_Entities.end())
            entitiesListStr += ", ";
    }
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddressAndType() entities:%s", entitiesListStr.c_str());*/
#endif

    for (RakNetEntityMap::iterator it = ms_Entities.begin(); it != ms_Entities.end(); ++it)
    {
        RakNetEntity *entity = it->second;
        if ((entity->getSystemAddress() == systemAddress) &&
            (entity->getXmlEntity()->getDefinedAttributes() & XmlEntity::DAType) &&
            (entity->getXmlEntity()->getType() == ETAvatar))
            return entity;
    }

#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddressAndType() systemAddress:%s, type:%s not found !", systemAddress.ToString(), XmlHelpers::convertEntityTypeToRepr(type).c_str());
#endif
    return 0;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::deleteByAddressAndType(SystemAddress& systemAddress, EntityType type)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::deleteByAddressAndType() systemAddress:%s, type:%s", systemAddress.ToString(), XmlHelpers::convertEntityTypeToRepr(type).c_str());

    RakNetEntity *entity = findByAddressAndType(systemAddress, type);
    if (entity == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::deleteByAddressAndType() systemAddress:%s not found !", systemAddress.ToString());
        return;
    }
    RakNetConnection::getSingletonPtr()->getReplicationManager()->removeReplica(entity);
    delete entity;
}

//-------------------------------------------------------------------------------------
const ReplicaUid& RakNetEntity::getUid() const
{
    return mXmlEntity->getUid();
}

//-------------------------------------------------------------------------------------
void RakNetEntity::setUid(const ReplicaUid& uid)
{
    mXmlEntity->setUid(uid);
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::serializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
#ifdef LOGRAKNET
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::serializeConstruction()");
#endif

    StringTable::Instance()->EncodeString("Entity", 128, bitStream);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::serialize()");

    // case 1 : client serialize updated attributes, server (authority) relay them
    // case 2 : server (authority) serialize all defined attributes when it is not a relay (eg. construction)
    // case 3 : client serialize all defined attributes when it has just created the entity (eg. broadcast serialization to system)
    XmlEntity::DefinedAttributes definedAttributes = mLastDeserializedDefinedAttributes;
    if ((mReplicationManager->isAuthority() && (serializationContext->serializationType != ST_RELAY_SERIALIZATION_TO_SYSTEMS)) || 
        (!mReplicationManager->isAuthority() && (serializationContext->serializationType == ST_SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM)))
        definedAttributes = mXmlEntity->getDefinedAttributes();

    // client is resetting the defined/updated attributes for next updates
    if (!mReplicationManager->isAuthority())
        mLastDeserializedDefinedAttributes = XmlEntity::DAUid;

#ifdef LOGRAKNET
    if (definedAttributes != XmlEntity::DAUid)
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
            "RakNetEntity::serialize() mSystemAddress:%s, uid:%s, name:%s, owner:%s, defAttr:0x%08x, ctxType:%d, recip@:%s, pos(%.2f,%.2f,%.2f)",
            mSystemAddress.ToString(),
            mXmlEntity->getUid().c_str(),
            mXmlEntity->getName().c_str(),
            mXmlEntity->getOwner().c_str(),
            definedAttributes,
            serializationContext->serializationType,
            serializationContext->recipientAddress.ToString(),
            (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().x : -1,
            (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().y : -1,
            (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().z : -1);
#endif

    bitStream->Write(definedAttributes);
    if (definedAttributes & XmlEntity::DAUid)
        RakNetConnection::SerializeString(bitStream, mXmlEntity->getUid());
    if (definedAttributes & XmlEntity::DAOwner)
        RakNetConnection::SerializeString(bitStream, mXmlEntity->getOwner());
    if (definedAttributes & XmlEntity::DAType)
        bitStream->Write(mXmlEntity->getType());
    if (definedAttributes & XmlEntity::DAName)
        RakNetConnection::SerializeString(bitStream, mXmlEntity->getName());
    if (definedAttributes & XmlEntity::DAVersion)
        bitStream->Write(mXmlEntity->getVersion());
    if (definedAttributes & XmlEntity::DAFlags)
        bitStream->Write(mXmlEntity->getFlags());
    if (definedAttributes & XmlEntity::DADisplacement)
        RakNetConnection::SerializeVector3(bitStream, mXmlEntity->getDisplacement());
    if (definedAttributes & XmlEntity::DAPosition)
        RakNetConnection::SerializeVector3(bitStream, mXmlEntity->getPosition());
    if (definedAttributes & XmlEntity::DAOrientation)
        RakNetConnection::SerializeQuaternion(bitStream, mXmlEntity->getOrientation());
    if (definedAttributes & XmlEntity::DAAnimation)
        bitStream->Write(mXmlEntity->getAnimation());
    if (definedAttributes & XmlEntity::DAAABoundingBox)
    {
        RakNetConnection::SerializeVector3(bitStream, mXmlEntity->getAABoundingBox().getMinimum());
        RakNetConnection::SerializeVector3(bitStream, mXmlEntity->getAABoundingBox().getMaximum());
    }
    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();
    if (definedAttributes & XmlEntity::DAContent)
    {
        RefCntPoolPtr<XmlData> xmlContentDatas = xmlContent->getDatas();
        bool hasContentDatas = (xmlContentDatas != RefCntPoolPtr<XmlData>::nullPtr);
        bitStream->Write(hasContentDatas);
        if (hasContentDatas)
            if (mXmlEntity->getType() == ETSite)
            {
                RefCntPoolPtr<XmlSceneContent> xmlSceneContent = xmlContentDatas;
                bitStream->Write(xmlSceneContent->getEntryGate().mGravity);
                RakNetConnection::SerializeVector3(bitStream, xmlSceneContent->getEntryGate().mPosition);
            }
        XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
        bitStream->Write((unsigned int)contentLodMap.size());
        for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
        {
            RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;
            bitStream->Write(xmlLodContent->getLevel());
            RefCntPoolPtr<XmlData> xmlLodContentDatas = xmlLodContent->getDatas();
            bool hasLodContentDatas = (xmlLodContentDatas != RefCntPoolPtr<XmlData>::nullPtr);
            bitStream->Write(hasLodContentDatas);
            if (hasLodContentDatas)
                if (mXmlEntity->getType() == ETSite)
                {
                    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent = xmlLodContentDatas;
                    RakNetConnection::SerializeString(bitStream, xmlSceneLodContent->getMainFilename());
                    RakNetConnection::SerializeString(bitStream, xmlSceneLodContent->getCollision());
                }
            XmlLodContent::LodContentFileList& lodContentFileList = xmlLodContent->getLodContentFileList();
            bitStream->Write((unsigned int)lodContentFileList.size());
            for(XmlLodContent::LodContentFileList::const_iterator itf = lodContentFileList.begin(); itf != lodContentFileList.end(); ++itf)
            {
                RakNetConnection::SerializeString(bitStream, itf->mFilename);
                bitStream->Write(itf->mVersion);
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::deserialize()");

    // First serialize after replica construction ?
    if (serializationType == ST_SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM)
        mReplicaFlags |= RFReady;

    // get defined/serialized attributes
    XmlEntity::DefinedAttributes definedAttributes;
    bitStream->Read(definedAttributes);
    mLastDeserializedDefinedAttributes = definedAttributes;

    if (definedAttributes & XmlEntity::DAUid)
    {
        EntityUID uid;
        RakNetConnection::DeserializeString(bitStream, uid);
        mXmlEntity->setUid(uid);
    }
    if (definedAttributes & XmlEntity::DAOwner)
    {
        NodeId owner;
        RakNetConnection::DeserializeString(bitStream, owner);
        mXmlEntity->setOwner(owner);
    }
    if (definedAttributes & XmlEntity::DAType)
    {
        EntityType type;
        bitStream->Read(type);
        mXmlEntity->setType(type);
    }
    if (definedAttributes & XmlEntity::DAName)
    {
        std::string name;
        RakNetConnection::DeserializeString(bitStream, name);
        mXmlEntity->setName(name);
    }
    if (definedAttributes & XmlEntity::DAVersion)
    {
        EntityVersion version;
        bitStream->Read(version);
        mXmlEntity->setVersion(version);
    }
    if (definedAttributes & XmlEntity::DAFlags)
    {
        EntityFlags flags;
        bitStream->Read(flags);
        mXmlEntity->setFlags(flags);
    }
    if (definedAttributes & XmlEntity::DADisplacement)
    {
        Ogre::Vector3 displacement;
        RakNetConnection::DeserializeVector3(bitStream, displacement);
        mXmlEntity->setDisplacement(displacement);
    }
    if (definedAttributes & XmlEntity::DAPosition)
    {
        Ogre::Vector3 position;
        RakNetConnection::DeserializeVector3(bitStream, position);
        mXmlEntity->setPosition(position);
    }
    if (definedAttributes & XmlEntity::DAOrientation)
    {
        Ogre::Quaternion orientation;
        RakNetConnection::DeserializeQuaternion(bitStream, orientation);
        mXmlEntity->setOrientation(orientation);
    }
    if (definedAttributes & XmlEntity::DAAnimation)
    {
        AnimationState animationState;
        bitStream->Read(animationState);
        mXmlEntity->setAnimation(animationState);
    }
    if (definedAttributes & XmlEntity::DAAABoundingBox)
    {
        Ogre::Vector3 min;
        Ogre::Vector3 max;
        RakNetConnection::DeserializeVector3(bitStream, min);
        RakNetConnection::DeserializeVector3(bitStream, max);
        Ogre::AxisAlignedBox AABoundingBox(min, max);
        mXmlEntity->setAABoundingBox(AABoundingBox);
    }

#ifdef LOGRAKNET
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "RakNetEntity::deserialize() sender:%s, mSystemAddress:%s, uid:%s, defAttr:0x%08x, name:%s, owner:%s, lastDeserAttr:0x%08x, mReplFlags:0x%08x, pos(%.2f,%.2f,%.2f)",
        sender.ToString(), mSystemAddress.ToString(),
        mXmlEntity->getUid().c_str(),
        definedAttributes,
        mXmlEntity->getName().c_str(),
        mXmlEntity->getOwner().c_str(),
        mLastDeserializedDefinedAttributes,
        mReplicaFlags,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().x : -1,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().y : -1,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().z : -1);
#endif

    if (definedAttributes & XmlEntity::DAContent)
    {
        RefCntPoolPtr<XmlContent> xmlContent;
        bool hasContentDatas;
        bitStream->Read(hasContentDatas);
        if (hasContentDatas)
            if (mXmlEntity->getType() == ETSite)
            {
                RefCntPoolPtr<XmlSceneContent> xmlSceneContent;
                EntryGateStruct entryGate;
                bitStream->Read(entryGate.mGravity);
                RakNetConnection::DeserializeVector3(bitStream, entryGate.mPosition);
                xmlSceneContent->setEntryGate(entryGate);
                xmlContent->setDatas(RefCntPoolPtr<XmlData>(xmlSceneContent));
            }

        XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
        contentLodMap.clear();
        unsigned int contentLodMapSize;
        bitStream->Read(contentLodMapSize);
        for (;contentLodMapSize > 0; --contentLodMapSize)
        {
            RefCntPoolPtr<XmlLodContent> xmlLodContent;
            Lod lod;
            bitStream->Read(lod);
            xmlLodContent->setLevel(lod);
            bool hasLodContentDatas;
            bitStream->Read(hasLodContentDatas);
            if (hasLodContentDatas)
                if (mXmlEntity->getType() == ETSite)
                {
                    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent;
                    std::string mainFilename;
                    RakNetConnection::DeserializeString(bitStream, mainFilename);
                    xmlSceneLodContent->setMainFilename(mainFilename);
                    std::string collision;
                    RakNetConnection::DeserializeString(bitStream, collision);
                    xmlSceneLodContent->setCollision(collision);
                    xmlLodContent->setDatas(RefCntPoolPtr<XmlData>(xmlSceneLodContent));
                }
            XmlLodContent::LodContentFileList& lodContentFileList = xmlLodContent->getLodContentFileList();
            lodContentFileList.clear();
            unsigned int lodContentFileListSize;
            bitStream->Read(lodContentFileListSize);
            for (;lodContentFileListSize > 0; --lodContentFileListSize)
            {
                LodContentFileStruct lodContentFileStruct;
                RakNetConnection::DeserializeString(bitStream, lodContentFileStruct.mFilename);
                bitStream->Read(lodContentFileStruct.mVersion);
                lodContentFileList.push_back(lodContentFileStruct);
            }
            contentLodMap[xmlLodContent->getLevel()] = xmlLodContent;
        }

        mXmlEntity->setContent(xmlContent);

        requestFilesFromCacheManager(sender);
    }
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::queryIsReady() const
{
    bool result = (mReplicaFlags & RFReady);
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
//        "RakNetEntity::queryIsReady() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
	return result;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::queryIsConstructionAuthority() const
{
    bool result;
    if (mReplicaFlags & RFConstructionAuthorized)
        result = true;
    else
        result = Replica::queryIsConstructionAuthority();
#ifdef LOGRAKNET
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "RakNetEntity::queryIsConstructionAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
	return result;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::queryIsSerializationAuthority() const
{
    bool result;
    if (mReplicaFlags & RFSerializationAuthorized)
        result = true;
    else
        result = Replica::queryIsSerializationAuthority();
#ifdef LOGRAKNET
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "RakNetEntity::queryIsSerializationAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
	return result;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::addFilesInCacheManager()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::addFilesInCacheManager()");

    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();

    if (!(mXmlEntity->getDefinedAttributes() & XmlEntity::DAContent))
        return;

    XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
    for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
    {
        RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;
        XmlLodContent::LodContentFileList& lodContentFileList = xmlLodContent->getLodContentFileList();
        for(XmlLodContent::LodContentFileList::const_iterator itf = lodContentFileList.begin(); itf != lodContentFileList.end(); ++itf)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::addFilesInCacheManager() adding file %s", itf->mFilename.c_str());
            RakNetConnection::getSingletonPtr()->getCacheManager()->addFile(itf->mFilename, itf->mVersion);
        }
    }
}

//-------------------------------------------------------------------------------------
void RakNetEntity::requestFilesFromCacheManager(const SystemAddress& sender)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::requestFilesFromCacheManager()");

    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();

    if (!(mXmlEntity->getDefinedAttributes() & XmlEntity::DAContent))
        return;
    
    XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
    for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
    {

        RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;

        XmlLodContent::LodContentFileList& lodContentFileList = xmlLodContent->getLodContentFileList();
        for (XmlLodContent::LodContentFileList::const_iterator itf = lodContentFileList.begin(); itf != lodContentFileList.end(); ++itf)
        {
            mTotalNbfiles++;
            mMissingFiles.push_back(*itf);
        }

        for (XmlLodContent::LodContentFileList::const_iterator itf = lodContentFileList.begin(); itf != lodContentFileList.end(); ++itf)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::requestFilesFromCacheManager() requesting file %s from %s", itf->mFilename.c_str(), sender.ToString());
            RakNetConnection::getSingletonPtr()->getCacheManager()->requestFile(sender, itf->mFilename, itf->mVersion, this);
        }
    }

}

//-------------------------------------------------------------------------------------
void RakNetEntity::onTransferComplete(const std::string& filename)
{
    for (XmlLodContent::LodContentFileList::iterator it = mMissingFiles.begin(); it != mMissingFiles.end(); ++it)
    {
        if (it->mFilename == filename)
        {
            mMissingFiles.erase(it);
            return;
        }
    }
}

//-------------------------------------------------------------------------------------
float RakNetEntity::onTransferProgress(const std::string& filename, float fProgress)
{
    float addedprogres = mTotalNbfiles - mMissingFiles.size();

    for (XmlLodContent::LodContentFileList::iterator it = mMissingFiles.begin(); it != mMissingFiles.end(); ++it)
    {
        if (it->mFilename == filename)
        {
            it->progress = fProgress;
            addedprogres += fProgress;
        }
        else
        {
            addedprogres += it->progress;
        }
    }

    return addedprogres/mTotalNbfiles;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
