#include "Avatar.h"
#include "OgreHelpers.h"

using namespace Solipsis;

String Avatar::mDefaultStateAnimName[SCount] = {
    "",
    "Idle",
    "Walk",
    "Run",
    "Fly",
    "Swim"
};

#define EPSILON_SPEED 0.1f
#define MAX_SPEED 1.0f
#define TRANSLATION_SPEED_MPS 6.0f
#define ROTATION_SPEED_RPS Radian(Math::HALF_PI)

//-------------------------------------------------------------------------------------
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
    mNameLabel->setScale(0.1f);
    mNameLabel->setCharacterHeight(1);
    mNameLabel->setColor(ColourValue::White);
    mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    Real aabbHeight = entity->getBoundingBox().getSize().y;
    mNameLabel->setAdditionalHeight(aabbHeight);
    mSceneNode->attachObject(mNameLabel);

    mGravity = false;
#ifdef PHYSICS
    mWorld = 0;
    mWorldGeometry = 0;
    mRayGeom = 0;
    mMaxUpdateTimeStep = -1.0;
    mCapsuleGeom = 0;
    mCapsuleGeomContact = false;
    mCapsuleBodyTrans = 0;
    mCapsuleBodyGeom = 0;
    mCapsuleBody = 0;
#elif PHYSX
    mPhysicsScene = 0;
    mControllerManager = 0;
    mCapsuleController = 0;
/*    mSceneQuery = 0;*/
#elif PHYSICSPLUGINS
    mPhysicsScene = 0;
    mPhysicsCharacter = 0;
#else
    mRaySceneQuery = mSceneNode->getCreator()->createRayQuery(Ray());
#endif
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mSceneNode == 0) return;

#ifdef PHYSICS
    destroyPhysics();
#elif PHYSX
    destroyPhysics();
#elif PHYSICSPLUGINS
    destroyPhysics();
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
//    OGRE_LOG("Avatar::setState()" + StringConverter::toString((int)state));
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
void Avatar::createPhysics(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry)
{
    destroyPhysics();

    // world ?
    if (world == 0) return;
    mWorld = world;
    if (worldGeometry == 0) return;
    mWorldGeometry = worldGeometry;

    // Compute radius and height of character
    Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize()*mSceneNode->getScale();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    // Create the ray for feet
    mRayGeom = new OgreOde::RayGeometry(mHeight, mWorld);

    // Create the torso collision geometry
    mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius*0.5, mHeight*0.25, mWorld);
    Quaternion upQuat;
    upQuat.FromAngleAxis(Radian(-Math::HALF_PI), Vector3::UNIT_X);
    mCapsuleGeom->setOrientation(upQuat);

    mCapsuleBody = new OgreOde::Body(mWorld, mSceneNode->getName() + "Bod");
    mCapsuleBody->setMass(OgreOde::CapsuleMass(1, mRadius*0.5, Vector3::UNIT_Y, mHeight*0.5));
    mCapsuleBody->setAffectedByGravity(false);
    mCapsuleBody->setAutoSleep(false);
    mCapsuleBody->setUserData(2);
    mCapsuleBodyTrans = new OgreOde::TransformGeometry(mWorld, mWorld->getDefaultSpace());
    mCapsuleBodyGeom = new OgreOde::CapsuleGeometry(mRadius*0.5, mHeight*0.5, mWorld);
    mCapsuleBodyGeom->setPosition(Vector3(0, aabbHalfSize.y, 0));
    mCapsuleBodyGeom->setOrientation(Quaternion(Degree(90), Vector3::UNIT_X));
    mCapsuleBodyTrans->setBody(mCapsuleBody);
    mCapsuleBodyTrans->setEncapsulatedGeometry(mCapsuleBodyGeom);
    mCapsuleBody->setPosition(mSceneNode->getPosition());
}

//-------------------------------------------------------------------------------------
void Avatar::destroyPhysics()
{
    delete mCapsuleBodyGeom;
    delete mCapsuleBodyTrans;
    delete mCapsuleBody;
    delete mCapsuleGeom;
    delete mRayGeom;

    mWorld = 0;
}

//-------------------------------------------------------------------------------------
void Avatar::setMaxUpdateTimeStep(Real maxUpdateTimeStep) {
    mMaxUpdateTimeStep = maxUpdateTimeStep;
}

