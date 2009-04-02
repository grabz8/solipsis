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
#include "XmlEvt.h"
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


Pool XmlEvt::mPool;

Pool& XmlEvt::getStaticPool() { return mPool; }
Pool& XmlEvt::getPool() const { return mPool; }

//-------------------------------------------------------------------------------------
std::string XmlEvt::toXmlString() const
{
    std::stringstream s;
    s << "<evt type=\"" << mType << "\">";
    if (!mDatas.isNull()) 
        s << mDatas->toXmlString();

    if (!mCommand.size()) 
        s << "<Command value=\"" << mCommand << "\"/>";

    s << "</evt>";
    return s.str();
}

//-------------------------------------------------------------------------------------
bool XmlEvt::toXmlElt(TiXmlElement& xmlElt) const
{
    TiXmlElement* evtElt = new TiXmlElement("evt");
    evtElt->SetAttribute("type", Ogre::StringConverter::toString(mType).c_str());

    if (!mDatas.isNull()) mDatas->toXmlElt(*evtElt);
    if (!mCommand.size()) 
    {
        TiXmlElement* cmdElt = new TiXmlElement("Command");
        cmdElt->SetAttribute("value", mCommand.c_str());
        evtElt->LinkEndChild(evtElt);
   }

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
    else if ((subElt = elt->FirstChildElement("Command")) != 0)
    {
        XmlHelpers::getAttribute(subElt,"value", attr);
        mCommand = attr;
    }

    return true;
}

} // namespace Solipsis
