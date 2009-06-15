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

#include "Prerequisites.h"
#include "Avatar.h"
#include "OgreTools/OgreHelpers.h"
#include "MainApplication/Navigator.h"
#include "Cameras/OrbitalCameraSupport.h"
#include <CTStringHelpers.h>
#include <CharacterManager.h>
#include <Character.h>
#include <CharacterInstance.h>
#include <IFaceController.h>
#include <VoiceEngineManager.h>

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

Avatar::Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, CharacterInstance* characterInstance) :

    OgrePeer(xmlEntity, isLocal)
,   mUpdatedXmlEntity((XmlEntity*)0)
,   mState(ASAvatarNone)
,   mMvtType(MT3rdPerson)
,   mCamerasSceneNode(0)
,   mAnimationState(0)
,   mNameLabel(0)
,   mChatLabel(0)
, 	m_pSoundIcon(0)
,   mSelectionObject(0)
,   mUpKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mDownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mLeftKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mRightKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mPgupKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mPgdownKeyMotion(MAX_SPEED/100, MAX_SPEED, 1.5, 0.5)
,   mVoiceMinDist(1.0f)
,   mVoiceMaxDist(100.0f)
,   mGhost(false)
{
    for (int a = 0;a < ASAvatarAnimCount; ++a)
        mStateAnimName[a] = mDefaultStateAnimName[a];


    if (isLocal)
    {
        mUpdatedXmlEntity.allocate();
        mUpdatedXmlEntity->setDefinedAttributes(XmlEntity::DANone);
        mUpdatedXmlEntity->setUid(mXmlEntity->getUid());
    }

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

    // Bind this avatar in the Voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine != 0)
        voiceEngine->setAvatarHandler(mXmlEntity->getUid(), this);

    // Update initial properties of user avatar in the voice engine
    if (isLocal)
        updateVoiceEngine(true, true);
}

//-------------------------------------------------------------------------------------
Avatar::~Avatar()
{
    // Unbind this avatar in the Voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine != 0)
        voiceEngine->removeAvatarHandler(mXmlEntity->getUid());

    if (mCharacterInstance == 0) return;
    if (getSceneNode() == 0) return;

    if (mCamerasSceneNode != 0)
        getSceneNode()->removeAndDestroyChild(mCamerasSceneNode->getName());

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
    if (m_pSoundIcon != 0)
    {
        delete m_pSoundIcon;
    }
    CharacterManager::getSingletonPtr()->destroyCharacterInstance(mCharacterInstance);
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
    mCharacterInstance->setGhost(mGhost);
    onSceneNodeChanged();
}

//-------------------------------------------------------------------------------------
void Avatar::onSceneNodeChanged()
{
    AxisAlignedBox entityBbox = getEntity()->getBoundingBox();
    Vector3 avatarSize = entityBbox.getSize();
    Vector3 avatarHalfSize = entityBbox.getHalfSize();

    // Name Label
    if (mNameLabel == 0)
    {
        mNameLabel = new MovableText(mXmlEntity->getUid() + "Label", mXmlEntity->getName().substr(0, 16), false, "BerlinSans32");
        mNameLabel->setScale(0.1f);
        mNameLabel->setCharacterHeight(1);
        mNameLabel->setSpaceWidth(1);
        mNameLabel->setColor(ColourValue::White);
        mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
        mNameLabel->showOnTop(true);
    }
    mNameLabel->setAdditionalHeight(avatarSize.y);
    getSceneNode()->attachObject(mNameLabel);

    // Chat Label
    if (mChatLabel == 0)
    {
//        mChatLabel = new MovableText(mXmlEntity->getUid() + "ChatLabel", " ", false, "BerlinSans32");
        mChatLabel = new MovableText(mXmlEntity->getUid() + "ChatLabel", " ", false, "DejaVuSans");
        mChatLabel->setScale(0.12f);
        mChatLabel->setCharacterHeight(1);
        mChatLabel->setSpaceWidth(1);
        mChatLabel->setColor(ColourValue::ColourValue(1.0f, 1.0f, 0.0f, 0.0f));
        mChatLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
        mChatLabel->showOnTop(true);
        mChatLabelAlphaTimer = 0.0f;
    }
	mChatLabel->setAdditionalHeight(avatarSize.y + 0.2f);
	getSceneNode()->attachObject(mChatLabel);

	if (isLocal())
	{
		mChatLabel->setVisible(false);
		mNameLabel->setVisible(false);
	}

    // Sound Icon
    if (m_pSoundIcon == 0) 
    {
        m_pSoundIcon = new SoundIcon(getSceneMgr(), getSceneNode(), mXmlEntity->getUid() + "Billboard_Sound", avatarSize.y+0.5);
    }

    //m_pSoundIcon->setStatus(SoundIcon::Showed);

    //setNameVisibility(!isLocal());

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
        mSelectionObject = new ManualObject(mXmlEntity->getUid() + "Sel");
        mSelectionObject->setQueryFlags(Navigator::QFAvatar);
    }
    AxisAlignedBox selectionBbox;
    selectionBbox.setExtents(entityBbox.getCenter() - entityBbox.getHalfSize()*0.5f, entityBbox.getCenter() + entityBbox.getHalfSize()*0.5f);
    mSelectionObject->setBoundingBox(selectionBbox);
    getSceneNode()->attachObject(mSelectionObject);

    getSceneNode()->setPosition(mXmlEntity->getPosition());
    getSceneNode()->setOrientation(mXmlEntity->getOrientation());
