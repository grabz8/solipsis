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

#include "Avatar.h"
#include "OgreHelpers.h"
#include "Navigator.h"
#include <CharacterManager.h>

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
Avatar::Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, CharacterInstance* characterInstance) :
#else
Avatar::Avatar(XmlEntity* xmlEntity, bool isLocal, CharacterInstance* characterInstance) :
#endif
    OgrePeer(xmlEntity, isLocal),
#ifdef POOL
    mUpdatedXmlEntity((XmlEntity*)0),
#endif
    mState(SNone),
    mMvtType(MT3rdPerson),
    mCamerasSceneNode(0),
    mAnimationState(0),
    mNameLabel(0),
	mChatLabel(0),
    mSelectionObject(0),
    mUpKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mDownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mLeftKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mRightKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mPgupKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5),
    mPgdownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
{
    setCharacterInstance(characterInstance);

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

//	if(!entity->isAttached())
//		getSceneNode()->attachObject(entity);

    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAPosition)
        getSceneNode()->setPosition(mXmlEntity->getPosition());
    else
        getSceneNode()->setPosition(Vector3::ZERO);
    mLastRealPosition = getSceneNode()->getPosition();

    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAOrientation)
        getSceneNode()->setOrientation(xmlEntity->getOrientation());
    else
        getSceneNode()->setOrientation(Quaternion::IDENTITY);

    mGravity = mXmlEntity->getFlags() & EFGravity;
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    if (mCharacterInstance == 0) return;
    if (getSceneNode() == 0) return;

    if (mSelectionObject != 0)
    {
        getSceneNode()->detachObject(mSelectionObject);
        delete mSelectionObject;
    }
    if (mNameLabel != 0)
    {
        getSceneNode()->detachObject(mNameLabel);
        delete mNameLabel;
    }
	if (mChatLabel != 0)
    {
        getSceneNode()->detachObject(mChatLabel);
        delete mChatLabel;
    }
    CharacterManager::getSingletonPtr()->destroyCharacterInstance(mCharacterInstance);

#ifdef POOL
#else
    delete mUpdatedXmlEntity;
#endif
}

//-------------------------------------------------------------------------------------
CharacterInstance* Avatar::getCharacterInstance()
{
    return mCharacterInstance;
}

//-------------------------------------------------------------------------------------
void Avatar::setCharacterInstance(CharacterInstance* characterInstance)
{
    mCharacterInstance = characterInstance;
    onSceneNodeChanged();
}

