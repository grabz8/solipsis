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
#include <Ogre.h>

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

#include "XmlLogin.h"
#include "XmlLodContent.h"
#include "XmlSceneLodContent.h"
#include "XmlContent.h"
#include "XmlSceneContent.h"
#include "XmlEntity.h"
#include "XmlAction.h"
#include "XmlEvt.h"

namespace Solipsis {


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

    if (!mDatas.isNull()) s << mDatas->toXmlString();

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

    if (!mDatas.isNull()) mDatas->toXmlElt(*lodElt);

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
    mLodContentFileList.clear();

    TiXmlElement* elt;
    const char* attr = 0;

    if (!XmlHelpers::getAttribute(xmlElt, "level", attr)) return false;
    XmlHelpers::convertDecStringToLod(attr, mLevel);

    if ((elt = xmlElt->FirstChildElement("sceneLodContent")) != 0)
    {

        RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent;
        xmlSceneLodContent->fromXmlElt(elt);
        mDatas = RefCntPoolPtr<XmlData>(xmlSceneLodContent);

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

    if (!mDatas.isNull()) s << mDatas->toXmlString();

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
    if (!mDatas.isNull()) 
        mDatas->toXmlElt(*contentElt);

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
    TiXmlElement* elt;

    if ((elt = xmlElt->FirstChildElement("sceneContent")) != 0)
    {

        RefCntPoolPtr<XmlSceneContent> xmlSceneContent;
        xmlSceneContent->fromXmlElt(elt);
        mDatas = RefCntPoolPtr<XmlData>(xmlSceneContent);

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
    if (mDefinedAttributes & DAProgress) 
        s << "<DownloadProgress value=\"" << mDownloadProgress<< "\" />";
     
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
    if (mDefinedAttributes & DAProgress)
    {
        TiXmlElement* elt = new TiXmlElement("DownloadProgress");
        elt->SetAttribute("value", mDownloadProgress);
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
   //     LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "XmlEntity::fromXmlElt() progress %f %%", mDownloadProgress*100);
       
        mDefinedAttributes |= DAProgress;
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
    if (definedAttributes & XmlEntity::DAProgress)
        dstXmlEntity->setDownloadProgress(srcXmlEntity->getDownloadProgress());
    
}

//-------------------------------------------------------------------------------------
std::string XmlAction::toXmlString() const
{
    std::stringstream s;
    s << "<action type=\"" << mType << "\"";
    s << " sourceEntityUid=\"" << mSourceEntityUid << "\"";
    s << " targetEntityUid=\"" << mTargetEntityUid << "\"";
    s << " broadcast=\"" << XmlHelpers::convertBoolToString(mBroadcast).c_str() << "\"";
    s << " desc=\"" << XmlHelpers::xmlEscape(XmlHelpers::convertWStringToUTF8("WCHAR_T", mDesc)).c_str() << "\"";
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
    actionElt->SetAttribute("broadcast", XmlHelpers::convertBoolToString(mBroadcast).c_str());
    actionElt->SetAttribute("desc", XmlHelpers::convertWStringToUTF8("WCHAR_T", mDesc).c_str());
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
    if (!XmlHelpers::getAttribute(xmlElt, "broadcast", attr)) return false;
    mBroadcast = XmlHelpers::convertStringToBool(attr);
    if (!XmlHelpers::getAttribute(xmlElt, "desc", attr)) return false;
    mDesc = XmlHelpers::convertUTF8ToWString("WCHAR_T", std::string(attr));

    return true;
}

//-------------------------------------------------------------------------------------
std::string XmlEvt::toXmlString() const
{
    std::stringstream s;
    s << "<evt type=\"" << mType << "\">";
    if (!mDatas.isNull()) 
        s << mDatas->toXmlString();

    s << "</evt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEvt::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* evtElt = new TiXmlElement("evt");
    evtElt->SetAttribute("type", Ogre::StringConverter::toString(mType).c_str());

    if (!mDatas.isNull()) mDatas->toXmlElt(*evtElt);

    xmlElt.LinkEndChild(evtElt);
    return true;
}

//-------------------------------------------------------------------------------------
bool XmlEvt::fromXmlElt(TiXmlElement* xmlElt)
{
    TiXmlElement* elt;
    const char* attr = 0;

    if ((elt = xmlElt->FirstChildElement("evt")) == 0)
        return false;

    if (!XmlHelpers::getAttribute(elt, "type", attr)) return false;
    XmlHelpers::convertDecStringToEventType(attr, mType);

    TiXmlElement* subElt;
    if ((subElt = elt->FirstChildElement("entity")) != 0)
    {
        RefCntPoolPtr<XmlEntity> xmlEntity;
        xmlEntity->fromXmlElt(subElt);
        mDatas = RefCntPoolPtr<XmlData>(xmlEntity);
    }
    else if ((subElt = elt->FirstChildElement("action")) != 0)
    {
        RefCntPoolPtr<XmlAction> xmlAction;
        xmlAction->fromXmlElt(subElt);
        mDatas = RefCntPoolPtr<XmlData>(xmlAction);
    }

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