#if 1 // GILLES FLY
    setState(ASAvatarFly);
#else
    setState(ASAvatarIdle);
#endif

    // Attach all camera supports to the new user avatar 
    if (isLocal())
        Navigator::getSingletonPtr()->getMainCameraSupportManager()->attachAllCameraSupportsToNode(getSceneNode());
}

//-------------------------------------------------------------------------------------
void Avatar::detachFromSceneNode()
{
    setState(ASAvatarNone);

    // Name Label
    getSceneNode()->detachObject(mNameLabel);

    // Picking
    getSceneNode()->detachObject(mSelectionObject);
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
    {
        if (lodContent0File->mFilename.find(".saf") == lodContent0File->mFilename.length() - 4)
        {
            lodContent0File->mFilename = safFilename;
            break;
        }
    }
    if (lodContent0File == lodContent0FileList.end())
    {
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .saf avatar file found !", "Avatar::onAvatarSave");
    }

    for (lodContent0File = lodContent0FileList.begin(); lodContent0File != lodContent0FileList.end(); ++lodContent0File)
    {
        if (lodContent0File->mFilename.find(".sif") == lodContent0File->mFilename.length() - 4)
        {
            lodContent0File->mFilename = sifFilename;
            lodContent0File->mVersion++;
            break;
        }
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
    mChatLabel->setVisible(visible);
    //m_SoundIcon->setVisible(false);
}

//-------------------------------------------------------------------------------------
void Avatar::setGhost(bool ghost)
{
    if (ghost == mGhost) return;
    mGhost = ghost;
    mCharacterInstance->setGhost(ghost);
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
    if (isLocal())
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
void Avatar::setVoiceDistances(float minDist, float maxDist)
{
    mVoiceMinDist = minDist;
    mVoiceMaxDist = maxDist;
    updateVoiceEngine(false, true);
}

//-------------------------------------------------------------------------------------
void Avatar::getVoiceDistances(float &minDist, float &maxDist)
{
    minDist = mVoiceMinDist;
    maxDist = mVoiceMaxDist;
}

//-------------------------------------------------------------------------------------
void Avatar::update(Real timeSinceLastFrame)
{
    animate(timeSinceLastFrame);

    if (mChatLabelAlphaTimer > 0.0f)
    {
        float smoothAngle = -1.0f;
        if (mChatLabelAlphaTimer <= 1.0f)
            smoothAngle = (1.0f - mChatLabelAlphaTimer)*Math::HALF_PI;
        else if (mChatLabelAlphaTimer >= 9.0f)
            smoothAngle = (mChatLabelAlphaTimer - 9.0f)*Math::HALF_PI;
        mChatLabelAlphaTimer -= timeSinceLastFrame;
        if (mChatLabelAlphaTimer < 0.0f)
        {
            mChatLabelAlphaTimer = 0.0f;
            smoothAngle = Math::HALF_PI;
        }
        if (smoothAngle >= 0.0f)
            mChatLabel->setColor(ColourValue::ColourValue(1.0f, 1.0f, 0.0f, Math::Cos(smoothAngle)));
    }

    if (m_pSoundIcon)
    {	
        m_pSoundIcon->animate(timeSinceLastFrame);
    }
}

//-------------------------------------------------------------------------------------

bool Avatar::updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    static int c;
    static unsigned long l = (unsigned long)-1;
    unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
    if (l == (unsigned long)-1) { l = n; c = 0; }
    c++;
    if (n - l > 10000)
    {
        Real fr = (Real)c/10.0f;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Avatar::updateEntity() fr=%.2f", fr);
        l = n; c = 0;
    }

    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();
    if (definedAttributes & XmlEntity::DAFlags)
    {
        mXmlEntity->setFlags(xmlEntity->getFlags());
        mGravity = mXmlEntity->getFlags() & EFGravity;
    }
//     if (definedAttributes & XmlEntity::DAAABoundingBox)
//     {
//         if (mpBox == NULL)
//         {
//             const Ogre::AxisAlignedBox & bbBox = mXmlEntity->getAABoundingBox();
//             mpBox = new MovableBox(mXmlEntity->getUid()+"_BBOX", bbBox.getSize(), false);
//             getSceneNode()->attachObject(mpBox);
//         }
//     }
    if (definedAttributes & XmlEntity::DAPosition)
    {
        mLastRealPosition = xmlEntity->getPosition();
        if (isLocal())
            updateVoiceEngine(true, false);
    }
    if (definedAttributes & XmlEntity::DAOrientation)
    {
        mLastRealOrientation = xmlEntity->getOrientation();
        if (isLocal())
        {
            mXmlEntity->setOrientation(xmlEntity->getOrientation());
            getSceneNode()->setOrientation(xmlEntity->getOrientation());
        }
    }
    if (definedAttributes & XmlEntity::DAAnimation)
    {
        setState(xmlEntity->getAnimation());
        mXmlEntity->setAnimation(xmlEntity->getAnimation());
    }
    if ((definedAttributes & XmlEntity::DAContent) && (xmlEntity->getDownloadProgress() >= 1.0f))
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Avatar::updateEntity() Destroy/Load new character of avatar uid:%s", mXmlEntity->getUid().c_str());
        detachFromSceneNode();
        CharacterManager::getSingletonPtr()->destroyCharacterInstance(mCharacterInstance);
        String defaultCharacterName = "";
        XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
        for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
            if (it->mFilename.find(".saf") == it->mFilename.length() - 4)
                defaultCharacterName = it->mFilename.substr(0, it->mFilename.length() - 4);
        CharacterInstance* characterInstance = CharacterManager::getSingletonPtr()->loadCharacterInstance(xmlEntity->getUid(), defaultCharacterName);
        if (characterInstance == 0)
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create character instance !", "Avatar::updateEntity");
        setCharacterInstance(characterInstance);
    }
    if (definedAttributes & XmlEntity::DAProgress)
    {
        OGRE_LOG("Progress for avatar " + 
            xmlEntity->getUid() + " : " + 
            StringConverter::toString((Real) xmlEntity->getDownloadProgress()));
    }

#ifdef LOGSNDRCV
    String log = "RCV uid:" + xmlEntity->getUid();
    if (definedAttributes & XmlEntity::DAPosition) log += " p:" + StringConverter::toString(mLastRealPosition);
    if (definedAttributes & XmlEntity::DAOrientation) log += " o:" + StringConverter::toString(mLastRealOrientation);
    LOGHANDLER_LOG(LogHandler::VL_DEBUG, log.c_str());
#endif

    return true;
}

