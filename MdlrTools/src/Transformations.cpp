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

#include "transformations.h"

namespace Solipsis {

Transformations::Transformations(void)
{
	mMode = SELECT ;
	mOldpos = Vector3::ZERO ;

	mAxeClicked = AxeClicked::NONE ;
	mPlaneClicked = AxeClicked::NONE ;
}

//-------------------------------------------------------------------------------------
Transformations::~Transformations(void)
{
}

//-------------------------------------------------------------------------------------
void Transformations::firstClickForTransformation( const OIS::MouseEvent &e)
{
	RaySceneQueryResult &result = raySceneQuery(e);
	firstClickForTransformation(result);
}

void Transformations::firstClickForTransformation( RaySceneQueryResult &result )
{
	String NameAxeX ;
	String NameAxeY ;
	String NameAxeZ ;

	switch( mMode )
	{
		case Mode::MOVE :
			NameAxeX = "moveX" ;
			NameAxeY = "moveY" ;
			NameAxeZ = "moveZ" ;
			break;
		case ROTATE :
			NameAxeX = "rotateX" ;
			NameAxeY = "rotateY" ;
			NameAxeZ = "rotateZ" ;
			break;
		case Mode::SCALE :
			NameAxeX = "scaleX" ;
			NameAxeY = "scaleY" ;
			NameAxeZ = "scaleZ" ;
			break;
	}
	onClickToTransformObject(result,NameAxeX, NameAxeY, NameAxeZ);
	mOldpos = Vector3::ZERO ;//Update mOldpos

}
//-------------------------------------------------------------------------------------
void Transformations::releasedClickForTransformation ()
{
	SceneNode* node;
	switch( mMode )
	{
		case Mode::MOVE :
		{
			switch (mAxeClicked)
			{
				case AxeClicked::X :
					node = mPlaneX->getParentSceneNode() ;
					mNode_X->detachObject(mPlaneZ);
					mNode_X->detachObject(mPlaneY);
					break ;
				case AxeClicked::Y :
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case AxeClicked::Z :
					mNode_Z->detachObject(mPlaneY);
					mNode_Z->detachObject(mPlaneZ);
					break ;
			}
			break;
		}
		case Mode::ROTATE :
		{
			switch (mAxeClicked)
			{
				case AxeClicked::X :
					mNode_X->detachObject(mPlaneX);
					mNode_X->detachObject(mPlaneZ);
					mNode_X->detachObject(mPlaneY);
					break ;
				case AxeClicked::Y :
					mNode_Y->detachObject(mPlaneX);
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case AxeClicked::Z :
					mNode_Z->detachObject(mPlaneX);
					mNode_Z->detachObject(mPlaneZ);
					mNode_Z->detachObject(mPlaneY);
					break ;
			}
			break;							
		}
		case Mode::SCALE :
		{
			switch (mAxeClicked)
			{
				case AxeClicked::X :
					mNode_X->detachObject(mPlaneY);
					mNode_X->detachObject(mPlaneZ);
					break ;
				case AxeClicked::Y :
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case AxeClicked::Z :
					mNode_Z->detachObject(mPlaneY);
					mNode_Z->detachObject(mPlaneZ);
					break ;
			}
			break;
		}
	}

	mAxeClicked = AxeClicked::NONE  ;
	mPlaneClicked = AxeClicked::NONE;
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::drapNdrop( const OIS::MouseEvent &e )
{
	Vector3 newpos = getMousePosOnDummyPlane (e);
	return drapNdrop(newpos);
}

Vector3 Transformations::drapNdrop( Vector3 newpos )
{
	//Calculate the mouse mouvement on the scene
//!!!	Vector3 newpos = getMousePosOnDummyPlane (x, y);
	Vector3 moving = Vector3::ZERO ; //newpos - mOldpos ;
	
	if(mOldpos != Vector3::ZERO)
	{
		//Calculate transformation
		switch(mMode )
		{
			case Mode::MOVE :	//Move objects
			{
				switch (mAxeClicked )	//search axis
				{
					case AxeClicked::X :		//move X		
						moving.x = newpos.x - mOldpos.x ;
						break;
					case AxeClicked::Y :		//move Y
						moving.y = newpos.y - mOldpos.y ;
						break ;
					case AxeClicked::Z :		//move Z
						moving.z = newpos.z - mOldpos.z ;
						break ;
				}
				break;
			}
			case Mode::ROTATE :	//Rotate objects
			{
				switch (mAxeClicked )	//search axis
				{
					case AxeClicked::X :		//move X		
						moving.z = newpos.z - mOldpos.z ;
						break;
					case AxeClicked::Y :		//move Y
						moving.x = newpos.x - mOldpos.x ;
						break ;
					case AxeClicked::Z :		//move Z
						moving.y = newpos.y - mOldpos.y ;
						break ;
				}
				break;
			}
			case Mode::SCALE :	//Scale objects, but not gizmos
			{
				moving = Vector3 (1, 1, 1);
				switch (mAxeClicked )	//search axis
				{
					case AxeClicked::X :		//move X		
						moving.x = abs (1 + (newpos.x - mOldpos.x)/100.0) ;
						break;
					case AxeClicked::Y :		//move Y
						moving.y = abs( 1 + (newpos.y - mOldpos.y)/100.0 ) ;
						break ;
					case AxeClicked::Z :		//move Z
						moving.z = abs (1 + (newpos.z - mOldpos.z)/100.0 ) ;
						break ;
				}
				break;
			}
		}
	}
	mOldpos = newpos;
	return moving;
}
//-------------------------------------------------------------------------------------
void Transformations::eventSelection()
{
	showGizmosMove(false) ;
	showGizmosScale(false) ;
	showGizmosRotate(false) ;

	mNode_X->detachAllObjects();
	mNode_Y->detachAllObjects();
	mNode_Z->detachAllObjects();

	mMode = SELECT ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventMove()
{
	mNode_X->detachAllObjects();
	mNode_Y->detachAllObjects();
	mNode_Z->detachAllObjects();
	attachMoveGizmos(true);

	showGizmosMove(true) ;
	showGizmosScale(false) ;
	showGizmosRotate(false) ;

	mMode = MOVE ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventRotation()
{
	mNode_X->detachAllObjects();
	mNode_Y->detachAllObjects();
	mNode_Z->detachAllObjects();

	//Attach the good Gizmos
	attachRotateGizmos(true);

	showGizmosMove(false) ;
	showGizmosScale(false) ;
	showGizmosRotate(true) ;

	mMode = ROTATE ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventScale()
{
	mNode_X->detachAllObjects();
	mNode_Y->detachAllObjects();
	mNode_Z->detachAllObjects();

	//Attach the good Gizmos
	attachScaleGizmos(true);

	showGizmosMove(false) ;
	showGizmosScale(true) ;
	showGizmosRotate(false) ;

	mMode = SCALE ;
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosMove(bool pShow)
{
	m_moveX->setVisible(pShow);
	m_moveY->setVisible(pShow);
	m_moveZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosScale(bool pShow)
{
	m_scaleX->setVisible(pShow);
	m_scaleY->setVisible(pShow);
	m_scaleZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosRotate(bool pShow)
{
	m_rotateX->setVisible(pShow);
	m_rotateY->setVisible(pShow);
	m_rotateZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::createGizmos(SceneNode* pGizmosParentNode, SceneManager * pSceneMgr, Camera* pCamera)
{
	mCamera = pCamera ;
	mRaySceneQuery = pSceneMgr->createRayQuery( Ray() );

	//SceneNodes :
	m_GizmosAxesNode = pGizmosParentNode->createChildSceneNode("GizmosAxesNode"); 
	
	//Create sceneNodes
	mNode_X = m_GizmosAxesNode->createChildSceneNode("move_widget_x");
	mNode_Y = m_GizmosAxesNode->createChildSceneNode("move_widget_y");
	mNode_Z = m_GizmosAxesNode->createChildSceneNode("move_widget_z");

	createGizmosMove(pSceneMgr);
	createGizmosScale(pSceneMgr);
	createGizmosRotate(pSceneMgr);

	MovablePlane * movPlane ;
	movPlane = new MovablePlane("dummy_plane_x");
	movPlane->normal = Vector3::UNIT_Y;
	MeshManager::getSingleton().createPlane("dummy_plane_x",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*movPlane, 1000, 1000, 1, 1, true, 1, 1, 1, Vector3::UNIT_X);
	mPlaneX = pSceneMgr->createEntity( "dummy_plane_x", "dummy_plane_x" );
	mPlaneX->setVisible(false);

	movPlane = new MovablePlane("dummy_plane_y");
	movPlane->normal = Vector3::UNIT_Z;
	MeshManager::getSingleton().createPlane("dummy_plane_y",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*movPlane, 1000, 1000, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
	mPlaneY = pSceneMgr->createEntity( "dummy_plane_y", "dummy_plane_y" );
	mPlaneY->setVisible(false);

	movPlane = new MovablePlane("dummy_plane_z");
	movPlane->normal = Vector3::UNIT_X;
	MeshManager::getSingleton().createPlane("dummy_plane_z",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*movPlane, 1000, 1000, 1, 1, true, 1, 1, 1, Vector3::UNIT_Z);
	mPlaneZ = pSceneMgr->createEntity( "dummy_plane_z", "dummy_plane_z" );
	mPlaneZ->setVisible(false);
}

//-------------------------------------------------------------------------------------
void Transformations::createGizmosMove(SceneManager * pSceneMgr)
{
	Entity* OgreAxes = pSceneMgr->createEntity( "AxeTest", "axes.mesh"  );
	OgreAxes->getSubEntity(0)->getMaterial()->clone("MaterialGizmosX");
	OgreAxes->getSubEntity(0)->getMaterial()->clone("MaterialGizmosY");
	OgreAxes->getSubEntity(0)->getMaterial()->clone("MaterialGizmosZ");

	m_moveX = pSceneMgr->createEntity( "moveX", "axe_move_x.mesh"  );
	m_moveX->getSubEntity(0)->setMaterialName("MaterialGizmosX");
	m_moveX->getSubEntity(0)->getMaterial()->setDepthFunction(CMPF_ALWAYS_PASS );
	m_moveX->getSubEntity(0)->getMaterial()->setAmbient( ColourValue (1,0,0,1));
	m_moveX->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("red_texture.tga");
	mNode_X->roll( Degree(-90));
	mNode_X->yaw( Degree (90));

	m_moveY = pSceneMgr->createEntity( "moveY", "axe_move_y.mesh"  );
	m_moveY->getSubEntity(0)->setMaterialName("MaterialGizmosY");
	m_moveY->getSubEntity(0)->getMaterial()->setDepthFunction(CMPF_ALWAYS_PASS );
	m_moveY->getSubEntity(0)->getMaterial()->setAmbient( ColourValue (0,1,0,1));
	m_moveY->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("green_texture.tga");
	mNode_Y->yaw(Degree(90));

	m_moveZ = pSceneMgr->createEntity( "moveZ", "axe_move_z.mesh"  );
	m_moveZ->getSubEntity(0)->setMaterialName("MaterialGizmosZ");
	m_moveZ->getSubEntity(0)->getMaterial()->setDepthFunction(CMPF_ALWAYS_PASS );
	m_moveZ->getSubEntity(0)->getMaterial()->setAmbient( ColourValue (0,0,1,1));
	m_moveZ->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("blue_texture.tga");
	mNode_Z->pitch( Degree(90));

	
	//Hide objects :
	m_moveX->setVisible(false);
	m_moveY->setVisible(false);
	m_moveZ->setVisible(false);
}


//-------------------------------------------------------------------------------------
void Transformations::createGizmosScale(SceneManager * pSceneMgr)
{
	m_scaleX = pSceneMgr->createEntity( "scaleX", "axe_scale_x.mesh"  );
	m_scaleX->getSubEntity(0)->setMaterialName("MaterialGizmosX");

	m_scaleY = pSceneMgr->createEntity( "scaleY", "axe_scale_y.mesh"  );
	m_scaleY->getSubEntity(0)->setMaterialName("MaterialGizmosY");

	m_scaleZ = pSceneMgr->createEntity( "scaleZ", "axe_scale_z.mesh"  );
	m_scaleZ->getSubEntity(0)->setMaterialName("MaterialGizmosZ");

	m_scaleX->setVisible(false);
	m_scaleY->setVisible(false);
	m_scaleZ->setVisible(false);

}
//-------------------------------------------------------------------------------------
void Transformations::createGizmosRotate(SceneManager * pSceneMgr)
{
	m_rotateX = pSceneMgr->createEntity( "rotateX", "axe_rotate_x.mesh"  );
	m_rotateX->getSubEntity(0)->setMaterialName("MaterialGizmosX");


	m_rotateY = pSceneMgr->createEntity( "rotateY", "axe_rotate_y.mesh"  );
	m_rotateY->getSubEntity(0)->setMaterialName("MaterialGizmosY");

	m_rotateZ = pSceneMgr->createEntity( "rotateZ", "axe_rotate_z.mesh"  );
	m_rotateZ->getSubEntity(0)->setMaterialName("MaterialGizmosZ");

	m_rotateX->setVisible(false);
	m_rotateY->setVisible(false);
	m_rotateZ->setVisible(false);
}
//-------------------------------------------------------------------------------------
void Transformations::attachMoveGizmos(bool pAttach)
{
	if (pAttach)		//attach Gizmos
	{
		mNode_X->attachObject(m_moveX);	//XX arrows
		mNode_Y->attachObject(m_moveY);	//YY arrows
		mNode_Z->attachObject(m_moveZ);	//ZZ arrows
	}
	else				//detach gizmos
	{
		mNode_X->detachObject(m_moveX);	//XX arrows
		mNode_Y->detachObject(m_moveY);	//YY arrows
		mNode_Z->detachObject(m_moveZ);	//ZZ arrows
	}
}
//-------------------------------------------------------------------------------------
void Transformations::attachScaleGizmos(bool pAttach)
{
	if (pAttach)		//attach Gizmos
	{
		mNode_X->attachObject(m_scaleX);	//XX arrows
		mNode_Y->attachObject(m_scaleY);	//YY arrows
		mNode_Z->attachObject(m_scaleZ);	//ZZ arrows
	}
	else				//detach gizmos
	{
		mNode_X->detachObject(m_scaleX);	//XX arrows
		mNode_Y->detachObject(m_scaleY);	//YY arrows
		mNode_Z->detachObject(m_scaleZ);	//ZZ arrows
	}
}
//-------------------------------------------------------------------------------------
void Transformations::attachRotateGizmos(bool pAttach)
{
	if (pAttach)		//attach Gizmos
	{
		mNode_X->attachObject(m_rotateX);	//XX arrows
		mNode_Y->attachObject(m_rotateY);	//YY arrows
		mNode_Z->attachObject(m_rotateZ);	//ZZ arrows
	}
	else				//detach gizmos
	{
		mNode_X->detachObject(m_rotateX);	//XX arrows
		mNode_Y->detachObject(m_rotateY);	//YY arrows
		mNode_Z->detachObject(m_rotateZ);	//ZZ arrows
	}
}

//-------------------------------------------------------------------------------------
RaySceneQueryResult& Transformations::raySceneQuery( const OIS::MouseEvent &e )
{
	Ray mouseRay = mCamera->getCameraToViewportRay( 
		0.5/float(e.state.width), 
		0.5/float(e.state.height) );
	return raySceneQuery(mouseRay);
}

RaySceneQueryResult& Transformations::raySceneQuery( Ray mouseRay )
{
	// Setup the ray scene query, use CEGUI's mouse position
	//CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	// TODO : get Navi coord here !!
	//Ray mouseRay = mCamera->getCameraToViewportRay( 
	//	mousePos.d_x/float(e.state.width), 
	//	mousePos.d_y/float(e.state.height) );
/*!!!
	Ray mouseRay = mCamera->getCameraToViewportRay( 
		0.5/float(x), 
		0.5/float(y) );
*/
	mRaySceneQuery->setRay( mouseRay );
	mRaySceneQuery->setSortByDistance( true, 5 );

	return mRaySceneQuery->execute();
}
//-------------------------------------------------------------------------------------
void Transformations::onClickToTransformObject(const OIS::MouseEvent &e, const String pNameAxeX,
		const String pNameAxeY, const String pNameAxeZ)
{
	onClickToTransformObject(raySceneQuery(e), pNameAxeX, pNameAxeY, pNameAxeZ);
}

void Transformations::onClickToTransformObject(RaySceneQueryResult &result, const String pNameAxeX,
		const String pNameAxeY, const String pNameAxeZ)
{
	bool manuel_objet = false ;		//for search manuel object
	
//!!!	RaySceneQueryResult &result = raySceneQuery(e);// Execute query
	RaySceneQueryResult::iterator itrRSQR =  result.begin( ); //+1

	if( ! result.empty() )
	{
		Ogre::String name;
		//search if there is a gizmo object in the same distance of the current object
		for(;itrRSQR != result.end() ; itrRSQR++)
		{
			name = itrRSQR->movable->getName();
			if( (name == "moveX") ||  (name == "moveY") || (name == "moveZ") ||
				(name == "scaleX") ||  (name == "scaleY") || (name == "scaleZ") ||
				(name == "rotateX") ||  (name == "rotateY") || (name == "rotateZ") )
			{
				manuel_objet = true ;
				break;
			}
		}

		if(manuel_objet)	//if we click on a gizmos
		{					//add planes to detect the mouse position on there
			MovableObject * nodeM = itrRSQR->movable ;
			String name = nodeM->getName();

			switch( mMode )
			{
				case Transformations::Mode::MOVE :
				{
					if (name == pNameAxeX)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						mAxeClicked = AxeClicked::X ;
					}
					else if (name == pNameAxeY)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = AxeClicked::Y ;
					}
					else if( name == pNameAxeZ)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = AxeClicked::Z ;
					}
					break;
				}
				case Transformations::Mode::ROTATE :
				{
					if (name == pNameAxeX)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneX);
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = AxeClicked::X ;
					}
					else if (name == pNameAxeY)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneX);
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = AxeClicked::Y ;
					}
					else if( name == pNameAxeZ)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneX);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						mAxeClicked = AxeClicked::Z ;
					}		
					//nodeM->getParentSceneNode()->attachObject(mSceneMgr->getEntity("dummy_plane_y"));
					
					break;
				}
				case Transformations::Mode::SCALE :
				{
					if (name == pNameAxeX)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
                        mAxeClicked = Solipsis::Transformations::AxeClicked::X ;
					}
					else if (name == pNameAxeY)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = Solipsis::Transformations::AxeClicked::Y ;
					}
					else if( name == pNameAxeZ)
					{
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = AxeClicked::Z ;
					}
					break;
				}
			}
		}
	}


	mRaySceneQuery->clearResults();
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::getMousePosOnDummyPlane (const OIS::MouseEvent &e)
{
	Ray mouseRay = mCamera->getCameraToViewportRay( 
		0.5/float(e.state.width), 
		0.5/float(e.state.height) );
	return getMousePosOnDummyPlane(mouseRay);
}

