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
#include "XmlAction.h"


namespace Solipsis {

Pool XmlAction::mPool;

Pool& XmlAction::getStaticPool() { return mPool; }
Pool& XmlAction::getPool() const { return mPool; }


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

} // namespace Solipsis
