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
#include <Character.h>
#include <CharacterInstance.h>

using namespace Solipsis;
using namespace CommonTools;

String Avatar::mDefaultStateAnimName[ASAvatarAnimCount] = {
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
    mState(ASAvatarNone),
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
    for (int a = 0;a < ASAvatarAnimCount; ++a)
        mStateAnimName[a] = mDefaultStateAnimName[a];

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

    setCharacterInstance(characterInstance);

//	if(!entity->isAttached())
//		getSceneNode()->attachObject(entity);

    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAPosition)
        getSceneNode()->setPosition(mXmlEntity->getPosition());
    else
        getSceneNode()->setPosition(Vector3::ZERO);
    mLastRealPosition = getSceneNode()->getPosition();

    if (mXmlEntity->getDefinedAttributes() & XmlEntity::DAOrientation)
        getSceneNode()->setOrientation(mXmlEntity->getOrientation());
    else
        getSceneNode()->setOrientation(Quaternion::IDENTITY);
    mLastRealOrientation = getSceneNode()->getOrientation();

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
        SceneNode* firstPersonCamNode = 0;
        SceneNode* thirdPersonCamNode = 0;
        SceneNode* turnAroundPersonCamNode = 0;
        SceneNode* turnAroundPersonCamPitchNode = 0;
        if (mCamerasSceneNode == 0)
        {
            // Create camera node/pitch nodes
            mCamerasSceneNode = getSceneNode()->createChildSceneNode(uidString + "CamerasNode");

            // Create First person camera node/pitch node
            firstPersonCamNode = mCamerasSceneNode->createChildSceneNode("FirstPersonCamNode");
            firstPersonCamNode->yaw(Radian(-Math::HALF_PI));
            SceneNode* pitchCamNode = firstPersonCamNode->createChildSceneNode("FirstPersonCamPitchNode");

            // Create the Third camera node/pitch node
            thirdPersonCamNode = mCamerasSceneNode->createChildSceneNode("ThirdPersonCamNode");
            thirdPersonCamNode->yaw(Radian(-Math::HALF_PI));
            pitchCamNode = thirdPersonCamNode->createChildSceneNode("ThirdPersonCamPitchNode");

        // GILLES begin
	        // Create the Fourth camera node/pitch node
            turnAroundPersonCamNode = mCamerasSceneNode->createChildSceneNode("TurnAroundPersonCamNode");
            turnAroundPersonCamPitchNode = turnAroundPersonCamNode->createChildSceneNode("TurnAroundPersonCamPitchNode");
            //pitchCamNode->yaw(Radian(Math::PI));
        // GILLES end
        }
        else
        {
            firstPersonCamNode = (SceneNode*)mCamerasSceneNode->getChild("FirstPersonCamNode");
            thirdPersonCamNode = (SceneNode*)mCamerasSceneNode->getChild("ThirdPersonCamNode");
            turnAroundPersonCamNode = (SceneNode*)mCamerasSceneNode->getChild("TurnAroundPersonCamNode");
            turnAroundPersonCamPitchNode = (SceneNode*)turnAroundPersonCamNode->getChild("TurnAroundPersonCamPitchNode");
            getSceneNode()->addChild(mCamerasSceneNode->getParentSceneNode()->removeChild(mCamerasSceneNode));
        }

        mCamerasSceneNode->setPosition(Vector3::ZERO);
        mCamerasSceneNode->setOrientation(Quaternion::IDENTITY);
        firstPersonCamNode->setPosition(Vector3(0, 0.95, 0)*avatarSize);
        thirdPersonCamNode->setPosition(Vector3(-4, 1.1, 0)*avatarSize.y);
        turnAroundPersonCamNode->setPosition(Vector3(0, 1.1, 0)*avatarSize);
        turnAroundPersonCamPitchNode->setPosition(Vector3(-4, 1.1, 0)*avatarSize);
    }

    getSceneNode()->setPosition(mXmlEntity->getPosition());
    getSceneNode()->setOrientation(mXmlEntity->getOrientation());
    setState(ASAvatarIdle);
}

