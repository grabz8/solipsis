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
//#include "ScreenshotManager.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPart::BodyPart(const String& name, const String& defaultBodyPartModelSubEntityName, const String& defaultBodyPartModelCompleteName, Character* owner) :
mName(name), mOwner(owner), mCanHaveNoBodyPartModel(false)
{
	addBodyPartModel(defaultBodyPartModelSubEntityName,defaultBodyPartModelCompleteName);

	mCurrentBodyPartModelIterator = mBodyPartModels.begin();
	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(true);

	mDefaultBodyPartModelIterator = mCurrentBodyPartModelIterator;
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
bool BodyPart::isColourModifiable()
{	
	return ((mCurrentBodyPartModelIterator->second != NULL)&&(mCurrentBodyPartModelIterator->second->isColourModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setColourModifiable(bool isColourModifiable)
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so can't change it's properties !");
    mCurrentBodyPartModelIterator->second->setColourModifiable(isColourModifiable);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& BodyPart::getColour()
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "There is no CurrentBodyPartModel for this BodyPart so this one can't have a colour !");
	return mCurrentBodyPartModelIterator->second->getColour();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setColour(const ColourValue&	colour)
{
	assert( (mCurrentBodyPartModelIterator->second  != NULL) && "There is no CurrentBodyPartModel for this BodyPart so this one can't have a colour !");
	mCurrentBodyPartModelIterator->second->setColour(colour);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool BodyPart::isTextureModifiable()
{
	return ((mCurrentBodyPartModelIterator->second  != NULL)&&(mCurrentBodyPartModelIterator->second ->isTextureModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr BodyPart::getCurrentTexture()
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so it can't have a current texture !");
	return mCurrentBodyPartModelIterator->second->getCurrentTexture();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setPreviousTextureAsCurrent()
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelIterator->second->setPreviousTextureAsCurrent();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setNextTextureAsCurrent()
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelIterator->second->setNextTextureAsCurrent();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::addTexture(const TexturePtr& texture)
{
	assert( (mCurrentBodyPartModelIterator->second != NULL) && "No current BodyPartModel, so can't change it's properties !");
	mCurrentBodyPartModelIterator->second->addTexture(texture);
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
BodyPartModel* BodyPart::getCurrentBodyPartModel()
{
	return mCurrentBodyPartModelIterator->second;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& BodyPart::getCurrentBodyPartModelName()
{
	return mCurrentBodyPartModelIterator->first;
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
void BodyPart::setCurrentBodyPartModel(const String& bodyPartModelName)
{
	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(false);

	if (bodyPartModelName == "None")
	{
		assert( mCanHaveNoBodyPartModel && "This BodyPart must have a BodyPartModel");
	}else{
        assert( (mBodyPartModels[bodyPartModelName] != NULL) && "BodyPartModel not found !");
	}
	mCurrentBodyPartModelIterator = mBodyPartModels.find(bodyPartModelName);

	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setPreviousBodyPartModelAsCurrent()
{
	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(false);

	if (mCurrentBodyPartModelIterator == mBodyPartModels.begin()) mCurrentBodyPartModelIterator = mBodyPartModels.end();
	mCurrentBodyPartModelIterator--;

	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setNextBodyPartModelAsCurrent()
{

	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(false);

	mCurrentBodyPartModelIterator++;
	if (mCurrentBodyPartModelIterator == mBodyPartModels.end()) mCurrentBodyPartModelIterator = mBodyPartModels.begin();

	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(true);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::setDefaultBodyPartModelAsCurrent()
{
	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(false);

	mCurrentBodyPartModelIterator = mDefaultBodyPartModelIterator;

	if (mCurrentBodyPartModelIterator->second != NULL) mCurrentBodyPartModelIterator->second->getSubEntity()->setVisible(true);
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
		mBodyPartModels[name] = new BodyPartModel(name,subEntity,this);
		subEntity->setVisible(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
CoupleOfPoses* BodyPart::getCoupleOfPoses(const String& name)
{
	return mCouplesOfPoses[name];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
CouplesOfPosesIterator BodyPart::getCouplesOfPosesIterator()
{
	return MapIterator<CouplesOfPosesMap>(mCouplesOfPoses.begin(),mCouplesOfPoses.end());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::addCoupleOfPoses(const String& name, ushort leftPoseIndex, const Vector3& leftPoseCameraCylindricCoordinates, const String& leftStateName, ushort rightPoseIndex, const Vector3& rightPoseCameraCylindricCoordinates, const String& rightStateName)
{
	CoupleOfPoses* coupleOfPoses = new CoupleOfPoses(name,leftStateName,leftPoseIndex,rightStateName,rightPoseIndex,this);
	mCouplesOfPoses[name] = coupleOfPoses;

	mOwner->addPoseReference(leftPoseIndex,0);
	mOwner->addPoseReference(rightPoseIndex,0);

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
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::resetCouplesOfPoses()
{
	CouplesOfPosesIterator couplesOfPosesIterator = getCouplesOfPosesIterator();
	while(couplesOfPosesIterator.hasMoreElements())
	{
		CoupleOfPoses* coupleOfPoses = couplesOfPosesIterator.getNext();
		coupleOfPoses->setPosition(0.5f);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPart::resetModifications()
{
	BodyPartModelsMapIterator bodyPartModelsMapIterator = getBodyPartModelsMapIterator();
	while(bodyPartModelsMapIterator.hasMoreElements())
	{
		BodyPartModel* bodyPartModel = bodyPartModelsMapIterator.getNext();
		if (bodyPartModel != NULL) bodyPartModel->resetModifications();
	}

	setDefaultBodyPartModelAsCurrent();

	resetCouplesOfPoses();
}

	

