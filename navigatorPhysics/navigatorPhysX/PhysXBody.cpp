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
