#ifndef CCAMERA_h__
#define CCAMERA_h__

#include <Ogre.h>

using namespace Ogre;

class CCamera
{
public:
	CCamera(Ogre::Camera * pCamera, Ogre::SceneManager * pSceneMgr);
	~CCamera(void);

	void resetCamera ();	//reset the position and the orientation of the camera

	void translateCamera(const float pX, const float pY, const float pZ);

	void zoomCamera(const float pValue);

	void rotateCamera(const float pX, const float pY, const Vector3 pCentre) ;
		///param :  pX and pY in degree
		///			pCentre : centre of rotation

private:
	//Camera :
	SceneNode * mTargetCamNode;
	SceneNode * mEyeCamNode;
	Ogre::Camera * mCamera ;

	//ArcBall :
	SceneNode * mOrientationArcBallNode;
	SceneNode * mPositionArcBallNode;
	SceneNode * mEyeArcBallNode;
};

#endif //CCAMERA_h__