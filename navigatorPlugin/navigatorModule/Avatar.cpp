#include "Avatar.h"

String Avatar::mDefaultStateAnimName[SCount] = {
    "",
    "Idle",
    "Walk",
    "Run",
    "Fly",
    "Swim"
};

#define EPSILON_SPEED 0.1
#define MAX_SPEED 400

Avatar::Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity) :
    OgrePeer(peer, "avatar"),
    mState(SNone),
    mMvtType(MT3rdPerson),
    mSceneNode(sceneNode),
    mEntity(entity),
    mAnimationState(0),
    mUpKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mDownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mLeftKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mRightKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mPgupKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mPgdownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
{
    for (int a = 0;a < SCount; ++a)
        mStateAnimName[a] = mDefaultStateAnimName[a];

    mSceneNode->attachObject(entity);

    // Set Name Label
    mNameLabel = new MovableText("Label" + peer->getLogin(), peer->getLogin(), false);
    mNameLabel->setCharacterHeight(6);
    mNameLabel->setColor(Ogre::ColourValue::White);
    mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    Real aabbHeightDiv2 = entity->getBoundingBox().getHalfSize().y;
    Real scale = mSceneNode->getScale().y;
    mNameLabel->setAdditionalHeight(aabbHeightDiv2*(1 + scale));
    mSceneNode->attachObject(mNameLabel);

    mGravity = false;
#ifdef PHYSICS
    mRayGeom = 0;
#ifdef CAPSULEGEOM
    mMaxUpdateTimeStep = -1.0;
    mCapsuleGeom = 0;
    mCapsuleGeomContact = false;
#endif
#ifdef FEET
    mFeetBody = 0;
    mRayGeomContact = false;
    mFeetGeom = 0;
    mFeetGeomContact = false;
#endif
#else
    mRaySceneQuery = mSceneNode->getCreator()->createRayQuery(Ray());
#endif
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mSceneNode == 0) return;
#ifdef PHYSICS
#ifdef FEET
    delete mFeetGeom;
    delete mFeetBody;
#endif
#ifdef CAPSULEGEOM
    delete mCapsuleGeom;
#endif
    delete mRayGeom;
#else
    if (mRaySceneQuery != 0)
        mSceneNode->getCreator()->destroyQuery(mRaySceneQuery);
#endif
    if (mEntity != 0) {
        mSceneNode->detachObject(mEntity);
        mSceneNode->getCreator()->destroyEntity(mEntity);
    }
    mSceneNode->getCreator()->destroySceneNode(mSceneNode->getName());
}

//-------------------------------------------------------------------------------------
SceneNode* Avatar::getSceneNode()
{
    return mSceneNode;
}

//-------------------------------------------------------------------------------------
Entity* Avatar::getEntity()
{
    return mEntity;
}

//-------------------------------------------------------------------------------------
void Avatar::setName(const String& name)
{
    mNameLabel->setCaption(name);
}

//-------------------------------------------------------------------------------------
void Avatar::setNameVisibility(bool visible)
{
    mNameLabel->setVisible(visible);
}

//-------------------------------------------------------------------------------------
void Avatar::setState(State state)
{
//    LogManager::getSingletonPtr()->logMessage("Avatar::setState()" + Ogre::StringConverter::toString((int)state));
    if (mStateAnimName[mState].length() > 0)
        stopAnimation();
    if (mStateAnimName[state].length() > 0)
        startAnimation(mStateAnimName[state]);
    mState = state;
}

//-------------------------------------------------------------------------------------
Avatar::State Avatar::getState()
{
    return mState;
}

//-------------------------------------------------------------------------------------
void Avatar::setStateAnimName(State state, const String& name)
{
    mStateAnimName[state] = name;
}

//-------------------------------------------------------------------------------------
void Avatar::setMvtType(MvtType mvtType)
{
    mMvtType = mvtType;
}

