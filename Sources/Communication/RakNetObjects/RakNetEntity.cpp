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
using namespace RakNetSolipsis;
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

#ifdef POOL
#else
    delete mXmlEntity;
#endif
}

//-------------------------------------------------------------------------------------
void RakNetEntity::addEntity(RakNetEntity* entity)
{
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
RakNetEntity* RakNetEntity::findByAddress(SystemAddress& systemAddress)
{
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddress() systemAddress:%s", systemAddress.ToString());
/*    std::string entitiesListStr;
    for (RakNetEntityMap::iterator it = ms_Entities.begin(); it != ms_Entities.end(); ++it)
    {
        entitiesListStr += it->second->mXmlEntity->getUid() + "(" + it->second->getSystemAddress().ToString() + ")";
        if (it != ms_Entities.end())
            entitiesListStr += ", ";
    }
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddress() entities:%s", entitiesListStr.c_str());*/
#endif

    for (RakNetEntityMap::iterator it = ms_Entities.begin(); it != ms_Entities.end(); ++it)
        if (it->second->getSystemAddress() == systemAddress)
            return (RakNetEntity*)it->second;

#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::findByAddress() systemAddress:%s not found !", systemAddress.ToString());
#endif
    return 0;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::deleteByAddress(SystemAddress& systemAddress)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::deleteByAddress() systemAddress:%s", systemAddress.ToString());

    RakNetEntity *entity = findByAddress(systemAddress);
    if (entity == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::deleteByAddress() systemAddress:%s not found !", systemAddress.ToString());
        return;
    }

    // Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
    entity->BroadcastDestruction();
    delete entity;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::SerializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::SerializeConstruction()");

    StringTable::Instance()->EncodeString("Entity", 128, bitStream);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::Serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::Serialize()");

    // case 1 : client serialize updated attributes, server relay them
    // case 2 : server serialize all defined attributes when it is not a relay (eg. construction)
    // case 3 : client serialize all defined attributes when it has just created the entity (eg. broadcast serialization to system)
    XmlEntity::DefinedAttributes definedAttributes = mLastDeserializedDefinedAttributes;
    if ((RakNetConnection::getSingletonPtr()->isServer() && (serializationContext->serializationType != RELAY_SERIALIZATION_TO_SYSTEMS)) || 
        (!RakNetConnection::getSingletonPtr()->isServer() && (serializationContext->serializationType == BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM)) ||
        (!RakNetConnection::getSingletonPtr()->isServer() && (serializationContext->serializationType == SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM)))
        definedAttributes = mXmlEntity->getDefinedAttributes();

    // client is resetting the defined/updated attributes for next updates
    if (!RakNetConnection::getSingletonPtr()->isServer())
        mLastDeserializedDefinedAttributes = XmlEntity::DAUid;

#ifdef LOGRAKNET
    if (definedAttributes != XmlEntity::DAUid)
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
            "RakNetEntity::Serialize() mSystemAddress:%s, uid:%s, name:%s, owner:%s, defAttr:0x%08x, ctxType:%d, recip@:%s, pos(%.2f,%.2f,%.2f)",
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
#ifdef POOL
    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();
#else
    XmlContent* xmlContent = mXmlEntity->getContent();
#endif
    if (definedAttributes & XmlEntity::DAContent)
    {
#ifdef POOL
        RefCntPoolPtr<XmlData> xmlContentDatas = xmlContent->getDatas();
        bool hasContentDatas = (xmlContentDatas != RefCntPoolPtr<XmlData>::nullPtr);
#else
        XmlData* xmlContentDatas = xmlContent->getDatas();
        bool hasContentDatas = (xmlContentDatas != 0);
#endif
        bitStream->Write(hasContentDatas);
        if (hasContentDatas)
            if (mXmlEntity->getType() == ETSite)
            {
#ifdef POOL
                RefCntPoolPtr<XmlSceneContent> xmlSceneContent = xmlContentDatas;
#else
                XmlSceneContent* xmlSceneContent = xmlContentDatas;
#endif
                bitStream->Write(xmlSceneContent->getEntryGate().mGravity);
                RakNetConnection::SerializeVector3(bitStream, xmlSceneContent->getEntryGate().mPosition);
            }
        XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
        bitStream->Write((unsigned int)contentLodMap.size());
        for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
        {
#ifdef POOL
            RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;
#else
            XmlLodContent* xmlLodContent = itl->second;
#endif
            bitStream->Write(xmlLodContent->getLevel());
#ifdef POOL
            RefCntPoolPtr<XmlData> xmlLodContentDatas = xmlLodContent->getDatas();
            bool hasLodContentDatas = (xmlLodContentDatas != RefCntPoolPtr<XmlData>::nullPtr);
#else
            XmlData* xmlLodContentDatas = xmlLodContent->getDatas();
            bool hasLodContentDatas = (xmlLodContentDatas != 0);
#endif
            bitStream->Write(hasLodContentDatas);
            if (hasLodContentDatas)
                if (mXmlEntity->getType() == ETSite)
                {
#ifdef POOL
                    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent = xmlLodContentDatas;
#else
                    XmlSceneLodContent* xmlSceneLodContent = xmlLodContentDatas;
#endif
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
void RakNetEntity::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::Deserialize()");

    // get defined/serialized attributes
    XmlEntity::DefinedAttributes definedAttributes;
    bitStream->Read(definedAttributes);
    mLastDeserializedDefinedAttributes = definedAttributes;

    if (definedAttributes & XmlEntity::DAUid)
    {
        EntityUID uid;
        RakNetConnection::DeserializeString(bitStream, uid);
        mXmlEntity->setUid(uid);
        // Server can serialize and compute visibility
        if (RakNetConnection::getSingletonPtr()->isServer())
            mReplicaFlags |= RFSerializationAuthorized | RFVisibilityAuthorized;
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
        "RakNetEntity::Deserialize() sender:%s, mSystemAddress:%s, uid:%s, name:%s, owner:%s, lastDeserAttr:0x%08x, pos(%.2f,%.2f,%.2f)",
        sender.ToString(), mSystemAddress.ToString(),
        mXmlEntity->getUid().c_str(),
        mXmlEntity->getName().c_str(),
        mXmlEntity->getOwner().c_str(),
        mLastDeserializedDefinedAttributes,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().x : -1,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().y : -1,
        (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().z : -1);
#endif

    if (definedAttributes & XmlEntity::DAContent)
    {
#ifdef POOL
        RefCntPoolPtr<XmlContent> xmlContent;
#else
        XmlContent* xmlContent = new XmlContent();
#endif
        bool hasContentDatas;
        bitStream->Read(hasContentDatas);
        if (hasContentDatas)
            if (mXmlEntity->getType() == ETSite)
            {
#ifdef POOL
                RefCntPoolPtr<XmlSceneContent> xmlSceneContent;
#else
                XmlSceneContent* xmlSceneContent = new XmlSceneContent();
#endif
                EntryGateStruct entryGate;
                bitStream->Read(entryGate.mGravity);
                RakNetConnection::DeserializeVector3(bitStream, entryGate.mPosition);
                xmlSceneContent->setEntryGate(entryGate);
#ifdef POOL
                xmlContent->setDatas(RefCntPoolPtr<XmlData>(xmlSceneContent));
#else
                xmlContent->setDatas(xmlSceneContent);
#endif
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
#ifdef POOL
                    RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent;
#else
                    XmlSceneLodContent* xmlSceneLodContent = new XmlSceneLodContent();
#endif
                    std::string mainFilename;
                    RakNetConnection::DeserializeString(bitStream, mainFilename);
                    xmlSceneLodContent->setMainFilename(mainFilename);
                    std::string collision;
                    RakNetConnection::DeserializeString(bitStream, collision);
                    xmlSceneLodContent->setCollision(collision);
#ifdef POOL
                    xmlLodContent->setDatas(RefCntPoolPtr<XmlData>(xmlSceneLodContent));
#else
                    xmlLodContent->setDatas(xmlSceneLodContent);
#endif
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
bool RakNetEntity::QueryIsConstructionAuthority(void) const
{
    bool result = Replica2::QueryIsConstructionAuthority();
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::QueryIsConstructionAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
    return result;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::QueryIsDestructionAuthority(void) const
{
    bool result;
    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        result = mReplicaFlags & RFSerializationAuthorized;
    else
//        result = Replica2::QueryIsDestructionAuthority();
        result = false;
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::QueryIsDestructionAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
    return result;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::QueryIsVisibilityAuthority(void) const
{
    bool result;
    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        result = mReplicaFlags & RFVisibilityAuthorized;
    else
        result = Replica2::QueryIsVisibilityAuthority();
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::QueryIsVisibilityAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
    return result;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::QueryIsSerializationAuthority(void) const
{
    bool result;
    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        result = mReplicaFlags & RFSerializationAuthorized;
    else
        result = Replica2::QueryIsSerializationAuthority();
#ifdef LOGRAKNET
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
//        "RakNetEntity::QueryIsSerializationAuthority() uid:%s, returning %s", mXmlEntity->getUid().c_str(), LOGHANDLER_LOGBOOL(result));
#endif
	return mReplicaFlags & RFSerializationAuthorized;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::addFilesInCacheManager()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetEntity::addFilesInCacheManager()");

#ifdef POOL
    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();
#else
    XmlContent* xmlContent = mXmlEntity->getContent();
#endif

    if (!(mXmlEntity->getDefinedAttributes() & XmlEntity::DAContent))
        return;

    XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
    for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
    {
#ifdef POOL
        RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;
#else
        XmlLodContent* xmlLodContent = itl->second;
#endif
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

#ifdef POOL
    RefCntPoolPtr<XmlContent> xmlContent = mXmlEntity->getContent();
#else
    XmlContent* xmlContent = mXmlEntity->getContent();
#endif

    if (!(mXmlEntity->getDefinedAttributes() & XmlEntity::DAContent))
        return;
    
    XmlContent::ContentLodMap& contentLodMap = xmlContent->getContentLodMap();
    for(XmlContent::ContentLodMap::const_iterator itl = contentLodMap.begin(); itl != contentLodMap.end(); ++itl)
    {

#ifdef POOL
        RefCntPoolPtr<XmlLodContent> xmlLodContent = itl->second;
#else
        XmlLodContent* xmlLodContent = itl->second;
#endif

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

/** See CacheManagerCallback. */
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