//-------------------------------------------------------------------------------------

bool Avatar::action(RefCntPoolPtr<XmlAction>& xmlAction)
{
    std::wstring wlabel = xmlAction->getDesc();
    if (wlabel.size() < 1) wlabel = L" ";
    StringHelpers::autoInsertHyphens(30, wlabel);
    mChatLabel->setCaption(wlabel);
    mChatLabelAlphaTimer = 10.0f;

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

#if 1 // GILLES FLY
	//Height between -4 and 100
Real nodeHeight = (20+getSceneNode()->getPosition().y)/10.0;
//Real factFly = std::max( (float)1.0 , std::min( (float)20.0 , (float)(nodeHeight*nodeHeight/2.0 ) ) ) ;

//Real nodeHeight = (30+getSceneNode()->getPosition().y)/10.0;
//Real factFly = std::max( (float)1.0 , std::min( (float)30.0 , (float)(nodeHeight*nodeHeight/2.0 ) ) ) ;
    Real factFly = 2.0;
#endif

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

#if 0 // GILLES FLY
        mvt += vpn*frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
#endif
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
 
#if 0 // GILLES FLY
            mvt += -vri*leftRightMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
#endif
            if ((Math::Abs(leftRightMvt) > EPSILON_SPEED) && (mState == ASAvatarIdle))
                nextState = ASAvatarWalk;
        }
        else
        {
            // Third/TurnAround person rotation
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

#if 1 // GILLES FLY
		//Translation speed variation in fly mode
		if(!isGravityEnabled())
		{
            if (mMvtType == MT1stPerson)
                mvt += -vri*leftRightMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*factFly;
			mvt += vpn*frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*factFly;
			mvt += vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*factFly/2.0;
			if ( (Math::Abs(upDownMvt) > MAX_SPEED*0.25) && (mState != ASAvatarWalk))
				nextState = ASAvatarFly;
		}
		else
		{
            if (mMvtType == MT1stPerson)
                mvt += -vri*leftRightMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
            mvt += vpn*frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
            mvt += vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame;
        }
#endif

        animLength = mAnimationState->getLength();
        if ((mState == ASAvatarWalk) || (mState == ASAvatarRun))
            if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
                animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP);
            else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)   // Avatar is rotating : mState = ASAvatarWalk
                animOffset = leftRightMvt*ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame*(animLength/ROTATION_ANIM_LOOP.valueRadians());
            else
                nextState = ASAvatarIdle;
