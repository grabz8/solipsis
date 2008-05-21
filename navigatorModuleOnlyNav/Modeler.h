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
#include "Object3D.h"
//#include "Selection.h"

#include "SolipsisErrorHandler.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"

#include "FileBrowser.h"
#include "Path.h"
// Tinyxml
#include "tinyxml.h"
// Directory management
#include <direct.h>
// Zip Management
#include "FileBuffer.h"
#include "MyZipArchive.h"


namespace Solipsis {

// Ogre Class 
//class Ogre::SceneManager;
//class Ogre::Camera;


class Avatar;
class Selection;
class Object3D;


class Modeler
{
private:
    static Modeler* ms_singletonPtr;

public:
	/// Default constructor
	Modeler(Ogre::SceneManager* pSceneMgr, Ogre::Camera* pCamera);
	/// Default destructor
	~Modeler(void);

	static Modeler* getSingletonPtr(SceneManager* pSceneMgr = NULL, Camera* pCamera = NULL);

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

	/// Test if the selection is empty
	bool isSelectionEmpty();
	/// Return the selection
	Selection* getSelection();
	/// Return the selected object
	Object3D* getSelected();
	/// Select an entity and show his boundbox
	bool selectNode(Entity* pEnt);
	/// Deselect all
	void deselectNode();
	/// Remove and destroy child
	void removeSelection();

	/// Return the lock state of the selection
	bool isSelectionLocked();
	/// Lock or unlock the selection
	void lockSelection(bool pLock);

	/// Test if we're in link mode
	bool isInLinkMode();
	/// Lock or unlock the link mode
	void lockLinkMode(bool pLock);

	/// Test if we've clicked on a gizmo tranformation
	bool isOnGizmo();
	/// Lock or unlock the gizmo transformation state
	void lockGizmo(bool pLock);

	/// Event for the move gizmo
	void eventMove();
	/// Event for the rotate gizmo
	void eventRotate();
	/// Event for the scale gizmo
	void eventScale();


	/// Load from / Save to a XML SOLIPSIS file
	bool XMLLoad(Vector3 pos = Vector3::ZERO, const char* pathToLoad = NULL);
	bool XMLImport(Vector3 pos = Vector3::ZERO, const char* pathToLoad = NULL);
	bool XMLSave(bool all = false, const char* pathToSave = NULL);

	/// Update the command list of the stored deformations
	bool updateCommand(Object3D::Command pCommand, Object3D* pObject);



	/// The execution path (to go back home each time)
	Ogre::String		mExecPath;

private:
	Object3D * createObjectWithXML(TiXmlDocument doc, string path, Vector3 pos);

private : 
	/// Backup old camera to set active when we will exit mode
	Ogre::Camera		*mCamera;
	/// The global Scene manager
	Ogre::SceneManager	*mSceneManager;
	/// The selection manager
	Selection			*mSelection;
	bool				mSelectionLocked;
	/// Link mode
	bool				mLinkMode;
	/// Gizmo
	bool				mOnGizmo;

	/// Primitive entities
	Entity				*mGenericBox;
	Entity				*mGenericPrism;
	Entity				*mGenericCylinder;
	Entity				*mGenericSphere;
	Entity				*mGenericTorus;
	Entity				*mGenericTube;
	Entity				*mGenericRing;
};

} // End namespace Solipsis