//-------------------------------------------------------------------------------------
Real Avatar::getMaxUpdateTimeStep() {
    return mMaxUpdateTimeStep;
}
#elif PHYSX
//-------------------------------------------------------------------------------------
void Avatar::createPhysics(NxScene* physicsScene, ::ControllerManager* controllerManager)
{
    destroyPhysics();

    // scene and controller manager ?
    if ((physicsScene == 0) || (controllerManager == 0)) return;
    mPhysicsScene = physicsScene;
    mControllerManager = controllerManager;

    // Compute radius and height of character
    Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize()*mSceneNode->getScale();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    NxCapsuleControllerDesc capsuleControllerDesc;
    capsuleControllerDesc.interactionFlag = NXIF_INTERACTION_INCLUDE;
    Vector3 pos = mSceneNode->getPosition();
    capsuleControllerDesc.position.set(pos.x, pos.y + (mHeight - mRadius), pos.z);
    capsuleControllerDesc.radius = mRadius*0.5f;
    capsuleControllerDesc.height = mHeight - mRadius;
    capsuleControllerDesc.upDirection = NX_Y;
    capsuleControllerDesc.slopeLimit = cosf(NxMath::degToRad(45.0f));
    capsuleControllerDesc.skinWidth = 0.1f;
    capsuleControllerDesc.stepOffset = mRadius;
    capsuleControllerDesc.callback = (NxUserControllerHitReport*)this;
    mCapsuleController = (NxCapsuleController*)mControllerManager->createController(physicsScene, capsuleControllerDesc);
/*
    NxSceneQueryDesc sceneQueryDesc;
    sceneQueryDesc.executeMode = NX_SQE_SYNCHRONOUS;
    sceneQueryDesc.report = (NxSceneQueryReport*)this;
    mSceneQuery = mPhysicsScene->createSceneQuery(sceneQueryDesc);*/
}

//-------------------------------------------------------------------------------------
void Avatar::destroyPhysics()
{
/*    if (mSceneQuery != 0)
        mPhysicsScene->releaseSceneQuery(*mSceneQuery);*/
    if (mCapsuleController != 0)
        mControllerManager->releaseController(*mCapsuleController);

    mPhysicsScene = 0;
}
#elif PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
void Avatar::createPhysics(IPhysicsScene* physicsScene)
{
    destroyPhysics();

    // scene ?
    if (physicsScene == 0)
        return;
    mPhysicsScene = physicsScene;

    // Compute radius and height of character
    Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize()*mSceneNode->getScale();
    mRadius = std::min(aabbHalfSize.x, aabbHalfSize.z);
    mHeight = aabbHalfSize.y*2;

    IPhysicsCharacter::Desc characterDesc;
    characterDesc.position = mSceneNode->getPosition();
    characterDesc.radius = mRadius;
    characterDesc.height = mHeight;
    characterDesc.stepOffset = mRadius;
    mPhysicsCharacter = mPhysicsScene->createCharacter();
    mPhysicsCharacter->create(mSceneNode, characterDesc);
}

//-------------------------------------------------------------------------------------
void Avatar::destroyPhysics()
{
    if (mPhysicsCharacter != 0)
        mPhysicsScene->destroyCharacter(mPhysicsCharacter);

    mPhysicsCharacter = 0;
    mPhysicsScene = 0;
}
#endif

