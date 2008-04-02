#include "Avatar.h"
#include "OgreHelpers.h"
#include "Navigator.h"

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
#define TRANSLATION_ANIM_LOOP 4
#define ROTATION_ANIM_LOOP Radian(Math::HALF_PI)
#define SMOOTH_FACTOR 10.0f
#define XMLUPDATE_DISPLACEMENT_THRESHOLD 0.001f
#define XMLUPDATE_ROTATION_THRESHOLD Radian(Math::PI*0.001f)

//-------------------------------------------------------------------------------------
#ifdef POOL
Avatar::Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, SceneNode* sceneNode, Entity* entity) :
#else
Avatar::Avatar(XmlEntity* xmlEntity, bool isLocal, SceneNode* sceneNode, Entity* entity) :
#endif
    OgrePeer(xmlEntity, isLocal),
#ifdef POOL
    mUpdatedXmlEntity((XmlEntity*)0),
#endif
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
#ifdef POOL
    if (isLocal)
    {
        mUpdatedXmlEntity.allocate();
        mUpdatedXmlEntity->setDefinedAttributes(XmlEntity::DANone);
        mUpdatedXmlEntity->setUid(mXmlEntity->getUid());
    }
#else
    mUpdatedXmlEntity = 0;
    if (isLocal)
        mUpdatedXmlEntity = new XmlEntity(mXmlEntity->getUid());
#endif

    for (int a = 0;a < SCount; ++a)
        mStateAnimName[a] = mDefaultStateAnimName[a];

    mSceneNode->attachObject(entity);

    String uidString = StringConverter::toString(xmlEntity->getUid());

    // Set Name Label
    mNameLabel = new MovableText(uidString + "Label", mXmlEntity->getName().substr(0, 16), false);
    mNameLabel->setScale(0.1f);
    mNameLabel->setCharacterHeight(1);
    mNameLabel->setColor(ColourValue::White);
    mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    Real aabbHeight = entity->getBoundingBox().getSize().y;
    mNameLabel->setAdditionalHeight(aabbHeight);
    mSceneNode->attachObject(mNameLabel);

/* simple test about color picking, bind 1 unique color to each pickable entity, set 1 flag when
   picking is expected, switch material of pickable entities, render into 1 picking texture, switch
   back materials and finally get the entity according to the picked color value */
/*    entity->setMaterialName("Solipsis/ColorPicking");
    SubEntity* subEntity = entity->getSubEntity(0);
    subEntity->setCustomParameter(1, Vector4(0.0f, 1.0f, 0.0f, 0.0f));*/
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//    entity->setQueryFlags(Navigator::QFAvatar);
    mSelectionObject = new ManualObject(uidString + "Sel");
    AxisAlignedBox entityBbox = entity->getBoundingBox();
    AxisAlignedBox selectionBbox;
    selectionBbox.setExtents(entityBbox.getCenter() - entityBbox.getHalfSize()*0.5f, entityBbox.getCenter() + entityBbox.getHalfSize()*0.5f);
    mSelectionObject->setBoundingBox(selectionBbox);
    mSelectionObject->setQueryFlags(Navigator::QFAvatar);
    mSceneNode->attachObject(mSelectionObject);

    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAPosition)
        sceneNode->setPosition(mXmlEntity->getPosition());
    else
        sceneNode->setPosition(Vector3::ZERO);
    mLastRealPosition = sceneNode->getPosition();
    mGravity = mXmlEntity->getFlags() & EFGravity;
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mSceneNode == 0) return;

    if (mSelectionObject != 0)
    {
        mSceneNode->detachObject(mSelectionObject);
        delete mSelectionObject;
    }
    if (mNameLabel != 0)
    {
        mSceneNode->detachObject(mNameLabel);
        delete mNameLabel;
    }
    if (mEntity != 0) {
        mSceneNode->detachObject(mEntity);
        mSceneNode->getCreator()->destroyEntity(mEntity);
    }
    mSceneNode->getCreator()->destroySceneNode(mSceneNode->getName());

#ifdef POOL
#else
    delete mUpdatedXmlEntity;
#endif
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

