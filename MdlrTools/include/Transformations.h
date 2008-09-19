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
		Transformations.h
	\brief
		Solipsis class for manage transformations (MOVE, SCALE, ROTATE) of objects 3D
	\author
		ARTEFACTO - Patrice Desfonds
*/


#ifndef __Transformation_h__
#define __Transformation_h__

#include <Ogre.h>
#include <OIS/OIS.h>
#include <String>

using namespace Ogre;
using namespace std;

namespace Solipsis {

class Transformations
{
public:
	///brief Constructor
	Transformations(void);

	///brief Destructor
	~Transformations(void);

/*
 *
 *	\brief 
 *		Create gizmos for move, rotate and scale.
 *		Call this function before use the others.
 *
 *		This class uses this hierarchy :
 *
 *								mSceneMgr
 *									/	
 *								   /
 *							NodeSelection (representing by pGizmosParentNode
 *								  /
 *							     /
 *						  GizmosAxesNode )
 *					   /		  |			  \
 *					  /			  |			   \
 *					 /			  |				\
 *					/			  |				 \
 *			move_Widget_x	move_Widget_y	  move_Widget_z
 *
 *		If user wants to MOVE : attach the entities moveX, moveY and moveZ to the lasts nodes,
 *			if he wants to SCALE : attach the entities scaleX, scaleY and scaleZ
 *			if he wants to Rotate : attach the entities rotateX, rotateY and rotateZ
 *
 *
 *	\param pGizmosParentNode = Ogre Node for attach all Gizmos
 *	\param pSceneMgr = Ogre Scene Manager
 *	\param pCamera = Ogre current Camaera
 *
*/
	void createGizmos(SceneNode* pGizmosParentNode, SceneManager * pSceneMgr, Camera* pCamera);
	void destroyGizmos(SceneManager * pSceneMgr);
				
	///brief Events set mode
	void eventSelection();
	void eventMove();
	void eventRotation();
	void eventScale();

	///brief Attach corrects plans and update mOldPos.
	///			Plans are attached on the nodes move_Widget_X, ..Y, and ..Z.
	///			It use for know where user has cliked on the scene
	void firstClickForTransformation( const OIS::MouseEvent &e);
	void firstClickForTransformation( RaySceneQueryResult &result);

	///brief Detach plans
	void releasedClickForTransformation ();

	///brief Calcul the new vector for the selected object
	///			Next you have to apply this on the selection and test mMode for know
	///			if we are in MOVE, ROTATE or SCALE
	Vector3 drapNdrop( const OIS::MouseEvent &e ) ;
	Vector3 drapNdrop( Vector3 newpos ) ;

	///brief Show or hide Gizmos
	void showGizmosMove(bool pShow);
	void showGizmosScale(bool pShow);
	void showGizmosRotate(bool pShow);

	///brief Give the mouse position (relative to the good dummy plane)
	///return Mouse Position in the scene 
	Vector3 getMousePosOnDummyPlane (const OIS::MouseEvent &e) ;
	Vector3 getMousePosOnDummyPlane (Ray mouseRay) ;

/*
 *
 *	\brief 
 *		When the user click on an axe (X, Y or Z), this function detect this axe and
 *			put result in mAxeClicked
 *
 *	\param pNameAxe = Name of axes attach on the move_Widget_
 *						(moveX, scaleX, RotateX, ...)
 *
*/
	void onClickToTransformObject(const OIS::MouseEvent &e, const String pNameAxeX,
								const String pNameAxeY, const String pNameAxeZ );	
	void onClickToTransformObject(RaySceneQueryResult &result, const String pNameAxeX,
								const String pNameAxeY, const String pNameAxeZ );


	enum Mode {
		SELECT,
		MOVE,
		ROTATE,
		SCALE };
	///brief Get the mode choosen by the user
	///return the mode choosen by the user
	Mode getMode() ;

	enum AxeClicked {
		NONE,
		CAMERA,		//Plane for camera only
		X,
		Y,
		Z };

private :
	///brief Create Move Gizmos and hide them
	void createGizmosMove(SceneManager * pSceneMgr);
	///brief Create Scale Gizmos and hide them
	void createGizmosScale(SceneManager * pSceneMgr);
	///brief Create Rotate Gizmos and hide them
	void createGizmosRotate(SceneManager * pSceneMgr);

    String mResourceGroup;

	///brief main node for manage gismos. It may be a child of the selection Node
	SceneNode* m_GizmosAxesNode;

	///brief Node for attach X Gizmos (its name is move_Widget_X)
	SceneNode * mNode_X;
	///brief Node for attach Y Gizmos (its name is move_Widget_Y)
	SceneNode * mNode_Y;
	///brief Node for attach Z Gizmos (its name is move_Widget_Z)
	SceneNode * mNode_Z;

	///brief Object for gizmos :
		//Gizmos move :
	Entity* m_moveX; 
	Entity* m_moveY; 
	Entity* m_moveZ;
		//Gizmos scale :
	Entity* m_scaleX ; 
	Entity* m_scaleY ; 
	Entity* m_scaleZ ;
		//Gizmos rotate :
	Entity* m_rotateX ;
	Entity* m_rotateY ;
	Entity* m_rotateZ ;

	///brief Attach or dettach Move Gizmos
	///param pAttach  = if true->attach else->detach
	void attachMoveGizmos(bool pAttach) ;
	///brief Attach or dettach Scale Gizmos
	///param pAttach  = if true->attach else->detach
	void attachScaleGizmos(bool pAttach) ;
	///brief Attach or dettach Rotate Gizmos
	///param pAttach  = if true->attach else->detach
	void attachRotateGizmos(bool pAttach) ;

	///brief Dummy plane X :
	Entity * mPlaneX;
	///brief Dummy plane Y :
	Entity * mPlaneY;
	///brief Dummy plane Z :
	Entity * mPlaneZ;


	///biref Make a ray trace
	///return the result of the ray trace
	RaySceneQueryResult& raySceneQuery( const OIS::MouseEvent &e );
	RaySceneQueryResult& raySceneQuery( Ray mouseRay );
	///brief The ray scene query pointer
	RaySceneQuery *mRaySceneQuery;	


	///brief For remember the first axe clicked by the user
	AxeClicked mAxeClicked ;
	///brief For remember the first plane clicked by the user
	AxeClicked mPlaneClicked ;


	///brief Current transformation selected (SELECT, MOVE, ROTATE, or SCALE)
	Mode mMode ;

	///brief Old mouse position
	Vector3 mOldpos;

	///brief Current Ogre Camera
	Camera* mCamera;

};

} //namespace

#endif //__Transformation_h__