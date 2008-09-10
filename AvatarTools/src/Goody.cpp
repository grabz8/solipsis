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

#include "Goody.h"
#include "AvatarEditor.h"
#include "Character.h"
#include "CharacterInstance.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyInstance::GoodyInstance(Goody* pGoody, CharacterInstance* owner) :
	mGoody(pGoody),
	mCurrentRotationsAnglesScrollPositions(Vector3(0.5,0.5,0.5)),
	mCurrentPositionScrollPositions(Vector3(0.5,0.5,0.5)),
    mOwner(owner)
{
    mCurrentGoodyModelIterator = mGoody->mDefaultGoodyModelIterator;
    mCurrentGoodyModelInstance = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyInstance::~GoodyInstance()
{
    delete mCurrentGoodyModelInstance;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool GoodyInstance::isColourModifiable()
{	
	return ((mCurrentGoodyModelInstance != 0)&&(mCurrentGoodyModelInstance->isColourModifiable()));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setColourModifiable(bool isColourModifiable)
{
	assert( (mCurrentGoodyModelInstance != 0) && "No current GoodyModel, so can't change it's properties !");
    mCurrentGoodyModelInstance->setColourModifiable(isColourModifiable);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& GoodyInstance::getColour()
{
	assert( (mCurrentGoodyModelInstance != 0) && "There is no CurrentGoodyModel for this Goody so this one can't have a colour !");
	return mCurrentGoodyModelInstance->getColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setColour(const ColourValue&	colour)
{
	assert( (mCurrentGoodyModelInstance  != 0) && "There is no CurrentGoodyModel for this Goody so this one can't have a colour !");
	mCurrentGoodyModelInstance->setColour(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool GoodyInstance::isTextureModifiable()
{
	return ((mCurrentGoodyModelIterator->second != 0) && (mCurrentGoodyModelIterator->second->isTextureModifiable()));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr GoodyInstance::getCurrentTexture()
{
	assert( (mCurrentGoodyModelInstance != 0) && "No current GoodyModel, so it can't have a current texture !");
	return mCurrentGoodyModelInstance->getCurrentTexture();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setPreviousTextureAsCurrent()
{
	assert( (mCurrentGoodyModelInstance != 0) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelInstance->setPreviousTextureAsCurrent();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setNextTextureAsCurrent()
{
	assert( (mCurrentGoodyModelInstance != 0) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelInstance->setNextTextureAsCurrent();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::addTexture(const TexturePtr& texture)
{
	assert( (mCurrentGoodyModelInstance != 0) && "No current GoodyModel, so can't change it's properties !");
	mCurrentGoodyModelInstance->addTexture(texture);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::updateGoodyPosition()
{
	assert( (mCurrentGoodyModelIterator->first != "None") && "No current GoodyModel so can't update its position at the screen !");

	Entity* characterEntity = mOwner->getEntity();
	Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
	if (goodyModelEntity->isAttached()) characterEntity->detachObjectFromBone(goodyModelEntity);
	characterEntity->attachObjectToBone(mGoody->mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Vector3& GoodyInstance::getCurrentPosition()
{
	static Vector3 currentPosition;

	if (mCurrentPositionScrollPositions.x <= 0.5f)
		currentPosition.x = (mGoody->mDefaultPosition.x - mGoody->mMinPosition.x)*(mCurrentPositionScrollPositions.x/0.5f) + mGoody->mMinPosition.x;
	else
		currentPosition.x = (mGoody->mMaxPosition.x - mGoody->mDefaultPosition.x)*((mCurrentPositionScrollPositions.x - 0.5f)/0.5f) + mGoody->mDefaultPosition.x;

	if (mCurrentPositionScrollPositions.y <= 0.5f)
		currentPosition.y = (mGoody->mDefaultPosition.y - mGoody->mMinPosition.y)*(mCurrentPositionScrollPositions.y/0.5f) + mGoody->mMinPosition.y;
	else
		currentPosition.y = (mGoody->mMaxPosition.y - mGoody->mDefaultPosition.y)*((mCurrentPositionScrollPositions.y - 0.5f)/0.5f) + mGoody->mDefaultPosition.y;

	if (mCurrentPositionScrollPositions.z <= 0.5f)
		currentPosition.z = (mGoody->mDefaultPosition.z - mGoody->mMinPosition.z)*(mCurrentPositionScrollPositions.z/0.5f) + mGoody->mMinPosition.z;
	else
		currentPosition.z = (mGoody->mMaxPosition.z - mGoody->mDefaultPosition.z)*((mCurrentPositionScrollPositions.z - 0.5f)/0.5f) + mGoody->mDefaultPosition.z;

	return currentPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Vector3& GoodyInstance::getCurrentRotationsAngles()
{
	static Vector3 currentRotationsAngles;

	if (mCurrentRotationsAnglesScrollPositions.x <= 0.5f)
		currentRotationsAngles.x = (mGoody->mDefaultRotationsAngles.x - mGoody->mMinRotationsAngles.x)*(mCurrentRotationsAnglesScrollPositions.x/0.5f) + mGoody->mMinRotationsAngles.x;
	else
		currentRotationsAngles.x = (mGoody->mMaxRotationsAngles.x - mGoody->mDefaultRotationsAngles.x)*((mCurrentRotationsAnglesScrollPositions.x - 0.5f)/0.5f) + mGoody->mDefaultRotationsAngles.x;

	if (mCurrentRotationsAnglesScrollPositions.y <= 0.5f)
		currentRotationsAngles.y = (mGoody->mDefaultRotationsAngles.y - mGoody->mMinRotationsAngles.y)*(mCurrentRotationsAnglesScrollPositions.y/0.5f) + mGoody->mMinRotationsAngles.y;
	else
		currentRotationsAngles.y = (mGoody->mMaxRotationsAngles.y - mGoody->mDefaultRotationsAngles.y)*((mCurrentRotationsAnglesScrollPositions.y - 0.5f)/0.5f) + mGoody->mDefaultRotationsAngles.y;

	if (mCurrentRotationsAnglesScrollPositions.z <= 0.5f)
		currentRotationsAngles.z = (mGoody->mDefaultRotationsAngles.z - mGoody->mMinRotationsAngles.z)*(mCurrentRotationsAnglesScrollPositions.z/0.5f) + mGoody->mMinRotationsAngles.z;
	else
		currentRotationsAngles.z = (mGoody->mMaxPosition.z - mGoody->mDefaultRotationsAngles.z)*((mCurrentRotationsAnglesScrollPositions.z - 0.5f)/0.5f) + mGoody->mDefaultRotationsAngles.z;

	return currentRotationsAngles;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const Quaternion& GoodyInstance::getCurrentOrientation()
{
	const Vector3& currentRotationsAngles = getCurrentRotationsAngles();

	static Quaternion orientation;
	orientation = Quaternion(Radian(Degree(currentRotationsAngles.x)),Vector3(1,0,0)) *
		Quaternion(Radian(Degree(currentRotationsAngles.y)),Vector3(0,1,0)) *
		Quaternion(Radian(Degree(currentRotationsAngles.z)),Vector3(0,0,1));

	return orientation;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentXScrollPosition()
{
	return mCurrentPositionScrollPositions.x;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentYScrollPosition()
{
	return mCurrentPositionScrollPositions.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentZScrollPosition()
{
	return mCurrentPositionScrollPositions.z;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentYawAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.x;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentPitchAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float GoodyInstance::getCurrentRollAngleScrollPosition()
{
	return mCurrentRotationsAnglesScrollPositions.z;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentXScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.x = position;

	if (mCurrentGoodyModelIterator->first != "None")
        updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentYScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.y = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentZScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentPositionScrollPositions.z = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::resetCurrentPosition()
{
	setCurrentXScrollPosition(0.5f);
	setCurrentYScrollPosition(0.5f);
	setCurrentZScrollPosition(0.5f);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentYawAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.x = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentPitchAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.y = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentRollAngleScrollPosition(float position)
{
	assert((position >= 0) && (position <= 1));

	mCurrentRotationsAnglesScrollPositions.z = position;

	if (mCurrentGoodyModelIterator->first != "None")
		updateGoodyPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::resetCurrentRotation()
{
	setCurrentYawAngleScrollPosition(0.5f);
	setCurrentPitchAngleScrollPosition(0.5f);
	setCurrentRollAngleScrollPosition(0.5f);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel* GoodyInstance::getCurrentGoodyModel()
{
	return mCurrentGoodyModelIterator->second;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& GoodyInstance::getCurrentGoodyModelName()
{
	return mCurrentGoodyModelIterator->first;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setCurrentGoodyModel(const String& goodyModelName)
{
	Entity* characterEntity = mOwner->getEntity();

	if (mCurrentGoodyModelIterator->second != 0)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->detachObjectFromBone(goodyModelEntity);
        delete mCurrentGoodyModelInstance;
        mCurrentGoodyModelInstance = 0;
	}

	if (goodyModelName != "None")
	{
		assert( (mGoody->mGoodyModels[goodyModelName] != 0) && "GoodyModel not found !");
	}
	mCurrentGoodyModelIterator = mGoody->mGoodyModels.find(goodyModelName);

	if (mCurrentGoodyModelIterator->second != 0)
	{
        mCurrentGoodyModelInstance = new GoodyModelInstance(mCurrentGoodyModelIterator->second, this);
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->attachObjectToBone(mGoody->mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setPreviousGoodyModelAsCurrent()
{
	Entity* characterEntity = mOwner->getEntity();

	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->detachObjectFromBone(goodyModelEntity);
        delete mCurrentGoodyModelInstance;
        mCurrentGoodyModelInstance = 0;
	}

	//changing the current GoodModel
	if (mCurrentGoodyModelIterator == mGoody->mGoodyModels.begin()) mCurrentGoodyModelIterator = mGoody->mGoodyModels.end();
	mCurrentGoodyModelIterator--;

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
        mCurrentGoodyModelInstance = new GoodyModelInstance(mCurrentGoodyModelIterator->second, this);
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->attachObjectToBone(mGoody->mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setNextGoodyModelAsCurrent()
{
	Entity* characterEntity = mOwner->getEntity();

	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->detachObjectFromBone(goodyModelEntity);
        delete mCurrentGoodyModelInstance;
        mCurrentGoodyModelInstance = 0;
	}

	//changing the current GoodModel
	mCurrentGoodyModelIterator++;
	if (mCurrentGoodyModelIterator == mGoody->mGoodyModels.end()) mCurrentGoodyModelIterator = mGoody->mGoodyModels.begin();

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
        mCurrentGoodyModelInstance = new GoodyModelInstance(mCurrentGoodyModelIterator->second, this);
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->attachObjectToBone(mGoody->mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::setDefaultGoodyModelAsCurrent()
{
	Entity* characterEntity = mOwner->getEntity();

	//hiding the old current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->detachObjectFromBone(goodyModelEntity);
        delete mCurrentGoodyModelInstance;
        mCurrentGoodyModelInstance = 0;
	}

	//changing the current GoodModel
	mCurrentGoodyModelIterator = mGoody->mDefaultGoodyModelIterator;

	//displaying the new current GoodyModel
	if (mCurrentGoodyModelIterator->second != 0)
	{
        mCurrentGoodyModelInstance = new GoodyModelInstance(mCurrentGoodyModelIterator->second, this);
		Entity* goodyModelEntity = mCurrentGoodyModelInstance->getEntity();
		characterEntity->attachObjectToBone(mGoody->mBoneName,goodyModelEntity,getCurrentOrientation(),getCurrentPosition());
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void GoodyInstance::resetModifications()
{
    if (mCurrentGoodyModelInstance != 0)
        mCurrentGoodyModelInstance->resetModifications();

	setDefaultGoodyModelAsCurrent();

	resetCurrentPosition();	
	resetCurrentRotation();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Goody::Goody(const String& name,
			 const String& boneName,
			 const Vector3& minRotationsAngles, const Vector3& defaultRotationsAngles, const Vector3& maxRotationsAngles,
			 const Vector3& minPosition, const Vector3& defaultPosition, const Vector3& maxPosition,
			 Character* owner) :
	mName(name),
	mBoneName(boneName),
	mMinRotationsAngles(minRotationsAngles), mDefaultRotationsAngles(defaultRotationsAngles), mMaxRotationsAngles(maxRotationsAngles),
	mMinPosition(minPosition), mDefaultPosition(defaultPosition), mMaxPosition(maxPosition),
	mOwner(owner)
{
	mGoodyModels["None"] = 0;
	mDefaultGoodyModelIterator = mGoodyModels.begin();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Goody::~Goody()
{
    GoodyModelsMapIterator goodyModelsMapIterator(mGoodyModels.begin(), mGoodyModels.end());
    while (goodyModelsMapIterator.hasMoreElements())
    {
        GoodyModel* goodyModel = goodyModelsMapIterator.getNext();
        if (goodyModel == 0) continue;
        Entity* goodyModelEntity = goodyModel->getEntity();
//        MeshManager::getSingleton().remove(goodyModelEntity->getMesh());
        AvatarEditor::getSingletonPtr()->getSceneManager()->destroyEntity(goodyModelEntity);
        delete goodyModel;
    }
    mGoodyModels.clear();
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
bool Goody::isGoodyModelModifiable()
{
	return (mGoodyModels.size() > 1);
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
	if ((bodyPartModel == 0) && (name != "None")) mGoodyModels.erase(name);
	return bodyPartModel;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModelsMapIterator Goody::getGoodyModelsMapIterator()
{
	return MapIterator<GoodyModelsMap>(mGoodyModels.begin(),mGoodyModels.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::setDefaultGoodyModel(const String& goodyModelName)
{
	if (goodyModelName != "None")
	{
		assert( (getGoodyModel(goodyModelName) != 0) && "GoodyModel not found !");
	}
	mDefaultGoodyModelIterator = mGoodyModels.find(goodyModelName);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void Goody::addGoodyModel(const String& meshName,const String& name, SceneManager* sceneMgr)
{
	assert((getGoodyModel(name) == 0)&&"GoodyModel already present in body part !");

	Entity* goodyModelEntity = sceneMgr->createEntity(name,meshName);	
	mGoodyModels[name] = new GoodyModel(name, goodyModelEntity, this);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
