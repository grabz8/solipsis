/**
	\file 
		Modeler.h
	\brief
		Solipsis Imperative Intuitive Modeler
	\author
		ARTEFACTO 2007 - Stéphane CHAPLAIN
*/

#pragma once

#include "OgrePeer.h"

namespace Solipsis {

// Ogre Class 
//class Ogre::SceneManager;
//class Ogre::Camera;


class Avatar;
class Selection;


class Modeler
{
public:
	/// Default constructor
	Modeler(Ogre::SceneManager* pSceneMgr, Ogre::Camera* pCamera);
	/// Default destructor
	~Modeler(void);

	/// Init the modeler mode
	bool	init(Avatar * playerAvatar);
	
	/// Create a box 
	bool createBox(Vector3	&player_pos);
	/// Create a corner. 
	bool createCorner(Vector3	&player_pos);
	/// Create a pyramid. 
	bool createPyramid(Vector3	&player_pos);
	/// Create a prism. 
	bool createPrism(Vector3	&player_pos);
	/// Create a cylinder. 
	bool createCylinder(Vector3	&player_pos);
	/// Create a half cylinder. 
	bool createHalfCyl(Vector3	&player_pos);
	/// Create a cone. 
	bool createCone(Vector3	&player_pos);
	/// Create a Halfcone. 
	bool createHalfCone(Vector3	&player_pos);
	/// Create a sphere. 
	bool createSphere(Vector3	&player_pos);
	/// Create a half sphere. 
	bool createHalfSphere(Vector3	&player_pos);
	/// Create a torus. 
	bool createTorus(Vector3	&player_pos);
	/// Create a tube. 
	bool createTube(Vector3	&player_pos);
	/// Create a ring. 
	bool createRing(Vector3	&player_pos);
	/// Create a mesh. 
	bool createMesh(Vector3	&player_pos);



private : 
	
	/// Backup old camera to set active when we will exit mode
	Ogre::Camera		*mCamera;
	/// The global Scene manager
	Ogre::SceneManager	*mSceneManager;
	/// The selection manager
	Selection		*mSelection;	

	/// Primitive entities
	Entity			*mGenericBox;
	Entity			*mGenericPrism;
	Entity			*mGenericCylinder;
	Entity			*mGenericSphere;
	Entity			*mGenericTorus;
	Entity			*mGenericTube;
	Entity			*mGenericRing;

};

} // End namespace Solipsis