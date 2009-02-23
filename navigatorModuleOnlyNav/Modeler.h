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

// Declarative modeling
#include "DeclarativeModeler.h"
// Tinyxml
#include "tinyxml.h"
// Directory management
#include <direct.h>
// Zip Management
#include "FileBuffer.h"
#include "MyZipArchive.h"

#include <XmlDatas.h>

#define DIST_AVATAR_OBJECT 1.5f

namespace Solipsis {

/** This class defines a callbacks interface on Modeler.
 */
class IModelerCallbacks
{
public:
	// Called when an Object3D was saved
	virtual bool onObject3DSave(const String& sofFilename, Object3D* object3D) { return true; }
	// Called when an Object3D was deleted
	virtual bool onObject3DDelete(Object3D* object3D) { return true; }
	// Called to known if Object3D is owned
	virtual bool isObject3DOwned(Object3D* object3D) { return false; }
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

	static Modeler* getSingletonPtr();

	/// Init the modeler mode
	bool	init(const String& pPath);

    /// Clean up
    void cleanUp();

	/// Create a plane 
	bool createPlane(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a box 
	bool createBox(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a corner. 
	bool createCorner(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a pyramid. 
	bool createPyramid(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a prism. 
	bool createPrism(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a cylinder. 
	bool createCylinder(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a half cylinder. 
	bool createHalfCyl(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a cone. 
	bool createCone(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a Halfcone. 
	bool createHalfCone(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a sphere. 
	bool createSphere(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a half sphere. 
	bool createHalfSphere(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a torus. 
	bool createTorus(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a tube. 
	bool createTube(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a ring. 
	bool createRing(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);
	/// Create a 3D scene by declatative modeling
	bool createSceneFromText( const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion& orientation, const std::string & s, std::string& errMsg, std::string& warnMsg );
	/// Create a mesh. 
	bool createMesh(const EntityUID& entityUID, const String& name, Vector3 &player_pos, Quaternion &orientation);

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
	int isOnGizmo();
	/// Lock or unlock the gizmo transformation state
	void lockGizmo(int pLock);

	/// Event for the move gizmo
	void eventMove();
	/// Event for the rotate gizmo
	void eventRotate();
	/// Event for the scale gizmo
	void eventScale();


	/// Load from / Save to a XML SOLIPSIS file
	bool XMLLoad(const String& filename, Object3DPtrList& loadedObjects, Vector3 pos = Vector3::ZERO, Quaternion orientation = Quaternion::IDENTITY);
	bool XMLImport(const EntityUID& entityUID, const String& name, const String& filename, Vector3 pos = Vector3::ZERO, Quaternion orientation = Quaternion::IDENTITY);
	bool XMLSave(bool all = false);

	/// Update the command list of the stored deformations
	bool updateCommand(Object3D::Command pCommand, Object3D* pObject, bool pForSave = false);



	/// The load/save path
	Ogre::String		mPath;
	/// The execution path (to go back home each time)
	Ogre::String		mExecPath;

private:
	Object3D * createObjectWithXML(TiXmlDocument doc, string path, Vector3 pos, Quaternion orientation);

public:
    /// See TextureManager::loadTexture
    virtual TexturePtr loadTexture(ModifiedMaterialManager* modifiedMaterialManager, const String& name, const TextureExtParamsMap& textureExtParamsMap);
    /// See TextureManager::releaseTexture
    virtual void releaseTexture(ModifiedMaterialManager* modifiedMaterialManager, const String& name, const TextureExtParamsMap& textureExtParamsMap);
    /// See TextureManager::loadTexture
    virtual void pauseEffect(ModifiedMaterialManager* modifiedMaterialManager, const String& name,TextureExtParamsMap &textureExtParamsMap);
    /// See TextureManager::loadTexture
    virtual void startEffect(ModifiedMaterialManager* modifiedMaterialManager, const String& name,TextureExtParamsMap &textureExtParamsMap);

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
	int				    mOnGizmo;

    /// Callbacks
    IModelerCallbacks	*mModelerCallbacks;

	DeclarativeModeler  *mDeclarativeModeler; 

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