//-------------------------------------------------------------------------------------
Avatar::MvtType Avatar::getMvtType()
{
    return mMvtType;
}

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
void Avatar::createPhysicsRayGeometry(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry)
{
#ifdef FEET
    delete mFeetGeom;
    delete mFeetBody;
#endif
#ifdef CAPSULEGEOM
    delete mCapsuleGeom;
#endif
    delete mRayGeom;

    // world ?
    if (world == 0) return;

    // Compute radius and height of character
    Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize()*mSceneNode->getScale();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    // Create the ray for feet
    mRayGeom = new OgreOde::RayGeometry(mHeight, world);

#ifdef CAPSULEGEOM
    // Create the torso collision geometry
    mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius*0.5, mHeight*0.25, world);
    Quaternion upQuat;
    upQuat.FromAngleAxis(Radian(-Ogre::Math::HALF_PI), Vector3::UNIT_X);
    mCapsuleGeom->setOrientation(upQuat);
#ifdef FEET
    // Create the feet body
    mFeetBody = new OgreOde::Body(world);
    mFeetBody->setMass(OgreOde::SphereMass(1, mRadius));
    mFeetBody->sleep();
    mFeetBody->setAffectedByGravity(false);
    mFeetBody->setPosition(mSceneNode->getPosition());
    mFeetGeom = new OgreOde::SphereGeometry(mRadius*0.5, world);
#endif
/*  // Space
  OgreOde::SimpleSpace* avatarSpace = new OgreOde::SimpleSpace(world, world->getDefaultSpace());
  avatarSpace->setInternalCollisions(false);
  // Body
  OgreOde::Body* mCapsuleBody = new OgreOde::Body(world);
  mCapsuleBody->setMass(OgreOde::CapsuleMass(1, mRadius*0.7, Vector3::UNIT_Y, mHeight*0.25));
  mCapsuleBody->setAffectedByGravity(false);
  // Geometry and its transform geometry
  mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius*0.7, mHeight*0.25, world);
  OgreOde::TransformGeometry* mCapsuleTrans = new OgreOde::TransformGeometry(world, avatarSpace);
  mCapsuleGeom->setPosition(Vector3(0, aabbHalfSize.y*1.5, 0));
  Quaternion upQuat;
  upQuat.FromAngleAxis(Radian(-Ogre::Math::HALF_PI), Vector3::UNIT_X);
  mCapsuleGeom->setOrientation(upQuat);
  // Bind body and geometry to transform geometry
  mCapsuleTrans->setBody(mCapsuleBody); 
  mCapsuleTrans->setEncapsulatedGeometry(mCapsuleGeom);
  // Attach body to scene node
  mSceneNode->attachObject(mCapsuleBody);*/
#endif
    mWorldGeometry = worldGeometry;
}
#endif

#ifdef CAPSULEGEOM
//-------------------------------------------------------------------------------------
void Avatar::setMaxUpdateTimeStep(Ogre::Real maxUpdateTimeStep) {
    mMaxUpdateTimeStep = maxUpdateTimeStep;
}

//-------------------------------------------------------------------------------------
Ogre::Real Avatar::getMaxUpdateTimeStep() {
    return mMaxUpdateTimeStep;
}
#endif

//-------------------------------------------------------------------------------------
void Avatar::setGravity(bool enabled) {
    mGravity = enabled;
#ifdef FEET
    if (!enabled)
        if ((mFeetBody != 0) && mFeetBody->isAttached())
        {
            mFeetBody->sleep();
            mFeetBody->setAffectedByGravity(false);
            mSceneNode->detachObject(mFeetBody);
            mRayGeom->setLength(mHeight);
        }
#endif
}

//-------------------------------------------------------------------------------------
bool Avatar::isGravityEnabled() {
    return mGravity;
}

//-------------------------------------------------------------------------------------
void Avatar::update(Ogre::Real timeSinceLastFrame)
{
#ifdef CAPSULEGEOM
    if (mMaxUpdateTimeStep > 0)
    {
        Ogre::Real totalTime = 0.0;
        for (;totalTime < timeSinceLastFrame - mMaxUpdateTimeStep; totalTime += mMaxUpdateTimeStep)
            animate(mMaxUpdateTimeStep);
        // last step
        timeSinceLastFrame -= totalTime;
    }
#endif
    animate(timeSinceLastFrame);
}

