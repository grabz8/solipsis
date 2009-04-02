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

#ifndef __XmlSceneContent_h__
#define __XmlSceneContent_h__

#include "XmlDatas.h"

namespace Solipsis 
{
    class XMLDATAS_EXPORT XmlSceneContent : public XmlData
    {

    protected:
        static Pool mPool;

    protected:
        EntryGateStruct mEntryGate;

    public:
        XmlSceneContent()
        {
            mEntryGate.mGravity = false;
            mEntryGate.mPosition = Ogre::Vector3::ZERO;
        }


        static Pool& getStaticPool();
        virtual Pool& getPool() const;
        virtual void clear() {
            mEntryGate.mGravity = false;
            mEntryGate.mPosition = Ogre::Vector3::ZERO;
        }


        virtual std::string toXmlString() const;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const;
        virtual bool fromXmlElt(TiXmlElement* xmlElt);

        void setEntryGate(const EntryGateStruct& entryGate) { mEntryGate = entryGate; }
        const EntryGateStruct& getEntryGate() { return mEntryGate; }
    };

    RefCntPoolPtr<XmlSceneContent> RefCntPoolPtr<XmlSceneContent>::nullPtr((XmlSceneContent*)0);

} // namespace Solipsis

#endif // #ifndef __XmlSceneContent_h__
