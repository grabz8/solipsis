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
    mCapsuleBody->SetCollisionID(0);
    mNode = node;
    mCapsuleBody->SetUserData((u32)this);
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
    mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(newPosition + Vector3(0.0f, 0.0f, 0.0f)));
    mCapsuleBodyContact = false;
    if (mCapsuleBodyContact)
    {
//        newPosition = oldPosition + displacement*Vector3(1.0f, 0.0f, 1.0f);
//        mCapsuleBody->SetPos(TokamakHelpers::Vector32neV3(newPosition));
    }
}

//-------------------------------------------------------------------------------------
