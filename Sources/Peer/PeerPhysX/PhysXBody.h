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

#ifndef __PhysXBody_h__
#define __PhysXBody_h__

#include <IPhysicsBody.h>
#include "PhysXPluginPrerequisites.h"

#include <NxPhysics.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX body.
*/
class PhysXBody : public IPhysicsBody
{
protected:
    /// Scene
    PhysXScene* mScene;
    /// Actor
    NxActor* mNxActor;

public:
    PhysXBody(PhysXScene* scene);
    virtual ~PhysXBody();

    /// @copydoc IPhysicsBody::createBox
    virtual void createBox(IPhysicsBodyListener* listener, const Vector3& extents);

    /// @copydoc IPhysicsBody::setPosition
    virtual void setPosition(const Vector3& position);
    /// @copydoc IPhysicsBody::setLinearVelocity
    virtual void setLinearVelocity(const Vector3& velocity);
    /// @copydoc IPhysicsBody::setAngularVelocity
    virtual void setAngularVelocity(const Vector3& velocity);
};

} // namespace Solipsis

#endif // #ifndef __PhysXBody_h__