Vector3 Transformations::getMousePosOnDummyPlane (Ray mouseRay)
{
	//ray tracing :
	//CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	//Ray mouseRay = mCamera->getCameraToViewportRay(
	//	mousePos.d_x/float(e.state.width), 
	//	mousePos.d_y/float(e.state.height) );

	// TODO !!!!!
/*!!!
	Ray mouseRay = mCamera->getCameraToViewportRay( 
		0.5/float(x), 
		0.5/float(y) );
*/
	mRaySceneQuery->setRay(mouseRay);
	mRaySceneQuery->setSortByDistance(true);
	RaySceneQueryResult &result = mRaySceneQuery->execute();


	RaySceneQueryResult::iterator itrRSQR =  result.begin( );

	if( ! result.empty() )
	{
		for ( ;itrRSQR != result.end(); itrRSQR++)	//search the good plane
		{
			if( itrRSQR->movable->getMovableType() == "Entity" )
			{
				String name = itrRSQR->movable->getName();	//search the good object (dummy_plane) :
				if(name == mPlaneX->getName())
				{
					if(mPlaneClicked == AxeClicked::NONE)	//if it is the first clck ...
						mPlaneClicked = AxeClicked::X ;		//	remenber this plane

					if(mPlaneClicked == AxeClicked::X)
					{
						//We can calculate the new position :
						Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
						mRaySceneQuery->clearResults();
						return pos ;
					}
				}
				else if(name == mPlaneY->getName())
				{
					if(mPlaneClicked == AxeClicked::NONE)	//if it is the first clck ...
						mPlaneClicked = AxeClicked::Y ;		//	remenber this plane

					if(mPlaneClicked == AxeClicked::Y)
					{
						//We can calculate the new position :
						Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
						mRaySceneQuery->clearResults();
						return pos ;
					}
				}
				else if(name == mPlaneZ->getName())
				{
					if(mPlaneClicked == AxeClicked::NONE)	//if it is the first clck ...
						mPlaneClicked = AxeClicked::Z ;		//	remenber this plane
	
					if(mPlaneClicked == AxeClicked::Z)
					{
						//We can calculate the new position :
						Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
						mRaySceneQuery->clearResults();
						return pos ;
					}
				}
			}

		}
	}

	mRaySceneQuery->clearResults();
    return mOldpos;
}
//-------------------------------------------------------------------------------------
Transformations::Mode Transformations::getMode()
{
	return mMode;
}

} //namespace