/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO 2007 - Stephane CHAPLAIN

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

#pragma once

#include "Object3D.h"

#include "SolipsisErrorHandler.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"
#include "Selection.h"

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

/** This class defines a callbacks interface on Modeler.
 */
class IModelerCallbacks
{
public:
	// Called when an Object3DList was saved
	virtual bool OnObject3DListSave(const String& sofPathname, const Object3DPtrList& object3DList) { return true; };
};

class Modeler : public ModifiedMaterialManager::MMMTextureManager
{
private:
    static Modeler* ms_singletonPtr;

public:
	/// Default constructor
	Modeler(Ogre::SceneManager* pSceneMgr, Ogre::Camera* pCamera, IModelerCallbacks* modelerCallbacks = 0);
	/// Default destructor
	~Modeler(void);

	static Modeler* getSingletonPtr(SceneManager* pSceneMgr = 0, Camera* pCamera = 0, IModelerCallbacks* modelerCallbacks = 0);

	/// Init the modeler mode
	bool	init();

	/// Create a plane 
	bool createPlane(Vector3	&player_pos);
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
	bool XMLLoad(const String& filename, Object3DPtrList& loadedObjects, Vector3 pos = Vector3::ZERO);
	bool XMLImport(const String& filename, Vector3 pos = Vector3::ZERO);
	bool XMLSave(bool all = false, const char* pathToSave = 0);

	/// Update the command list of the stored deformations
	bool updateCommand(Object3D::Command pCommand, Object3D* pObject);



	/// The execution path (to go back home each time)
	Ogre::String		mExecPath;

private:
	Object3D * createObjectWithXML(TiXmlDocument doc, string path, Vector3 pos);

public:
    /// See TextureManager::loadTexture
    virtual TexturePtr loadTexture(ModifiedMaterialManager* modifiedMaterialManager, Entity* entity, const String& name, const TextureExtParamsMap& textureExtParamsMap);
    /// See TextureManager::releaseTexture
    virtual void releaseTexture(ModifiedMaterialManager* modifiedMaterialManager, const String& name, const TextureExtParamsMap& textureExtParamsMap);

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

    /// Callbacks
    IModelerCallbacks	*mModelerCallbacks;

	/// Primitive entities
	Entity				*mGenericPlane;
	Entity				*mGenericBox;
	Entity				*mGenericPrism;
	Entity				*mGenericCylinder;
	Entity				*mGenericSphere;
	Entity				*mGenericTorus;
	Entity				*mGenericTube;
	Entity				*mGenericRing;
};

} // End namespace Solipsis