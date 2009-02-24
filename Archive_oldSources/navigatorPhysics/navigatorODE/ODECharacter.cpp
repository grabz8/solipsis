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

#include "ODECharacter.h"
#include "ODEScene.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
ODECharacter::ODECharacter(ODEScene* scene) :
    mScene(scene),
    mRayGeom(0),
    mCapsuleGeom(0),
    mCapsuleGeomContact(false),
    mCapsuleBodyTrans(0),
    mCapsuleBodyGeom(0),
    mCapsuleBody(0)
{
}

//-------------------------------------------------------------------------------------
ODECharacter::~ODECharacter()
{
    delete mCapsuleBodyGeom;
    delete mCapsuleBodyTrans;
    delete mCapsuleBody;
    delete mCapsuleGeom;
    delete mRayGeom;
}

//-------------------------------------------------------------------------------------
void ODECharacter::create(SceneNode* node, Desc& desc)
{
    assert(mCapsuleBody == 0);
    assert(mScene != 0);

    mDesc = desc;
    OgreOde::World* world = mScene->getWorld();

    // Create the ray for feet
    mRayGeom = new OgreOde::RayGeometry(mDesc.height, world);

    // Create the torso collision geometry
    mCapsuleGeom = new OgreOde::CapsuleGeometry(mDesc.radius*0.5f, mDesc.height*0.25f, world);
    Quaternion upQuat;
    upQuat.FromAngleAxis(Radian(-Math::HALF_PI), Vector3::UNIT_X);
    mCapsuleGeom->setOrientation(upQuat);
    mCapsuleGeom->setPosition(node->getPosition() + Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f));

    // Create the torso collidable body
    mCapsuleBody = new OgreOde::Body(world, node->getName() + "_body");
    mCapsuleBody->setMass(OgreOde::CapsuleMass(1.0f, mDesc.radius*0.5f, Vector3::UNIT_Y, mDesc.height*0.5f));
    mCapsuleBody->setAffectedByGravity(false);
    mCapsuleBody->setAutoSleep(false);
    mCapsuleBody->setUserData(2);
    mCapsuleBodyTrans = new OgreOde::TransformGeometry(world, world->getDefaultSpace());
    mCapsuleBodyGeom = new OgreOde::CapsuleGeometry(mDesc.radius*0.5f, mDesc.height*0.5f, world);
    mCapsuleBodyGeom->setPosition(Vector3(0.0f, mDesc.height*0.5f, 0.0f));
    mCapsuleBodyGeom->setOrientation(Quaternion(Degree(90), Vector3::UNIT_X));
    mCapsuleBodyTrans->setBody(mCapsuleBody);
    mCapsuleBodyTrans->setEncapsulatedGeometry(mCapsuleBodyGeom);
    mCapsuleBody->setPosition(node->getPosition());
}

//-------------------------------------------------------------------------------------
void ODECharacter::move(Vector3& displacement)
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
void ODECharacter::getPosition(Vector3& position)
{
    position = mCapsuleGeom->getPosition() - Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f);
}

