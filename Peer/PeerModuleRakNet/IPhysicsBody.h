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

#ifndef __IPhysicsBody_h__
#define __IPhysicsBody_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

namespace Solipsis {

/** This class represents a generic Physics body.
*/
class PEERMODULE_EXPORT IPhysicsBodyListener
{
public:
    /** Sets the position of the body.
    */
    virtual void setPosition(const Ogre::Vector3& pos) = 0;
    /** Sets the orientation of this node via a quaternion.
    */
    virtual void setOrientation(const Ogre::Quaternion& q) = 0;
};

/** This class represents a generic Physics body.
*/
class PEERMODULE_EXPORT IPhysicsBody
{
public:
    IPhysicsBody() {}
    virtual ~IPhysicsBody() {}

    /** Create a box.
    @remarks An implementation must be supplied for this method.
    */
    virtual void createBox(IPhysicsBodyListener* listener, const Ogre::Vector3& extents) = 0;

    /** Set position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setPosition(const Ogre::Vector3& position) = 0;
    /** Set linear velocity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setLinearVelocity(const Ogre::Vector3& velocity) = 0;
    /** Set angular velocity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setAngularVelocity(const Ogre::Vector3& velocity) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsBody_h__