//-------------------------------------------------------------------------------------
void Avatar::setGravity(bool enabled) {
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
bool Avatar::isGravityEnabled() {
    return mGravity;
}

//-------------------------------------------------------------------------------------
void Avatar::update(Real timeSinceLastFrame)
{
#ifdef PHYSICS
    if (mMaxUpdateTimeStep > 0)
    {
        Real totalTime = 0.0;
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
void Avatar::animate(Real timeSinceLastFrame)
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
    mvt += vpn*frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
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
        mvt += -vri*leftRightMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
        if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
            nextState = SWalk;
    }
    else
    {
        // Third person rotation
        mSceneNode->yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
        if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
            nextState = SWalk;
    }

    if (mPgupKeyMotion.isPressed() && mGravity)
        setGravity(false);
#ifdef PHYSICS
#elif PHYSX
#elif PHYSICSPLUGINS
#else
    if (mPgdownKeyMotion.isPressed() && !mGravity)
        setGravity(true);
#endif
    mPgupKeyMotion.update(timeSinceLastFrame);
    mPgdownKeyMotion.update(timeSinceLastFrame);
    upDownMvt = mPgupKeyMotion.getMotion() - mPgdownKeyMotion.getMotion();
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

#ifdef PHYSICS
    mSceneNode->translate(vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
    // Move it !
    mSceneNode->translate(mvt);
    Vector3 aabbHalfSize = mEntity->getBoundingBox().getHalfSize();
    // Collide physics ray with world
    if ((mRayGeom != 0) && (mWorldGeometry != 0))
    {
        Vector3 pos = mSceneNode->getPosition();
        Real dispGravity = 0.0f;
        if (mGravity)
            dispGravity = -9.80665f*timeSinceLastFrame;
        // fire ray downward to collisionListener
        mRayGeom->setDefinition(pos + Vector3(0, mHeight*0.5, 0), Vector3::NEGATIVE_UNIT_Y);
        mRayGeomContact = false;
        mRayGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
        if (mGravity)
        {
            if (mRayGeomContact)
            {
                if (mRayGeomLastContact.getPosition().y < pos.y - mHeight*0.5 - dispGravity)
                    mSceneNode->setPosition(pos - Vector3(0, mHeight*0.5 + dispGravity, 0));
                else
                    mSceneNode->setPosition(mRayGeomLastContact.getPosition());
            }
            else
                mSceneNode->translate(Vector3(0, dispGravity, 0));
        }
        else
        {
            if (mRayGeomContact && (mRayGeomLastContact.getPosition().y >= pos.y))
                    mSceneNode->setPosition(mRayGeomLastContact.getPosition());
        }
    }
    // Collide physics capsule with world
    if ((mCapsuleGeom != 0) && (mWorldGeometry != 0))
    {
        // test capsule to collisionListener
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
                OGRE_LOG(log.str());
            }
            Plane contactPlane(mCapsuleGeomLastContact.getNormal(), mCapsuleGeomLastContact.getPosition());
            Vector3 mvtOnContactPlane;
#if (OGRE_VERSION < ((1 << 16) | (4 << 8) | 5))
            // 1 bug on projectVector() in Ogre < 1.4.5
            mvtOnContactPlane = -contactPlane.projectVector(mvt)*Vector3(1, 0, 1);
#else
            mvtOnContactPlane = contactPlane.projectVector(mvt)*Vector3(1, 0, 1);
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
            mCapsuleGeom->setPosition(pos - mvt + mvtOnContactPlane + Vector3(0, aabbHalfSize.y*1.5, 0));
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
                mSceneNode->translate(-mvt + mvtOnContactPlane);
        }
    }
    if (mCapsuleBody != 0) {
        mCapsuleBody->setPosition(mSceneNode->getPosition());
        mCapsuleBody->setOrientation(Quaternion::IDENTITY);
        mCapsuleBody->setLinearVelocity(mvt);
        mCapsuleBody->setAngularVelocity(Vector3::ZERO);
    }
#elif PHYSX
    mSceneNode->translate(vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
    // Move it !
    mSceneNode->translate(mvt);
    // Collide physics capsule with world
    if (mCapsuleController != 0)
    {
        NxExtendedVec3 newPos = mCapsuleController->getFilteredPosition();
        mSceneNode->setPosition(newPos.x, newPos.y - (mHeight - mRadius), newPos.z);
        Vector3 mvtTotal = mvt + (vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
        NxVec3 displacement(mvtTotal.x, mvtTotal.y, mvtTotal.z);
        if (mGravity)
            displacement.y += -9.80665f*timeSinceLastFrame;
        NxU32 collisionFlags;
        mCapsuleController->move(displacement, PhysXHelpers::CG_COLLIDABLE_MASK, 0.001f, collisionFlags, 1.0f);
    }
#elif PHYSICSPLUGINS
    // Move physics character
    if (mPhysicsCharacter != 0)
    {
        Vector3 displacement = mvt + (vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
        if (mGravity)
            displacement.y += -9.80665f*timeSinceLastFrame;
        mPhysicsCharacter->move(displacement);
        Vector3 newPosition;
        mPhysicsCharacter->getPosition(newPosition);
        mSceneNode->setPosition(newPosition);
    }
#else
    mSceneNode->translate(vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
    // Move it !
    mSceneNode->translate(mvt);
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

//-------------------------------------------------------------------------------------
void Avatar::movementKeyPressed(Solipsis::KeyCode code)
{
    using namespace Solipsis;
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
#ifdef PHYSICS
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
#elif PHYSX
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
#elif PHYSICSPLUGINS
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
#endif
    }
}

//-------------------------------------------------------------------------------------
void Avatar::movementKeyReleased(Solipsis::KeyCode code)
{
    using namespace Solipsis;
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
#endif

//-------------------------------------------------------------------------------------
