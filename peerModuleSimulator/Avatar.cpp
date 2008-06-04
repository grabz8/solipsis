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

#include "Avatar.h"

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Avatar::Avatar(XmlEntity* xmlEntity) :
    Entity(xmlEntity)
#ifdef PHYSICSPLUGINS
    ,mPhysicsCharacter(0)
#endif
{
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
void Avatar::createPhysics(IPhysicsScene* physicsScene)
{
    Entity::createPhysics(physicsScene);

    // Compute radius and height of character
    Vector3 aabbHalfSize = mXmlEntity->getAABoundingBox().getHalfSize();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    IPhysicsCharacter::Desc characterDesc;
    characterDesc.position = mXmlEntity->getPosition();
    characterDesc.radius = mRadius;
    characterDesc.height = mHeight;
    characterDesc.stepOffset = mRadius;
    mPhysicsCharacter = mPhysicsScene->createCharacter();
    mPhysicsCharacter->create(characterDesc);

    mGravity = mXmlEntity->getFlags() & EFGravity;

    mDirty = false;
}

//-------------------------------------------------------------------------------------
void Avatar::destroyPhysics()
{
    if ((mPhysicsScene != 0) && (mPhysicsCharacter != 0))
        mPhysicsScene->destroyCharacter(mPhysicsCharacter);

    mPhysicsCharacter = 0;

    Entity::destroyPhysics();
}
#endif

//-------------------------------------------------------------------------------------
bool Avatar::update(Real timeSinceLastFrame)
{
#ifdef PHYSICSPLUGINS
    // Move physics character
    if (mPhysicsCharacter != 0)
    {
        Vector3 displacement = mXmlEntity->getDisplacement()*timeSinceLastFrame;
        if (mGravity)
            displacement.y += -9.80665f*timeSinceLastFrame;
        mPhysicsCharacter->move(displacement);
        Vector3 newPosition;
        mPhysicsCharacter->getPosition(newPosition);
        if ((newPosition - mXmlEntity->getPosition()).squaredLength() > 0.0001f)
            mDirty = true;
        mXmlEntity->setPosition(newPosition);
    }
#endif

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
