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

#ifndef __TokamakBody_h__
#define __TokamakBody_h__

#include "IPhysicsBody.h"
#include "TokamakPluginPrerequisites.h"

#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

class TokamakCollider
{
public:
    virtual bool isAnimated() = 0;
    /** Collision callback. */
    virtual void collisionCallback(neCollisionInfo& collisionInfo) = 0;
};

/** This class manages a Tokamak body.
*/
class TokamakBody : public IPhysicsBody, public TokamakCollider
{
    friend class TokamakScene;

protected:
    /// Scene
    TokamakScene* mScene;
    /// Node
    SceneNode* mNode;
    /// Body
    neRigidBody* mBody;

public:
    TokamakBody(TokamakScene* scene);
    virtual ~TokamakBody();

    /// @copydoc IPhysicsBody::createBox
    virtual void createBox(SceneNode* node, const Vector3& extents);

    /// @copydoc IPhysicsBody::setPosition
    virtual void setPosition(const Vector3& position);
    /// @copydoc IPhysicsBody::setLinearVelocity
    virtual void setLinearVelocity(const Vector3& velocity);
    /// @copydoc IPhysicsBody::setAngularVelocity
    virtual void setAngularVelocity(const Vector3& velocity);

    bool isAnimated() { return false; }
    /** Collision callback. */
    void collisionCallback(neCollisionInfo& collisionInfo) {}
};

} // namespace Solipsis

#endif // #ifndef __TokamakBody_h__