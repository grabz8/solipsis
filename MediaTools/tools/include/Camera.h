/**
	\file 
		Camera.h
	\brief
		Solipsis Camera
	\author
		ARTEFACTO - Patrice Desfonds
*/

#ifndef CCAMERA_h__
#define CCAMERA_h__

#include <Ogre.h>

using namespace Ogre;

class CCamera
{
public:
/*
 *	\brief  
 *		Camera constructor	
 *
 *		This class uses this hierarchy :
 *
 *							mSceneMgr
 *								/	
 *							   /
 *					NodeCameraCentrerInWorld
 *							  /			\
 *						     /			 \
 *					NodeCameraTarget	NodeArcBallPosition
 *						   /				\
 *					      /					 \
 *					 NodeCamEye			NodeArcBallOrientation
 *											   \
 *												\
 *											NodeArcBalleEye
 *
 *
 *	\param pCamera = Ogre camera object
 *	\param pSceneMgr = Ogre Scene manager
*/
	CCamera(Ogre::Camera * pCamera, Ogre::SceneManager * pSceneMgr);

/*
 *
 *	\brief Camera Destructor
 *
*/
	~CCamera(void);

/*
 *
 *	\brief 
 *		Reset the initial position and orientation of the camera
 *
*/
	void resetCamera ();

/*
 *
 *	\brief 
 *		Make a translation on the camera
 *		Translate NodeCameraTarget by a vector which is relative to the Camera
 *
 *	\param pX, pY and pZ = Real x, y and z values representing the translation
 *
*/
	void translateCamera(const float pX, const float pY, const float pZ);

/*
 *
 *	\brief 
 *		Make a zoom with the camera
 *		Translate NodeCameraTarget on Z direction
 *
 *	\param pValue = value representing the zoom
 *
*/
	void zoomCamera(const float pValue);

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
	void rotateCamera(const float pX, const float pY, const Vector3 pCentre) ;

private:
/*
 * 
 *	\brief Ogre Node for the Camera target
 *
*/
	SceneNode * mTargetCamNode;

/*
 * 
 *	\brief Ogre Node for the Camera eye
 *
*/
	SceneNode * mEyeCamNode;

/*
 * 
 *	\brief Ogre Camera object
 *
*/
	Ogre::Camera * mCamera ;

/*
 * 
 *	\brief Ogre Nodes used for calculate the arcBall rotation
 *
*/
	SceneNode * mOrientationArcBallNode;
	SceneNode * mPositionArcBallNode;
	SceneNode * mEyeArcBallNode;
};

#endif //CCAMERA_h__