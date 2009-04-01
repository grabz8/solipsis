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

#ifndef __XmlDatas_h__
#define __XmlDatas_h__

#include <pthread.h>

#include <Ogre.h>
#include <list>
#include <map>
#include <ostream>

#include "XmlDatasPrerequisites.h"
#include "XmlDatasBasicTypes.h"
#include "PoolEntry.h"

#include "XmlHelpers.h"

namespace Solipsis 
{

    class XMLDATAS_EXPORT XmlData : public PoolEntry
    {
    public:
        virtual std::string toXmlString() const = 0;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const = 0;
        virtual bool fromXmlElt(TiXmlElement* xmlElt) = 0;

        /** Function for writing to a stream.
        */
        inline friend std::ostream& operator<<(std::ostream& o, const XmlData& data)
        {
            o << data.toXmlString();
            return o;
        }
    };

    class XMLDATAS_EXPORT XmlLogin : public XmlData
    {
    protected:
        static Pool mPool;

    protected:
        std::string mUsername;
        std::string mWorldHost;
        unsigned short mWorldPort;
        NodeId mNodeId;

    public:
        XmlLogin()
        {}
        XmlLogin(const std::string& username, const std::string& worldHost, unsigned short worldPort, const NodeId& nodeId) :
        mUsername(username),
            mWorldHost(worldHost),
            mWorldPort(worldPort),
            mNodeId(nodeId)
        {}

        static Pool& getStaticPool();
        virtual Pool& getPool() const;
        virtual void clear() {
            mUsername.clear();
            mWorldHost.clear();
            mWorldPort = 0;
            mNodeId.clear();
        }

        virtual std::string toXmlString() const;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const;
        virtual bool fromXmlElt(TiXmlElement* xmlElt);

        void setUsername(const std::string& username) { mUsername = username; }
        const std::string& getUsername() { return mUsername; }

        void setWorldHost(const std::string& worldHost) { mWorldHost = worldHost; }
        const std::string& getWorldHost() { return mWorldHost; }

        void setWorldPort(unsigned short worldPort) { mWorldPort = worldPort; }
        unsigned short getWorldPort() { return mWorldPort; }

        void setNodeId(const NodeId& nodeId) { mNodeId = nodeId; }
        const NodeId& getNodeId() { return mNodeId; }
    };

    class XMLDATAS_EXPORT XmlLodContent : public XmlData
    {

    protected:
        static Pool mPool;


    public:
        typedef std::list<LodContentFileStruct> LodContentFileList;

    protected:
        Lod mLevel;

        RefCntPoolPtr<XmlData> mDatas;
        LodContentFileList mLodContentFileList;

    public:
        XmlLodContent() :
          mDatas(RefCntPoolPtr<XmlData>::nullPtr)
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mLevel = 0;
              mDatas = RefCntPoolPtr<XmlData>::nullPtr;
              mLodContentFileList.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setLevel(Lod level) { mLevel = level; }
          Lod getLevel() { return mLevel; }


          void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
          RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }

