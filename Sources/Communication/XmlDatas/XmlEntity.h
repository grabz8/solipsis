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

#ifndef __XmlEntity_h__
#define __XmlEntity_h__

#include "XmlContent.h"

namespace Solipsis 
{
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
        static const DefinedAttributes DADownloadProgress = (DefinedAttributes)(DAAABoundingBox << 1);
        static const DefinedAttributes DAUploadProgress = (DefinedAttributes)(DADownloadProgress << 1);
        static const DefinedAttributes DAContent = (DefinedAttributes)(DAUploadProgress << 1);

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
        float mUploadProgress;

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
            mDownloadProgress(1.0f),
            mUploadProgress(1.0f),
            mShape(RefCntPoolPtr<XmlData>::nullPtr),
            mContent(RefCntPoolPtr<XmlContent>::nullPtr)
        {}

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

        void setDownloadProgress(const float progress) { mDownloadProgress = progress; mDefinedAttributes |= DADownloadProgress; }
        const float getDownloadProgress() { return mDownloadProgress; }

        void setUploadProgress(const float progress) { mUploadProgress = progress; mDefinedAttributes |= DAUploadProgress; }
        const float getUploadProgress() { return mUploadProgress; }

        void setContent(RefCntPoolPtr<XmlContent>& content) { mContent = content; mDefinedAttributes |= DAContent; }
        RefCntPoolPtr<XmlContent>& getContent() { return mContent; }
    };

    RefCntPoolPtr<XmlEntity> RefCntPoolPtr<XmlEntity>::nullPtr((XmlEntity*)0);
} // namespace Solipsis

#endif // #ifndef __XmlEntity_h__