//-------------------------------------------------------------------------------------
void Avatar::detachFromSceneNode()
{
    setState(ASAvatarNone);

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
void Avatar::onAvatarSave()
{
    assert(isLocal());

    String safFilename = mCharacterInstance->getCharacter()->getPath()->getLastFileName(true);
    String sifFilename = mCharacterInstance->getUidPath()->getLastFileName(true);

    // Get the content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = mXmlEntity->getContent()->getContentLodMap();
    XmlLodContent::LodContentFileList& lodContent0FileList = contentLodMap[0]->getLodContentFileList();
    XmlLodContent::LodContentFileList::iterator lodContent0File;
    for (lodContent0File = lodContent0FileList.begin(); lodContent0File != lodContent0FileList.end(); ++lodContent0File)
        if (lodContent0File->mFilename.find(".saf") == lodContent0File->mFilename.length() - 4)
        {
            lodContent0File->mFilename = safFilename;
            break;
        }
    if (lodContent0File == lodContent0FileList.end())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .saf avatar file found !", "Avatar::onAvatarSave");

    for (lodContent0File = lodContent0FileList.begin(); lodContent0File != lodContent0FileList.end(); ++lodContent0File)
        if (lodContent0File->mFilename.find(".sif") == lodContent0File->mFilename.length() - 4)
        {
            lodContent0File->mFilename = sifFilename;
            lodContent0File->mVersion++;
            break;
        }
    if (lodContent0File == lodContent0FileList.end())
    {
        LodContentFileStruct lodContent0FileSif;
        lodContent0FileSif.mFilename = sifFilename;
        lodContent0FileSif.mVersion = 0;
        lodContent0FileList.push_back(lodContent0FileSif);
    }
}

//-------------------------------------------------------------------------------------
void Avatar::setNameVisibility(bool visible)
{
    mNameLabel->setVisible(visible);
}

//-------------------------------------------------------------------------------------
void Avatar::setState(Solipsis::AnimationState state)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Avatar::setState() state:%d", (int)state);
    if (mStateAnimName[mState].length() > 0)
        stopAnimation();
    if (mStateAnimName[state].length() > 0)
        startAnimation(mStateAnimName[state]);
    mState = state;
    if (mIsLocal)
        mUpdatedXmlEntity->setAnimation(mState);
}

//-------------------------------------------------------------------------------------
Solipsis::AnimationState Avatar::getState()
{
    return mState;
}

//-------------------------------------------------------------------------------------
void Avatar::setStateAnimName(Solipsis::AnimationState state, const String& name)
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
    unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
    if (l == (unsigned long)-1) { l = n; c = 0; }
    c++;
    if (n - l > 10000)
    {
        Real fr = (Real)c/10.0f;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Avatar::update() fr=%.2f", fr);
        l = n; c = 0;
    }

    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
    if (definedAttributes & XmlEntity::DAFlags)
        mXmlEntity->setFlags(xmlEntity->getFlags());
    if (definedAttributes & XmlEntity::DAPosition)
    {
        mLastRealPosition = xmlEntity->getPosition();
    }
    if (definedAttributes & XmlEntity::DAOrientation)
    {
        mLastRealOrientation = xmlEntity->getOrientation();
    }
    if (definedAttributes & XmlEntity::DAAnimation)
    {
        setState(xmlEntity->getAnimation());
        mXmlEntity->setAnimation(xmlEntity->getAnimation());
    }
    if (definedAttributes & XmlEntity::DAContent)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Avatar::update() Destroy/Load new character of avatar uid:%s", mXmlEntity->getUidString().c_str());
        detachFromSceneNode();
        CharacterManager::getSingletonPtr()->destroyCharacterInstance(mCharacterInstance);
        String defaultCharacterName = "";
        XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
        for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
            if (it->mFilename.find(".saf") == it->mFilename.length() - 4)
                defaultCharacterName = it->mFilename.substr(0, it->mFilename.length() - 4);
        CharacterInstance* characterInstance = CharacterManager::getSingletonPtr()->loadCharacterInstance(xmlEntity->getUidString(), defaultCharacterName);
        if (characterInstance == 0)
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create character instance !", "Avatar::update");
        setCharacterInstance(characterInstance);
    }

#ifdef LOGSNDRCV
    String log = "RCV uid:" + xmlEntity->getUidString();
    if (definedAttributes & XmlEntity::DAPosition) log += " p:" + StringConverter::toString(mLastRealPosition);
    if (definedAttributes & XmlEntity::DAOrientation) log += " o:" + StringConverter::toString(mLastRealOrientation);
    LOGHANDLER_LOG(LogHandler::VL_DEBUG, log);
#endif

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
    Ogre::AnimationState* animationStateToStop = getEntity()->getAnimationState(mStateAnimName[mState]);
    animationStateToStop->setLoop(false);
    animationStateToStop->setEnabled(false);
}

