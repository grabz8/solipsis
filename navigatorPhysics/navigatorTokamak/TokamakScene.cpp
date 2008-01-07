#include "TokamakScene.h"
#include "TokamakBody.h"
#include "TokamakCharacter.h"
#include "TokamakHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
TokamakScene::TokamakScene() :
    mSceneManager(0),
    mPhysicsSim(0),
    mTimeSinceLastFrame(0.0f),
    mMaxStepInSeconds(1.0f/60.0f)
{
}

//-------------------------------------------------------------------------------------
TokamakScene::~TokamakScene()
{
    if (mPhysicsSim != 0)
        neSimulator::DestroySimulator(mPhysicsSim);
}

//-------------------------------------------------------------------------------------
bool TokamakScene::create(SceneManager* sceneManager)
{
    mSceneManager = sceneManager;

    // Create the scene
    neSimulatorSizeInfo simSizeInfo;
    mGravity = Vector3(0.0f, -9.80665f, 0.0f);
    mPhysicsSim = neSimulator::CreateSimulator(simSizeInfo, 0, &TokamakHelpers::Vector32neV3(mGravity));
    if (mPhysicsSim == 0)
        return false;

    // Set the collision callback
    mPhysicsSim->SetCollisionCallback(collisionCallback);

    // Set the default material 0
    mPhysicsSim->SetMaterial(0, 0.99f, 0.01f);

    // Set the default time step
    setTiming(1.0f/60.0f);
    mTimeSinceLastFrame = 0.0f;

    return true;
}

//-------------------------------------------------------------------------------------
void TokamakScene::setTiming(Real maxStepInSeconds)
{
    assert(maxStepInSeconds > 0.0f);
    mMaxStepInSeconds = maxStepInSeconds;
}

//-------------------------------------------------------------------------------------
void TokamakScene::getTiming(Real& maxStepInSeconds, Real& timeSinceLastFrame)
{
    maxStepInSeconds = mMaxStepInSeconds;
    timeSinceLastFrame = mTimeSinceLastFrame;
}

//-------------------------------------------------------------------------------------
void TokamakScene::preStep(Real timeSinceLastFrame)
{
    mTimeSinceLastFrame = timeSinceLastFrame;

    // Step physics (no threading in Tokamak)
    if (mPhysicsSim != 0)
    {
        Real totalTime = 0.0f;
        for (;totalTime < timeSinceLastFrame - mMaxStepInSeconds; totalTime += mMaxStepInSeconds)
            mPhysicsSim->Advance(mMaxStepInSeconds);
        // last step
        timeSinceLastFrame -= totalTime;
        mPhysicsSim->Advance(timeSinceLastFrame);

        // Update bodies
        for (std::map<String, neRigidBody*>::iterator it = mPhysicsBodies.begin();it != mPhysicsBodies.end(); ++it)
        {
            neRigidBody* body = it->second;
            TokamakBody* tokamakBody = (TokamakBody*)body->GetUserData();
            if (tokamakBody == 0) continue;
            SceneNode* node = tokamakBody->mNode;
            assert(node != 0);
            node->setPosition(TokamakHelpers::neV32Vector3(body->GetPos()));
            node->setOrientation(TokamakHelpers::neQ2Quaternion(body->GetRotationQ()));
        }
    }
}

//-------------------------------------------------------------------------------------
void TokamakScene::postStep()
{
    // No threading in Tokamak
}

//-------------------------------------------------------------------------------------
void TokamakScene::getGravity(Vector3& gravity)
{
    assert(mPhysicsSim != 0);
    // Gravity is set on simulation creation
    gravity = mGravity;
}

//-------------------------------------------------------------------------------------
void TokamakScene::setGravity(Vector3& gravity)
{
    assert(mPhysicsSim != 0);
    // Gravity is set on simulation creation
    mGravity = gravity;
}

//-------------------------------------------------------------------------------------
bool TokamakScene::setTerrainMesh(const Entity& entity)
{
    assert(mPhysicsSim != 0);

    // Create the world collision mesh
    mWorldGeometry = TokamakHelpers::convertMesh(entity.getMesh(),
        entity.getParentNode()->getWorldPosition(),
        entity.getParentNode()->getWorldOrientation(),
        entity.getParentNode()->getScale());
    mPhysicsSim->SetTerrainMesh(&mWorldGeometry);

    return true;
}

//-------------------------------------------------------------------------------------
IPhysicsBody* TokamakScene::createBody()
{
    return new TokamakBody(this);
}

//-------------------------------------------------------------------------------------
void TokamakScene::destroyBody(IPhysicsBody* body)
{
    assert(body != 0);
    delete body;
}

//-------------------------------------------------------------------------------------
IPhysicsCharacter* TokamakScene::createCharacter()
{
    return new TokamakCharacter(this);
}

//-------------------------------------------------------------------------------------
void TokamakScene::destroyCharacter(IPhysicsCharacter* character)
{
    assert(character != 0);
    delete character;
}

//-------------------------------------------------------------------------------------
void TokamakScene::collisionCallback(neCollisionInfo& collisionInfo)
{
    if (collisionInfo.bodyA != 0)
    {
        if (collisionInfo.typeA == NE_RIGID_BODY)
        {
            neAnimatedBody* animatedBody = (neAnimatedBody*)collisionInfo.bodyA;
            TokamakCharacter* character = (TokamakCharacter*)animatedBody->GetUserData();
            character->collisionCallback(collisionInfo);
        }
        else if (collisionInfo.typeA == NE_ANIMATED_BODY)
        {
            neRigidBody* rigidBody = (neRigidBody*)collisionInfo.bodyA;
            TokamakBody* body = (TokamakBody*)rigidBody->GetUserData();
            body->collisionCallback(collisionInfo);
        }
    }
    if (collisionInfo.bodyB != 0)
    {
        if (collisionInfo.typeB == NE_RIGID_BODY)
        {
            neAnimatedBody* animatedBody = (neAnimatedBody*)collisionInfo.bodyB;
            TokamakCharacter* character = (TokamakCharacter*)animatedBody->GetUserData();
            character->collisionCallback(collisionInfo);
        }
        else if (collisionInfo.typeB == NE_ANIMATED_BODY)
        {
            neRigidBody* rigidBody = (neRigidBody*)collisionInfo.bodyB;
            TokamakBody* body = (TokamakBody*)rigidBody->GetUserData();
            body->collisionCallback(collisionInfo);
        }
    }
}

//-------------------------------------------------------------------------------------
