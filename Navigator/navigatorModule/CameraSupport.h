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
		CameraSupport.h
	\brief
		Solipsis Camera Support
	\author
		Orange - Jerome Royan
*/

#ifndef CAMERASUPPORT_h__
#define CAMERASUPPORT_h__

#include <Ogre.h>


using namespace Ogre;
namespace Solipsis {

class CameraSupportManager;

class CameraSupport
{
public:
 enum CameraSupportMode {
        CSMFirstPerson,   
        CSMOrbital     
    };

/*
 *	\brief  
 *		CameraSupport constructor	
 *
 *		This class uses this hierarchy :
 *
 *                            ObjectNode
 *							  /			\
 *						     /			 \
 *					 FPCameraSupportNode	OrbitalCameraSupportNode
 *						   /				\
 *					      /					 \
 *				YawFPCameraSupportNode			YawOrbitalCameraSupportNode
 *						/					   \
 *					   /                        \
 *			PitchFPCameraSupportNode			PitchOrbitalCameraSupportNode
 *                                                 \
 *                                                  \
 *                                               DistOribtalCameraSupportNode
 *
 *
 *  \param cameraNodeName = Name of the camera Node ( 
 *	\param pSceneMgr = Ogre Scene manager
*/
    CameraSupport( std::string cameraNodeName, int cameraSupportIndex, Ogre::SceneManager * pSceneMgr );

/*
 *
 *	\brief Camera Destructor
 *
*/
	~CameraSupport();


/*
 *
 *	\brief Get CameraSupport Name
 *
*/
    inline std::string getName(void) { return mCameraSupportName; };

/*
 *
 *	\brief Get CameraSupport mode (FirstPerson, Orbital, ...)
 *
*/
    inline int getIndex(void) { return mCameraSupportIndex; };


/*
 *
 *	\brief Get CameraSupport mode (FirstPerson, Orbital, ...)
 *
*/
	virtual CameraSupportMode getMode() = 0;

/*
 *
 *	\brief attach the camera Node to a node of the scene (RootNode, Avatar Node to follow him, etc)
 *
*/
    void attachCameraSupportToNode (SceneNode* node);

/*
 *
 *	\brief detach the cameraSupportNode from a node of the scene (RootNode, Avatar Node to follow him, etc)
 *
*/
    void detachCameraSupport();

/*
 *
 *	\brief return true if the camera support is attached to a node of the scene graph, false otherwise 
 *
*/
    inline bool isCameraSupportAttachedToNode (void) { return (mCameraSupportSceneNode->getParentSceneNode()!=0); };

/*
 *
 *  \brief Attach a Ogre::camera to the Support (DistNode for orbitalCamera, PitchNode for FPCamera, etc)
 *
*/
    virtual void attachCameraToSupport (Ogre::Camera * camera) = 0;

/*
 *
 *  \brief Detach a Ogre::camera to the leaf node (DistNode for orbitalCamera, PitchNode for FPCamera, etc)
 *
*/
    virtual void detachCameraFromSupport (Ogre::Camera * camera) = 0;
 

/*
 *
 *	\brief Get the root node of the Camera Support (FirstPerson, Orbital, ...)
 *
*/
    inline SceneNode* getCameraSupportNode (void) { return mCameraSupportSceneNode ;};

/*
 *
 *	\brief Set the camera support manager with which this camera is attach
 *
*/
    inline void setCameraSupportManager (CameraSupportManager* cameraSupportMgr) { mCameraSupportMgr = cameraSupportMgr; };

/*
 *
 *	\brief Get the camera Support manager with which this camera is attach
 *
*/
    inline CameraSupportManager* getCameraSupportManager(void) { return mCameraSupportMgr; };

/*
 *
 *	\brief 
 *		Reset the initial position and orientation of the camera Support
 *
*/
	virtual void resetCameraSupport () = 0 ;

/*
 *
 *	\brief Set the Camera Node Position
 *
 *  \param position = 3D Vector that corresponds to the position of the root Camera Node (camera position in case of a FirstPersonCamera, target position in case of a OrbitalCamera)
 * 
*/
    void setCameraSupportNodePosition (const Vector3 &position);

/*
 *
 *	\brief Set the Camera Support Node Orientation
 *
 *  \param orientation = Quaternion that corresponds to the orientation of the root of the Camera Support (camera orientation in case of a FirstPersonCamera, target orientation in case of a OrbitalCamera)
 *
*/
    void setCameraSupportNodeOrientation (const Quaternion &orientation);

/*
 *
 *	\brief 
 *		Make a translation on the camera Support Node (Camera in case of FPCam, Target in case of OrbitalCame)
 *		    -Translate the camera in case of a FPCameraSupport
 *          -Translate the Target (on which the cameraSupport Node is attached) in the case of a OrbitalCamera 
 *
 *	\param pX, pY and pZ = Real x, y and z values representing the translation
 *
*/
    void translateCameraSupport(const float pX, const float pY, const float pZ);

/*
 *
 *	\brief 
 *		Make a Yaw Rotation of the Support camera 
 *		    -Yaw according the world in case of a FPCameraSupport
 *          -Yaw rotation according to the target orientation in the case of a OrbitalCameraSupport 
 *
 *	\param yaw = Yaw angle in degrees representing the rotation
 *
*/
    virtual void yaw(const Ogre::Degree) ;

/*
 *
 *	\brief 
 *		Make a Yaw Rotation of the camera support 
 *		    -Yaw according the world in case of a FPCameraSupport
 *          -Yaw rotation according to the target orientation in the case of a OrbitalCameraSupport 
 *
 *	\param yaw = Yaw angle in radians representing the rotation
 *
*/
    virtual void yaw(const Ogre::Radian) ;

/*
 *
 *	\brief 
 *		Return the Yaw Rotation in radian of the camera support 
 *
  *
*/
    virtual Ogre::Radian getYaw() ;

/*
 *
 *	\brief 
 *		Make a Pitch Rotation of the camera support
 *		    -Pitch according the world in case of a FPCameraSupport
 *          -Pitch rotation according to the target orientation in the case of a OrbitalCameraSupport 
 *
 *	\param pitch = Pitch angle in degrees representing the rotation
 *
*/
	virtual void pitch(const Ogre::Degree);

/*
 *
 *	\brief 
 *		Make a Pitch Rotation of the camera support
 *		    -Pitch according the world in case of a FPCameraSupport
 *          -Pitch rotation according to the target orientation in the case of a OrbitalCameraSupport 
 *
 *	\param pitch = Pitch angle in degrees representing the rotation
 *
*/
	virtual void pitch(const Ogre::Radian);

/*
 *
 *	\brief 
 *		Return the Pitch Rotation in radian of the camera support 
 *
  *
*/
    virtual Ogre::Radian getPitch() ;

/*
 *
 *	\brief 
 *		Make an arcBall rotation around a 3D point.
 *		Translate the Camera and set it in the good orientation. It uses NodeArcBallPosition, 
 *			NodeArcBallOrientation and NodeArcBalleEye to calculate the correct position of 
 *			the camera, and then it make yaw and pictch on NodeCamEye. The pitch is relative to 
 *			the Camera, and the yaw is relative to the world.
 *			
 *	\param pX and pY = value of the rotation (in degree)
 *	\param pCentre = centre of rotation
 *
*/
//	void rotateCamera(const float pX, const float pY, const Vector3 pCentre) ;

protected:
    
/*
 * 
 *	\brief Ogre Node for the Camera Support
 *
*/
	SceneNode * mCameraSupportSceneNode;

/*
 * 
 *	\brief Name of the Camera Support
 *
*/
    std::string mCameraSupportName;

/*
 * 
 *	\brief Index of the Camera Support
 *
*/
    int mCameraSupportIndex;

/*
 * 
 *	\brief The Camera Support manager on which the camera is attached
 *
*/
	CameraSupportManager * mCameraSupportMgr;

/*
 * 
 *	\brief The Scene manager 
 *
*/
    SceneManager* mSceneMgr;

/*
 * 
 *	\brief Ogre Node for the Yaw Camera Support
 *
*/
	SceneNode * mYawSupportSceneNode;

/*
 * 
 *	\brief Ogre Node for the Pitch Camera Support
 *
*/
	SceneNode * mPitchSupportSceneNode;

};

} //namespace

#endif //CAMERASUPPORT_h__

