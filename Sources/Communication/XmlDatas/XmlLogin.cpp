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
#include "XmlLogin.h"
// #include <Ogre.h>
// 
// #include <stdio.h>
// #include <stdlib.h>
// #include <iconv.h>
// 
// #include "XmlLogin.h"
// #include "XmlLodContent.h"
// #include "XmlSceneLodContent.h"
// #include "XmlContent.h"
// #include "XmlSceneContent.h"
// #include "XmlEntity.h"
// #include "XmlAction.h"
// #include "XmlEvt.h"

namespace Solipsis {


Pool XmlLogin::mPool;


Pool& XmlLogin::getStaticPool() { return mPool; }
Pool& XmlLogin::getPool() const { return mPool; }


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


} // namespace Solipsis