//-------------------------------------------------------------------------------------
void Avatar::startAnimation(const String &name, bool loop)
{
    if (name.length() == 0) return;
    mAnimationState = mEntity->getAnimationState(name);
    mAnimationState->setLoop(loop);
    mAnimationState->setEnabled(true);
}

//-------------------------------------------------------------------------------------
void Avatar::stopAnimation()
{
    if (mStateAnimName[mState].length() == 0) return;
    AnimationState* animationStateToStop = mEntity->getAnimationState(mStateAnimName[mState]);
    animationStateToStop->setLoop(false);
    animationStateToStop->setEnabled(false);
}

//-------------------------------------------------------------------------------------
void Avatar::animate(Ogre::Real timeSinceLastFrame)
{
    Vector3 vpn = mSceneNode->getOrientation()*Vector3::UNIT_X;
    Vector3 vup = mSceneNode->getOrientation()*Vector3::UNIT_Y;
    Vector3 vri = mSceneNode->getOrientation()*Vector3::UNIT_Z;
    Real frontBackMvt;
    Real leftRightMvt;
    Real upDownMvt;
    Vector3 mvt = Vector3(0, 0, 0);
    State nextState = mState;
    Real animOffset = 0;

    mUpKeyMotion.update(timeSinceLastFrame);
    mDownKeyMotion.update(timeSinceLastFrame);
    frontBackMvt = mUpKeyMotion.getMotion() - mDownKeyMotion.getMotion();
    mvt += vpn*frontBackMvt*timeSinceLastFrame;
    if ((Math::Abs(frontBackMvt) > EPSILON_SPEED) && (Math::Abs(frontBackMvt) < MAX_SPEED*0.9) && (mState != SWalk))
        nextState = SWalk;
    if ((Math::Abs(frontBackMvt) > MAX_SPEED*0.9) && (mState != SRun))
        nextState = SRun;

    mLeftKeyMotion.update(timeSinceLastFrame);
    mRightKeyMotion.update(timeSinceLastFrame);
    leftRightMvt = mLeftKeyMotion.getMotion() - mRightKeyMotion.getMotion();
    if (mMvtType == MT1stPerson)
    {
        // First person straff
        mvt += -vri*leftRightMvt*timeSinceLastFrame;
        if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
            nextState = SWalk;
    }
    else
    {
        // Third person rotation
        mSceneNode->yaw(Radian(Math::PI/1000)*leftRightMvt*timeSinceLastFrame);
        if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
            nextState = SWalk;
    }

    if (mPgupKeyMotion.isPressed() && mGravity)
        setGravity(false);
#ifdef FEET
#else
    if (mPgdownKeyMotion.isPressed() && !mGravity)
        setGravity(true);
#endif
    mPgupKeyMotion.update(timeSinceLastFrame);
    mPgdownKeyMotion.update(timeSinceLastFrame);
    upDownMvt = mPgupKeyMotion.getMotion() - mPgdownKeyMotion.getMotion();
    mSceneNode->translate(vup*upDownMvt*timeSinceLastFrame);
//    if ((Math::Abs(upDownMvt) > MAX_SPEED*0.9) && (mState != SFly))
//        nextState = SFly;

    if ((mState == SWalk) || (mState == SRun))
        if (Math::Abs(frontBackMvt) > EPSILON_SPEED)
            animOffset = (frontBackMvt/(MAX_SPEED/5))*timeSinceLastFrame;
        else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)
            animOffset = (leftRightMvt/(MAX_SPEED))*timeSinceLastFrame;
        else
            nextState = SIdle;
    else
        animOffset = timeSinceLastFrame;
    if (mAnimationState != 0)
        mAnimationState->addTime(animOffset);

    if (mState != nextState)
        setState(nextState);

    // Move it !
    mSceneNode->translate(mvt);

#ifdef PHYSICS
#ifdef FEET
    if (mFeetGeom != 0)
        mFeetGeom->setPosition(mSceneNode->getPosition());
