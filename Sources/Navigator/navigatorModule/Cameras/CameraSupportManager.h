/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - Patrice Desfonds

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

/**
	\file 
		CameraSupportManager.h
	\brief
		Solipsis CameraSupportMgr
	\author
		Orange Labs - Jerome Royan
*/

#ifndef CAMERASUPPORTMGR_h__
#define CAMERASUPPORTMGR_h__

#include "CameraSupport.h"


using namespace Ogre;
namespace Solipsis {

class CameraSupportManager
{
public:
/*
 *	\brief  
 *		Camera constructor	
 *
 *		This class uses this hierarchy :
 *
 *							mSceneMgr
 *								|	
 *							    |
 *					    CameraSupportManagerNode
 *
 *
 *	\param pSceneMgr = Ogre Scene manager
*/
	CameraSupportManager( Ogre::Camera * pOgreCamera);

/*
 *
 *	\brief Camera support Manager Destructor
 *
*/
	~CameraSupportManager(void);

/*
 *
 *	\brief Add a camera support to the CameraSupportManager
 *
*/
    void addCameraSupport(CameraSupport* camera);

/*
 *
 *	\brief Remove a camera  support from the CameraSupportManager
 *
*/
    void removeCameraSupport(CameraSupport* camera);

/*
 *
 *	\brief Reset the orientation and translation of all camera  supports
 *
*/
    void resetCameraSupports();    
    
/*
 *
 *	\brief Attach all defined camera according to the node given in argument (for example a avatar)
 *
*/
    void attachAllCameraSupportsToNode(Ogre::SceneNode* node);

/*
 *
 *	\brief active the camera support thanks to its index, and attach Ogre::Camera to the leaf node
 *
*/
    bool activeCameraSupport (int cameraIndex);

/*
 *
 *	\brief active the last camera, and attach Ogre::Camera to the leaf node
 *
*/
    bool activeLastCameraSupport ( );

/*
 *
 *	\brief Remove a camera support from the CameraManager
 *
 *      Warning, delete the corresponding CameraNode* from the scene graph, as well as the Ogre::Camera instance
 *
*/
    void removeCameraSupport(int cameraIndex);

/*
 *
 *	\brief Get the camera support thanks to its index
 *
*/
    CameraSupport* getCameraSupport(int cameraIndex);

/*
 *
 *	\brief return the name of the active camera support
 *
*/    
    inline std::string getActiveCameraSupportName () { return (mActiveCameraSupport != 0 ?  mActiveCameraSupport->getName() : std::string("")); };

/*
 *
 *	\brief return the index of the active camera support
 *
*/    
    inline int getActiveCameraSupportIndex () { return (mActiveCameraSupport != 0 ?  mActiveCameraSupport->getIndex() : -1); };

/*
 *
 *	\brief return the index of the last camera support
 *
*/    
    inline int getLastCameraSupportIndex () { return (mLastCameraSupport != 0 ?  mLastCameraSupport->getIndex() : -1); };


/*
 *
 *	\brief return the active camera support
 *
*/
    inline CameraSupport* getActiveCameraSupport() { return mActiveCameraSupport; };

/*
 *
 *	\brief remove and delete all camera supports nodes
 *
*/
    void suppressAllCameraSupports ();

private:
/*
 * 
 *	\brief Camera supports present in the scene
 *
*/
    std::map<int,CameraSupport*> mCameraSupports;

    CameraSupport* mActiveCameraSupport;

    CameraSupport* mLastCameraSupport;

    Ogre::Camera * mOgreCamera;

};

} //namespace

#endif //CAMERASUPPORTMGR_h__

