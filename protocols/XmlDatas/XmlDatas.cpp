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
Pool XmlLodContent::mPool;
Pool XmlSceneLodContent::mPool;
Pool XmlContent::mPool;
Pool XmlSceneContent::mPool;
Pool XmlEntity::mPool;
Pool XmlAction::mPool;
Pool XmlEvt::mPool;
Pool& XmlLogin::getStaticPool() { return mPool; }
Pool& XmlLogin::getPool() const { return mPool; }
Pool& XmlLodContent::getStaticPool() { return mPool; }
Pool& XmlLodContent::getPool() const { return mPool; }
Pool& XmlSceneLodContent::getStaticPool() { return mPool; }
Pool& XmlSceneLodContent::getPool() const { return mPool; }
Pool& XmlContent::getStaticPool() { return mPool; }
Pool& XmlContent::getPool() const { return mPool; }
Pool& XmlSceneContent::getStaticPool() { return mPool; }
Pool& XmlSceneContent::getPool() const { return mPool; }
Pool& XmlEntity::getStaticPool() { return mPool; }
Pool& XmlEntity::getPool() const { return mPool; }
Pool& XmlAction::getStaticPool() { return mPool; }
Pool& XmlAction::getPool() const { return mPool; }
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
std::string XmlHelpers::convertUCharToHexString(unsigned char value)
{
    char valueStr[3];
    _snprintf(valueStr, sizeof(valueStr) - 1, "%02X", value);
    valueStr[2] = '\0';
    return valueStr;
}