//-------------------------------------------------------------------------------------
void ODECharacter::moveStep(Vector3& displacement)
{
    assert(mCapsuleBody != 0);

    OgreOde::TriangleMeshGeometry* worldGeometry = mScene->getWorldGeometry();

    Vector3 oldPosition = mCapsuleGeom->getPosition() - Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f);
    Vector3 newPosition = oldPosition + displacement;
    // Collide physics ray with world
    // fire ray downward to collisionListener
    mRayGeom->setDefinition(newPosition + Vector3(0.0f, mDesc.height*0.5f, 0.0f), Vector3::NEGATIVE_UNIT_Y);
    mRayGeomContact = false;
    mRayGeom->collide(worldGeometry, (OgreOde::CollisionListener*)this);
    if (mRayGeomContact && (mRayGeomLastContact.getPosition().y >= newPosition.y))
        newPosition.y = mRayGeomLastContact.getPosition().y;
    // Collide physics capsule geometry with world
    // test capsule to collisionListener
    Vector3 savedPosition = newPosition;
    mCapsuleGeom->setPosition(newPosition + Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f));
    mCapsuleGeomContact = false;
    mCapsuleGeom->collide(worldGeometry, (OgreOde::CollisionListener*)this);
    if (mCapsuleGeomContact)
    {
        static int sl = 0;
        sl++;
        if ((sl % 60) == 0)
        {
            StringUtil::StrStreamType log;
            log.precision(2);
            log.width(5);
            log << "Avatar::animate() mCapsuleGeomLastContact.(pos=("
                << mCapsuleGeomLastContact.getPosition().x << ", " << mCapsuleGeomLastContact.getPosition().y << ", " << mCapsuleGeomLastContact.getPosition().z
                << "), normal=("
                << mCapsuleGeomLastContact.getNormal().x << ", " << mCapsuleGeomLastContact.getNormal().y << ", " << mCapsuleGeomLastContact.getNormal().z
                << "), depth="
                << mCapsuleGeomLastContact.getPenetrationDepth();
            OGRE_LOG(log.str());
        }
        Plane contactPlane(mCapsuleGeomLastContact.getNormal(), mCapsuleGeomLastContact.getPosition());
        Vector3 mvtOnContactPlane;
#if (OGRE_VERSION < ((1 << 16) | (4 << 8) | 5))
        // 1 bug on projectVector() in Ogre < 1.4.5
        mvtOnContactPlane = -contactPlane.projectVector(displacement)*Vector3(1.0f, 0.0f, 1.0f);
#else
        mvtOnContactPlane = contactPlane.projectVector(displacement)*Vector3(1.0f, 0.0f, 1.0f);
#endif
        if ((sl % 60) == 0)
        {
            StringUtil::StrStreamType log;
            log.precision(2);
            log.width(5);
            log << "Avatar::animate() mvtOnContactPlane=("
                << mvtOnContactPlane.x << ", " << mvtOnContactPlane.y << ", " << mvtOnContactPlane.z
                << ")";
            OGRE_LOG(log.str());
        }
        newPosition = savedPosition - displacement*Vector3(1.0f, 0.0f, 1.0f) + mvtOnContactPlane;
        mCapsuleGeom->setPosition(newPosition + Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f));
        mCapsuleGeomContact = false;
        mCapsuleGeom->collide(worldGeometry, (OgreOde::CollisionListener*)this);
        if (mCapsuleGeomContact)
        {
            if ((sl % 60) == 0)
            {
                StringUtil::StrStreamType log;
                log.precision(2);
                log.width(5);
                log << "Avatar::animate() 2nd contact mCapsuleGeomLastContact.(pos=("
                    << mCapsuleGeomLastContact.getPosition().x << ", " << mCapsuleGeomLastContact.getPosition().y << ", " << mCapsuleGeomLastContact.getPosition().z
                    << "), normal=("
                    << mCapsuleGeomLastContact.getNormal().x << ", " << mCapsuleGeomLastContact.getNormal().y << ", " << mCapsuleGeomLastContact.getNormal().z
                    << "), depth="
                    << mCapsuleGeomLastContact.getPenetrationDepth();
                OGRE_LOG(log.str());
            }
            newPosition = oldPosition;
            mCapsuleGeom->setPosition(newPosition + Vector3(0.0f, mDesc.height*0.5f*1.5f, 0.0f));
        }
    }

    // Move the collidable body
    mCapsuleBody->setPosition(newPosition);
    mCapsuleBody->setOrientation(Quaternion::IDENTITY);
    mCapsuleBody->setLinearVelocity(displacement);
    mCapsuleBody->setAngularVelocity(Vector3::ZERO);
}

//-------------------------------------------------------------------------------------
bool ODECharacter::collision(OgreOde::Contact* contact)
{
    if (mRayGeom == 0) return true;

    if (contact->getFirstGeometry()->getID() == mRayGeom->getID() ||
	    contact->getSecondGeometry()->getID() == mRayGeom->getID())
    {
        mRayGeomLastContact = *contact;
        mRayGeomContact = true;
    }
    else if (contact->getFirstGeometry()->getID() == mCapsuleGeom->getID() ||
        contact->getSecondGeometry()->getID() == mCapsuleGeom->getID())
    {
        mCapsuleGeomLastContact = *contact;
        mCapsuleGeomContact = true;
    }

    return true;
}

//-------------------------------------------------------------------------------------

// Just some source code to sample movement instead of frameTime into update() and animate()
// => seems good but no feet responses ??!??!!? so still some stuff ...
#if 0
/*
    if (mMaxUpdateTimeStep > 0)
    {
        Real totalTime = 0.0;
        for (;totalTime < timeSinceLastFrame - mMaxUpdateTimeStep; totalTime += mMaxUpdateTimeStep)
            animate(mMaxUpdateTimeStep);
        // last step
        timeSinceLastFrame -= totalTime;
    }
*/
    // Move it !
    Real mvtDist = mvt.length();
    Vector3 mvtVector = mvt;
    mvtVector.normalise();
    Real mvtTotal = 0.0;
    bool breakAfter1Loop = (mvtDist <= mRadius/2.0f);
    while (mvtTotal < mvtDist)
    {
        Real mvtStep = mRadius/2.0;
        mvtTotal += mvtStep;
        if (mvtTotal > mvtDist)
            mvtStep -= (mvtTotal - mvtDist);
        mvt = mvtVector*mvtStep;
        mSceneNode->translate(mvt);
...
        if (breakAfter1Loop) break;
    }
