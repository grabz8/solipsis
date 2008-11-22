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

#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	mOrientationArcBallNode->setOrientation( mEyeCamNode->getWorldOrientation() );
	//Put nodeEye on the camera position
	Vector3 ObjectPosition = mEyeCamNode->getWorldPosition() - mPositionArcBallNode->getWorldPosition() ;
#else
	mOrientationArcBallNode->setOrientation( mEyeCamNode->_getDerivedOrientation() );
	//Put nodeEye on the camera position
	Vector3 ObjectPosition = mEyeCamNode->_getDerivedPosition() - mPositionArcBallNode->_getDerivedPosition() ;
#endif
	
	mEyeArcBallNode->translate( ObjectPosition, Node::TS_WORLD);//->setPosition( ObjectPosition ) ;

	//... ArcBall is correctly positionned

	//Rotate mOrientationArcBallNode around the centre :
	mOrientationArcBallNode->pitch(Degree(pY));
	mOrientationArcBallNode->yaw(Degree(pX), Node::TS_PARENT);

	//Now we put the camera on mEyeArcBallNode ...
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	mEyeCamNode->setPosition(mEyeArcBallNode->getWorldPosition() - mTargetCamNode->getWorldPosition() );
#else
	mEyeCamNode->setPosition(mEyeArcBallNode->_getDerivedPosition() - mTargetCamNode->_getDerivedPosition() );
#endif
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