#if 1 // GILLES FLY
		//Animation speed  variation in fly mode
		else if(mState == ASAvatarFly)
		{
			if(!isGravityEnabled()) 
			{
				nextState = ASAvatarFly;
				//Min and Max animOffset
                Real factWalk = std::min((float)1.0,std::max((float)1.0/factFly,(float)0.1));
				if (Math::Abs(frontBackMvt) > EPSILON_SPEED)    // Avatar is walking or running
					animOffset = frontBackMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame*(animLength/TRANSLATION_ANIM_LOOP)*factWalk;
				else if (Math::Abs(leftRightMvt) > EPSILON_SPEED)   // Avatar is rotating : mState = ASAvatarWalk
					animOffset = leftRightMvt*ROTATION_SPEED_RPS.valueRadians()*timeSinceLastFrame*(animLength/ROTATION_ANIM_LOOP.valueRadians())*factWalk;
				else
					animOffset = timeSinceLastFrame/2.0;	
			}
			//if gravity is enable, it's because Avatar touch terrain
			else
				nextState = ASAvatarIdle;
		}
#endif
        else // mState = ASAvatarIdle / ASAvatarFly / ASAvatarSwim
            animOffset = timeSinceLastFrame;
        if (mAnimationState != 0)
            mAnimationState->addTime(animOffset);

        if (mState != nextState)
            setState(nextState);

        // Move physics character
#if 1 // GILLES FLY
        Vector3 displacement = mvt;
        //Vector3 displacement = mvt + (vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame * factFly);
#else
        Vector3 displacement = mvt + (vup*upDownMvt*TRANSLATION_SPEED_MPS*timeSinceLastFrame);
#endif

		// update facial animation
		IFaceController* pFaceController = getCharacterInstance()->getFaceController();
		if(pFaceController)
			pFaceController->updateFace( timeSinceLastFrame );

        // Update XML entity
        Vector3 d = displacement/timeSinceLastFrame;
        if ((d - mUpdatedXmlEntity->getDisplacement()).length() > XMLUPDATE_DISPLACEMENT_THRESHOLD)
        {
            mUpdatedXmlEntity->setDisplacement(d);
#ifdef LOGSNDRCV
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SND uid:%s d:%s", mUpdatedXmlEntity->getUid().c_str(), StringConverter::toString(mUpdatedXmlEntity->getDisplacement()).c_str());
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
void Avatar::updateVoiceEngine(bool updatePosDirVel, bool updateDist)
{
    // Update position of avatar in the Voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0) return;

    float pos[3], dir[3], vel[3], dist[2];
    float *ppos, *pdir, *pvel, *pdist;
    ppos = pdir = pvel = pdist = 0;
    if (updatePosDirVel)
    {
        pos[0] = mLastRealPosition.x; pos[1] = mLastRealPosition.y; pos[2] = mLastRealPosition.z;
        Vector3 vpn = getSceneNode()->getOrientation()*Vector3::UNIT_X;
        dir[0] = vpn.x; dir[1] = vpn.y; dir[2] = vpn.z;
        vel[0] = vel[1] = vel[2] = 0.0f;
        ppos = pos; pdir = dir; pvel = vel;
    }
    if (updateDist)
    {
        dist[0] = mVoiceMinDist;
        dist[1] = mVoiceMaxDist;
        pdist = dist;
    }
    voiceEngine->updateRecordingAvatar(ppos, pdir, pvel, pdist);
}

//-------------------------------------------------------------------------------------
