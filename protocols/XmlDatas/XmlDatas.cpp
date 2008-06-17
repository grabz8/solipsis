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

#include "XmlDatas.h"
#include <stdio.h>
#include <stdlib.h>

namespace Solipsis {

#ifdef POOL
Pool XmlLogin::mPool;
Pool XmlContent::mPool;
Pool XmlEntity::mPool;
Pool XmlEvt::mPool;
Pool& XmlLogin::getStaticPool() { return mPool; }
Pool& XmlLogin::getPool() const { return mPool; }
Pool& XmlContent::getStaticPool() { return mPool; }
Pool& XmlContent::getPool() const { return mPool; }
Pool& XmlEntity::getStaticPool() { return mPool; }
Pool& XmlEntity::getPool() const { return mPool; }
Pool& XmlEvt::getStaticPool() { return mPool; }
Pool& XmlEvt::getPool() const { return mPool; }
#endif

//-------------------------------------------------------------------------------------
bool XmlHelpers::getAttribute(TiXmlElement* elt, const char* attrName, const char*& attr)
{
    if (elt == 0)
        return false;
    attr = elt->Attribute(attrName);
    if ((attr == 0) || (attr[0] == '\0'))
        return false;
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertUIntToHexString(unsigned int value)
{
    char valueStr[9];
    _snprintf(valueStr, 8, "%08X", value);
    valueStr[8] = '\0';
    return valueStr;
}

//-------------------------------------------------------------------------------------
unsigned int XmlHelpers::convertHexStringToUInt(const char* str)
{
    unsigned int value;
    sscanf(str, "%08X", &value);
    return value;
}

//-------------------------------------------------------------------------------------
std::ostream& XmlHelpers::ostreamVector3(std::ostream& o, const Ogre::Vector3& v)
{
    o << "x=\"" << v.x << "\" y=\"" << v.y << "\" z=\"" << v.z << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
std::ostream& XmlHelpers::ostreamQuaternion(std::ostream& o, const Ogre::Quaternion& q)
{
    o << "x=\"" << q.x << "\" y=\"" << q.y << "\" z=\"" << q.z << "\" w=\"" << q.w << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltVector3(TiXmlElement* xmlElt, Ogre::Vector3& v)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "x", attr)) return false;
    v.x = atof(attr);
    if (!getAttribute(xmlElt, "y", attr)) return false;
    v.y = atof(attr);
    if (!getAttribute(xmlElt, "z", attr)) return false;
    v.z = atof(attr);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltQuaternion(TiXmlElement* xmlElt, Ogre::Quaternion& q)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "x", attr)) return false;
    q.x = atof(attr);
    if (!getAttribute(xmlElt, "y", attr)) return false;
    q.y = atof(attr);
    if (!getAttribute(xmlElt, "z", attr)) return false;
    q.z = atof(attr);
    if (!getAttribute(xmlElt, "w", attr)) return false;
    q.w = atof(attr);
    return true;
}

//-------------------------------------------------------------------------------------
const std::string& convertEventTypeToRepr(const EventType& evtType)
{
    static std::string EventTypeRepr[] = {
        "ETNewEntity",
        "ETLostEntity",
        "ETUpdatedEntity",
        "ETActionOnEntity",
        "ETStatusReport"
    };
    return EventTypeRepr[evtType];
}

//-------------------------------------------------------------------------------------
const std::string& convertEntityTypeToRepr(const EntityType& entityType)
{
    static std::string EntityTypeRepr[] = {
        "ETAvatar",
        "ETSite",
        "ETObject"
    };
    return EntityTypeRepr[entityType];
}

//-------------------------------------------------------------------------------------
std::string convertEntityFlagsToRepr(const EntityFlags& entityFlags)
{
    std::string entityFlagsRepr;
    if (entityFlags & EFGravity)
        entityFlagsRepr += "EFGravity|";
    if (entityFlagsRepr.empty())
        entityFlagsRepr = "EFNone";
    else
        entityFlagsRepr.erase(entityFlagsRepr.length() - 1, 1);
    return entityFlagsRepr;
}

//-------------------------------------------------------------------------------------
const std::string& convertShapeTypeToRepr(const ShapeType& shapeType)
{
    static std::string ShapeTypeRepr[] = {
        "STPoint",
        "STCircle",
        "ST2DRect",
        "STSphere",
        "STBox",
        "STPolygonVolume"
    };
    return ShapeTypeRepr[shapeType];
}