#endif
    // Collide physics ray with world
    if (mGravity && (mRayGeom != 0))
    {
#ifdef FEET
        mRayGeomContact = false;
#endif
        // fire ray downward to collisionListener
        mRayGeom->setDefinition(mSceneNode->getPosition() + Vector3(0, mHeight*0.5, 0), Vector3::NEGATIVE_UNIT_Y);
        mRayGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
#ifdef FEET
        if (!mRayGeomContact && (mFeetBody != 0))
        {
            mFeetGeomContact = false;
            mFeetGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
            if (!mFeetGeomContact)
            {
            if (!mFeetBody->isAttached())
            {
                    mSceneNode->attachObject(mFeetBody);
                    mFeetBody->setAffectedByGravity(true);
                    mFeetBody->setLinearVelocity(Vector3::ZERO);
                    mFeetBody->setAngularVelocity(Vector3::ZERO);
                    mFeetBody->wake();
            }
            mFeetBody->setPosition(mSceneNode->getPosition());
            mRayGeom->setLength(mHeight/2);
            }
        }
#endif
    }
#ifdef CAPSULEGEOM
    // Collide physics capsule with world 
    if (mCapsuleGeom != 0)
    {
        // test capsule to collisionListener
        Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize();
        Vector3 pos = mSceneNode->getPosition();
        mCapsuleGeom->setPosition(pos + Vector3(0, aabbHalfSize.y*1.5, 0));
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
                LogManager::getSingletonPtr()->logMessage(log.str());
            }
            Ogre::Plane contactPlane(mCapsuleGeomLastContact.getNormal(), mCapsuleGeomLastContact.getPosition());
            Vector3 mvtOnContactPlane;
            mvtOnContactPlane = contactPlane.projectVector(mvt)*Vector3(1, 0, 1);
            if ((sl % 60) == 0)
            {
                StringUtil::StrStreamType log;
                log.precision(2);
                log.width(5);
                log << "Avatar::animate() mvtOnContactPlane=("
                    << mvtOnContactPlane.x << ", " << mvtOnContactPlane.y << ", " << mvtOnContactPlane.z
                    << ")";
                LogManager::getSingletonPtr()->logMessage(log.str());
            }
            mCapsuleGeom->setPosition(pos - mvt - mvtOnContactPlane + Vector3(0, aabbHalfSize.y*1.5, 0));
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
                    LogManager::getSingletonPtr()->logMessage(log.str());
                }
                mSceneNode->translate(-mvt);
            }
            else
                mSceneNode->translate(-mvt - mvtOnContactPlane);
        }
    }
#endif
#else
    if (mGravity && (mRaySceneQuery != 0))
    {
        //Here is a fake gravity, follow ground if any
        Vector3 pos = mSceneNode->getPosition();
        Vector3 avatarSize = mEntity->getBoundingBox().getSize();
        Ray ray(pos + avatarSize/2, Vector3::NEGATIVE_UNIT_Y); //Ray from the middle of avatar, direction:floor
        mRaySceneQuery->setRay(ray);
        mRaySceneQuery->setSortByDistance(true);
        RaySceneQueryResult &result = mRaySceneQuery->execute();
        RaySceneQueryResult::iterator itray = result.begin();
        for (; itray!=result.end(); ++itray)
        {
            if (itray->worldFragment)
            {
                Real height = itray->worldFragment->singleIntersection.y;
                mSceneNode->setPosition(pos.x,height+10.0f,pos.z);
                break;
            }
        }
    }
#endif
}

#ifndef NAVIGATORMODULE_SELF_BUILD // to replace by 1 interface classes
//-------------------------------------------------------------------------------------
void Avatar::movementKeyPressed(OIS::KeyCode code)
{
    using namespace OIS;
    switch (code) {
       case KC_UP:
           mUpKeyMotion.setState(true);
       break;
       case KC_DOWN:
           mDownKeyMotion.setState(true);
       break;
       case KC_LEFT:
           mLeftKeyMotion.setState(true);
       break;
       case KC_RIGHT:
           mRightKeyMotion.setState(true);
       break;
       case KC_PGUP:
           mPgupKeyMotion.setState(true);
       break;
       case KC_PGDOWN:
           mPgdownKeyMotion.setState(true);
       break;
#ifdef FEET
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
#endif
    }
}

