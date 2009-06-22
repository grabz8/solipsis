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
#include "XmlEntity.h"

namespace Solipsis {



Pool XmlEntity::mPool;


Pool& XmlEntity::getStaticPool() { return mPool; }
Pool& XmlEntity::getPool() const { return mPool; }

//-------------------------------------------------------------------------------------
std::string XmlEntity::toXmlString() const
{
    std::stringstream s;
    if (!mDefinedAttributes & DAUid) return s.str();

    s << "<entity uid=\"" << mUid << "\"";

    if (mDefinedAttributes & DAOwner) s << " owner=\"" << mOwner << "\"";
    if (mDefinedAttributes & DAType) s << " type=\"" << mType << "\"";
    if (mDefinedAttributes & DAName) s << " name=\"" << mName << "\"";
    if (mDefinedAttributes & DAVersion) s << " version=\"" << XmlHelpers::convertEntityVersionToHexString(mVersion) << "\"";
    s << ">";
    if (mDefinedAttributes & DAFlags) s << "<flags bitmask=\"" << XmlHelpers::convertEntityFlagsToHexString(mFlags) << "\" />";
    if (mDefinedAttributes & DADisplacement) XmlHelpers::ostreamVector3(s << "<displacement ", mDisplacement) << " />";
    if (mDefinedAttributes & DAPosition) XmlHelpers::ostreamVector3(s << "<position ", mPosition) << " />";
    if (mDefinedAttributes & DAOrientation) XmlHelpers::ostreamQuaternion(s << "<orientation ", mOrientation) << " />";
    if (mDefinedAttributes & DAAnimation) s << "<animation state=\"" << XmlHelpers::convertAnimationStateToHexString(mAnimationState) << "\" />";
    if (mDefinedAttributes & DAAABoundingBox)
    {
        s << "<aabb>";
        XmlHelpers::ostreamVector3(s << "<min ", mAABoundingBox.getMinimum()) << " />";
        XmlHelpers::ostreamVector3(s << "<max ", mAABoundingBox.getMaximum()) << " />";
        s << "</aabb>";
    }
    if (mDefinedAttributes & DADownloadProgress)
        s << "<DownloadProgress value=\"" << mDownloadProgress<< "\" />";
    if (mDefinedAttributes & DAUploadProgress)
        s << "<UploadProgress value=\"" << mUploadProgress<< "\" />";
     
    if (!mShape.isNull())
        s << mShape->toXmlString();
    if (!mContent.isNull())
        s << mContent->toXmlString();

    s << "</entity>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEntity::toXmlElt(TiXmlElement& xmlElt) const
{
    if (!mDefinedAttributes & DAUid) return false;
    TiXmlElement* entityElt = new TiXmlElement("entity");
    entityElt->SetAttribute("uid", mUid.c_str());
    if (mDefinedAttributes & DAOwner) entityElt->SetAttribute("owner", mOwner.c_str());
    if (mDefinedAttributes & DAType) entityElt->SetAttribute("type", Ogre::StringConverter::toString(mType).c_str());
    if (mDefinedAttributes & DAName) entityElt->SetAttribute("name", mName.c_str());
    if (mDefinedAttributes & DAVersion) entityElt->SetAttribute("version", XmlHelpers::convertEntityVersionToHexString(mVersion).c_str());
    if (mDefinedAttributes & DAFlags)
    {
        TiXmlElement* flagsElt = new TiXmlElement("flags");
        flagsElt->SetAttribute("bitmask", XmlHelpers::convertEntityFlagsToHexString(mFlags).c_str());
        entityElt->LinkEndChild(flagsElt);
    }
    if (mDefinedAttributes & DADisplacement)
        entityElt->LinkEndChild(XmlHelpers::toXmlEltVector3("displacement", mDisplacement));
    if (mDefinedAttributes & DAPosition)
        entityElt->LinkEndChild(XmlHelpers::toXmlEltVector3("position", mPosition));
    if (mDefinedAttributes & DAOrientation)
        entityElt->LinkEndChild(XmlHelpers::toXmlEltQuaternion("orientation", mOrientation));
    if (mDefinedAttributes & DAAnimation)
    {
        TiXmlElement* animationElt = new TiXmlElement("animation");
        animationElt->SetAttribute("state", XmlHelpers::convertAnimationStateToHexString(mAnimationState).c_str());
        entityElt->LinkEndChild(animationElt);
    }
    if (mDefinedAttributes & DAAABoundingBox)
    {
        TiXmlElement* aabbElt = new TiXmlElement("aabb");
        aabbElt->LinkEndChild(XmlHelpers::toXmlEltVector3("min", mAABoundingBox.getMinimum()));
        aabbElt->LinkEndChild(XmlHelpers::toXmlEltVector3("max", mAABoundingBox.getMaximum()));
        entityElt->LinkEndChild(aabbElt);
    }
    if (mDefinedAttributes & DADownloadProgress)
    {
        TiXmlElement* elt = new TiXmlElement("DownloadProgress");
        elt->SetAttribute("value", mDownloadProgress);
        entityElt->LinkEndChild(elt);
    } 
    if (mDefinedAttributes & DAUploadProgress)
    {
        TiXmlElement* elt = new TiXmlElement("UploadProgress");
        elt->SetAttribute("value", mUploadProgress);
        entityElt->LinkEndChild(elt);
    } 

    if (!mShape.isNull()) 
        mShape->toXmlElt(*entityElt);
    if (!mContent.isNull())
        mContent->toXmlElt(*entityElt);

    xmlElt.LinkEndChild(entityElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlEntity::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "uid", attr)) return false;
    mUid = attr;
    mDefinedAttributes |= DAUid;
    if (XmlHelpers::getAttribute(xmlElt, "owner", attr))
    {
        mOwner = attr;
        mDefinedAttributes |= DAOwner;
    }
    if (XmlHelpers::getAttribute(xmlElt, "type", attr))
    {
        XmlHelpers::convertDecStringToEntityType(attr, mType);
        mDefinedAttributes |= DAType;
    }
    if (XmlHelpers::getAttribute(xmlElt, "name", attr))
    {
        mName = attr;
        mDefinedAttributes |= DAName;
    }
    if (XmlHelpers::getAttribute(xmlElt, "version", attr))
    {
        mVersion = XmlHelpers::convertHexStringToEntityVersion(attr);
        mDefinedAttributes |= DAVersion;
    }

    if ((elt = xmlElt->FirstChildElement("flags")) != 0)
    {
        if (!XmlHelpers::getAttribute(elt, "bitmask", attr)) return false;
        mFlags = XmlHelpers::convertHexStringToEntityFlags(attr);
        mDefinedAttributes |= DAFlags;
    }
    if ((elt = xmlElt->FirstChildElement("displacement")) != 0)
    {
        XmlHelpers::fromXmlEltVector3(elt, mDisplacement);
        mDefinedAttributes |= DADisplacement;
    }
    if ((elt = xmlElt->FirstChildElement("position")) != 0)
    {
        XmlHelpers::fromXmlEltVector3(elt, mPosition);
        mDefinedAttributes |= DAPosition;
    }
    if ((elt = xmlElt->FirstChildElement("orientation")) != 0)
    {
        XmlHelpers::fromXmlEltQuaternion(elt, mOrientation);
        mDefinedAttributes |= DAOrientation;
    }
    if ((elt = xmlElt->FirstChildElement("animation")) != 0)
    {
        if (XmlHelpers::getAttribute(elt, "state", attr))
        {
            mAnimationState = XmlHelpers::convertHexStringToAnimationState(attr);
            mDefinedAttributes |= DAAnimation;
        }
    }
    if ((elt = xmlElt->FirstChildElement("aabb")) != 0)
    {
        Ogre::Vector3 min, max;
        TiXmlElement* subElt;
        if ((subElt = elt->FirstChildElement("min")) != 0)
            XmlHelpers::fromXmlEltVector3(subElt, min);
        if ((subElt = elt->FirstChildElement("max")) != 0)
            XmlHelpers::fromXmlEltVector3(subElt, max);
        mAABoundingBox.setExtents(min, max);
        mDefinedAttributes |= DAAABoundingBox;
    }
    if ((elt = xmlElt->FirstChildElement("shape")) != 0)
    {
        //
    }
    if ((elt = xmlElt->FirstChildElement("content")) != 0)
    {
        RefCntPoolPtr<XmlContent> xmlContent;
        xmlContent->fromXmlElt(elt);
        mContent = RefCntPoolPtr<XmlContent>(xmlContent);
        mDefinedAttributes |= DAContent;
    }
    if ((elt = xmlElt->FirstChildElement("DownloadProgress")) != 0)
    {
        mDownloadProgress = XmlHelpers::convertStringToFloat(elt->Attribute("value"));
        mDefinedAttributes |= DADownloadProgress;
    }
    if ((elt = xmlElt->FirstChildElement("UploadProgress")) != 0)
    {
        mUploadProgress = XmlHelpers::convertStringToFloat(elt->Attribute("value"));
        mDefinedAttributes |= DAUploadProgress;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void XmlEntity::copyEntityDefinedAttributes(RefCntPoolPtr<XmlEntity>& srcXmlEntity, RefCntPoolPtr<XmlEntity>& dstXmlEntity)
{
    XmlEntity::DefinedAttributes definedAttributes = dstXmlEntity->getDefinedAttributes();
    if (definedAttributes & XmlEntity::DAUid)
        dstXmlEntity->setUid(srcXmlEntity->getUid());
    if (definedAttributes & XmlEntity::DAFlags)
        dstXmlEntity->setFlags(srcXmlEntity->getFlags());
    if (definedAttributes & XmlEntity::DAPosition)
        dstXmlEntity->setPosition(srcXmlEntity->getPosition());
    if (definedAttributes & XmlEntity::DAOrientation)
        dstXmlEntity->setOrientation(srcXmlEntity->getOrientation());
    if (definedAttributes & XmlEntity::DAAnimation)
        dstXmlEntity->setAnimation(srcXmlEntity->getAnimation());
    if (definedAttributes & XmlEntity::DAContent)
        dstXmlEntity->setContent(srcXmlEntity->getContent());
    if (definedAttributes & XmlEntity::DADownloadProgress)
        dstXmlEntity->setDownloadProgress(srcXmlEntity->getDownloadProgress());
    if (definedAttributes & XmlEntity::DAUploadProgress)
        dstXmlEntity->setUploadProgress(srcXmlEntity->getUploadProgress());
}

} // namespace Solipsis
