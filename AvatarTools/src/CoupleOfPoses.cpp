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
