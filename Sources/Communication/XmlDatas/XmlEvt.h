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

#ifndef __XmlEvt_h__
#define __XmlEvt_h__

#include "XmlDatas.h"

namespace Solipsis 
{
    class XMLDATAS_EXPORT XmlEvt : public XmlData
    {
    protected:
        static Pool mPool;

    protected:
        EventType mType;
        RefCntPoolPtr<XmlData> mDatas;
        std::string mCommand;

    public:
        XmlEvt() :
            mType(ETNewEntity),
            mDatas(RefCntPoolPtr<XmlData>::nullPtr)
        {}

        XmlEvt(const EventType& type) :
            mType(type),
            mDatas(RefCntPoolPtr<XmlData>::nullPtr)
        {}

        static Pool& getStaticPool();
        virtual Pool& getPool() const;
        virtual void clear() {
            mType = ETNewEntity;
            mDatas = RefCntPoolPtr<XmlData>::nullPtr;
        }

        virtual std::string toXmlString() const;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const;
        virtual bool fromXmlElt(TiXmlElement* xmlElt);

        void setType(const EventType& type) { mType = type; }
        EventType getType() { return mType; }
        const std::string& getTypeRepr() { return XmlHelpers::convertEventTypeToRepr(mType); }

        void setDatas(RefCntPoolPtr<XmlData>& datas) { mDatas = datas; }
        RefCntPoolPtr<XmlData>& getDatas() { return mDatas; }

        void setCommand(const std::string & cmd) { mCommand = cmd; }
        const std::string& getCommand() { return mCommand; }
    };

    RefCntPoolPtr<XmlEvt> RefCntPoolPtr<XmlEvt>::nullPtr((XmlEvt*)0);
} // namespace Solipsis

#endif // #ifndef __XmlEvt_h__
