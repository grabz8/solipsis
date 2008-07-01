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

#ifndef __Avatar_h__
#define __Avatar_h__

#include "Entity.h"
#ifdef PHYSICSPLUGINS
#include "IPhysicsCharacter.h"
#endif

namespace Solipsis {

/** This class manages 1 avatar entity.
*/
class Avatar : public Entity
{
protected:
    /// Radius
    Ogre::Real mRadius;
    /// Height
    Ogre::Real mHeight;
#ifdef PHYSICSPLUGINS
    /// Physics Character
    IPhysicsCharacter* mPhysicsCharacter;
#endif
public: // TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO Entity class should have 1 processEvt() virtual method called by Node
    /// Dirty flag
    bool mDirty;
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mUpdatedXmlEntity;
#else
    XmlEntity mUpdatedXmlEntity;
#endif

public:
    /** Constructor. */
#ifdef POOL
    Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, const NodeId& managerNodeId);
#else
    Avatar(XmlEntity* xmlEntity, const NodeId& managerNodeId);
#endif
    /** Destructor. */
    virtual ~Avatar();

#ifdef PHYSICSPLUGINS
    /** See Entity. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** See Entity. */
    virtual void destroyPhysics();
#endif

    /** See Entity. */
    virtual bool update(Ogre::Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __Avatar_h__