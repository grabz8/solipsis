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

#include "PhysXCharacter.h"
#include "PhysXScene.h"
#include "PhysXHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
PhysXCharacter::PhysXCharacter(PhysXScene* scene) :
    mScene(scene),
    mNxCapsuleController(0)
{
}

//-------------------------------------------------------------------------------------
PhysXCharacter::~PhysXCharacter()
{
    if (mNxCapsuleController != 0)
    {
        NxControllerManager* nxControllerManager = mScene->getNxControllerManager();
        assert(nxControllerManager != 0);
        nxControllerManager->releaseController(*mNxCapsuleController);
    }
}

//-------------------------------------------------------------------------------------
void PhysXCharacter::create(Desc& desc)
{
    assert(mNxCapsuleController == 0);
    assert(mScene != 0);

    mDesc = desc;
    NxScene* nxScene = mScene->getNxScene();
    assert(nxScene != 0);

    NxCapsuleControllerDesc capsuleControllerDesc;
    capsuleControllerDesc.interactionFlag = NXIF_INTERACTION_INCLUDE;
    capsuleControllerDesc.position.set(mDesc.position.x, mDesc.position.y + (mDesc.height - mDesc.radius), mDesc.position.z);
    capsuleControllerDesc.radius = mDesc.radius*0.5f;
    capsuleControllerDesc.height = mDesc.height - mDesc.radius;
    capsuleControllerDesc.upDirection = NX_Y;
    capsuleControllerDesc.slopeLimit = cosf(NxMath::degToRad(45.0f));
    capsuleControllerDesc.skinWidth = 0.1f;
    capsuleControllerDesc.stepOffset = mDesc.radius;
    capsuleControllerDesc.callback = (NxUserControllerHitReport*)0;
//    capsuleControllerDesc.userData = (void*)listener;
    NxControllerManager* nxControllerManager = mScene->getNxControllerManager();
    assert(nxControllerManager != 0);
    mNxCapsuleController = (NxCapsuleController*)nxControllerManager->createController(nxScene, capsuleControllerDesc);
}

//-------------------------------------------------------------------------------------
void PhysXCharacter::move(Vector3& displacement)
{
    assert(mNxCapsuleController != 0);

/*    IPhysicsCharacterListener* listener = (IPhysicsCharacterListener*)mNxCapsuleController->getUserData();
    NxExtendedVec3 newPos = mNxCapsuleController->getFilteredPosition();
    listener->setPosition(newPos.x, newPos.y - (mDesc.height - mDesc.radius), newPos.z);*/
#if 1 // GILLES FLY
    mNxCapsuleController->move(PhysXHelpers::Vector32NxVec3(displacement), IPhysicsScene::CG_COLLIDABLE_MASK, 0.001f, mCollisionFlags, 1.0f);
#else
    NxU32 collisionFlags;
    mNxCapsuleController->move(PhysXHelpers::Vector32NxVec3(displacement), IPhysicsScene::CG_COLLIDABLE_MASK, 0.001f, collisionFlags, 1.0f);
#endif

#ifdef VRD
    NxPhysicsSDK* sdk = PhysXHelpers::getNxPhysicsSDK();
    NxRemoteDebugger* rd = sdk->getFoundationSDK().getRemoteDebugger();
    NxExtendedVec3 pos = mNxCapsuleController->getFilteredPosition();
    NxVec3 target(pos.x, pos.y, pos.z);
    NxVec3 eye = target + NxVec3(0.0f, 20.0f, 0.0f);
    rd->writeParameter(eye, sdk, true, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
    rd->writeParameter(target, sdk, true, "Target", NX_DBG_EVENTMASK_EVERYTHING);
#endif
}

//-------------------------------------------------------------------------------------
void PhysXCharacter::getPosition(Vector3& position)
{
    assert(mNxCapsuleController != 0);
    NxExtendedVec3 NxPosition = mNxCapsuleController->getFilteredPosition();
    position = Vector3(NxPosition.x, NxPosition.y - (mDesc.height - mDesc.radius), NxPosition.z);
}

//-------------------------------------------------------------------------------------
