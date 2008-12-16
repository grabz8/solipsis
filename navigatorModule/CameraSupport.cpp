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

#include "cameraSupport.h"

using namespace Solipsis;

CameraSupport::CameraSupport(std::string cameraSupportNodeName, int cameraSupportIndex, Ogre::SceneManager * pSceneMgr) :
mCameraSupportName(cameraSupportNodeName),
mCameraSupportIndex(cameraSupportIndex),
mCameraSupportMgr(0),
mSceneMgr(pSceneMgr)
{
    mCameraSupportSceneNode = pSceneMgr->createSceneNode(cameraSupportNodeName);
    mCameraSupportSceneNode->setOrientation( Quaternion::IDENTITY );
    mCameraSupportSceneNode->setPosition( Vector3::ZERO );

    std::string yawSupportNodeName("Yaw");
    yawSupportNodeName+=cameraSupportNodeName;
    mYawSupportSceneNode = mCameraSupportSceneNode->createChildSceneNode(yawSupportNodeName);
    mYawSupportSceneNode->setOrientation( Quaternion::IDENTITY );
    mYawSupportSceneNode->setPosition( Vector3::ZERO );

    std::string pitchSupportNodeName=("Pitch");
    pitchSupportNodeName+=cameraSupportNodeName;
    mPitchSupportSceneNode = mYawSupportSceneNode->createChildSceneNode(pitchSupportNodeName);
    mPitchSupportSceneNode->setOrientation( Quaternion::IDENTITY );
    mPitchSupportSceneNode->setPosition( Vector3::ZERO );
}

//-------------------------------------------------------------------------------------
CameraSupport::~CameraSupport()
{
    mCameraSupportSceneNode->removeAndDestroyAllChildren();
    mSceneMgr->destroySceneNode(mCameraSupportSceneNode->getName());
}

//-------------------------------------------------------------------------------------
void CameraSupport::setCameraSupportNodePosition (const Vector3 &position)
{
    if (mCameraSupportSceneNode != 0)
    {
        mCameraSupportSceneNode->setPosition(position);
    }
}

//-------------------------------------------------------------------------------------
void CameraSupport::setCameraSupportNodeOrientation (const Quaternion &orientation)
{
    if (mCameraSupportSceneNode != 0)
    {
           mCameraSupportSceneNode->setOrientation(orientation);
    }
}

//-------------------------------------------------------------------------------------
void CameraSupport::translateCameraSupport(const float pX, const float pY, const float pZ)
{
    if (mCameraSupportSceneNode != 0)
    {
        mCameraSupportSceneNode->translate(pX, pY, pZ);
    }
}

//-------------------------------------------------------------------------------------
void CameraSupport::yaw(const Ogre::Degree yaw)
{
    mYawSupportSceneNode->yaw(yaw);
}

//-------------------------------------------------------------------------------------
void CameraSupport::yaw(const Ogre::Radian yaw)
{
    mYawSupportSceneNode->yaw(yaw);
}

//-------------------------------------------------------------------------------------
Ogre::Radian CameraSupport::getYaw()
{
    return mYawSupportSceneNode->getOrientation().getYaw();
}

//-------------------------------------------------------------------------------------
void CameraSupport::pitch(const Ogre::Degree pitch)
{
    mPitchSupportSceneNode->pitch(pitch);
}

//-------------------------------------------------------------------------------------
void CameraSupport::pitch(const Ogre::Radian pitch)
{
    mPitchSupportSceneNode->pitch(pitch);
}

//-------------------------------------------------------------------------------------
Ogre::Radian CameraSupport::getPitch()
{
    return mPitchSupportSceneNode->getOrientation().getPitch();
}

//-------------------------------------------------------------------------------------
void CameraSupport::attachCameraSupportToNode (SceneNode* node) 
{ 
    detachCameraSupport(); 
    node->addChild(mCameraSupportSceneNode); 
}

//-------------------------------------------------------------------------------------
void CameraSupport::detachCameraSupport()
{
    Ogre::Node* parent=mCameraSupportSceneNode->getParent();
    if (parent!=0)
        parent->removeChild(mCameraSupportSceneNode);
}