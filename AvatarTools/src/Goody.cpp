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

#include "Character.h"
#include "Goody.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
Goody::Goody(const String& name,
			 const String& boneName,
			 const Vector3& minRotationsAngles, const Vector3& defaultRotationsAngles, const Vector3& maxRotationsAngles,
			 const Vector3& minPosition, const Vector3& defaultPosition, const Vector3& maxPosition,
			 Character* owner) :
	mName(name),
	mBoneName(boneName),
	mMinRotationsAngles(minRotationsAngles), mCurrentRotationsAnglesScrollPositions(Vector3(0.5,0.5,0.5)), mDefaultRotationsAngles(defaultRotationsAngles), mMaxRotationsAngles(maxRotationsAngles),
	mMinPosition(minPosition), mCurrentPositionScrollPositions(Vector3(0.5,0.5,0.5)), mDefaultPosition(defaultPosition), mMaxPosition(maxPosition),
	mOwner(owner)
{
	mGoodyModels["None"] = NULL;
	mCurrentGoodyModelIterator = mGoodyModels.begin();
	mDefaultGoodyModelIterator = mCurrentGoodyModelIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& Goody::getName()
{
	return mName;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
Character* Goody::getOwner()
{
	return mOwner;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& Goody::getBoneName()
{
	return mBoneName;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool Goody::isColourModifiable()
{	
	return ((mCurrentGoodyModelIterator->second != NULL)&&(mCurrentGoodyModelIterator->second->isColourModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setColourModifiable(bool isColourModifiable)
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "No current GoodyModel, so can't change it's properties !");
    mCurrentGoodyModelIterator->second->setColourModifiable(isColourModifiable);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& Goody::getColour()
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "There is no CurrentGoodyModel for this Goody so this one can't have a colour !");
	return mCurrentGoodyModelIterator->second->getColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setColour(const ColourValue&	colour)
{
	assert( (mCurrentGoodyModelIterator->second  != NULL) && "There is no CurrentGoodyModel for this Goody so this one can't have a colour !");
	mCurrentGoodyModelIterator->second->setColour(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool Goody::isTextureModifiable()
{
	return ((mCurrentGoodyModelIterator->second  != NULL)&&(mCurrentGoodyModelIterator->second ->isTextureModifiable()));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr Goody::getCurrentTexture()
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "No current GoodyModel, so it can't have a current texture !");
	return mCurrentGoodyModelIterator->second->getCurrentTexture();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setPreviousTextureAsCurrent()
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelIterator->second->setPreviousTextureAsCurrent();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setNextTextureAsCurrent()
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelIterator->second->setNextTextureAsCurrent();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::addTexture(const TexturePtr& texture)
{
	assert( (mCurrentGoodyModelIterator->second != NULL) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelIterator->second->addTexture(texture);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::updateGoodyPosition()
{
	assert( (mCurrentGoodyModelIterator->first != "None") && "No current GoodyModel so can't update its position at the screen !");

	Entity* avatarEntity = mOwner->getEntity();
	Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
	if (goodyModelEntity->isAttached()) avatarEntity->detachObjectFromBone(goodyModelEntity);
	avatarEntity->attachObjectToBone(mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Vector3& Goody::getCurrentPosition()
{
	static Vector3 currentPosition;

	if (mCurrentPositionScrollPositions.x <= 0.5f)
		currentPosition.x = (mDefaultPosition.x - mMinPosition.x)*(mCurrentPositionScrollPositions.x/0.5f) + mMinPosition.x;
	else
		currentPosition.x = (mMaxPosition.x - mDefaultPosition.x)*((mCurrentPositionScrollPositions.x - 0.5f)/0.5f) + mDefaultPosition.x;

	if (mCurrentPositionScrollPositions.y <= 0.5f)
		currentPosition.y = (mDefaultPosition.y - mMinPosition.y)*(mCurrentPositionScrollPositions.y/0.5f) + mMinPosition.y;
	else
		currentPosition.y = (mMaxPosition.y - mDefaultPosition.y)*((mCurrentPositionScrollPositions.y - 0.5f)/0.5f) + mDefaultPosition.y;

	if (mCurrentPositionScrollPositions.z <= 0.5f)
		currentPosition.z = (mDefaultPosition.z - mMinPosition.z)*(mCurrentPositionScrollPositions.z/0.5f) + mMinPosition.z;
	else
		currentPosition.z = (mMaxPosition.z - mDefaultPosition.z)*((mCurrentPositionScrollPositions.z - 0.5f)/0.5f) + mDefaultPosition.z;

	return currentPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Vector3& Goody::getCurrentRotationsAngles()
{
	static Vector3 currentRotationsAngles;

	if (mCurrentRotationsAnglesScrollPositions.x <= 0.5f)
		currentRotationsAngles.x = (mDefaultRotationsAngles.x - mMinRotationsAngles.x)*(mCurrentRotationsAnglesScrollPositions.x/0.5f) + mMinRotationsAngles.x;
	else
		currentRotationsAngles.x = (mMaxRotationsAngles.x - mDefaultRotationsAngles.x)*((mCurrentRotationsAnglesScrollPositions.x - 0.5f)/0.5f) + mDefaultRotationsAngles.x;

	if (mCurrentRotationsAnglesScrollPositions.y <= 0.5f)
		currentRotationsAngles.y = (mDefaultRotationsAngles.y - mMinRotationsAngles.y)*(mCurrentRotationsAnglesScrollPositions.y/0.5f) + mMinRotationsAngles.y;
	else
		currentRotationsAngles.y = (mMaxRotationsAngles.y - mDefaultRotationsAngles.y)*((mCurrentRotationsAnglesScrollPositions.y - 0.5f)/0.5f) + mDefaultRotationsAngles.y;

	if (mCurrentRotationsAnglesScrollPositions.z <= 0.5f)
		currentRotationsAngles.z = (mDefaultRotationsAngles.z - mMinRotationsAngles.z)*(mCurrentRotationsAnglesScrollPositions.z/0.5f) + mMinRotationsAngles.z;
	else
		currentRotationsAngles.z = (mMaxPosition.z - mDefaultRotationsAngles.z)*((mCurrentRotationsAnglesScrollPositions.z - 0.5f)/0.5f) + mDefaultRotationsAngles.z;

	return currentRotationsAngles;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Quaternion& Goody::getCurrentOrientation()
{
	const Vector3& currentRotationsAngles = getCurrentRotationsAngles();

	static Quaternion orientation;
	orientation = Quaternion(Radian(Degree(currentRotationsAngles.x)),Vector3(1,0,0)) *
		Quaternion(Radian(Degree(currentRotationsAngles.y)),Vector3(0,1,0)) *
		Quaternion(Radian(Degree(currentRotationsAngles.z)),Vector3(0,0,1));

	return orientation;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentXScrollPosition()
{
	return mCurrentPositionScrollPositions.x;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentYScrollPosition()
{
	return mCurrentPositionScrollPositions.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentZScrollPosition()
{
	return mCurrentPositionScrollPositions.z;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentYawAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.x;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentPitchAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float Goody::getCurrentRollAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.z;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentXScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.x = position;

	if (mCurrentGoodyModelIterator->first != "None")
        updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentYScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.y = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentZScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.z = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::resetCurrentPosition()
{
	setCurrentXScrollPosition(0.5f);
	setCurrentYScrollPosition(0.5f);
	setCurrentZScrollPosition(0.5f);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentYawAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.x = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentPitchAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.y = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentRollAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.z = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::resetCurrentRotation()
{
	setCurrentYawAngleScrollPosition(0.5f);
	setCurrentPitchAngleScrollPosition(0.5f);
	setCurrentRollAngleScrollPosition(0.5f);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool Goody::isGoodyModelModifiable()
{
	return (mGoodyModels.size() > 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel* Goody::getCurrentGoodyModel()
{
	return mCurrentGoodyModelIterator->second;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& Goody::getCurrentGoodyModelName()
{
	return mCurrentGoodyModelIterator->first;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel* Goody::getDefaultGoodyModel()
{
	return mDefaultGoodyModelIterator->second;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel* Goody::getGoodyModel(const String& name)
{
	GoodyModel* bodyPartModel = mGoodyModels[name];
	if ((bodyPartModel == NULL)&&(name != "None")) mGoodyModels.erase(name);
	return bodyPartModel;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModelsMapIterator Goody::getGoodyModelsMapIterator()
{
	return MapIterator<GoodyModelsMap>(mGoodyModels.begin(),mGoodyModels.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setCurrentGoodyModel(const String& goodyModelName)
{
	Entity* avatarEntity = mOwner->getEntity();

	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->detachObjectFromBone(goodyModelEntity);
	}

	if (goodyModelName != "None")
	{
		assert( (mGoodyModels[goodyModelName] != NULL) && "GoodyModel not found !");
	}
	mCurrentGoodyModelIterator = mGoodyModels.find(goodyModelName);

	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->attachObjectToBone(mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setPreviousGoodyModelAsCurrent()
{
	Entity* avatarEntity = mOwner->getEntity();

	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->detachObjectFromBone(goodyModelEntity);
	}

	//changing the current GoodModel
	if (mCurrentGoodyModelIterator == mGoodyModels.begin()) mCurrentGoodyModelIterator = mGoodyModels.end();
	mCurrentGoodyModelIterator--;

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->attachObjectToBone(mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setNextGoodyModelAsCurrent()
{
	Entity* avatarEntity = mOwner->getEntity();


	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->detachObjectFromBone(goodyModelEntity);
	}

	//changing the current GoodModel
	mCurrentGoodyModelIterator++;
	if (mCurrentGoodyModelIterator == mGoodyModels.end()) mCurrentGoodyModelIterator = mGoodyModels.begin();

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->attachObjectToBone(mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setDefaultGoodyModelAsCurrent()
{
	Entity* avatarEntity = mOwner->getEntity();

	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->detachObjectFromBone(goodyModelEntity);
	}

	//changing the current GoodModel
	mCurrentGoodyModelIterator = mDefaultGoodyModelIterator;

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != NULL)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelIterator->second->getEntity();
		avatarEntity->attachObjectToBone(mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setDefaultGoodyModel(const String& goodyModelName)
{
	if (goodyModelName != "None")
	{
		assert( (getGoodyModel(goodyModelName) != NULL) && "GoodyModel not found !");
	}
	mDefaultGoodyModelIterator = mGoodyModels.find(goodyModelName);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::addGoodyModel(const String& meshName,const String& name, SceneManager* sceneMgr)
{
	assert((getGoodyModel(name) == NULL)&&"GoodyModel already present in body part !");

	Entity* entity = sceneMgr->createEntity(name,meshName);	
	mGoodyModels[name] = new GoodyModel(name,entity,this);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::resetModifications()
{
	GoodyModelsMapIterator goodyModelsMapIterator = getGoodyModelsMapIterator();
	while(goodyModelsMapIterator.hasMoreElements())
	{
		GoodyModel* goodyModel = goodyModelsMapIterator.getNext();
		if (goodyModel != NULL) goodyModel->resetModifications();
	}

	setDefaultGoodyModelAsCurrent();

	resetCurrentPosition();	
	resetCurrentRotation();
}
