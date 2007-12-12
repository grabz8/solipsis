#include "camera.h"

namespace Solipsis {

CCamera::CCamera(Ogre::Camera * pCamera, Ogre::SceneManager * pSceneMgr)
{
	mCamera = pCamera ;

	//Create all nodes for manipulate the camera :
	//-----------------------------------------------------
    
		//For the target camera
	SceneNode * node = pSceneMgr->getRootSceneNode()->createChildSceneNode( "NodeCameraCenterInWorld" );
	mTargetCamNode = node->createChildSceneNode( "NodeCamTarget"); 

		//Create the pitch node
	mEyeCamNode = mTargetCamNode->createChildSceneNode( "NodeCamEye", Vector3( 0, 200, 600 )  );
	mEyeCamNode->attachObject( mCamera );
	mEyeCamNode->pitch( Degree (-10)) ;
//	mCamera->lookAt(mTargetCamNode->getWorldPosition());

		//For Arcball : 
	mPositionArcBallNode = node->createChildSceneNode("NodeArcBallPosition");
	mOrientationArcBallNode = mPositionArcBallNode->createChildSceneNode("NodeArcBallOrientation");
	mEyeArcBallNode = mOrientationArcBallNode->createChildSceneNode("NodeArcBallEye");
}
//-------------------------------------------------------------------------------------
CCamera::~CCamera(void)
{
}
//-------------------------------------------------------------------------------------
void CCamera::translateCamera(const float pX, const float pY, const float pZ)
{
	Vector3 mouvement (pX, pY, pZ) ;
	mTargetCamNode->translate(	mTargetCamNode->getOrientation() *
								mEyeCamNode->getOrientation() *	
								mouvement );
}
//-------------------------------------------------------------------------------------
void CCamera::zoomCamera(const float pValue)
{
	Vector3 direction (0, 0, pValue);
	mTargetCamNode->translate(	mTargetCamNode->getOrientation() *
								mEyeCamNode->getOrientation() *	
								direction );
}
//-------------------------------------------------------------------------------------
void CCamera::rotateCamera(const float pX, const float pY, const Vector3 pCentre)
{
	//Init all nodes psoitions and orientations :
	//---------------------------------------------
		//init orientation :
	mPositionArcBallNode->setOrientation( mPositionArcBallNode->getInitialOrientation() );
	mOrientationArcBallNode->setOrientation( mOrientationArcBallNode->getInitialOrientation() );
	mEyeArcBallNode->setOrientation( mEyeArcBallNode->getInitialOrientation() );

		//Init position :
	mOrientationArcBallNode->setPosition( mOrientationArcBallNode->getInitialPosition() );
	mEyeArcBallNode->setPosition( mEyeArcBallNode->getInitialPosition() );
	mPositionArcBallNode->setPosition( pCentre ) ;

	mOrientationArcBallNode->setOrientation( mEyeCamNode->getWorldOrientation() );	
	
		//Put nodeEye on the camera position
	Vector3 ObjectPosition = mEyeCamNode->getWorldPosition() - mPositionArcBallNode->getWorldPosition() ;
	mEyeArcBallNode->translate( ObjectPosition, Node::TS_WORLD);//->setPosition( ObjectPosition ) ;

	//... ArcBall is correctly positionned

	//Rotate mOrientationArcBallNode around the centre :
	mOrientationArcBallNode->pitch(Degree(pY));
	mOrientationArcBallNode->yaw(Degree(pX), Node::TS_PARENT);

	//Now we put the camera on mEyeArcBallNode ...
	mEyeCamNode->setPosition(mEyeArcBallNode->getWorldPosition() - mTargetCamNode->getWorldPosition() );

	//And the camera looks the scene :
	mEyeCamNode->pitch(Degree(pY));
	mEyeCamNode->yaw(Degree(pX ), Node::TS_PARENT);
}


//-------------------------------------------------------------------------------------
void CCamera::resetCamera ()
{
	mTargetCamNode->setOrientation( mTargetCamNode->getInitialOrientation() );
	mTargetCamNode->setPosition( mTargetCamNode->getInitialPosition() );

	mEyeCamNode->setOrientation( mEyeCamNode->getInitialOrientation() );
	mEyeCamNode->setPosition(  Vector3( 0, 200, 600 ) );
	mEyeCamNode->pitch( Degree (-10)) ;

//	mCamera->lookAt(mTargetCamNode->getWorldPosition());
}

} //namespace