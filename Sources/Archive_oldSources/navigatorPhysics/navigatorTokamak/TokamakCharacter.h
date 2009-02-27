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

#ifndef __TokamakCharacter_h__
#define __TokamakCharacter_h__

#include "IPhysicsCharacter.h"
#include "TokamakPluginPrerequisites.h"

#include "TokamakBody.h"

#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a Tokamak character.
*/
class TokamakCharacter : public IPhysicsCharacter, public TokamakCollider
{
protected:
    /// Scene
    TokamakScene* mScene;
    /// Description
    Desc mDesc;
    /// Node
    SceneNode* mNode;
    /// Capsule body
    neAnimatedBody* mCapsuleBody;
/*    neRigidBody* mCapsuleBody;*/
    /// Capsule body contact detected ?
    bool mCapsuleBodyContact;

public:
    TokamakCharacter(TokamakScene* scene);
    virtual ~TokamakCharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(SceneNode* node, Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);

    bool isAnimated() { return true; }
    /** Collision callback. */
    void collisionCallback(neCollisionInfo& collisionInfo);

protected:
    /** Move the character by step. */
    void moveStep(Vector3& displacement);
};

} // namespace Solipsis

#endif // #ifndef __TokamakCharacter_h__