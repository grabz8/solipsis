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

#include "PhysXBody.h"
#include "PhysXScene.h"
#include "PhysXHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
PhysXBody::PhysXBody(PhysXScene* scene) :
    mScene(scene),
    mNxActor(0)
{
}

//-------------------------------------------------------------------------------------
PhysXBody::~PhysXBody()
{
    if (mNxActor != 0)
    {
        NxScene* nxScene = mScene->getNxScene();
        assert(nxScene != 0);
        nxScene->releaseActor(*mNxActor);
    }
}

//-------------------------------------------------------------------------------------
void PhysXBody::createBox(SceneNode* node, const Vector3& extents)
{
    assert(mNxActor == 0);
    assert(mScene != 0);

    NxScene* nxScene = mScene->getNxScene();

    NxBodyDesc boxBodyDesc;
    boxBodyDesc.setToDefault();
    boxBodyDesc.angularDamping = 0.5f;

    NxBoxShapeDesc boxShapeDesc;
    boxShapeDesc.dimensions = PhysXHelpers::Vector32NxVec3(extents*0.5f);
    boxShapeDesc.localPose.t = NxVec3(0.0f, 0.0f, 0.0f);

    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&boxShapeDesc);
    actorDesc.body = &boxBodyDesc;
    actorDesc.density = 1.0f;
    mNxActor = nxScene->createActor(actorDesc);
    mNxActor->userData = (void*)node;

    nxScene->setActorPairFlags(*(mScene->getNxActor()), *mNxActor, NX_NOTIFY_ON_TOUCH);
}

//-------------------------------------------------------------------------------------
void PhysXBody::setPosition(const Vector3& position)
{
    mNxActor->setGlobalPosition(PhysXHelpers::Vector32NxVec3(position));
}

//-------------------------------------------------------------------------------------
void PhysXBody::setLinearVelocity(const Vector3& velocity)
{
    mNxActor->setLinearVelocity(PhysXHelpers::Vector32NxVec3(velocity));
}

//-------------------------------------------------------------------------------------
void PhysXBody::setAngularVelocity(const Vector3& velocity)
{
    mNxActor->setAngularVelocity(PhysXHelpers::Vector32NxVec3(velocity));
}

//-------------------------------------------------------------------------------------