//-------------------------------------------------------------------------------------
void Avatar::setGravity(bool enabled)
{
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
bool Avatar::isGravityEnabled()
{
    return mGravity;
}

//-------------------------------------------------------------------------------------
void Avatar::update(Real timeSinceLastFrame)
{
    animate(timeSinceLastFrame);
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Avatar::update(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool Avatar::update(XmlEntity* xmlEntity)
#endif
{
    static int c;
    static unsigned long l = (unsigned long)-1;
    if (xmlEntity->getDefinedAttributes() & XmlEntity::DAFlags)
        mXmlEntity->setFlags(xmlEntity->getFlags());
    if (xmlEntity->getDefinedAttributes() & XmlEntity::DAPosition)
    {
        mLastRealPosition = xmlEntity->getPosition();
#ifdef LOGSNDRCV
        OGRE_LOG("RCV uid:" + StringConverter::toString(xmlEntity->getUid()) + " " + StringConverter::toString(mLastRealPosition));
#endif
        unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
        if (l == (unsigned long)-1) { l = n; c = 0; }
        c++;
        if (n - l > 10000)
        {
            Real fr = (Real)c/10.0f;
            OGRE_LOG("Avatar::update() fr=" + StringConverter::toString(fr));
            l = n; c = 0;
        }
    }
    if (xmlEntity->getDefinedAttributes() & XmlEntity::DAOrientation)
    {
        mSceneNode->setOrientation(xmlEntity->getOrientation());
    }

    return true;
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
    State nextState = mState;
    Real animOffset = 0;
    Real animLength = 0;

    if (isLocal())
    {
        Vector3 vpn = mSceneNode->getOrientation()*Vector3::UNIT_X;
        Vector3 vup = mSceneNode->getOrientation()*Vector3::UNIT_Y;
        Vector3 vri = mSceneNode->getOrientation()*Vector3::UNIT_Z;
        Real frontBackMvt;
        Real leftRightMvt;
        Real upDownMvt;
        Vector3 mvt = Vector3(0, 0, 0);

        mUpdatedXmlEntity->setDefinedAttributes(mUpdatedXmlEntity->getDefinedAttributes() & ~(XmlEntity::DAFlags | XmlEntity::DADisplacement | XmlEntity::DAOrientation));

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
// GILLES begin
        else if (mMvtType == MTArountPerson)
        {
            // TurnAround person rotation
            //MovableObject* movable = mSceneNode->getAttachedObject(2);
            //mSceneNode->detachObject (movable);
            mSceneNode->yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
            //mSceneNode->attachObject (movable);
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
                nextState = SWalk;
        }
// GILLES end
        else
        {
            // Third person rotation
            yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == SIdle))
                nextState = SWalk;
        }
        if (!mSceneNode->getOrientation().equals(mUpdatedXmlEntity->getOrientation(), XMLUPDATE_ROTATION_THRESHOLD))
            mUpdatedXmlEntity->setOrientation(mSceneNode->getOrientation());

        if (mPgupKeyMotion.isPressed() && isGravityEnabled())
            setGravity(false);
        if ((mUpdatedXmlEntity->getFlags() & EFGravity) != mGravity)
            mUpdatedXmlEntity->setFlags(mUpdatedXmlEntity->getFlags() ^ EFGravity);

        mPgupKeyMotion.update(timeSinceLastFrame);
        mPgdownKeyMotion.update(timeSinceLastFrame);
        upDownMvt = mPgupKeyMotion.getMotion() - mPgdownKeyMotion.getMotion();
    //    if ((Math::Abs(upDownMvt) > MAX_SPEED*0.9) && (mState != SFly))
    //        nextState = SFly;

        animLength = mAnimationState->getLength();
        if ((mState == SWalk) || (mState == SRun))
            if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
                animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP);
            else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)   // Avatar is rotating : mState = SWalk
                animOffset = leftRightMvt*ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame*(animLength/ROTATION_ANIM_LOOP.valueRadians());
            else
                nextState = SIdle;
        else // mState = SIdle / SFly / SSwim
            animOffset = timeSinceLastFrame;
        if (mAnimationState != 0)
            mAnimationState->addTime(animOffset);

        if (mState != nextState)
            setState(nextState);

        // Move physics character
        Vector3 displacement = mvt + (vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);

        // Update XML entity
        Vector3 d = displacement/timeSinceLastFrame;
        if ((d - mUpdatedXmlEntity->getDisplacement()).length() > XMLUPDATE_DISPLACEMENT_THRESHOLD)
        {
            mUpdatedXmlEntity->setDisplacement(d);
#ifdef LOGSNDRCV
            OGRE_LOG("SND uid:" + StringConverter::toString(mUpdatedXmlEntity->getUid()) + " " + StringConverter::toString(mUpdatedXmlEntity->getDisplacement()));
#endif
        }
    }
    // Smooth X,Z + Smoothless Y positionning (smooth even with only 8 updates/sec)
    Vector3 renderedDisplacement = mLastRealPosition - mSceneNode->getPosition();
    Real motionXZ = std::min(1.0f, SMOOTH_FACTOR*timeSinceLastFrame);
    Real motionY = std::min(1.0f, SMOOTH_FACTOR*2*timeSinceLastFrame);
    Vector3 m(motionXZ, motionY, motionXZ);
    mSceneNode->translate(renderedDisplacement*m);
    // Direct positionning
    //mSceneNode->setPosition(mLastRealPosition);
    // Smooth X,Z + Direct Y positionning
    //mSceneNode->setPosition(mSceneNode->getPosition()*Vector3(1, 0, 1) + mLastRealPosition*Vector3::UNIT_Y);

    if (!isLocal())
    {
        Vector3 vpn = mSceneNode->getOrientation()*Vector3::UNIT_X;
        Real frontBackMvt = (renderedDisplacement*m).length()/(TRANSLATION_SPEED_MPS*timeSinceLastFrame);
        if (vpn.dotProduct(renderedDisplacement) < 0)
            frontBackMvt = -frontBackMvt;
        if ((Math::Abs(frontBackMvt) > EPSILON_SPEED) && (Math::Abs(frontBackMvt) < MAX_SPEED*0.9) && (mState != SWalk))
            nextState = SWalk;
        if ((Math::Abs(frontBackMvt) > MAX_SPEED*0.9) && (mState != SRun))
            nextState = SRun;

        animLength = mAnimationState->getLength();
        if ((mState == SWalk) || (mState == SRun))
            if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
                animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP);
            else
                nextState = SIdle;
        else
            animOffset = timeSinceLastFrame;
        if (mAnimationState != 0)
            mAnimationState->addTime(animOffset);

        if (mState != nextState)
            setState(nextState);
    }
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
       case KC_END:
           setGravity(!isGravityEnabled());
       break;
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

//-------------------------------------------------------------------------------------
void Avatar::yaw(const Radian& angle)
{
    // Update XML entity
    if (isLocal())
        mSceneNode->yaw(angle);
}

//-------------------------------------------------------------------------------------
