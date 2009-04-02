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
#include "XmlLodContent.h"
#include "XmlSceneLodContent.h"

namespace Solipsis {



Pool XmlLodContent::mPool;

Pool& XmlLodContent::getStaticPool() { return mPool; }
Pool& XmlLodContent::getPool() const { return mPool; }


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


} // namespace Solipsis
