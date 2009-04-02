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

#ifndef __XmlAction_h__
#define __XmlAction_h__

#include "XmlDatas.h"

namespace Solipsis 
{
    class XMLDATAS_EXPORT XmlAction : public XmlData
    {
    protected:
        static Pool mPool;


    protected:
        ActionType mType;
        EntityUID mSourceEntityUid;
        EntityUID mTargetEntityUid;
        bool mBroadcast;
        std::wstring mDesc;

    public:
        XmlAction() :
          mType(ATNone),
              mSourceEntityUid(""),
              mTargetEntityUid(""),
              mBroadcast(false),
              mDesc(L"")
          {}


          static Pool& getStaticPool();
          virtual Pool& getPool() const;
          virtual void clear() {
              mType = ATNone;
              mSourceEntityUid.clear();
              mTargetEntityUid.clear();
              mBroadcast = false;
              mDesc.clear();
          }


          virtual std::string toXmlString() const;
          virtual bool toXmlElt(TiXmlElement& xmlElt) const;
          virtual bool fromXmlElt(TiXmlElement* xmlElt);

          void setType(const ActionType& type) { mType = type; }
          ActionType getType() { return mType; }
          const std::string& getTypeRepr() { return XmlHelpers::convertActionTypeToRepr(mType); }

          void setSourceEntityUid(const EntityUID& sourceEntityUid) { mSourceEntityUid = sourceEntityUid; }
          const EntityUID& getSourceEntityUid() { return mSourceEntityUid; }

          void setTargetEntityUid(const EntityUID& targetEntityUid) { mTargetEntityUid = targetEntityUid; }
          const EntityUID& getTargetEntityUid() { return mTargetEntityUid; }

          void setBroadcast(bool broadcast) { mBroadcast = broadcast; }
          bool getBroadcast() { return mBroadcast; }

          void setDesc(const std::wstring& desc) { mDesc = desc; }
          const std::wstring& getDesc() { return mDesc; }
    };

    RefCntPoolPtr<XmlAction> RefCntPoolPtr<XmlAction>::nullPtr((XmlAction*)0);
} // namespace Solipsis

#endif // #ifndef __XmlAction_h__