//-------------------------------------------------------------------------------------
void Avatar::onSceneNodeChanged()
{
    AxisAlignedBox entityBbox = getEntity()->getBoundingBox();
    Vector3 avatarSize = entityBbox.getSize();
    Vector3 avatarHalfSize = entityBbox.getHalfSize();
    String uidString = mXmlEntity->getUidString();

    // Name Label
    if (mNameLabel == 0)
    {
        mNameLabel = new MovableText(uidString + "Label", mXmlEntity->getName().substr(0, 16), false);
        mNameLabel->setScale(0.1f);
        mNameLabel->setCharacterHeight(1);
        mNameLabel->setColor(ColourValue::White);
        mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    }
    mNameLabel->setAdditionalHeight(avatarSize.y);
    getSceneNode()->attachObject(mNameLabel);

	// Chat Label
    if (mChatLabel == 0)
    {
        mChatLabel = new MovableText(uidString + "ChatLabel", " ", false);
        mChatLabel->setScale(0.15f);
        mChatLabel->setCharacterHeight(1);
		mChatLabel->setSpaceWidth(1);
        mChatLabel->setColor(ColourValue(.8,1,.8,1));
        mChatLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    }
	mChatLabel->setAdditionalHeight(avatarSize.y + .2);
	getSceneNode()->attachObject(mChatLabel);

    // Picking
/* simple test about color picking, bind 1 unique color to each pickable entity, set 1 flag when
   picking is expected, switch material of pickable entities, render into 1 picking texture, switch
   back materials and finally get the entity according to the picked color value */
/*    getEntity()->setMaterialName("Solipsis/ColorPicking");
    SubEntity* subEntity = getEntity()->getSubEntity(0);
    subEntity->setCustomParameter(1, Vector4(0.0f, 1.0f, 0.0f, 0.0f));*/
/* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//    getEntity()->setQueryFlags(Navigator::QFAvatar);
    if (mSelectionObject == 0)
    {
        mSelectionObject = new ManualObject(uidString + "Sel");
        mSelectionObject->setQueryFlags(Navigator::QFAvatar);
    }
    AxisAlignedBox selectionBbox;
    selectionBbox.setExtents(entityBbox.getCenter() - entityBbox.getHalfSize()*0.5f, entityBbox.getCenter() + entityBbox.getHalfSize()*0.5f);
    mSelectionObject->setBoundingBox(selectionBbox);
    getSceneNode()->attachObject(mSelectionObject);

    // Create or re-attach the cameras scene node on the character instance scene node
    if (isLocal())
    {
        if (mCamerasSceneNode == 0)
        {
            // Create camera node/pitch nodes
            mCamerasSceneNode = getSceneNode()->createChildSceneNode(uidString + "CamerasNode");

            // Create First person camera node/pitch node
            SceneNode* camNode = mCamerasSceneNode->createChildSceneNode("FirstPersonCamNode", Vector3(0, 0.95, 0)*avatarSize);
            camNode->yaw(Radian(-Math::HALF_PI));
            SceneNode* pitchCamNode = camNode->createChildSceneNode("FirstPersonCamPitchNode");

            // Create the Third camera node/pitch node
            camNode = mCamerasSceneNode->createChildSceneNode("ThirdPersonCamNode", Vector3(-4, 1.1, 0)*avatarSize.y);
            camNode->yaw(Radian(-Math::HALF_PI));
            pitchCamNode = camNode->createChildSceneNode("ThirdPersonCamPitchNode");

        // GILLES begin
	        // Create the Fourth camera node/pitch node
            camNode = mCamerasSceneNode->createChildSceneNode("TurnAroundPersonCamNode", Vector3(0, 1.1, 0)*avatarSize);
            pitchCamNode = camNode->createChildSceneNode("TurnAroundPersonCamPitchNode", Vector3(-4, 1.1, 0)*avatarSize);
            //pitchCamNode->yaw(Radian(Math::PI));
        // GILLES end
        }
        else
        {
            mCamerasSceneNode->setPosition(Vector3::ZERO);
            mCamerasSceneNode->setOrientation(Quaternion::IDENTITY);
            getSceneNode()->addChild(mCamerasSceneNode->getParentSceneNode()->removeChild(mCamerasSceneNode));
        }
    }
}

//-------------------------------------------------------------------------------------
void Avatar::detachFromSceneNode()
{
    // Name Label
    getSceneNode()->detachObject(mNameLabel);

    // Picking
    getSceneNode()->detachObject(mSelectionObject);

    // Re-attach cameras scene node on parent
    if (isLocal())
    {
        mCamerasSceneNode->setPosition(getSceneNode()->getPosition());
        mCamerasSceneNode->setOrientation(getSceneNode()->getOrientation());
        getSceneNode()->getParentSceneNode()->addChild(getSceneNode()->removeChild(mCamerasSceneNode));
    }
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
        OGRE_LOG("RCV uid:" + xmlEntity->getUidString() + " " + StringConverter::toString(mLastRealPosition));
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
        getSceneNode()->setOrientation(xmlEntity->getOrientation());
    }

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Avatar::action(RefCntPoolPtr<XmlAction>& xmlAction)
#else
bool Avatar::action(XmlAction* xmlAction)
#endif
{
	String label = xmlAction->getDesc();
	if( label.size() < 1 ) label = " ";
	mChatLabel->setCaption( label );

    return true;
}

//-------------------------------------------------------------------------------------
void Avatar::startAnimation(const String &name, bool loop)
{
    if (name.length() == 0) return;
    mAnimationState = getEntity()->getAnimationState(name);
    mAnimationState->setLoop(loop);
    mAnimationState->setEnabled(true);
}

//-------------------------------------------------------------------------------------
void Avatar::stopAnimation()
{
    if (mStateAnimName[mState].length() == 0) return;
    AnimationState* animationStateToStop = getEntity()->getAnimationState(mStateAnimName[mState]);
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
        Vector3 vpn = getSceneNode()->getOrientation()*Vector3::UNIT_X;
        Vector3 vup = getSceneNode()->getOrientation()*Vector3::UNIT_Y;
        Vector3 vri = getSceneNode()->getOrientation()*Vector3::UNIT_Z;
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
            //MovableObject* movable = getSceneNode()->getAttachedObject(2);
            //getSceneNode()->detachObject (movable);
            getSceneNode()->yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
            //getSceneNode()->attachObject (movable);
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
        if (!getSceneNode()->getOrientation().equals(mUpdatedXmlEntity->getOrientation(), XMLUPDATE_ROTATION_THRESHOLD))
            mUpdatedXmlEntity->setOrientation(getSceneNode()->getOrientation());

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
            OGRE_LOG("SND uid:" + mUpdatedXmlEntity->getUidString() + " " + StringConverter::toString(mUpdatedXmlEntity->getDisplacement()));
#endif
        }
    }
    // Smooth X,Z + Smoothless Y positionning (smooth even with only 8 updates/sec)
    Vector3 renderedDisplacement = mLastRealPosition - getSceneNode()->getPosition();
    Real motionXZ = std::min(1.0f, SMOOTH_FACTOR*timeSinceLastFrame);
    Real motionY = std::min(1.0f, SMOOTH_FACTOR*2*timeSinceLastFrame);
    Vector3 m(motionXZ, motionY, motionXZ);
    getSceneNode()->translate(renderedDisplacement*m);
    // Direct positionning
    //getSceneNode()->setPosition(mLastRealPosition);
    // Smooth X,Z + Direct Y positionning
    //getSceneNode()->setPosition(getSceneNode()->getPosition()*Vector3(1, 0, 1) + mLastRealPosition*Vector3::UNIT_Y);

    if (!isLocal())
    {
        Vector3 vpn = getSceneNode()->getOrientation()*Vector3::UNIT_X;
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
        getSceneNode()->yaw(angle);
}

//-------------------------------------------------------------------------------------
