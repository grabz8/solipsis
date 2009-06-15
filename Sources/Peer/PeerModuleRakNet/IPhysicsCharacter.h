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

#ifndef __IPhysicsCharacter_h__
#define __IPhysicsCharacter_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

namespace Solipsis {

/** This class represents a generic Physics character.
@remarks
Character is assumed centered on X and Z axis, Y origin is just under feet.
*/
class PEERMODULE_EXPORT IPhysicsCharacter
{
public:
    /// Description of 1 character
    typedef struct {
        /// Position 
        Ogre::Vector3 position;
        /// Radius
        Ogre::Real radius;
        /// Height
        Ogre::Real height;
        /// Max offset character can step up (stair's step, ...)
        Ogre::Real stepOffset;
    } Desc;
public:
    IPhysicsCharacter() {}
    virtual ~IPhysicsCharacter() {}

    /** Create.
    @remarks An implementation must be supplied for this method.
    */
    virtual void create(Desc& desc) = 0;
    /** Move.
    @remarks An implementation must be supplied for this method.
    */
    virtual void move(Ogre::Vector3& displacement) = 0;
    /** Get current position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getPosition(Ogre::Vector3& position) = 0;

#if 1 // GILLES YES
    /** Get a collision test.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getCollision(bool &collision) = 0;
#endif
};

} // end namespace

#endif // #ifndef __IPhysicsCharacter_h__