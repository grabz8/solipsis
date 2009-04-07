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

#include "PhysXScene.h"
#include "PhysXEngine.h"
#include "PhysXBody.h"
#include "PhysXCharacter.h"
#include "PhysXHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
PhysXScene::PhysXScene(PhysXEngine* engine) :
    mEngine(engine),
    mNxScene(0),
    mNxControllerManager(0),
    mNxGeometry(0),
    mNxActor(0),
    m_bTerrainMeshLoaded(false),
    mTimeSinceLastFrame(0.0f),
    mMaxStepInSeconds(1.0f/60.0f),
    mFirstStep(true)
{
}

//-------------------------------------------------------------------------------------
PhysXScene::~PhysXScene()
{
    if (PhysXHelpers::getNxPhysicsSDK() == 0)
        return;

    NxReleaseControllerManager(mNxControllerManager);

    if (mNxScene != 0)
    {
        NxActor** actors = mNxScene->getActors();
        for (NxU32 a=0; a<mNxScene->getNbActors(); ++a)
        {
            NxActor* actor = actors[a];
            mNxScene->releaseActor(*actor);
        }
        PhysXHelpers::getNxPhysicsSDK()->releaseScene(*mNxScene);
    }
}

//-------------------------------------------------------------------------------------
bool PhysXScene::create()
{
    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = NxVec3(0.0f, -9.80665f, 0.0f);
    mNxScene = PhysXHelpers::getNxPhysicsSDK()->createScene(sceneDesc);
    if (mNxScene == 0)
        return false;

    // Set the default material 0
	NxMaterial* defaultMaterial = mNxScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

    // Set the default time step
    setTiming(1.0f/60.0f);
    mTimeSinceLastFrame = 0.0f;

    // Create the controller manager
    mNxControllerManager = NxCreateControllerManager(PhysXHelpers::getNxUserAllocatorDefault());
    if (mNxControllerManager == 0)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
void PhysXScene::setTiming(Real maxStepInSeconds)
{
    assert(maxStepInSeconds > 0.0f);
    mMaxStepInSeconds = maxStepInSeconds;
    mNxScene->setTiming(mMaxStepInSeconds, 8, NX_TIMESTEP_FIXED);
}

//-------------------------------------------------------------------------------------
void PhysXScene::getTiming(Real& maxStepInSeconds, Real& timeSinceLastFrame)
{
    maxStepInSeconds = mMaxStepInSeconds;
    timeSinceLastFrame = mTimeSinceLastFrame;
}

//-------------------------------------------------------------------------------------
void PhysXScene::preStep(Real timeSinceLastFrame)
{
    mTimeSinceLastFrame = timeSinceLastFrame;

    // Step physics part 1
    if (mNxScene != 0)
    {
//        mNxScene->simulate(evt.timeSinceLastFrame);
//        mNxScene->flushStream();
//        mNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
        // To avoid blocking on fetchResults(), we will shift actions
        // in order to let simulation threading during the rendering
        if (!mFirstStep)
            mNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

        // Update controllers
        NxReal maxTimestep;
        NxTimeStepMethod method;
        NxU32 maxIter;
        NxU32 numSubSteps;
        mNxScene->getTiming(maxTimestep, maxIter, method, &numSubSteps);
        if (numSubSteps)
            mNxControllerManager->updateControllers();

        // Update actors
        NxActor** actors = mNxScene->getActors();
        for (NxU32 a=0; a<mNxScene->getNbActors(); ++a)
        {
            NxActor* actor = actors[a];
            if (actor->userData == 0) continue;
            IPhysicsBodyListener* listener = (IPhysicsBodyListener*)actor->userData;
            listener->setPosition(PhysXHelpers::NxVec32Vector3(actor->getGlobalPosition()));
            listener->setOrientation(PhysXHelpers::NxQuat2Quaternion(actor->getGlobalOrientationQuat()));
        }
    }
}

//-------------------------------------------------------------------------------------
void PhysXScene::postStep()
{
    // Step physics part 2
    if (mNxScene != 0)
    {
        mNxScene->simulate(mTimeSinceLastFrame);
        mNxScene->flushStream();
        mFirstStep = false;
    }
}

//-------------------------------------------------------------------------------------
void PhysXScene::getGravity(Vector3& gravity)
{
    NxVec3 v;
    mNxScene->getGravity(v);
    gravity = PhysXHelpers::NxVec32Vector3(v);
}

//-------------------------------------------------------------------------------------
void PhysXScene::setGravity(Vector3& gravity)
{
    mNxScene->setGravity(PhysXHelpers::Vector32NxVec3(gravity));
}

//-------------------------------------------------------------------------------------
bool PhysXScene::setTerrainMesh(const MeshPtr mesh,
                                const Vector3& position,
                                const Quaternion& orientation,
                                const Vector3& scale)
{
    m_bTerrainMeshLoaded = true;
    mNxGeometry = PhysXHelpers::cookMesh(mesh, position, orientation, scale);
    if (mNxGeometry == 0)
        return false;
    NxTriangleMeshShapeDesc triangleMeshShapeDesc;
    NxActorDesc actorDesc;
    triangleMeshShapeDesc.meshData = mNxGeometry;
    triangleMeshShapeDesc.group = IPhysicsScene::CG_COLLIDABLE_NON_PUSHABLE;
    actorDesc.shapes.pushBack(&triangleMeshShapeDesc);
    mNxActor = mNxScene->createActor(actorDesc);
    mNxActor->userData = (void*)0;

    return true;
}

//-------------------------------------------------------------------------------------
void PhysXScene::unsetTerrainMesh()
{
    if (mNxActor == 0) return;
    m_bTerrainMeshLoaded = false;
    mNxScene->releaseActor(*mNxActor);
    mNxActor = 0;
}

//-------------------------------------------------------------------------------------
bool PhysXScene::hasTerrainmesh()
{
    return m_bTerrainMeshLoaded;
}

//-------------------------------------------------------------------------------------
IPhysicsBody* PhysXScene::createBody()
{
    return new PhysXBody(this);
}

//-------------------------------------------------------------------------------------
void PhysXScene::destroyBody(IPhysicsBody* body)
{
    assert(body != 0);
    delete body;
}

//-------------------------------------------------------------------------------------
IPhysicsCharacter* PhysXScene::createCharacter()
{
    return new PhysXCharacter(this);
}

//-------------------------------------------------------------------------------------
void PhysXScene::destroyCharacter(IPhysicsCharacter* character)
{
    assert(character != 0);
    delete character;
}

//-------------------------------------------------------------------------------------
