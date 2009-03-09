/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

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

#include "BodyPart.h"
#include "Character.h"
#include "CharacterInstance.h"
//#include "ScreenshotManager.h"

using namespace Solipsis;


//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartInstance::BodyPartInstance(BodyPart* pBodyPart, CharacterInstance* owner) :
	mBodyPart(pBodyPart),
    mOwner(owner),
    mGhost(false)
{
	mCurrentBodyPartModelIterator = mBodyPart->mDefaultBodyPartModelIterator;
    if (mCurrentBodyPartModelIterator->second != 0)
    {
        mCurrentBodyPartModelInstance = new BodyPartModelInstance(mCurrentBodyPartModelIterator->second, this);
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(true);
    }
    else
        mCurrentBodyPartModelInstance = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartInstance::~BodyPartInstance()
{
    delete mCurrentBodyPartModelInstance;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool BodyPartInstance::isColourModifiable()
{	
	return ((mCurrentBodyPartModelIterator->second != NULL)&&(mCurrentBodyPartModelIterator->second->isColourModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setColourModifiable(bool isColourModifiable)
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so can't change it's properties !");
    mCurrentBodyPartModelIterator->second->setColourModifiable(isColourModifiable);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& BodyPartInstance::getColour()
{
	assert( (mCurrentBodyPartModelInstance != 0) && "There is no CurrentBodyPartModel for this BodyPart so this one can't have a colour !");
	return mCurrentBodyPartModelInstance->getColour();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setColour(const ColourValue&	colour)
{
	assert( (mCurrentBodyPartModelInstance  != 0) && "There is no CurrentBodyPartModel for this BodyPart so this one can't have a colour !");
	mCurrentBodyPartModelInstance->setColour(colour);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool BodyPartInstance::isTextureModifiable()
{
	return ((mCurrentBodyPartModelIterator->second  != NULL)&&(mCurrentBodyPartModelIterator->second ->isTextureModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr BodyPartInstance::getCurrentTexture()
{
	assert( (mCurrentBodyPartModelInstance != 0) && "No current BodyPartModel, so it can't have a current texture !");
	return mCurrentBodyPartModelInstance->getCurrentTexture();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setPreviousTextureAsCurrent()
{
	assert( (mCurrentBodyPartModelInstance != 0) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelInstance->setPreviousTextureAsCurrent();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setNextTextureAsCurrent()
{
	assert( (mCurrentBodyPartModelInstance != 0) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelInstance->setNextTextureAsCurrent();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::addTexture(const TexturePtr& texture)
{
	assert( (mCurrentBodyPartModelInstance != 0) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelInstance->addTexture(texture);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModel* BodyPartInstance::getCurrentBodyPartModel()
{
	return mCurrentBodyPartModelIterator->second;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& BodyPartInstance::getCurrentBodyPartModelName()
{
	return mCurrentBodyPartModelIterator->first;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setCurrentBodyPartModel(const String& bodyPartModelName)
{
	if (mCurrentBodyPartModelIterator->second != NULL)
	{
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(false);
        delete mCurrentBodyPartModelInstance;
        mCurrentBodyPartModelInstance = 0;
	}

	if (bodyPartModelName == "None")
	{
		assert( mBodyPart->mCanHaveNoBodyPartModel && "This BodyPart must have a BodyPartModel");
	}else{
        assert( (mBodyPart->mBodyPartModels[bodyPartModelName] != NULL) && "BodyPartModel not found !");
	}
	mCurrentBodyPartModelIterator = mBodyPart->mBodyPartModels.find(bodyPartModelName);

	if (mCurrentBodyPartModelIterator->second != NULL)
	{
        mCurrentBodyPartModelInstance = new BodyPartModelInstance(mCurrentBodyPartModelIterator->second, this);
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(true);
        mCurrentBodyPartModelInstance->setGhost(mGhost);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setPreviousBodyPartModelAsCurrent()
{
	if (mCurrentBodyPartModelIterator->second != NULL)
	{
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(false);
        delete mCurrentBodyPartModelInstance;
        mCurrentBodyPartModelInstance = 0;
	}

	if (mCurrentBodyPartModelIterator == mBodyPart->mBodyPartModels.begin()) mCurrentBodyPartModelIterator = mBodyPart->mBodyPartModels.end();
	mCurrentBodyPartModelIterator--;

	if (mCurrentBodyPartModelIterator->second != NULL)
	{
        mCurrentBodyPartModelInstance = new BodyPartModelInstance(mCurrentBodyPartModelIterator->second, this);
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(true);
        mCurrentBodyPartModelInstance->setGhost(mGhost);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setNextBodyPartModelAsCurrent()
{
	if (mCurrentBodyPartModelIterator->second != NULL)
	{
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(false);
        delete mCurrentBodyPartModelInstance;
        mCurrentBodyPartModelInstance = 0;
	}

	mCurrentBodyPartModelIterator++;
	if (mCurrentBodyPartModelIterator == mBodyPart->mBodyPartModels.end()) mCurrentBodyPartModelIterator = mBodyPart->mBodyPartModels.begin();

	if (mCurrentBodyPartModelIterator->second != NULL)
	{
        mCurrentBodyPartModelInstance = new BodyPartModelInstance(mCurrentBodyPartModelIterator->second, this);
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(true);
        mCurrentBodyPartModelInstance->setGhost(mGhost);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setDefaultBodyPartModelAsCurrent()
{
	if (mCurrentBodyPartModelIterator->second != NULL)
	{
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(false);
        delete mCurrentBodyPartModelInstance;
        mCurrentBodyPartModelInstance = 0;
	}

	mCurrentBodyPartModelIterator = mBodyPart->mDefaultBodyPartModelIterator;

	if (mCurrentBodyPartModelIterator->second != NULL)
	{
        mCurrentBodyPartModelInstance = new BodyPartModelInstance(mCurrentBodyPartModelIterator->second, this);
		SubEntity* bodyPartModelSubEntity = mCurrentBodyPartModelInstance->getSubEntity();
		bodyPartModelSubEntity->setVisible(true);
        mCurrentBodyPartModelInstance->setGhost(mGhost);
	}
}
#if 0 //GREG
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::resetCouplesOfPoses()
{
	CouplesOfPosesMapIterator couplesOfPosesMapIterator = getCouplesOfPosesMapIterator();
	while(couplesOfPosesMapIterator.hasMoreElements())
	{
		CoupleOfPoses* coupleOfPoses = couplesOfPosesMapIterator.getNext();
		coupleOfPoses->setPosition(0.5f);
	}
}
#endif //GREG
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::resetModifications()
{
    if (mCurrentBodyPartModelInstance != 0)
        mCurrentBodyPartModelInstance->resetModifications();

	setDefaultBodyPartModelAsCurrent();

#if 0 //GREG
	resetCouplesOfPoses();
#endif //GREG
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartInstance::setGhost(bool ghost)
{
    mGhost = ghost;
    if (mCurrentBodyPartModelInstance != 0)
        mCurrentBodyPartModelInstance->setGhost(ghost);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPart::BodyPart(const String& name, const String& defaultBodyPartModelSubEntityName, const String& defaultBodyPartModelCompleteName, Character* owner) :
mName(name), mOwner(owner), mCanHaveNoBodyPartModel(false)
{
	addBodyPartModel(defaultBodyPartModelSubEntityName,defaultBodyPartModelCompleteName);

	mDefaultBodyPartModelIterator = mBodyPartModels.begin();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPart::~BodyPart()
{
    BodyPartModelsMapIterator bodyPartModelsMapIterator(mBodyPartModels.begin(), mBodyPartModels.end());
    while (bodyPartModelsMapIterator.hasMoreElements())
    {
        BodyPartModel* bodyPartModel = bodyPartModelsMapIterator.getNext();
        delete bodyPartModel;
    }
    mBodyPartModels.clear();
    CouplesOfPosesMapIterator couplesOfPosesMapIterator(mCouplesOfPoses.begin(), mCouplesOfPoses.end());
    while (couplesOfPosesMapIterator.hasMoreElements())
    {
        CoupleOfPoses* coupleOfPoses = couplesOfPosesMapIterator.getNext();
        delete coupleOfPoses;
    }
    mCouplesOfPoses.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& BodyPart::getName()
{
	return mName;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
Character* BodyPart::getOwner()
{
	return mOwner;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool BodyPart::isBodyPartModelModifiable()
{
	size_t nbBodyPartModels;
	nbBodyPartModels = (mCanHaveNoBodyPartModel)? 1 : 0;
	nbBodyPartModels += mBodyPartModels.size();
	return (nbBodyPartModels > 1);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModel* BodyPart::getDefaultBodyPartModel()
{
	return mDefaultBodyPartModelIterator->second;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModel* BodyPart::getBodyPartModel(const String& name)
{
	BodyPartModel* bodyPartModel = mBodyPartModels[name];
	if ((bodyPartModel == NULL)&&(name != "None")) mBodyPartModels.erase(name);
	return bodyPartModel;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModelsMapIterator BodyPart::getBodyPartModelsMapIterator()
{
	return MapIterator<BodyPartModelsMap>(mBodyPartModels.begin(),mBodyPartModels.end());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setDefaultBodyPartModel(const String& bodyPartModelName)
{
	if (bodyPartModelName == "None")
	{
		assert( mCanHaveNoBodyPartModel && "This BodyPart must have a BodyPartModel");
	}else{
        assert( (getBodyPartModel(bodyPartModelName) != NULL) && "BodyPartModel not found !");
	}
	mDefaultBodyPartModelIterator = mBodyPartModels.find(bodyPartModelName);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::addBodyPartModel(const String& subEntityName,const String& name)
{
	if (name == "None")
	{
		mBodyPartModels["None"] = NULL;
		mCanHaveNoBodyPartModel = true;
	}
	else
	{
		SubEntity* subEntity = mOwner->getEntity()->getSubEntity(subEntityName);
		assert((subEntity != NULL)&&"SubEntity not found!");

		assert((getBodyPartModel(name) == NULL)&&"BodyPartModel already present in body part !");
		mBodyPartModels[name] = new BodyPartModel(name,subEntityName,subEntity,this);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
CoupleOfPoses* BodyPart::getCoupleOfPoses(const String& name)
{
	return mCouplesOfPoses[name];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
CouplesOfPosesMapIterator BodyPart::getCouplesOfPosesMapIterator()
{
	return MapIterator<CouplesOfPosesMap>(mCouplesOfPoses.begin(),mCouplesOfPoses.end());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::addCoupleOfPoses(const String& name, ushort leftPoseIndex, const Vector3& leftPoseCameraCylindricCoordinates, const String& leftStateName, ushort rightPoseIndex, const Vector3& rightPoseCameraCylindricCoordinates, const String& rightStateName)
{
	CoupleOfPoses* coupleOfPoses = new CoupleOfPoses(name,leftStateName,leftPoseIndex,rightStateName,rightPoseIndex,this);
	mCouplesOfPoses[name] = coupleOfPoses;

#if 0 //GREG
	mOwner->addPoseReference(leftPoseIndex,0);
	mOwner->addPoseReference(rightPoseIndex,0);
#endif //GREG

//GILLES	ScreenshotManager::getSingleton().addCoupleOfPosesToRenderToTexture(coupleOfPoses,leftPoseCameraCylindricCoordinates,rightPoseCameraCylindricCoordinates);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::addCoupleOfPoses(const String& name, const String& leftPoseName, const Vector3& leftPoseCameraCylindricCoordinates, const String& leftStateName, const String& rightPoseName, const Vector3& rightPoseCameraCylindricCoordinates, const String& rightStateName)
{
	Pose* leftPose = mOwner->getMesh()->getPose(leftPoseName);
	ushort leftPoseIndex = 0;
	while( (leftPoseIndex < mOwner->getMesh()->getPoseCount()) && (mOwner->getMesh()->getPose(leftPoseIndex) != leftPose) )
	{
		leftPoseIndex++;
	}
		
	Pose* rightPose = mOwner->getMesh()->getPose(rightPoseName);
	ushort rightPoseIndex = 0;
	while( (rightPoseIndex < mOwner->getMesh()->getPoseCount()) && (mOwner->getMesh()->getPose(rightPoseIndex) != rightPose) )
	{
		rightPoseIndex++;
	}

	addCoupleOfPoses(name, leftPoseIndex, leftPoseCameraCylindricCoordinates, leftStateName, rightPoseIndex, rightPoseCameraCylindricCoordinates, rightStateName);	
}
