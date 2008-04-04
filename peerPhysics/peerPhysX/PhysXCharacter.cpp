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
    NxU32 collisionFlags;
    mNxCapsuleController->move(PhysXHelpers::Vector32NxVec3(displacement), IPhysicsScene::CG_COLLIDABLE_MASK, 0.001f, collisionFlags, 1.0f);

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
