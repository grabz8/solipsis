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

#ifndef __ODECharacter_h__
#define __ODECharacter_h__

#include "IPhysicsCharacter.h"
#include "ODEPluginPrerequisites.h"

#include "OgreOde_Core.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a ODE character.
*/
class ODECharacter : public IPhysicsCharacter, public OgreOde::CollisionListener
{
protected:
    /// Scene
    ODEScene* mScene;
    /// Description
    Desc mDesc;
    /// Ray to compute feet contact
    OgreOde::RayGeometry* mRayGeom;
    /// Last ray geometry contact
    OgreOde::Contact mRayGeomLastContact;
    /// Ray geometry contact detected ?
    bool mRayGeomContact;
    /// Maximum time step to update collision
    Real mMaxUpdateTimeStep;
    /// Capsule to compute body contact
    OgreOde::CapsuleGeometry* mCapsuleGeom;
    /// Last capsule geometry contact
    OgreOde::Contact mCapsuleGeomLastContact;
    /// Capsule geometry contact detected ?
    bool mCapsuleGeomContact;
    /// Capsule transform geometry
    OgreOde::TransformGeometry* mCapsuleBodyTrans;
    /// Capsule to compute body collisions
    OgreOde::CapsuleGeometry* mCapsuleBodyGeom;
    /// Body capsule
    OgreOde::Body* mCapsuleBody;

public:
    ODECharacter(ODEScene* scene);
    virtual ~ODECharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(SceneNode* node, Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);

protected:
    /** Move the character by step. */
    void moveStep(Vector3& displacement);

    /** See OgreOde::CollisionListener. */
    bool collision(OgreOde::Contact* contact);
};

} // namespace Solipsis

#endif // #ifndef __ODECharacter_h__