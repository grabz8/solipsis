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
		FirstPersCameraSupport.h
	\brief
		Solipsis FPCameraSupport
	\author
		Orange - Jerome Royan
*/

#ifndef FPCAMERASUPPORT_h__
#define FPCAMERASUPPORT_h__

#include "cameraSupport.h"

using namespace Ogre;
namespace Solipsis {

class FPCameraSupport : public CameraSupport
{
public:
/*
 *	\brief  
 *		Camera constructor	
 *
 *		This class uses this hierarchy :
 *
 *					           SceneNode (Avatar, RootNode, etc)
 *							      |		
 *						          |			 
 *					        FPCameraSupportNode	
 *						          |				
 *					              |					
 *				            YawFPCameraSupportNode			
 *						          |					   
 *					              |                        
 *			                PitchFPCameraSupportNode			
 *                                              
 *
 *
 *  \param cameraNodeName = Name of the camera Node
 *	\param pSceneMgr = Ogre Scene manager
*/
    FPCameraSupport(std::string cameraSupportNodeName, int cameraSupportIndex, Ogre::SceneManager * pSceneMgr);

/*
 *
 *	\brief Camera Destructor
 *
*/
	~FPCameraSupport(void);

/*
 *
 *	\brief Get Camera mode (FirstPerson, Orbital, ...)
 *
*/
    inline virtual CameraSupportMode getMode() { return  CSMFirstPerson; };

/*
 *
 *  \brief Attach a Ogre::camera to the DistNode 
 *
*/
    virtual void attachCameraToSupport (Ogre::Camera * camera);

/*
 *
 *  \brief Detach a Ogre::camera to the DistNode 
 *
*/
    virtual void detachCameraFromSupport (Ogre::Camera * camera);

/*
 *
 *	\brief 
 *		Reset the initial position and orientation of the camera
 *
*/
	virtual void resetCameraSupport ();

/*
 *
 *	\brief 
 *		Make a zoom with the camera Support
 *		    - Translate the NodeDist Camera in case of an OrbitalCamera in z direction
 *          - Translate the NodeCamera in z direction
 *
 *	\param pValue = value representing the zoom
 *
*/
	virtual void zoom(const float pValue);

protected:

};

} //namespace

#endif //FPCAMERASUPPORT_h__
