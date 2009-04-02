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
#include "XmlSceneLodContent.h"
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

Pool XmlSceneLodContent::mPool;


Pool& XmlSceneLodContent::getStaticPool() { return mPool; }
Pool& XmlSceneLodContent::getPool() const { return mPool; }


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


} // namespace Solipsis
