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

#ifndef __XmlLodContent_h__
#define __XmlLodContent_h__

#include "XmlDatas.h"

namespace Solipsis 
{
    class XMLDATAS_EXPORT XmlLodContent : public XmlData
    {

    protected:
        static Pool mPool;


    public:
        typedef std::list<LodContentFileStruct> LodContentFileList;

    protected:
        Lod mLevel;

        RefCntPoolPtr<XmlData> mDatas;
        LodContentFileList mLodContentFileList;

    public:
        XmlLodContent() :
          mDatas(RefCntPoolPtr<XmlData>::nullPtr)
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mLevel = 0;
              mDatas = RefCntPoolPtr<XmlData>::nullPtr;
              mLodContentFileList.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setLevel(Lod level) { mLevel = level; }
          Lod getLevel() { return mLevel; }


          void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
          RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }

          LodContentFileList& getLodContentFileList() { return mLodContentFileList; }
    };

    RefCntPoolPtr<XmlLodContent> RefCntPoolPtr<XmlLodContent>::nullPtr((XmlLodContent*)0);
} // namespace Solipsis

#endif // #ifndef __XmlLodContent_h__
