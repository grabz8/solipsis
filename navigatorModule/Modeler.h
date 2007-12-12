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