          LodContentFileList& getLodContentFileList() { return mLodContentFileList; }
    };

    class XMLDATAS_EXPORT XmlSceneLodContent : public XmlData
    {

    protected:
        static Pool mPool;

    protected:
        std::string mMainFilename;
        std::string mCollision;

    public:
        XmlSceneLodContent() :
          mMainFilename(""),
              mCollision("")
          {}
          XmlSceneLodContent(const std::string& mainFilename, const std::string& collision) :
          mMainFilename(mainFilename),
              mCollision(collision)
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mMainFilename.clear();
              mCollision.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setMainFilename(const std::string& mainFilename) { mMainFilename = mainFilename; }
          const std::string& getMainFilename() { return mMainFilename; }

          void setCollision(const std::string& collision) { mCollision = collision; }
          const std::string& getCollision() { return mCollision; }
    };

    class XMLDATAS_EXPORT XmlContent : public XmlData
    {

    protected:
        static Pool mPool;


    public:
        typedef std::map<Lod, RefCntPoolPtr<XmlLodContent>> ContentLodMap;

    protected:

        RefCntPoolPtr<XmlData> mDatas;

        ContentLodMap mContentLodMap;

    public:
        XmlContent() :

          mDatas(RefCntPoolPtr<XmlData>::nullPtr)

          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mDatas = RefCntPoolPtr<XmlData>::nullPtr;
              mContentLodMap.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);


          void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
          RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }


          ContentLodMap& getContentLodMap() { return mContentLodMap; }
    };

    class XMLDATAS_EXPORT XmlSceneContent : public XmlData
    {

    protected:
        static Pool mPool;

    protected:
        EntryGateStruct mEntryGate;

    public:
        XmlSceneContent()
        {
            mEntryGate.mGravity = false;
            mEntryGate.mPosition = Ogre::Vector3::ZERO;
        }


        static Pool& getStaticPool();
        virtual Pool& getPool() const;
        virtual void clear() {
            mEntryGate.mGravity = false;
            mEntryGate.mPosition = Ogre::Vector3::ZERO;
        }


        virtual std::string toXmlString() const;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const;
        virtual bool fromXmlElt(TiXmlElement* xmlElt);

        void setEntryGate(const EntryGateStruct& entryGate) { mEntryGate = entryGate; }
        const EntryGateStruct& getEntryGate() { return mEntryGate; }
    };

    /**
    @brief	A serializable (in xml form) structure containing the state of an entity (avatar, object, ....)
    */
    class XMLDATAS_EXPORT XmlEntity : public XmlData
    {
    protected:
        static Pool mPool;

    public:
        typedef unsigned int DefinedAttributes;
        static const DefinedAttributes DANone = (DefinedAttributes)0;
        static const DefinedAttributes DAUid = (DefinedAttributes)1;
        static const DefinedAttributes DAOwner = (DefinedAttributes)(DAUid << 1);
        static const DefinedAttributes DAType = (DefinedAttributes)(DAOwner << 1);
        static const DefinedAttributes DAName = (DefinedAttributes)(DAType << 1);
        static const DefinedAttributes DAVersion = (DefinedAttributes)(DAName << 1);
        static const DefinedAttributes DAFlags = (DefinedAttributes)(DAVersion << 1);
        static const DefinedAttributes DADisplacement = (DefinedAttributes)(DAFlags << 1);
        static const DefinedAttributes DAPosition = (DefinedAttributes)(DADisplacement << 1);
        static const DefinedAttributes DAOrientation = (DefinedAttributes)(DAPosition << 1);
        static const DefinedAttributes DAAnimation = (DefinedAttributes)(DAOrientation << 1);
        static const DefinedAttributes DAAABoundingBox = (DefinedAttributes)(DAAnimation << 1);
        static const DefinedAttributes DAContent = (DefinedAttributes)(DAAABoundingBox << 1);
        static const DefinedAttributes DAProgress = (DefinedAttributes)(DAContent << 1);

    protected:
        DefinedAttributes mDefinedAttributes;
        EntityUID mUid;
        NodeId mOwner;
        EntityType mType;
        std::string mName;
        EntityVersion mVersion;
        EntityFlags mFlags;
        Ogre::Vector3 mDisplacement;
        Ogre::Vector3 mPosition;
        Ogre::Quaternion mOrientation;
        AnimationState mAnimationState;
        Ogre::AxisAlignedBox mAABoundingBox;
        float mDownloadProgress;


        RefCntPoolPtr<XmlData> mShape;
        RefCntPoolPtr<XmlContent> mContent;


    public:
        XmlEntity() :
          mDefinedAttributes(DANone),
              mUid(""),
              mOwner(""),
              mType(ETAvatar),
              mName(""),
              mVersion(0),
              mFlags(EFNone),
              mDisplacement(Ogre::Vector3::ZERO),
              mPosition(Ogre::Vector3::ZERO),
              mOrientation(Ogre::Quaternion::IDENTITY),
              mAnimationState(ASNone),
              mDownloadProgress(0),
              mShape(RefCntPoolPtr<XmlData>::nullPtr),
              mContent(RefCntPoolPtr<XmlContent>::nullPtr)
          {
          }


          XmlEntity(const EntityUID& uid) :
          mDefinedAttributes(DAUid),
              mUid(uid),
              mOwner(""),
              mType(ETAvatar),
              mName(""),
              mVersion(0),
              mFlags(EFNone),
              mDisplacement(Ogre::Vector3::ZERO),
              mPosition(Ogre::Vector3::ZERO),
              mOrientation(Ogre::Quaternion::IDENTITY),
              mAnimationState(ASNone),
              mShape(RefCntPoolPtr<XmlData>::nullPtr),
              mContent(RefCntPoolPtr<XmlContent>::nullPtr)
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mDefinedAttributes = DANone;
              mUid.clear();
              mOwner.clear();
              mType = ETAvatar;
              mName.clear();
              mVersion = 0;
              mFlags = EFNone;
              mDisplacement = Ogre::Vector3::ZERO;
              mPosition = Ogre::Vector3::ZERO;
              mOrientation = Ogre::Quaternion::IDENTITY;
              mAnimationState = ASNone;
              mShape = RefCntPoolPtr<XmlData>::nullPtr;
              mContent = RefCntPoolPtr<XmlContent>::nullPtr;
          }

          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          static void copyEntityDefinedAttributes(RefCntPoolPtr<XmlEntity>& srcXmlEntity, RefCntPoolPtr<XmlEntity>& dstXmlEntity);

          void setDefinedAttributes(const DefinedAttributes& definedAttributes) { mDefinedAttributes = definedAttributes; }
          DefinedAttributes getDefinedAttributes() { return mDefinedAttributes; }

          void setUid(const EntityUID& uid) { mUid = uid; mDefinedAttributes |= DAUid; }
          const EntityUID& getUid() { return mUid; }

          void setOwner(const NodeId& owner) { mOwner = owner; mDefinedAttributes |= DAOwner; }
          const NodeId& getOwner() { return mOwner; }

          void setType(const EntityType& type) { mType = type; mDefinedAttributes |= DAType; }
          EntityType getType() { return mType; }
          const std::string& getTypeRepr() { return XmlHelpers::convertEntityTypeToRepr(mType); }

          void setName(const std::string& name) { mName = name; mDefinedAttributes |= DAName; }
          const std::string& getName() { return mName; }

          void setVersion(const EntityVersion& version) { mVersion = version; mDefinedAttributes |= DAVersion; }
          const EntityVersion& getVersion() { return mVersion; }
          std::string getVersionString() { return XmlHelpers::convertEntityVersionToHexString(mVersion); }

          void setFlags(const EntityFlags& flags) { mFlags = flags; mDefinedAttributes |= DAFlags; }
          EntityFlags getFlags() { return mFlags; }
          std::string getFlagsString() { return XmlHelpers::convertEntityFlagsToHexString(mFlags); }
          std::string getFlagsRepr() { return XmlHelpers::convertEntityFlagsToRepr(mFlags); }

          void setDisplacement(const Ogre::Vector3& displacement) { mDisplacement = displacement; mDefinedAttributes |= DADisplacement; }
          const Ogre::Vector3& getDisplacement() { return mDisplacement; }

          void setPosition(const Ogre::Vector3& position) { mPosition = position; mDefinedAttributes |= DAPosition; }
          const Ogre::Vector3& getPosition() { return mPosition; }

          void setOrientation(const Ogre::Quaternion& orientation) { mOrientation = orientation; mDefinedAttributes |= DAOrientation; }
          const Ogre::Quaternion& getOrientation() { return mOrientation; }

          void setAnimation(AnimationState animationState) { mAnimationState = animationState; mDefinedAttributes |= DAAnimation; }
          AnimationState getAnimation() { return mAnimationState; }

          void setAABoundingBox(const Ogre::AxisAlignedBox& AABoundingBox) { mAABoundingBox = AABoundingBox; mDefinedAttributes |= DAAABoundingBox; }
          const Ogre::AxisAlignedBox& getAABoundingBox() { return mAABoundingBox; }

          void setDownloadProgress(const float progress) 
          { 
              mDownloadProgress = progress; 
              mDefinedAttributes |= DAProgress; 
          }
          const float getDownloadProgress() { return mDownloadProgress; }


          void setContent(RefCntPoolPtr<XmlContent>& content) 
          { 
              mContent = content; 
              // remove Content if progress is not 100%
              mDefinedAttributes |= DAContent; 
          }
          RefCntPoolPtr<XmlContent>& getContent() { return mContent; }
    };

    class XMLDATAS_EXPORT XmlAction : public XmlData
    {

    protected:
        static Pool mPool;


    protected:
        ActionType mType;
        EntityUID mSourceEntityUid;
        EntityUID mTargetEntityUid;
        bool mBroadcast;
        std::wstring mDesc;

    public:
        XmlAction() :
          mType(ATNone),
              mSourceEntityUid(""),
              mTargetEntityUid(""),
              mBroadcast(false),
              mDesc(L"")
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mType = ATNone;
              mSourceEntityUid.clear();
              mTargetEntityUid.clear();
              mBroadcast = false;
              mDesc.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setType(const ActionType& type) { mType = type; }
          ActionType getType() { return mType; }
          const std::string& getTypeRepr() { return XmlHelpers::convertActionTypeToRepr(mType); }

          void setSourceEntityUid(const EntityUID& sourceEntityUid) { mSourceEntityUid = sourceEntityUid; }
          const EntityUID& getSourceEntityUid() { return mSourceEntityUid; }

          void setTargetEntityUid(const EntityUID& targetEntityUid) { mTargetEntityUid = targetEntityUid; }
          const EntityUID& getTargetEntityUid() { return mTargetEntityUid; }

          void setBroadcast(bool broadcast) { mBroadcast = broadcast; }
          bool getBroadcast() { return mBroadcast; }

          void setDesc(const std::wstring& desc) { mDesc = desc; }
          const std::wstring& getDesc() { return mDesc; }
    };

    class XMLDATAS_EXPORT XmlEvt : public XmlData
    {

    protected:
        static Pool mPool;


    protected:
        EventType mType;

        RefCntPoolPtr<XmlData> mDatas;

    public:
        XmlEvt() :  mType(ETNewEntity),
                    mDatas(RefCntPoolPtr<XmlData>::nullPtr)
        {}

        XmlEvt(const EventType& type) :
                    mType(type),
                    mDatas(RefCntPoolPtr<XmlData>::nullPtr)
        {}

          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mType = ETNewEntity;
              mDatas = RefCntPoolPtr<XmlData>::nullPtr;
          }

          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setType(const EventType& type) { mType = type; }
          EventType getType() { return mType; }
          const std::string& getTypeRepr() { return XmlHelpers::convertEventTypeToRepr(mType); }

          void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
          RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }
    };


    RefCntPoolPtr<XmlData> RefCntPoolPtr<XmlData>::nullPtr((XmlData*)0);
    RefCntPoolPtr<XmlLogin> RefCntPoolPtr<XmlLogin>::nullPtr((XmlLogin*)0);
    RefCntPoolPtr<XmlLodContent> RefCntPoolPtr<XmlLodContent>::nullPtr((XmlLodContent*)0);
    RefCntPoolPtr<XmlSceneLodContent> RefCntPoolPtr<XmlSceneLodContent>::nullPtr((XmlSceneLodContent*)0);
    RefCntPoolPtr<XmlContent> RefCntPoolPtr<XmlContent>::nullPtr((XmlContent*)0);
    RefCntPoolPtr<XmlSceneContent> RefCntPoolPtr<XmlSceneContent>::nullPtr((XmlSceneContent*)0);
    RefCntPoolPtr<XmlEntity> RefCntPoolPtr<XmlEntity>::nullPtr((XmlEntity*)0);
    RefCntPoolPtr<XmlAction> RefCntPoolPtr<XmlAction>::nullPtr((XmlAction*)0);
    RefCntPoolPtr<XmlEvt> RefCntPoolPtr<XmlEvt>::nullPtr((XmlEvt*)0);


} // namespace Solipsis

#endif // #ifndef __XmlDatas_h__
