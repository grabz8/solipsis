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

#include "Prerequisites.h"
#include "FirstPersonCameraSupport.h"

using namespace Solipsis;

FPCameraSupport::FPCameraSupport( std::string cameraSupportNodeName, int cameraSupportIndex, Ogre::SceneManager * pSceneMgr) : CameraSupport(cameraSupportNodeName, cameraSupportIndex, pSceneMgr)
{
}

//-------------------------------------------------------------------------------------
void FPCameraSupport::attachCameraToSupport (Ogre::Camera * camera)
{
    mPitchSupportSceneNode->attachObject(camera);
}

//-------------------------------------------------------------------------------------
void FPCameraSupport::detachCameraFromSupport (Ogre::Camera * camera)
{
    mPitchSupportSceneNode->detachObject(camera);
}

//-------------------------------------------------------------------------------------
void FPCameraSupport::resetCameraSupport ()
{
    mCameraSupportSceneNode->setOrientation (Quaternion::IDENTITY);
    mYawSupportSceneNode->setOrientation (Quaternion::IDENTITY);
    mPitchSupportSceneNode->setOrientation (Quaternion::IDENTITY);
 
    mCameraSupportSceneNode->setPosition ( Vector3::ZERO );
    mYawSupportSceneNode->setPosition (Vector3::ZERO);
    mPitchSupportSceneNode->setPosition (Vector3::ZERO);
}

//-------------------------------------------------------------------------------------
void FPCameraSupport::zoom(const float pValue)
{
    mCameraSupportSceneNode->translate (pValue, 0.0, 0.0);
}