#endif

// Code to manage ODE collisions with only 1 capsule
#if 0
    /// Max offset to step up
    Real mStepOffset;

void Avatar::createPhysics(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry)
{
...
    // Create the torso collision geometry
    mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius*0.5, mHeight - mRadius, world);
    Quaternion upQuat;
    upQuat.FromAngleAxis(Radian(-Math::HALF_PI), Vector3::UNIT_X);
    mCapsuleGeom->setOrientation(upQuat);
    mStepOffset = mRadius;

void Avatar::animate(Real timeSinceLastFrame)
{
...
    // Collide physics capsule with world
    if (mCapsuleGeom != 0)
    {
        Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize();
        Vector3 pos = mSceneNode->getPosition();
        mCapsuleGeom->setPosition(pos + Vector3(0, mHeight*0.5f, 0.0f));
        mCapsuleGeomContact = false;
        mCapsuleGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
        if (mCapsuleGeomContact)
        {
            static int sl = 0;
            sl++;
            if ((sl % 60) == 0)
            {
                StringUtil::StrStreamType log;
                log.precision(2);
                log.width(5);
                log << "Avatar::animate() mCapsuleGeomLastContact.(pos=("
                    << mCapsuleGeomLastContact.getPosition().x << ", " << mCapsuleGeomLastContact.getPosition().y << ", " << mCapsuleGeomLastContact.getPosition().z
                    << "), normal=("
                    << mCapsuleGeomLastContact.getNormal().x << ", " << mCapsuleGeomLastContact.getNormal().y << ", " << mCapsuleGeomLastContact.getNormal().z
                    << "), depth="
                    << mCapsuleGeomLastContact.getPenetrationDepth();
                OGRE_LOG(log.str());
            }
            if (mCapsuleGeomLastContact.getPosition().y - pos.y > mStepOffset)
            {
            Vector3 mvtCorrection;
/*            // step up ?
            if (mCapsuleGeomLastContact.getPosition().y - pos.y <= mStepOffset)
            {
                mvtCorrection = Vector3(0.0f, mStepOffset, 0.0f);
                StringUtil::StrStreamType log;
                log.precision(2);
                log.width(5);
                log << "Avatar::animate() mStepOffset=" << mCapsuleGeomLastContact.getPosition().y - pos.y;
                OGRE_LOG(log.str());
            }
            else*/
            {
                Plane contactPlane(mCapsuleGeomLastContact.getNormal(), mCapsuleGeomLastContact.getPosition());
                Vector3 mvtOnContactPlane;
#if (OGRE_VERSION < ((1 << 16) | (4 << 8) | 5))
                // 1 bug on projectVector() in Ogre < 1.4.5
                mvtOnContactPlane = -contactPlane.projectVector(mvt)*Vector3(1, 0, 1);
#else
                mvtOnContactPlane = contactPlane.projectVector(mvt)*Vector3(1, 0, 1);
#endif
                mvtCorrection = - mvt + mvtOnContactPlane;
            }
            mCapsuleGeom->setPosition(pos + Vector3(0.0f, mHeight*0.5f, 0.0f) + mvtCorrection);
            mCapsuleGeomContact = false;
            mCapsuleGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
            if (mCapsuleGeomContact)
            {
                if ((sl % 60) == 0)
                {
                    StringUtil::StrStreamType log;
                    log.precision(2);
                    log.width(5);
                    log << "Avatar::animate() 2nd contact mCapsuleGeomLastContact.(pos=("
                        << mCapsuleGeomLastContact.getPosition().x << ", " << mCapsuleGeomLastContact.getPosition().y << ", " << mCapsuleGeomLastContact.getPosition().z
                        << "), normal=("
                        << mCapsuleGeomLastContact.getNormal().x << ", " << mCapsuleGeomLastContact.getNormal().y << ", " << mCapsuleGeomLastContact.getNormal().z
                        << "), depth="
                        << mCapsuleGeomLastContact.getPenetrationDepth();
                    OGRE_LOG(log.str());
                }
                mSceneNode->translate(-mvt);
            }
            else
                mSceneNode->translate(mvtCorrection);
            }
        }
    }
    if (mGravity && (mRayGeom != 0))
    {
        // fire ray downward to collisionListener
        mRayGeom->setDefinition(mSceneNode->getPosition() + Vector3(0.0f, mHeight*0.5, 0.0f), Vector3::NEGATIVE_UNIT_Y);
        mRayGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
        Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize();
        Vector3 pos = mSceneNode->getPosition();
        mCapsuleGeom->setPosition(pos + Vector3(0.0f, mHeight*0.5f, 0.0f));
    }
#endif
