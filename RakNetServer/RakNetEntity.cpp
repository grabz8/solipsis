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
#include "RakNetAvatarNode.h"
#include "RakNetConnection.h"
#include <StringTable.h>

using namespace RakNet;

namespace Solipsis {

//-------------------------------------------------------------------------------------
RakNetEntity::RakNetEntity() :
    mReplicaFlags(RFNone),
    mLastDeserializedDefinedAttributes(XmlEntity::DANone)
{
    RakNetConnection::getSingleton()->logMessage("RakNetEntity::RakNetEntity()");
}

//-------------------------------------------------------------------------------------
RakNetEntity::~RakNetEntity()
{
    RakNetConnection::getSingleton()->logMessage("RakNetEntity::~RakNetEntity()");

#ifdef POOL
#else
    delete mXmlEntity;
#endif
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::SerializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
    RakNetConnection::getSingleton()->logMessage("RakNetEntity::SerializeConstruction()");

    StringTable::Instance()->EncodeString("Entity", 128, bitStream);

//    RakNetConnection::getSingleton()->mFileListTransfer.Send(fileList, RakNetConnection::getSingleton()->mRakPeer, serializationContext->recipientAddress, , , false);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::Serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetEntity::Serialize()");

    // case 1 : client serialize updated attributes, server relay them
    // case 2 : server serialize all defined attributes when it is not a relay (eg. construction)
    // case 3 : client serialize all defined attributes when it has just created the entity (eg. broadcast serialization to system)
    XmlEntity::DefinedAttributes definedAttributes = mLastDeserializedDefinedAttributes;
    if ((RakNetConnection::getSingleton()->mServer && (serializationContext->serializationType != RELAY_SERIALIZATION_TO_SYSTEMS)) || 
        (!RakNetConnection::getSingleton()->mServer && (serializationContext->serializationType == BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM)))
        definedAttributes = mXmlEntity->getDefinedAttributes();

    // client is resetting the defined/updated attributes for next updates
    if (!RakNetConnection::getSingleton()->mServer)
        mLastDeserializedDefinedAttributes = XmlEntity::DAUid;

#ifdef LOGRAKNET
    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetEntity::Serialize() uid:0x%08x, name:%s, owner:%s, defAttr:0x%08x, ctxType:%d, recip@:%s, pos(%.2f,%.2f,%.2f)", mXmlEntity->getUid(), mXmlEntity->getName().c_str(), mXmlEntity->getOwner().c_str(), definedAttributes, serializationContext->serializationType, serializationContext->recipientAddress.ToString(), (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().x : -1, (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().y : -1, (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().z : -1);
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
#endif

    bitStream->Write(definedAttributes);
    if (definedAttributes & XmlEntity::DAUid)
        bitStream->Write(mXmlEntity->getUid());
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
    bool hasContent = (xmlContent != RefCntPoolPtr<XmlContent>::nullPtr);
#else
    XmlContent* xmlContent = mXmlEntity->getContent();
    bool hasContent = (xmlContent != 0);
#endif
    bitStream->Write(hasContent);
    if (hasContent)
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
                RakNetConnection::SerializeString(bitStream, itf->filename);
                bitStream->Write(itf->version);
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
void RakNetEntity::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetEntity::Deserialize()");

    // get defined/serialized attributes
    XmlEntity::DefinedAttributes definedAttributes;
    bitStream->Read(definedAttributes);
    mLastDeserializedDefinedAttributes = definedAttributes;

    if (definedAttributes & XmlEntity::DAUid)
    {
        EntityUID uid;
        bitStream->Read(uid);
        mXmlEntity->setUid(uid);
        /// Server can serialize
        if (RakNetConnection::getSingleton()->mServer)
            mReplicaFlags |= RFSerializationAuthorized;
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
    bool hasContent;
    bitStream->Read(hasContent);
    if (hasContent)
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
                XmlSceneContent::EntryGateStruct entryGate;
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
                XmlLodContent::LodContentFileStruct lodContentFileStruct;
                RakNetConnection::DeserializeString(bitStream, lodContentFileStruct.filename);
                bitStream->Read(lodContentFileStruct.version);
                lodContentFileList.push_back(lodContentFileStruct);
            }
            contentLodMap[xmlLodContent->getLevel()] = xmlLodContent;
        }

        mXmlEntity->setContent(xmlContent);
    }

#ifdef LOGRAKNET
    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetEntity::Deserialize() uid:0x%08x, name:%s, owner:%s, lastDeserAttr:0x%08x, pos(%.2f,%.2f,%.2f)", mXmlEntity->getUid(), mXmlEntity->getName().c_str(), mXmlEntity->getOwner().c_str(), mLastDeserializedDefinedAttributes, (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().x : -1, (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().y : -1, (definedAttributes & XmlEntity::DAPosition) ? mXmlEntity->getPosition().z : -1);
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
#endif
}

//-------------------------------------------------------------------------------------
bool RakNetEntity::QueryIsSerializationAuthority(void) const
{
#ifdef LOGRAKNET
    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetEntity::QueryIsSerializationAuthority() uid:0x%08x, returning %s", mXmlEntity->getUid(), (mReplicaFlags & RFSerializationAuthorized) ? "true" : "false");
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
#endif
	return mReplicaFlags & RFSerializationAuthorized;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