//-------------------------------------------------------------------------------------
void Avatar::movementKeyReleased(OIS::KeyCode code)
{
    using namespace OIS;
    switch (code) {
       case KC_UP:
           mUpKeyMotion.setState(false);
       break;
       case KC_DOWN:
           mDownKeyMotion.setState(false);
       break;
       case KC_LEFT:
           mLeftKeyMotion.setState(false);
       break;
       case KC_RIGHT:
           mRightKeyMotion.setState(false);
       break;
       case KC_PGUP:
           mPgupKeyMotion.setState(false);
       break;
       case KC_PGDOWN:
           mPgdownKeyMotion.setState(false);
       break;
    }
}
#endif
//-------------------------------------------------------------------------------------
void Avatar::movementKeyPressed(NavigatorModule::KeyCode code)
{
    using namespace NavigatorModule;
    switch (code) {
       case KC_UP:
           mUpKeyMotion.setState(true);
       break;
       case KC_DOWN:
           mDownKeyMotion.setState(true);
       break;
       case KC_LEFT:
           mLeftKeyMotion.setState(true);
       break;
       case KC_RIGHT:
           mRightKeyMotion.setState(true);
       break;
       case KC_PGUP:
           mPgupKeyMotion.setState(true);
       break;
       case KC_PGDOWN:
           mPgdownKeyMotion.setState(true);
       break;
#ifdef FEET
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
#endif
    }
}

//-------------------------------------------------------------------------------------
void Avatar::movementKeyReleased(NavigatorModule::KeyCode code)
{
    using namespace NavigatorModule;
    switch (code) {
       case KC_UP:
           mUpKeyMotion.setState(false);
       break;
       case KC_DOWN:
           mDownKeyMotion.setState(false);
       break;
       case KC_LEFT:
           mLeftKeyMotion.setState(false);
       break;
       case KC_RIGHT:
           mRightKeyMotion.setState(false);
       break;
       case KC_PGUP:
           mPgupKeyMotion.setState(false);
       break;
       case KC_PGDOWN:
           mPgdownKeyMotion.setState(false);
       break;
    }
}

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
bool Avatar::collision(OgreOde::Contact* contact)
{
    if (mRayGeom == 0) return true;
    if (contact->getFirstGeometry()->getID() == mRayGeom->getID() ||
	    contact->getSecondGeometry()->getID() == mRayGeom->getID())
    {
#ifdef LEXI
        mSceneNode->setPosition(contact->getPosition() + Vector3(0, 47, 0));
#else
        mSceneNode->setPosition(contact->getPosition());
#endif
#ifdef FEET
        if ((mFeetBody != 0) && mFeetBody->isAttached())
        {
            mFeetBody->sleep();
            mFeetBody->setAffectedByGravity(false);
            mSceneNode->detachObject(mFeetBody);
            mRayGeom->setLength(mHeight);
        }
        mRayGeomContact = true;
#endif
    }
#ifdef FEET
    else if (contact->getFirstGeometry()->getID() == mFeetGeom->getID() ||
        contact->getSecondGeometry()->getID() == mFeetGeom->getID())
    {
        mFeetGeomLastContact = *contact;
        mFeetGeomContact = true;
    }
#endif
#ifdef CAPSULEGEOM
    else if (contact->getFirstGeometry()->getID() == mCapsuleGeom->getID() ||
        contact->getSecondGeometry()->getID() == mCapsuleGeom->getID())
    {
        mCapsuleGeomLastContact = *contact;
        mCapsuleGeomContact = true;
    }
#endif

    return true;
}
#endif





// Just 1 some source code to sample movement instead of frameTime into update() and animate()
// => seems good but no feet responses ??!??!!? so still some stuff ...
#if 0
/*
    if (mMaxUpdateTimeStep > 0)
    {
        Ogre::Real totalTime = 0.0;
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
    bool breakAfter1Loop = (mvtDist <= mRadius/2.0);
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