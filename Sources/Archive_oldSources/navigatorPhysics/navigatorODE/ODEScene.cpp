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

#include "ODEScene.h"
#include "ODEBody.h"
#include "ODECharacter.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
ODEScene::ODEScene() :
    mSceneManager(0),
    mWorld(0),
    mStepHandler(0),
    mWorldGeometry(0),
    mTimeSinceLastFrame(0.0f),
    mMaxStepInSeconds(1.0f/60.0f)
{
}

//-------------------------------------------------------------------------------------
ODEScene::~ODEScene()
{
    delete mWorldGeometry;
    delete mStepHandler;
    delete mWorld;
}

//-------------------------------------------------------------------------------------
bool ODEScene::create(SceneManager* sceneManager)
{
    mSceneManager = sceneManager;

    // Create the world
    mWorld = new OgreOde::World(sceneManager);
    mWorld->setGravity(Vector3(0.0f, -9.80665f, 0.0f));
    mWorld->setCFM(10e-5f);
    mWorld->setERP(0.8f);
    mWorld->setAutoSleep(true);
    mWorld->setContactCorrectionVelocity(0.1f);
    mWorld->setContactSurfaceLayer(0.01f);
    mWorld->setAutoSleepLinearThreshold(1.0f);
    mWorld->setAutoSleepAngularThreshold(1.0f);

    // Set the collision listener
    mWorld->setCollisionListener(dynamic_cast<OgreOde::CollisionListener*>(this));

    // Create something that will step the world, but don't do it automatically
    mStepHandler = new OgreOde::ForwardFixedStepHandler(mWorld, OgreOde::StepHandler::QuickStep, Real(1.0f/60.0f), Real(1000.0f), Real(1.0f));
    mStepHandler->setAutomatic(OgreOde::StepHandler::AutoMode_NotAutomatic, Root::getSingletonPtr());

    // Set the default time step
    setTiming(1.0f/60.0f);
    mTimeSinceLastFrame = 0.0f;

    return true;
}

//-------------------------------------------------------------------------------------
IPhysicsScene::DebugFlag ODEScene::getDebugFlags()
{
    assert(mWorld != 0);
    DebugFlag debugFlags = IPhysicsScene::DF_NONE;
    if (mWorld->getShowDebugContact())
        debugFlags = (IPhysicsScene::DebugFlag)(debugFlags | IPhysicsScene::DF_CONTACTS);
    if (mWorld->getShowDebugGeometries())
        debugFlags = (IPhysicsScene::DebugFlag)(debugFlags | IPhysicsScene::DF_GEOMETRIES);
    return debugFlags;
}

//-------------------------------------------------------------------------------------
void ODEScene::setDebugFlags(IPhysicsScene::DebugFlag debugFlags)
{
    assert(mWorld != 0);
    mWorld->setShowDebugContact((debugFlags & IPhysicsScene::DF_CONTACTS) != 0);
    mWorld->setShowDebugGeometries((debugFlags & IPhysicsScene::DF_GEOMETRIES) != 0);
}

//-------------------------------------------------------------------------------------
void ODEScene::setTiming(Real maxStepInSeconds)
{
    assert(maxStepInSeconds > 0.0f);
    assert(mStepHandler != 0);
    mMaxStepInSeconds = maxStepInSeconds;
    mStepHandler->setStepSize(mMaxStepInSeconds);
}

//-------------------------------------------------------------------------------------
void ODEScene::getTiming(Real& maxStepInSeconds, Real& timeSinceLastFrame)
{
    maxStepInSeconds = mMaxStepInSeconds;
    timeSinceLastFrame = mTimeSinceLastFrame;
}

//-------------------------------------------------------------------------------------
void ODEScene::preStep(Real timeSinceLastFrame)
{
    mTimeSinceLastFrame = timeSinceLastFrame;

    // Step physics (no threading in ODE)
    if (mStepHandler != 0)
        mStepHandler->step(mTimeSinceLastFrame);
    if (mWorld != 0)
        mWorld->synchronise();
}

//-------------------------------------------------------------------------------------
void ODEScene::postStep()
{
    // No threading in ODE
}

//-------------------------------------------------------------------------------------
void ODEScene::getGravity(Vector3& gravity)
{
    assert(mWorld != 0);
    gravity = mWorld->getGravity();
}

//-------------------------------------------------------------------------------------
void ODEScene::setGravity(Vector3& gravity)
{
    assert(mWorld != 0);
    mWorld->setGravity(gravity);
}

//-------------------------------------------------------------------------------------
bool ODEScene::setTerrainMesh(const Entity& entity)
{
    assert(mWorld != 0);
    OgreOde::EntityInformer entityInformer((Entity*)&entity, entity.getParentSceneNode()->_getFullTransform());
    mWorldGeometry = entityInformer.createStaticTriangleMesh(mWorld, mWorld->getDefaultSpace());
    return (mWorldGeometry != 0);
}

//-------------------------------------------------------------------------------------
IPhysicsBody* ODEScene::createBody()
{
    return new ODEBody(this);
}

//-------------------------------------------------------------------------------------
void ODEScene::destroyBody(IPhysicsBody* body)
{
    assert(body != 0);
    delete body;
}

//-------------------------------------------------------------------------------------
IPhysicsCharacter* ODEScene::createCharacter()
{
    return new ODECharacter(this);
}

//-------------------------------------------------------------------------------------
void ODEScene::destroyCharacter(IPhysicsCharacter* character)
{
    assert(character != 0);
    delete character;
}

//-------------------------------------------------------------------------------------
bool ODEScene::collision(OgreOde::Contact* contact)
{
    /*
    we have 2 collidable objects from our object system, if one of the Collide function returns false, we return false in this method, too,
    else we return true, so ode computes a normal collision.
    true means ode will treat this like a normal collison => rigid body behavior
    false means ode will not treat this collision at all => objects ignore each other
    */
    // Check for collisions between things that are connected and ignore them
/*    OgreOde::Geometry * const g1 = contact->getFirstGeometry();
    OgreOde::Geometry * const g2 = contact->getSecondGeometry();
    if (g1 && g2)
    {
        const OgreOde::Body * const b1 = g1->getBody();
        const OgreOde::Body * const b2 = g2->getBody();
        if (b1 && b2 && OgreOde::Joint::areConnected(b1, b2)) 
            return false; 
    }*/

    contact->setCoulombFriction(0.9f);
    contact->setBouncyness(0.25f);
    contact->setSoftness(0.8f, 10e-5f);

    OgreOde::Geometry * const g1 = contact->getFirstGeometry();
    OgreOde::Geometry * const g2 = contact->getSecondGeometry();
    if ((g1 != mWorldGeometry) && (g2 != mWorldGeometry))
    {
        OgreOde::Body * const b1 = g1->getBody();
        OgreOde::Body * const b2 = g2->getBody();
        if ((b1->getUserData() == 1) && (b2->getUserData() == 2))
            b1->addForceAt(contact->getNormal()*contact->getPenetrationDepth()*981, contact->getPosition());
        if ((b1->getUserData() == 2) && (b2->getUserData() == 1))
            b2->addForceAt(contact->getNormal()*contact->getPenetrationDepth()*981, contact->getPosition());
    }
/*    OgreOde::Body * const b1 = g1->getBody();
    OgreOde::Body * const b2 = g2->getBody();
    if (b1 != 0) b1->wake();
    if (b2 != 0) b2->wake();*/

    return true;
}

//-------------------------------------------------------------------------------------
