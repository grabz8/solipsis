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

#ifndef __PhysXCharacter_h__
#define __PhysXCharacter_h__

#include <IPhysicsCharacter.h>
#include "PhysXPluginPrerequisites.h"

#include <NxPhysics.h>
#include <NxController.h>
#include <ControllerManager.h>
#include <NxCapsuleController.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX character.
*/
class PhysXCharacter : public IPhysicsCharacter
{
protected:
    /// Scene
    PhysXScene* mScene;
    /// Description
    Desc mDesc;
    /// Capsule controller
    NxCapsuleController* mNxCapsuleController;
#if 1 // GILLES FLY
    /// Collision?
    NxU32 mCollisionFlags;
#endif
public:
    PhysXCharacter(PhysXScene* scene);
    virtual ~PhysXCharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);
#if 1 // GILLES YES
	/// @copydoc IPhysicsCharacter::getCollision
	void getCollision(bool &collision){collision = (0 != mCollisionFlags);}
#endif
};

} // namespace Solipsis

#endif // #ifndef __PhysXCharacter_h__