//-------------------------------------------------------------------------------------
std::string XmlLogin::toXmlString() const
{
    std::stringstream s;
    s << "<username>" << mUsername << "</username>";
    s << "<pwd>" << mPwd << "</pwd>";
    s << "<ctxt>";
    s << "<cnxMode>";
    s << 0;
    s << "</cnxMode>";
    s << "</ctxt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlLogin::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;

    if ((elt = xmlElt->FirstChildElement("username")) == 0)
        return false;
    mUsername = elt->GetText();

    if ((elt = xmlElt->FirstChildElement("pwd")) == 0)
        return false;
    mPwd = elt->GetText();

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlContent::toXmlString() const
{
    std::stringstream s;
    s << "<content>";
    for (ContentLodMap::const_iterator lod = mContentLodMap.begin(); lod != mContentLodMap.end(); ++lod)
    {
        const ContentFileList& contentFileList = lod->second;
        s << "<lod level=\"" << lod->first << "\">";
        s << "<files>";
        for (ContentFileList::const_iterator file = contentFileList.begin(); file != contentFileList.end(); ++file)
        {
            s << "<file name=\"" << (*file) << "\"/>";
        }
        s << "</files>";
        s << "</lod>";
    }
    s << "</content>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlContent::fromXmlElt(TiXmlElement* xmlElt)
{
    mContentLodMap.clear();

    ContentFileList contentFileList;
    TiXmlElement* elt;
    const char* attr = 0;

    for (TiXmlElement* lodElt = xmlElt->FirstChildElement("lod"); lodElt != 0; lodElt = lodElt->NextSiblingElement("lod"))
    {
        if (!XmlHelpers::getAttribute(lodElt, "level", attr)) return false;
        Lod lod;
        convertDecStringToLod(attr, lod);

        if ((elt = lodElt->FirstChildElement("files")) == 0)
            return false;

        contentFileList.clear();
        for (TiXmlElement* fileElt = elt->FirstChildElement("file"); fileElt != 0; fileElt = fileElt->NextSiblingElement("file"))
        {
            if (!XmlHelpers::getAttribute(fileElt, "name", attr)) return false;
            contentFileList.push_back(std::string(attr));
        }

        mContentLodMap[lod] = contentFileList;
    }

    return true;
}

//-------------------------------------------------------------------------------------
XmlEntity::~XmlEntity()
{
    delete mAnimation;
    delete mShape;
    delete mContent;
}

//-------------------------------------------------------------------------------------
std::string XmlEntity::toXmlString() const
{
    std::stringstream s;
    if (!mDefinedAttributes & DAUid) return s.str();
    std::string uidStr;
    s << "<entity uid=\"" << convertEntityUIDToHexString(mUid) << "\"";
    if (mDefinedAttributes & DAOwner) s << " owner=\"" << mOwner << "\"";
    if (mDefinedAttributes & DAType) s << " type=\"" << mType << "\"";
    if (mDefinedAttributes & DAName) s << " name=\"" << mName << "\"";
    s << ">";
    if (mDefinedAttributes & DAFlags) s << "<flags bitmask=\"" << mFlags << "\" />";
    if (mDefinedAttributes & DADisplacement) XmlHelpers::ostreamVector3(s << "<displacement ", mDisplacement) << " />";
    if (mDefinedAttributes & DAPosition) XmlHelpers::ostreamVector3(s << "<position ", mPosition) << " />";
    if (mDefinedAttributes & DAOrientation) XmlHelpers::ostreamQuaternion(s << "<orientation ", mOrientation) << " />";
    if (mDefinedAttributes & DAAABoundingBox)
    {
        s << "<aabb>";
        XmlHelpers::ostreamVector3(s << "<min ", mAABoundingBox.getMinimum()) << " />";
        XmlHelpers::ostreamVector3(s << "<max ", mAABoundingBox.getMaximum()) << " />";
        s << "</aabb>";
    }
    if (mAnimation != 0) s << mAnimation->toXmlString();
    if (mShape != 0) s << mShape->toXmlString();
    if (mContent != 0) s << mContent->toXmlString();
    s << "</entity>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEntity::fromXmlElt(TiXmlElement* xmlElt)
{
    delete mAnimation;
    mAnimation = 0;
    delete mShape;
    mShape = 0;
    delete mContent;
    mContent = 0;

    TiXmlElement* elt;
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "uid", attr)) return false;
    mUid = convertHexStringToEntityUID(attr);
    mDefinedAttributes |= DAUid;
    if (XmlHelpers::getAttribute(xmlElt, "owner", attr))
    {
        mOwner = attr;
        mDefinedAttributes |= DAOwner;
    }
    if (XmlHelpers::getAttribute(xmlElt, "type", attr))
    {
        convertDecStringToEntityType(attr, mType);
        mDefinedAttributes |= DAType;
    }
    if (XmlHelpers::getAttribute(xmlElt, "name", attr))
    {
        mName = attr;
        mDefinedAttributes |= DAName;
    }

    if ((elt = xmlElt->FirstChildElement("flags")) != 0)
    {
        if (!XmlHelpers::getAttribute(elt, "bitmask", attr)) return false;
        mFlags = convertHexStringToEntityFlags(attr);
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
    if ((elt = xmlElt->FirstChildElement("animation")) != 0)
    {
        mDefinedAttributes |= DAAnimation;
    }
    if ((elt = xmlElt->FirstChildElement("shape")) != 0)
    {
//        mShape = new XmlShape();
//        mShape->fromXmlElt(elt);
    }
    if ((elt = xmlElt->FirstChildElement("content")) != 0)
    {
        mContent = new XmlContent();
        mContent->fromXmlElt(elt);
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlEvt::toXmlString() const
{
    std::stringstream s;
    s << "<evt type=\"" << mType << "\">";
#ifdef POOL
    if (!mDatas.isNull()) s << mDatas->toXmlString();
#else
    if (mDatas != 0) s << mDatas->toXmlString();
#endif
    s << "</evt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEvt::fromXmlElt(TiXmlElement* xmlElt)
{
#ifdef POOL
#else
    delete mDatas;
    mDatas = 0;
#endif

    TiXmlElement* elt;
    const char* attr = 0;

    if ((elt = xmlElt->FirstChildElement("evt")) == 0)
        return false;

    if (!XmlHelpers::getAttribute(elt, "type", attr)) return false;
    convertDecStringToEventType(attr, mType);

    if ((elt = elt->FirstChildElement("entity")) != 0)
    {
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity;
        xmlEntity->fromXmlElt(elt);
        mDatas = RefCntPoolPtr<XmlData>(xmlEntity);
#else
        XmlEntity* entity = new XmlEntity();
        entity->fromXmlElt(elt);
        mDatas = entity;
#endif
    }

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