//-------------------------------------------------------------------------------------
unsigned char XmlHelpers::convertHexStringToUChar(const char* str)
{
    unsigned int value;
    sscanf(str, "%02X", &value);
    return value;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertUIntToHexString(unsigned int value)
{
    char valueStr[9];
    _snprintf(valueStr, sizeof(valueStr) - 1, "%08X", value);
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
std::string XmlHelpers::convertBoolToString(bool value)
{
    return (value ? "true" : "false");
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::convertStringToBool(const char* str)
{
    if (stricmp(str, "false") == 0)
        return false;
    return true;
}

//-------------------------------------------------------------------------------------
std::ostream& XmlHelpers::ostreamVector3(std::ostream& o, const Ogre::Vector3& v)
{
    o << "x=\"" << v.x << "\" y=\"" << v.y << "\" z=\"" << v.z << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltVector3(const std::string& eltName, const Ogre::Vector3& v)
{
    TiXmlElement* vector3Elt = new TiXmlElement(eltName.c_str());
    vector3Elt->SetAttribute("x", Ogre::StringConverter::toString(v.x).c_str());
    vector3Elt->SetAttribute("y", Ogre::StringConverter::toString(v.y).c_str());
    vector3Elt->SetAttribute("z", Ogre::StringConverter::toString(v.z).c_str());
    return vector3Elt;
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
std::ostream& XmlHelpers::ostreamQuaternion(std::ostream& o, const Ogre::Quaternion& q)
{
    o << "x=\"" << q.x << "\" y=\"" << q.y << "\" z=\"" << q.z << "\" w=\"" << q.w << "\"";
    return o;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltQuaternion(const std::string& eltName, const Ogre::Quaternion& q)
{
    TiXmlElement* quaternionElt = new TiXmlElement(eltName.c_str());
    quaternionElt->SetAttribute("x", Ogre::StringConverter::toString(q.x).c_str());
    quaternionElt->SetAttribute("y", Ogre::StringConverter::toString(q.y).c_str());
    quaternionElt->SetAttribute("z", Ogre::StringConverter::toString(q.z).c_str());
    quaternionElt->SetAttribute("w", Ogre::StringConverter::toString(q.w).c_str());
    return quaternionElt;
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
TiXmlElement* XmlHelpers::toXmlEltLodContentFileStruct(const std::string& eltName, const LodContentFileStruct& s)
{
    TiXmlElement* lodContentFileStructElt = new TiXmlElement(eltName.c_str());
    lodContentFileStructElt->SetAttribute("name", s.mFilename.c_str());
    lodContentFileStructElt->SetAttribute("version", XmlHelpers::convertFileVersionToHexString(s.mVersion).c_str());
    return lodContentFileStructElt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltLodContentFileStruct(TiXmlElement* xmlElt, LodContentFileStruct& s)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "name", attr)) return false;
    s.mFilename = std::string(attr);
    if (!getAttribute(xmlElt, "version", attr)) return false;
    s.mVersion = XmlHelpers::convertHexStringToFileVersion(attr);
    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* XmlHelpers::toXmlEltEntryGateStruct(const std::string& eltName, const EntryGateStruct& s)
{
    TiXmlElement* entryGateStructElt = new TiXmlElement(eltName.c_str());
    entryGateStructElt->SetAttribute("gravity", XmlHelpers::convertBoolToString(s.mGravity).c_str());
    entryGateStructElt->LinkEndChild(XmlHelpers::toXmlEltVector3("position", s.mPosition));
    return entryGateStructElt;
}

//-------------------------------------------------------------------------------------
bool XmlHelpers::fromXmlEltEntryGateStruct(TiXmlElement* xmlElt, EntryGateStruct& s)
{
    const char* attr = 0;
    if (!getAttribute(xmlElt, "gravity", attr)) return false;
    s.mGravity = XmlHelpers::convertStringToBool(attr);
    TiXmlElement* subElt;
    if ((subElt = xmlElt->FirstChildElement("position")) == 0)
        return false;
    XmlHelpers::fromXmlEltVector3(subElt, s.mPosition);
    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertAuthentTypeToRepr(const AuthentType& authentType)
{
    char prefix[2];
    prefix[0] = authentType;
    prefix[1] = '\0';
    return std::string(prefix);
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertEventTypeToRepr(const EventType& evtType)
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
const std::string& XmlHelpers::convertEntityTypeToRepr(const EntityType& entityType)
{
    static std::string EntityTypeRepr[] = {
        "ETAvatar",
        "ETSite",
        "ETObject"
    };
    return EntityTypeRepr[entityType];
}

//-------------------------------------------------------------------------------------
const std::string& XmlHelpers::convertActionTypeToRepr(const ActionType& actionType)
{
    static std::string ActionTypeRepr[] = {
        "ATNone",
        "ATChat"
    };
    return ActionTypeRepr[actionType];
}

//-------------------------------------------------------------------------------------
std::string XmlHelpers::convertEntityFlagsToRepr(const EntityFlags& entityFlags)
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
const std::string& XmlHelpers::convertShapeTypeToRepr(const ShapeType& shapeType)
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
    s << "<worldHost>" << mWorldHost << "</worldHost>";
    s << "<worldPort>" << mWorldPort << "</worldPort>";
    s << "<nodeId>" << mNodeId << "</nodeId>";
    s << "<ctxt>";
    s << "<cnxMode>";
    s << "0";
    s << "</cnxMode>";
    s << "</ctxt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlLogin::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* usernameElt = new TiXmlElement("username");
    TiXmlText* username = new TiXmlText(mUsername.c_str());
    usernameElt->LinkEndChild(username);
    xmlElt.LinkEndChild(usernameElt);
    TiXmlElement* worldHostElt = new TiXmlElement("worldHost");
    TiXmlText* worldHost = new TiXmlText(mWorldHost.c_str());
    worldHostElt->LinkEndChild(worldHost);
    xmlElt.LinkEndChild(worldHostElt);
    TiXmlElement* worldPortElt = new TiXmlElement("worldPort");
    TiXmlText* worldPort = new TiXmlText(Ogre::StringConverter::toString(mWorldPort).c_str());
    worldPortElt->LinkEndChild(worldPort);
    xmlElt.LinkEndChild(worldPortElt);
    TiXmlElement* nodeIdElt = new TiXmlElement("nodeId");
    TiXmlText* nodeId = new TiXmlText(mNodeId.c_str());
    nodeIdElt->LinkEndChild(nodeId);
    xmlElt.LinkEndChild(nodeIdElt);
    TiXmlElement* ctxtElt = new TiXmlElement("ctxt");
    TiXmlText* ctxt = new TiXmlText("0");
    ctxtElt->LinkEndChild(ctxt);
    TiXmlElement* cnxModeElt = new TiXmlElement("cnxMode");
    ctxtElt->LinkEndChild(cnxModeElt);
    xmlElt.LinkEndChild(ctxtElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlLogin::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;

    if ((elt = xmlElt->FirstChildElement("username")) == 0)
        return false;
    mUsername = elt->GetText();

    if ((elt = xmlElt->FirstChildElement("worldHost")) == 0)
        return false;
    mWorldHost = elt->GetText();

    if ((elt = xmlElt->FirstChildElement("worldPort")) == 0)
        return false;
    mWorldPort = atoi(elt->GetText());

    if ((elt = xmlElt->FirstChildElement("nodeId")) == 0)
        return false;
    mNodeId = elt->GetText();

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlLodContent::toXmlString() const
{
    std::stringstream s;
    s << "<lod level=\"" << mLevel << "\">";
#ifdef POOL
    if (!mDatas.isNull()) s << mDatas->toXmlString();
#else
    if (mDatas != 0) s << mDatas->toXmlString();
#endif
    s << "<files>";
    for (LodContentFileList::const_iterator file = mLodContentFileList.begin(); file != mLodContentFileList.end(); ++file)
    {
        s << "<file name=\"" << file->mFilename << "\" version=\"" << XmlHelpers::convertFileVersionToHexString(file->mVersion) << "\" />";
    }
    s << "</files>";
    s << "</lod>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlLodContent::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* lodElt = new TiXmlElement("lod");
    lodElt->SetAttribute("level", mLevel);
#ifdef POOL
    if (!mDatas.isNull()) mDatas->toXmlElt(*lodElt);
#else
    if (mDatas != 0) mDatas->toXmlElt(*lodElt);
#endif
    TiXmlElement* filesElt = new TiXmlElement("files");
    for (LodContentFileList::const_iterator file = mLodContentFileList.begin(); file != mLodContentFileList.end(); ++file)
    {
        TiXmlElement* fileElt = XmlHelpers::toXmlEltLodContentFileStruct("file", *file);
        filesElt->LinkEndChild(fileElt);
    }
    lodElt->LinkEndChild(filesElt);
    xmlElt.LinkEndChild(lodElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlLodContent::fromXmlElt(TiXmlElement* xmlElt)
{
#ifdef POOL
#else
    delete mDatas;
    mDatas = 0;
#endif
    mLodContentFileList.clear();

    TiXmlElement* elt;
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "level", attr)) return false;
    XmlHelpers::convertDecStringToLod(attr, mLevel);

    if ((elt = xmlElt->FirstChildElement("sceneLodContent")) != 0)
    {
#ifdef POOL
        RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent;
        xmlSceneLodContent->fromXmlElt(elt);
        mDatas = RefCntPoolPtr<XmlData>(xmlSceneLodContent);
#else
        XmlSceneLodContent* sceneLodContent = new XmlSceneLodContent();
        sceneLodContent->fromXmlElt(elt);
        mDatas = sceneLodContent;
#endif
    }

    if ((elt = xmlElt->FirstChildElement("files")) == 0)
        return false;

    for (TiXmlElement* fileElt = elt->FirstChildElement("file"); fileElt != 0; fileElt = fileElt->NextSiblingElement("file"))
    {
        LodContentFileStruct lodContentFileStruct;
        if (!XmlHelpers::fromXmlEltLodContentFileStruct(fileElt, lodContentFileStruct))
            return false;
        mLodContentFileList.push_back(lodContentFileStruct);
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlSceneLodContent::toXmlString() const
{
    std::stringstream s;
    s << "<sceneLodContent mainFilename=\"" << mMainFilename << "\" collision=\"" << mCollision << "\" />";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlSceneLodContent::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* sceneLodContentElt = new TiXmlElement("sceneLodContent");
    sceneLodContentElt->SetAttribute("mainFilename", mMainFilename.c_str());
    sceneLodContentElt->SetAttribute("collision", mCollision.c_str());
    xmlElt.LinkEndChild(sceneLodContentElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlSceneLodContent::fromXmlElt(TiXmlElement* xmlElt)
{
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "mainFilename", attr)) return false;
    mMainFilename = attr;
    if (XmlHelpers::getAttribute(xmlElt, "collision", attr))
        mCollision = attr;

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlContent::toXmlString() const
{
    std::stringstream s;
    s << "<content>";
#ifdef POOL
    if (!mDatas.isNull()) s << mDatas->toXmlString();
#else
    if (mDatas != 0) s << mDatas->toXmlString();
#endif
    for (ContentLodMap::const_iterator lod = mContentLodMap.begin(); lod != mContentLodMap.end(); ++lod)
    {
        s << lod->second->toXmlString();
    }
    s << "</content>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlContent::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* contentElt = new TiXmlElement("content");
#ifdef POOL
    if (!mDatas.isNull()) mDatas->toXmlElt(*contentElt);
#else
    if (mDatas != 0) mDatas->toXmlElt(*contentElt);
#endif
    for (ContentLodMap::const_iterator lod = mContentLodMap.begin(); lod != mContentLodMap.end(); ++lod)
    {
        lod->second->toXmlElt(*contentElt);
    }
    xmlElt.LinkEndChild(contentElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlContent::fromXmlElt(TiXmlElement* xmlElt)
{
#ifdef POOL
#else
    delete mDatas;
    mDatas = 0;
#endif

    TiXmlElement* elt;

    if ((elt = xmlElt->FirstChildElement("sceneContent")) != 0)
    {
#ifdef POOL
        RefCntPoolPtr<XmlSceneContent> xmlSceneContent;
        xmlSceneContent->fromXmlElt(elt);
        mDatas = RefCntPoolPtr<XmlData>(xmlSceneContent);
#else
        XmlSceneContent* sceneContent = new XmlSceneContent();
        sceneContent->fromXmlElt(elt);
        mDatas = sceneContent;
#endif
    }

    mContentLodMap.clear();

    for (TiXmlElement* lodElt = xmlElt->FirstChildElement("lod"); lodElt != 0; lodElt = lodElt->NextSiblingElement("lod"))
    {
        RefCntPoolPtr<XmlLodContent> xmlLodContent;
        xmlLodContent->fromXmlElt(lodElt);
        mContentLodMap[xmlLodContent->getLevel()] = xmlLodContent;
    }

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlSceneContent::toXmlString() const
{
    std::stringstream s;
    s << "<sceneContent>";
    s << "<entryGate gravity=\"" + XmlHelpers::convertBoolToString(mEntryGate.mGravity) + "\">";
    XmlHelpers::ostreamVector3(s << "<position ", mEntryGate.mPosition) << " />";
    s << "</entryGate>";
    s << "</sceneContent>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlSceneContent::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* sceneLodContentElt = new TiXmlElement("sceneContent");
    TiXmlElement* entryGateElt = XmlHelpers::toXmlEltEntryGateStruct("entryGate", mEntryGate);
    sceneLodContentElt->LinkEndChild(entryGateElt);
    xmlElt.LinkEndChild(sceneLodContentElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlSceneContent::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;
    const char* attr = 0;

    if ((elt = xmlElt->FirstChildElement("entryGate")) == 0)
        return false;
    if (!XmlHelpers::fromXmlEltEntryGateStruct(elt, mEntryGate))
        return false;

    return true;
}

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
#ifdef POOL
    if (!mShape.isNull()) s << mShape->toXmlString();
    if (!mContent.isNull()) s << mContent->toXmlString();
#else
    if (mShape != 0) s << mShape->toXmlString();
    if (mContent != 0) s << mContent->toXmlString();
#endif
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
#ifdef POOL
    if (!mShape.isNull()) mShape->toXmlElt(*entityElt);
    if (!mContent.isNull()) mContent->toXmlElt(*entityElt);
#else
    if (mShape != 0) mShape->toXmlElt(*entityElt);
    if (mContent != 0) mContent->toXmlElt(*entityElt);
#endif
    xmlElt.LinkEndChild(entityElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlEntity::fromXmlElt(TiXmlElement* xmlElt)
{
#ifdef POOL
#else
    delete mAnimation;
    mAnimation = 0;
    delete mShape;
    mShape = 0;
    delete mContent;
    mContent = 0;
#endif

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
#ifdef POOL
        RefCntPoolPtr<XmlContent> xmlContent;
        xmlContent->fromXmlElt(elt);
        mContent = RefCntPoolPtr<XmlContent>(xmlContent);
#else
        mContent = new XmlContent();
        mContent->fromXmlElt(elt);
#endif
        mDefinedAttributes |= DAContent;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void XmlEntity::copyEntityDefinedAttributes(RefCntPoolPtr<XmlEntity>& srcXmlEntity, RefCntPoolPtr<XmlEntity>& dstXmlEntity)
{
    XmlEntity::DefinedAttributes definedAttributes = dstXmlEntity->getDefinedAttributes();
    if (definedAttributes & XmlEntity::DAUid)
        dstXmlEntity->setUid(srcXmlEntity->getUid());
    if (definedAttributes & XmlEntity::DAPosition)
        dstXmlEntity->setPosition(srcXmlEntity->getPosition());
    if (definedAttributes & XmlEntity::DAOrientation)
        dstXmlEntity->setOrientation(srcXmlEntity->getOrientation());
    if (definedAttributes & XmlEntity::DAAnimation)
        dstXmlEntity->setAnimation(srcXmlEntity->getAnimation());
    if (definedAttributes & XmlEntity::DAContent)
        dstXmlEntity->setContent(srcXmlEntity->getContent());
}

//-------------------------------------------------------------------------------------
std::string XmlAction::toXmlString() const
{
    std::stringstream s;
    s << "<action type=\"" << mType << "\"";
    s << " sourceEntityUid=\"" << mSourceEntityUid << "\"";
    s << " targetEntityUid=\"" << mTargetEntityUid << "\"";
    s << " desc=\"" << mDesc << "\"";
    s << " />";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlAction::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* actionElt = new TiXmlElement("action");
    actionElt->SetAttribute("type", Ogre::StringConverter::toString(mType).c_str());
    actionElt->SetAttribute("sourceEntityUid", mSourceEntityUid.c_str());
    actionElt->SetAttribute("targetEntityUid", mTargetEntityUid.c_str());
    actionElt->SetAttribute("desc", mDesc.c_str());
    xmlElt.LinkEndChild(actionElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlAction::fromXmlElt(TiXmlElement* xmlElt)
{
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "type", attr)) return false;
    XmlHelpers::convertDecStringToActionType(attr, mType);
    if (!XmlHelpers::getAttribute(xmlElt, "sourceEntityUid", attr)) return false;
    mSourceEntityUid = attr;
    if (!XmlHelpers::getAttribute(xmlElt, "targetEntityUid", attr)) return false;
    mTargetEntityUid = attr;
    if (!XmlHelpers::getAttribute(xmlElt, "desc", attr)) return false;
    mDesc = attr;

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
bool XmlEvt::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* evtElt = new TiXmlElement("evt");
    evtElt->SetAttribute("type", Ogre::StringConverter::toString(mType).c_str());
#ifdef POOL
    if (!mDatas.isNull()) mDatas->toXmlElt(*evtElt);
#else
    if (mDatas != 0) mDatas->toXmlElt(*evtElt);
#endif
    xmlElt.LinkEndChild(evtElt);
    return true;
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
    XmlHelpers::convertDecStringToEventType(attr, mType);

    TiXmlElement* subElt;
    if ((subElt = elt->FirstChildElement("entity")) != 0)
    {
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity;
        xmlEntity->fromXmlElt(subElt);
        mDatas = RefCntPoolPtr<XmlData>(xmlEntity);
#else
        XmlEntity* xmlEntity = new XmlEntity();
        xmlEntity->fromXmlElt(subElt);
        mDatas = xmlEntity;
#endif
    }
    else if ((subElt = elt->FirstChildElement("action")) != 0)
    {
#ifdef POOL
        RefCntPoolPtr<XmlAction> xmlAction;
        xmlAction->fromXmlElt(subElt);
        mDatas = RefCntPoolPtr<XmlData>(xmlAction);
#else
        XmlAction* xmlAction = new XmlAction();
        xmlAction->fromXmlElt(subElt);
        mDatas = xmlAction;
#endif
    }

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
