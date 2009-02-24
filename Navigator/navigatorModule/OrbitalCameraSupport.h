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
		OrbitalCamera.h
	\brief
		Solipsis OrbitalCameraSupport
	\author
		Orange - Jerome Royan
*/

#ifndef ORBITALCAMERASUPPORT_h__
#define ORBITALCAMERASUPPORT_h__

#include "CameraSupport.h"

using namespace Ogre;
namespace Solipsis {

class OrbitalCameraSupport : public CameraSupport
{
public:
/*
 *	\brief  
 *		OrbitalCameraSupport constructor	
 *
 *		This class uses this hierarchy :
 *
 *					           SceneNode (Avatar, RootNode, etc)
 *							      |		
 *						          |			 
 *					      OrbitalCameraSupportNode	
 *						          |				
 *					              |					
 *				         YawOrbitalCameraSupportNode			
 *						          |					   
 *					              |                        
 *			            PitchOrbitalCameraSupportNode			
 *                                |
 *                                |
 *                       DistOrbitalCameraSupportNode
 *                                                  
 *                                              
 *
 *
 *  \param cameraNodeName = Name of the camera Node
 *	\param pSceneMgr = Ogre Scene manager
*/
    OrbitalCameraSupport(std::string cameraSupportNodeName, int cameraSupportIndex, Ogre::SceneManager * pSceneMgr);

/*
 *
 *	\brief Camera Support Destructor
 *
*/
	~OrbitalCameraSupport(void);

/*
 *
 *	\brief Get Camera Support mode (FirstPerson, Orbital, ...)
 *
*/
    inline virtual CameraSupportMode getMode() { return  CSMOrbital; };

/*
 *
 *  \brief Attach a Ogre::camera to the DistNode 
 *
*/
    void attachCameraToSupport (Ogre::Camera * camera);

/*
 *
 *  \brief Detach a Ogre::camera to the DistNode 
 *
*/
    void detachCameraFromSupport (Ogre::Camera * camera);

/*
 *
 *	\brief 
 *		Reset the initial position and orientation of the camera Support
 *
*/
	void resetCameraSupport ();

/*
 *
 *	\brief 
 *	
 *
*/
	void setDistanceFromTarget(const float pValue);

/*
 *
 *	\brief 
 *	
 *
*/
    void moveToTarget(const float pValue);

protected:

/*
 * 
 *	\brief Ogre Node for the Pitch Camera
 *
*/
	SceneNode * mDistSupportSceneNode;

};

} //namespace

#endif //ORBITALCAMERASUPPORT_h__