//-------------------------------------------------------------------------------------
void Avatar::animate(Real timeSinceLastFrame)
{
    if (timeSinceLastFrame == 0.0)
        return;

    AnimationState nextState = mState;
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

        mUpdatedXmlEntity->setDefinedAttributes(mUpdatedXmlEntity->getDefinedAttributes() & ~(XmlEntity::DAFlags | XmlEntity::DADisplacement | XmlEntity::DAOrientation | XmlEntity::DAAnimation));

        mUpKeyMotion.update(timeSinceLastFrame);
        mDownKeyMotion.update(timeSinceLastFrame);
        frontBackMvt = mUpKeyMotion.getMotion() - mDownKeyMotion.getMotion();
        mvt += vpn*frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
        if ((Math::Abs(frontBackMvt) > EPSILON_SPEED) && (Math::Abs(frontBackMvt) < MAX_SPEED*0.9) && (mState != ASAvatarWalk))
            nextState = ASAvatarWalk;
        if ((Math::Abs(frontBackMvt) > MAX_SPEED*0.9) && (mState != ASAvatarRun))
            nextState = ASAvatarRun;

        mLeftKeyMotion.update(timeSinceLastFrame);
        mRightKeyMotion.update(timeSinceLastFrame);
        leftRightMvt = mLeftKeyMotion.getMotion() - mRightKeyMotion.getMotion();
        if (mMvtType == MT1stPerson)
        {
            // First person straff
            mvt += -vri*leftRightMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == ASAvatarIdle))
                nextState = ASAvatarWalk;
        }
// GILLES begin
        else if (mMvtType == MTArountPerson)
        {
            // TurnAround person rotation
            //MovableObject* movable = getSceneNode()->getAttachedObject(2);
            //getSceneNode()->detachObject (movable);
            getSceneNode()->yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
            mXmlEntity->setOrientation(getSceneNode()->getOrientation());
            //getSceneNode()->attachObject (movable);
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == ASAvatarIdle))
                nextState = ASAvatarWalk;
        }
// GILLES end
        else
        {
            // Third person rotation
            yaw(leftRightMvt*ROTATION_SPEED_RPS*timeSinceLastFrame);
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == ASAvatarIdle))
                nextState = ASAvatarWalk;
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
    //    if ((Math::Abs(upDownMvt) > MAX_SPEED*0.9) && (mState != ASAvatarFly))
    //        nextState = ASAvatarFly;

        animLength = mAnimationState->getLength();
        if ((mState == ASAvatarWalk) || (mState == ASAvatarRun))
            if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
                animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP);
            else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)   // Avatar is rotating : mState = ASAvatarWalk
                animOffset = leftRightMvt*ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame*(animLength/ROTATION_ANIM_LOOP.valueRadians());
            else
                nextState = ASAvatarIdle;
        else // mState = ASAvatarIdle / ASAvatarFly / ASAvatarSwim
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
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SND uid:%s d:%s", mUpdatedXmlEntity->getUidString().c_str(), StringConverter::toString(mUpdatedXmlEntity->getDisplacement()).c_str());
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
    mXmlEntity->setPosition(getSceneNode()->getPosition());

    if (!isLocal())
    {
        Vector3 vpn = getSceneNode()->getOrientation()*Vector3::UNIT_X;
        Real frontBackMvt = (renderedDisplacement*m).length()/(TRANSLATION_SPEED_MPS*timeSinceLastFrame);
        if (vpn.dotProduct(renderedDisplacement) < 0)
            frontBackMvt = -frontBackMvt;
        Real motionOrientation = std::min(1.0f, SMOOTH_FACTOR*timeSinceLastFrame);
        Quaternion newOrientation = Quaternion::Slerp(motionOrientation, getSceneNode()->getOrientation(), mLastRealOrientation, true);
        Vector3 newVpn = newOrientation*Vector3::UNIT_X;
        getSceneNode()->setOrientation(newOrientation);
        mXmlEntity->setOrientation(getSceneNode()->getOrientation());
        Radian toAngle;
        Vector3 toAxis;
        vpn.getRotationTo(newVpn).ToAngleAxis(toAngle, toAxis);
        Real leftRightMvt = toAngle.valueRadians()/(ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame);
        animLength = mAnimationState->getLength();
        if ((mState == ASAvatarWalk) || (mState == ASAvatarRun))
        {
            if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
                animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP);
            else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)   // Avatar is rotating : mState = ASAvatarWalk
                animOffset = leftRightMvt*ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame*(animLength/ROTATION_ANIM_LOOP.valueRadians());
        }
        else
            animOffset = timeSinceLastFrame;
        if (mAnimationState != 0)
            mAnimationState->addTime(animOffset);
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
    {
        getSceneNode()->yaw(angle);
        mXmlEntity->setOrientation(getSceneNode()->getOrientation());
    }
}

//-------------------------------------------------------------------------------------
