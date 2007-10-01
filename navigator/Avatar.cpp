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

Avatar::Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity, RaySceneQuery* raySceneQuery) :
    OgrePeer(peer),
    mState(SNone),
    mMvtType(MT3rdPerson),
    mGravity(false),
    mSceneNode(sceneNode),
    mEntity(entity),
    mRaySceneQuery(raySceneQuery),
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
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mSceneNode == 0) return;
    if (mRaySceneQuery != 0)
        mSceneNode->getCreator()->destroyQuery(mRaySceneQuery);
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
void Avatar::setGravity(bool enabled) {
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
void Avatar::setState(State state)
{
    LogManager::getSingletonPtr()->logMessage("Avatar::setState()" + Ogre::StringConverter::toString((int)state));
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

//-------------------------------------------------------------------------------------
void Avatar::update(Ogre::Real timeSinceLastFrame)
{
    animate(timeSinceLastFrame);
}

//-------------------------------------------------------------------------------------
void Avatar::startAnimation(const String &name, bool loop)
{
    mAnimationState = mEntity->getAnimationState(name);
    mAnimationState->setLoop(loop);
    mAnimationState->setEnabled(true);
}

//-------------------------------------------------------------------------------------
void Avatar::stopAnimation()
{
    mAnimationState = mEntity->getAnimationState(mStateAnimName[mState]);
    mAnimationState->setLoop(false);
    mAnimationState->setEnabled(false);
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
    mAnimationState->addTime(animOffset);

    if (mState != nextState)
        setState(nextState);

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
    /*    Vector3 hit;
        if (RaycastFromPoint(ray, result, String("station"), hit))
        mSceneNode->setPosition(pos.x,hit.y,pos.z);*/
    }
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
