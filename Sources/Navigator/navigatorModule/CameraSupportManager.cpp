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
#include "CameraSupportManager.h"

namespace Solipsis {

    CameraSupportManager::CameraSupportManager(Ogre::Camera * pOgreCamera) :
mActiveCameraSupport(0),
mLastCameraSupport(0),
mOgreCamera(pOgreCamera)
{
}
//-------------------------------------------------------------------------------------
CameraSupportManager::~CameraSupportManager(void)
{
    suppressAllCameraSupports();
}

//-------------------------------------------------------------------------------------
void CameraSupportManager::addCameraSupport(CameraSupport* cameraSupport)
{
    mCameraSupports.insert(std::make_pair(cameraSupport->getIndex(),cameraSupport));
    cameraSupport->setCameraSupportManager(this);
}

//-------------------------------------------------------------------------------------
void CameraSupportManager::removeCameraSupport(CameraSupport* cameraSupport)
{
    mCameraSupports.erase(cameraSupport->getIndex());
    cameraSupport->setCameraSupportManager(0);
    if (cameraSupport==mLastCameraSupport)
        mLastCameraSupport=0;
}

//-------------------------------------------------------------------------------------
 void CameraSupportManager::suppressAllCameraSupports ()
{
    for (std::map<int,CameraSupport*>::iterator it = mCameraSupports.begin(); it != mCameraSupports.end(); it++)
    {
        delete (it->second);
    }
    mCameraSupports.clear();
    mActiveCameraSupport = 0;
    mLastCameraSupport = 0;
}

//-------------------------------------------------------------------------------------
void CameraSupportManager::resetCameraSupports()
{
    for (std::map<int,CameraSupport*>::iterator it = mCameraSupports.begin(); it != mCameraSupports.end(); it++)
    {
        it->second->resetCameraSupport();
    }

}

//-------------------------------------------------------------------------------------
void CameraSupportManager::attachAllCameraSupportsToNode(Ogre::SceneNode* node)
{
    for (std::map<int,CameraSupport*>::iterator it = mCameraSupports.begin(); it != mCameraSupports.end(); it++)
    {
        it->second->attachCameraSupportToNode(node);
    }
}

//-------------------------------------------------------------------------------------
CameraSupport* CameraSupportManager::getCameraSupport(int cameraSupportIndex)
{
    std::map<int,CameraSupport*>::iterator it = mCameraSupports.find(cameraSupportIndex);
    if (it == mCameraSupports.end())
        return 0;
    else
        return (it->second);
}

bool CameraSupportManager::activeCameraSupport (int cameraSupportIndex)
{
    CameraSupport * tempCameraSupport=getCameraSupport(cameraSupportIndex);
    if (tempCameraSupport==0)
        return false;
    mLastCameraSupport=mActiveCameraSupport;
    mActiveCameraSupport=tempCameraSupport;
    if (mLastCameraSupport!=0)
        mLastCameraSupport->detachCameraFromSupport(mOgreCamera);
    mActiveCameraSupport->attachCameraToSupport(mOgreCamera);
    return true;

}   

bool CameraSupportManager::activeLastCameraSupport ()
{
    if (mLastCameraSupport==0)
        return false;
    CameraSupport* tempCameraSupport=mActiveCameraSupport;
    mActiveCameraSupport=mLastCameraSupport;
    mLastCameraSupport=tempCameraSupport;
    mLastCameraSupport->detachCameraFromSupport(mOgreCamera);
    mActiveCameraSupport->attachCameraToSupport(mOgreCamera);
    return true;
}

}//namespace