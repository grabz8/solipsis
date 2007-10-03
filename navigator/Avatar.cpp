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
//    lookAtTheGoodDirection();

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
    mPhysicsRay = 0;
    mCapsuleGeom = 0;
//    mContact = false;
#else
    mRaySceneQuery = mSceneNode->getCreator()->createRayQuery(Ray());
#endif
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mSceneNode == 0) return;
#ifdef PHYSICS
    delete mCapsuleGeom;
    delete mPhysicsRay;
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
    delete mCapsuleGeom;
    delete mPhysicsRay;
    if (world == 0) return;
    AxisAlignedBox aab = mEntity->getBoundingBox();
    Vector3 min = aab.getMinimum()*mSceneNode->getScale();
    Vector3 max = aab.getMaximum()*mSceneNode->getScale();
    Vector3 size(fabs(max.x - min.x), fabs(max.y - min.y),fabs(max.z - min.z));
    mRadius = (size.x > size.z) ? size.z/2 : size.x/2;
    mPhysicsRay = new OgreOde::RayGeometry(100, world);
///    OgreOde::Body* mCapsuleBody = new OgreOde::Body(world);
///    mSceneNode->attachObject(mCapsuleBody);
///    mCapsuleBody->setMass(OgreOde::BoxMass(1,Vector3(1, 1, 1)));
///    mCapsuleBody->setAffectedByGravity(false);
///    mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius, size.y, world, world->getDefaultSpace());
///    Quaternion upQuat;
///    upQuat.FromAngleAxis(Radian(-Ogre::Math::HALF_PI), Vector3::UNIT_X);
///    mCapsuleGeom->setOrientation(upQuat);
///    mCapsuleGeom->setBody(mCapsuleBody);
//    mCapsuleGeom = new OgreOde::CapsuleGeometry(mRadius, size.y, world);
//    Quaternion upQuat;
//    upQuat.FromAngleAxis(Radian(-Ogre::Math::HALF_PI), Vector3::UNIT_X);
//    mCapsuleGeom->setOrientation(upQuat);
    mWorldGeometry = worldGeometry;
}
#endif
//-------------------------------------------------------------------------------------
void Avatar::setGravity(bool enabled) {
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
void Avatar::update(Ogre::Real timeSinceLastFrame)
{
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
    State nextState = mState;
    Real animOffset = 0;

    mUpKeyMotion.update(timeSinceLastFrame);
    mDownKeyMotion.update(timeSinceLastFrame);
    frontBackMvt = mUpKeyMotion.getMotion() - mDownKeyMotion.getMotion();
    mSceneNode->translate(vpn*frontBackMvt*timeSinceLastFrame);
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
        mSceneNode->translate(-vri*leftRightMvt*timeSinceLastFrame);
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
    else if (mPgdownKeyMotion.isPressed() && !mGravity)
        setGravity(true);
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

#ifdef PHYSICS
    // Create world physics ray
    if (mGravity && (mPhysicsRay != 0)/* && (mCapsuleGeom != 0)*/)
    {
        // raise desired ray position a little above character's scenenode
        Vector3 pos = mSceneNode->getPosition();
        // may need to raise it higher for better accuracy
        pos.y += mRadius*2;

        // fire ray downward to collisionListener
        mPhysicsRay->setDefinition(pos, Vector3::NEGATIVE_UNIT_Y);
        mPhysicsRay->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
        // test capsule to collisionListener
//        mCapsuleGeom->setPosition(pos + Vector3(0, 47, 16));
///        mCapsuleGeom->collide(mWorldGeometry, (OgreOde::CollisionListener*)this);
    }
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
        for(;itray!=result.end();++itray)
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

#ifdef PHYSICS
//-------------------------------------------------------------------------------------
bool Avatar::collision(OgreOde::Contact* contact)
{
    if (mPhysicsRay == 0) return true;
    if (contact->getFirstGeometry()->getID() == mPhysicsRay->getID() ||
	    contact->getSecondGeometry()->getID() == mPhysicsRay->getID())
    {
#ifdef LEXI
        mSceneNode->setPosition(contact->getPosition() + Vector3(0, 47, 0));
#else
        mSceneNode->setPosition(contact->getPosition());
#endif
    }
    else if (contact->getFirstGeometry()->getID() == mCapsuleGeom->getID() ||
        contact->getSecondGeometry()->getID() == mCapsuleGeom->getID())
    {
///        mSceneNode->translate(-contact->getNormal()*contact->getPenetrationDepth());
//        mCapsuleLastContact = *contact;
//        mContact = true;
    }

    return true;
}
#endif

/*
//-------------------------------------------------------------------------------------
void Avatar::lookAtTheGoodDirection()
{
    Vector3 src = mSceneNode->getOrientation()*Vector3::UNIT_X;
    Vector3 direction = mDirection;
    direction.y = 0; //Facing horizon !
    if ((1.0f + src.dotProduct(direction)) < 0.0001f) {
        mSceneNode->yaw(Degree(180));
    }
    else
    {
        Ogre::Quaternion quat = src.getRotationTo(direction);
        mSceneNode->rotate(quat);
    } 
}
*/
