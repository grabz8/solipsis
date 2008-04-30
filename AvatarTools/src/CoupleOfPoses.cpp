#include "Character.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
CoupleOfPoses::CoupleOfPoses(const String& name, const String& leftStateName, ushort leftPoseIndex, const String& rightStateName, ushort rightPoseIndex, BodyPart* owner) :
	mName(name),
	mPosition(0.5f),
	mLeftStateName(leftStateName), 
	mLeftPoseIndex(leftPoseIndex), 
	mRightStateName(rightStateName),
	mRightPoseIndex(rightPoseIndex),
	mOwner(owner)
{}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPart* CoupleOfPoses::getOwner()
{
    return mOwner;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& CoupleOfPoses::getName()
{
	return mName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float CoupleOfPoses::getPosition()
{
	return mPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& CoupleOfPoses::getLeftStateName()
{
	return mLeftStateName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ushort CoupleOfPoses::getLeftPoseIndex()
{
	return mLeftPoseIndex;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Image* CoupleOfPoses::getLeftPoseImageSet()
{
	return mLeftPoseImageSet;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& CoupleOfPoses::getRightStateName()
{
	return mRightStateName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ushort CoupleOfPoses::getRightPoseIndex()
{
	return mRightPoseIndex;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Image* CoupleOfPoses::getRightPoseImageSet()
{
	return mRightPoseImageSet;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
void CoupleOfPoses::setPosition(float position)
{
	assert((position >= 0.0f)&&(position <= 1.0f));
	mPosition = position;
	
	mOwner->getOwner()->updatePoseReference(mLeftPoseIndex,1.0f - mPosition);
	mOwner->getOwner()->updatePoseReference(mRightPoseIndex,mPosition);
}
	
//--------------------------------------------------------------------------------------------------------------------------------------------------
void CoupleOfPoses::setLeftPoseImageSet(Image* imageSet)
{
	mLeftPoseImageSet = imageSet;	
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void CoupleOfPoses::setRightPoseImageSet(Image* imageSet)
{	
	mRightPoseImageSet = imageSet;
}
