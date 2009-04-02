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
#include "XmlContent.h"
#include "XmlSceneContent.h"


namespace Solipsis {



Pool XmlContent::mPool;

Pool& XmlContent::getStaticPool() { return mPool; }
Pool& XmlContent::getPool() const { return mPool; }

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

} // namespace Solipsis
