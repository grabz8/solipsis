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

#include "TokamakCharacter.h"
#include "TokamakScene.h"
#include "TokamakHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
TokamakCharacter::TokamakCharacter(TokamakScene* scene) :
    mScene(scene),
    mNode(0),
    mCapsuleBody(0)
{
}

//-------------------------------------------------------------------------------------
TokamakCharacter::~TokamakCharacter()
{
}

//-------------------------------------------------------------------------------------
void TokamakCharacter::create(SceneNode* node, Desc& desc)
{
    assert(mCapsuleBody == 0);
    assert(mScene != 0);

    mDesc = desc;
    neSimulator* physicsSim = mScene->getSimulator();

    // Create the torso collidable body
    mCapsuleBody = physicsSim->CreateAnimatedBody();
    mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(desc.position));
    neGeometry* capsuleGeom = mCapsuleBody->AddGeometry();
    capsuleGeom->SetCylinder(mDesc.radius*2, mDesc.height);
    capsuleGeom->SetMaterialIndex(0);
    mCapsuleBody->UpdateBoundingInfo();
    mCapsuleBody->SetCollisionID(1);
    mNode = node;
//    mCapsuleBody->SetUserData((u32)this);
    mCapsuleBody->SetUserData((u32)((TokamakCollider*)this));
/*    //BEGIN
    mCapsuleBody = physicsSim->CreateRigidBody();
    mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(desc.position));
    neGeometry* capsuleGeom = mCapsuleBody->AddGeometry();
    capsuleGeom->SetCylinder(mDesc.radius*2, mDesc.height);
    mCapsuleBody->SetMass(1.0f);
    mCapsuleBody->GravityEnable(false);
    mCapsuleBody->SetInertiaTensor(neCylinderInertiaTensor(mDesc.radius*2, mDesc.height, mCapsuleBody->GetMass()));
    capsuleGeom->SetMaterialIndex(0);
    mCapsuleBody->UpdateBoundingInfo();
    mCapsuleBody->SetCollisionID(1);
    mNode = node;
    mCapsuleBody->SetUserData((u32)((TokamakCollider*)this));
    //END*/
}

//-------------------------------------------------------------------------------------
void TokamakCharacter::move(Vector3& displacement)
{
    assert(mCapsuleBody != 0);

    Real maxStepInSeconds, timeSinceLastFrame;
    mScene->getTiming(maxStepInSeconds, timeSinceLastFrame);

    Vector3 displacementStep = displacement;
    if ((maxStepInSeconds > 0.0f) && (timeSinceLastFrame > maxStepInSeconds))
    {
        Vector3 velocity = displacement/timeSinceLastFrame;
        displacementStep = velocity*maxStepInSeconds;
        Real totalTime = 0.0f;
        for (;totalTime < timeSinceLastFrame - maxStepInSeconds; totalTime += maxStepInSeconds)
            moveStep(displacementStep);
        displacementStep = velocity*(timeSinceLastFrame - totalTime);
    }
    // last step
    moveStep(displacementStep);
}

//-------------------------------------------------------------------------------------
void TokamakCharacter::getPosition(Vector3& position)
{
    position = TokamakHelpers::neV32Vector3(mCapsuleBody->GetPos());
}

//-------------------------------------------------------------------------------------
void TokamakCharacter::collisionCallback(neCollisionInfo& collisionInfo)
{
    mCapsuleBodyContact = true;
}

//-------------------------------------------------------------------------------------
void TokamakCharacter::moveStep(Vector3& displacement)
{
    assert(mCapsuleBody != 0);

    displacement *= Vector3(1.0f, 0.0f, 1.0f);

    Vector3 oldPosition = TokamakHelpers::neV32Vector3(mCapsuleBody->GetPos()) - Vector3(0.0f, 0.0f, 0.0f);
    Vector3 newPosition = oldPosition + displacement;
    // Collide physics capsule body with scene
    Vector3 savedPosition = newPosition;
/*    //BEGIN
    mCapsuleBody->SetAngularMomentum(TokamakHelpers::Vector32neV3(Vector3::ZERO));
	mCapsuleBody->SetTorque(TokamakHelpers::Vector32neV3(Vector3::ZERO));
    mCapsuleBody->SetRotation(TokamakHelpers::Quaternion2neQ(Quaternion::IDENTITY));
    //END*/
    mCapsuleBodyContact = false;
    mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(newPosition + Vector3(0.0f, 0.0f, 0.0f)));
    if (mCapsuleBodyContact)
    {
//        newPosition = oldPosition + displacement*Vector3(1.0f, 0.0f, 1.0f);
//        mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(newPosition));
    }
}

//-------------------------------------------------------------------------------------
