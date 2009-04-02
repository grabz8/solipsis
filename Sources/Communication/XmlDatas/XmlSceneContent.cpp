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
#include "XmlSceneContent.h"

namespace Solipsis 
{
Pool XmlSceneContent::mPool;

Pool& XmlSceneContent::getStaticPool() { return mPool; }
Pool& XmlSceneContent::getPool() const { return mPool; }

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


} // namespace Solipsis
