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

#ifndef __OgrePeer_h__
#define __OgrePeer_h__

#include "XmlDatas.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a basic Ogre representation of a peer.
 */
class OgrePeer
{
protected:
    /// Associated entity
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mXmlEntity;
#else
    XmlEntity* mXmlEntity;
#endif
    /// Type
    bool mIsLocal;
    /// Level of detail
    int mLod;

public:
    /** Constructor.
    */
#ifdef POOL
    OgrePeer(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal);
#else
    OgrePeer(XmlEntity* xmlEntity, bool isLocal);
#endif
    virtual ~OgrePeer();

    /** Gets the entity. */
#ifdef POOL
    RefCntPoolPtr<XmlEntity>& getXmlEntity();
#else
    XmlEntity* getXmlEntity();
#endif
    /** Gets the updated entity. */
#ifdef POOL
    virtual RefCntPoolPtr<XmlEntity>& getUpdatedXmlEntity() { return RefCntPoolPtr<XmlEntity>::nullPtr; }
#else
    virtual XmlEntity* getUpdatedXmlEntity() { return 0; }
#endif
    /** Test if this peer is a local entity. */
    bool isLocal() { return mIsLocal; }

    /** Update. */
    virtual void update(Real timeSinceLastFrame) = 0;
    /** Update entity. */
#ifdef POOL
    virtual bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity) = 0;
#else
    virtual bool updateEntity(XmlEntity* xmlEntity) = 0;
#endif
    /** Action on entity. */
#ifdef POOL
    virtual bool action(RefCntPoolPtr<XmlAction>& xmlAction) = 0;
#else
    virtual bool action(XmlAction* xmlAction) = 0;
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeer_h__