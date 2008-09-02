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

#include "Modeler.h"
#include "Selection.h"
#include "Transformations.h"
#include "Navigator.h"
#include <OgreExternalTextureSourceManager.h>
#include <Navi.h>
#include <CTIO.h>
#include <Plugin_3ds.h>
#include <Plugin_skp.h>

namespace Solipsis {

Modeler* Modeler::ms_singletonPtr = 0;

Modeler::Modeler(SceneManager* pSceneMgr, Camera* pCamera, IModelerCallbacks* modelerCallbacks)
{
	mSceneManager = pSceneMgr;
	mCamera = pCamera;
    mModelerCallbacks = modelerCallbacks;

	mExecPath = _getcwd(NULL, 0);
	SOLIPSISINFO("Current working directory is : ",mExecPath.c_str());

	ms_singletonPtr = this;

    ModifiedMaterialManager::setMMMTextureManager(this);
}

Modeler::~Modeler(void)
{
	// Delete our materials : lests assume that Ogre will do it alone
	// Free our Center scene node
	mSceneManager->getRootSceneNode()->removeAndDestroyChild("NodeCentreRotation");
	// Free our primitives
	mSceneManager->destroyEntity(mGenericBox);
	mSceneManager->destroyEntity(mGenericPrism);
	mSceneManager->destroyEntity(mGenericCylinder);
	mSceneManager->destroyEntity(mGenericSphere);
	mSceneManager->destroyEntity(mGenericTorus);
	mSceneManager->destroyEntity(mGenericTube);
	mSceneManager->destroyEntity(mGenericRing);

	// delete the selection manager
	delete mSelection;
	mSelection = 0;

}

Modeler* Modeler::getSingletonPtr(SceneManager* pSceneMgr, Camera* pCamera, IModelerCallbacks* modelerCallbacks)
{
	if(!ms_singletonPtr)
		ms_singletonPtr = new Modeler(pSceneMgr, pCamera, modelerCallbacks);

    return ms_singletonPtr;
}
bool Modeler::init()
{
	static bool init = false;
	if(init) return true;

	// Init all the material we will need
	MaterialPtr material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matRed","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0); 

	material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matGreen","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,1,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0); 

	material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matBlue","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 
 
	material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matRedNoDepth","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0); 
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matGreenNoDepth","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,1,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0); 
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	material = (MaterialPtr) (MaterialManager::getSingleton().createOrRetrieve("matBlueNoDepth","debugger").first); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	SceneNode * node = mSceneManager->getRootSceneNode()->createChildSceneNode("NodeCentreRotation");
	SceneNode * node2 = node->createChildSceneNode("NodeCentreObject");
	mSelection = new Selection (node, node2);

	mLinkMode = false;
	mOnGizmo = false;

	//create and put gizmos in the centre of selection
	node = mSceneManager->getRootSceneNode()->createChildSceneNode("NodeSelection");	//for find all objects selected
	//node = mSceneManager->getSceneNode("NodeSelection");
	mSelection->mTransformation->createGizmos( node , mSceneManager, mCamera );	

	// Init generic primitives to clone when the creation will be called
	mGenericPlane = mSceneManager->createEntity( "GenericPlane", "Plane.mesh" );
	mGenericBox = mSceneManager->createEntity( "GenericBox", "Box.mesh" );
	mGenericPrism = mSceneManager->createEntity( "GenericPrism", "Prism.mesh" );
	mGenericCylinder = mSceneManager->createEntity( "GenericCylinder", "Cylinder.mesh" );
	mGenericSphere = mSceneManager->createEntity( "GenericSphere", "Sphere.mesh" );
	mGenericTorus = mSceneManager->createEntity( "GenericTorus", "Torus.mesh" );
	mGenericTube = mSceneManager->createEntity( "GenericTube", "Tube.mesh" );
	mGenericRing = mSceneManager->createEntity( "GenericRing", "Ring.mesh" );

	//mModifiedColor = ModifiedColor::NONE ;
	//mPrecedentVerticalScrollPosition = 0;

	//mExecPath = _getcwd(NULL, 0);
	//SOLIPSISINFO("Current working directory is : ",mExecPath.c_str());
	//mModeLink = false ;

	//mTransfoButton.clear();

	//mNeedHandle = true;

	init = true;
	return true; // Success
}

/// Create a plane.
bool Modeler::createPlane(Vector3 &player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Plane%.3u",++num);

	MeshPtr mptr = mGenericPlane->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DPlane* obj = new Object3DPlane( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	//OGRE_LOG("Modeler::createPlane()");
	//OGRE_LOG(name);
	return true;
}

/// Create a box.
bool Modeler::createBox(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Box%.3u",++num);

	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DBox* obj = new Object3DBox( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	//OGRE_LOG("Modeler::createBox()");
	//OGRE_LOG(name);
	return true;
}


/// Create a corner. 
bool Modeler::createCorner(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Corner%.3u",++num);

	//genMeshCorner( String(name) + ".mesh", 100, 100, 100 );
	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DCorner* obj = new Object3DCorner( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a pyramid. 
bool Modeler::createPyramid(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Pyramid%.3u",++num);

	//genMeshPyramid( String(name) + ".mesh", 100, 100, 100 );
	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DPyramid* obj = new Object3DPyramid( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a prism. 
bool Modeler::createPrism(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Prism%.3u",++num);

	//genMeshPrism( String(name) + ".mesh", 100, 100 );
	MeshPtr mptr = mGenericPrism->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DPrism* obj = new Object3DPrism( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a cylinder. 
bool Modeler::createCylinder(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Cylinder%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DCylinder* obj = new Object3DCylinder( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a half cylinder. 
bool Modeler::createHalfCyl(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfCylinder%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DHalfCylinder* obj = new Object3DHalfCylinder( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a cone. 
bool Modeler::createCone(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Cone%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DCone* obj = new Object3DCone( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a Halfcone. 
bool Modeler::createHalfCone(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfCone%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DHalfCone* obj = new Object3DHalfCone( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a sphere. 
bool Modeler::createSphere(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Sphere%.3u",++num);

	MeshPtr mptr = mGenericSphere->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DSphere* obj = new Object3DSphere( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a half sphere. 
bool Modeler::createHalfSphere(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfSphere%.3i",++num);

	MeshPtr mptr = mGenericSphere->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DHalfSphere* obj = new Object3DHalfSphere( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a torus. 
bool Modeler::createTorus(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Torus%.3i",++num);

	MeshPtr mptr = mGenericTorus->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DTorus* obj = new Object3DTorus( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a tube. 
bool Modeler::createTube(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Tube%.3i",++num);

	MeshPtr mptr = mGenericTube->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DTube* obj = new Object3DTube( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a ring. 
bool Modeler::createRing(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Ring%.3i",++num);

	MeshPtr mptr = mGenericRing->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DRing* obj = new Object3DRing( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Create a mesh. 
bool Modeler::createMesh(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Mesh%.3u",++num);

	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	entity->setCastShadows(true);
#endif
    entity->setQueryFlags(Navigator::QFObject);
	node->attachObject( entity );

	Object3DOther* obj = new Object3DOther( String(name), node );
	mSelection->add3DObject(obj);
	obj->mCentreSelection = player_pos;

	node->setPosition(player_pos);
	return true;
}

/// Test if the selection is empty
bool Modeler::isSelectionEmpty()
{
	if( mSelection ) 
		return mSelection->isEmpty();

	return false;
}

/// Return the selected object
Object3D* Modeler::getSelected()
{
	if( mSelection )
		return mSelection->getFirstSelectedObject();

	return 0;
}

/// ...
bool Modeler::selectNode(Entity* pEnt)
{
	if( mSelection )
		return mSelection->clickNode( pEnt );

	return false;
}

/// ...
void Modeler::deselectNode()
{
	if( mSelection )
	{
		mSelection->deselect_all();
		if( mSelection->mTransformation )
			mSelection->mTransformation->eventSelection();
	}
}

/// Remove and destroy child
void Modeler::removeSelection()
{
	Object3D* obj = getSelected();
	do
	{
		// delete 3D entity
		mSelection->remove3DObject( obj );
		obj = mSelection->getFirstSelectedObject();
	}
	while ( obj != 0 );

//	mSelection->mTransformation->showGizmosMove( false );
//	mSelection->mTransformation->showGizmosRotate( false );
//	mSelection->mTransformation->showGizmosScale( false );
}

/// Return the lock state of the selection
bool Modeler::isSelectionLocked()
{
	return mSelectionLocked;
}

/// Lock or unlock the selection
void Modeler::lockSelection(bool pLock)
{
	mSelectionLocked = pLock;
}

/// Test if we're in link mode
bool Modeler::isInLinkMode()
{
	return mLinkMode;
}

/// Lock or unlock the link mode
void Modeler::lockLinkMode(bool pLock)
{
	mLinkMode = pLock;
}

/// Test if we've clicked on a gizmo
bool Modeler::isOnGizmo()
{
	return mOnGizmo;
}

/// Lock or unlock the gizmo transformation state
void Modeler::lockGizmo(bool pLock)
{
	mOnGizmo = pLock;
}

/// Event for the move gizmo
void Modeler::eventMove()
{
	if( mSelection )
	{
		//put the gizmos in the centre of selection
		mSceneManager->getSceneNode("NodeSelection")->setPosition( mSelection->getCenterPosition() );

		/*
		if(mSelection->getNumSelectedObjects() != 0)
			mSelection->mTransformation->firstClickForTransformation(e) ;
		else
			mSelection->mTransformation->eventSelection();
		*/

		mSelection->mTransformation->eventMove();
	}
}

/// Event for the rotate gizmo
void Modeler::eventRotate()
{
	if( mSelection )
	{
		mSceneManager->getSceneNode("NodeSelection")->setPosition( mSelection->getCenterPosition() );
		mSelection->mTransformation->eventRotation();
	}
}

/// Event for the scale gizmo
void Modeler::eventScale()
{
	if( mSelection )
	{
		mSceneManager->getSceneNode("NodeSelection")->setPosition( mSelection->getCenterPosition() );
		mSelection->mTransformation->eventScale();
	}
}

/// Return the selection
Selection* Modeler::getSelection()
{
	if( mSelection )
		return mSelection;

	return 0;
}

/// Load from a XML SOLIPSIS file (.sof)
bool Modeler::XMLLoad(const String& filename, Object3DPtrList& loadedObjects, Vector3 pos)
{
    if (filename.empty())
        return false;

    // Go back to the main directory
	_chdir(mExecPath.c_str());

	Path FilePath (filename) ;

	//Get current path
	size_t nameSizeChar = filename.find_last_of( '\\' );
	//size_t nameSizeChar = path.find_last_of( '/' );
	std::string texturepath (filename, 0, nameSizeChar+1);

	//get only name of file (without extension)
	size_t extPos = filename.find_last_of( '.' );
//		std::string name (FilePath.getLastFileName(false) , nameSizeChar+1, FilePath.getLastFileName(false).length());
	std::string name (FilePath.getLastFileName(false));

	// If the ext is a sof => We have to extract the file 
	Ogre::String ext = FilePath.getExtension() ;	//path.substr(path.length() - 4, path.length());
	if (ext != "sof")
        return false;

	// Go back to the main directory
	_chdir(mExecPath.c_str());

	MyZipArchive zz (filename) ;

	if(zz.isArchivePresent() )
	{
// TODO The addResourceLocation will create 1 file handle on the .sof file
// => to delete/rename/... the .sof file we have to unload this resource location first !!!!
		//Adding the zip to the ressource location and load all the medias in the zip.
		//ResourceGroupManager::getSingleton().createResourceGroup(name + "Resources");
//				ResourceGroupManager::getSingleton().addResourceLocation(FilePath.getUniversalPath(),"Zip");//, name + "Resources");
		//ResourceGroupManager::getSingleton().initialiseResourceGroup(name + "Resources");

		for( int i=0 ; i<zz.getNbFile() ; i++)	//search all XML files
		{
			Path currentFileName ( zz.getName(i) ) ;
			string ext =  currentFileName.getExtension() ;

			if ( (strcmp ( currentFileName.getExtension().c_str() , "xml")) == 0 )
			{
				//we find a XML file, so we create an object :
				FileBuffer buff = zz.readFile( zz.getName(i) );
				TiXmlDocument doc;
				doc.Parse(buff.getBufferFormatedToText().c_str());
				if (doc.Error())
				{
					SOLIPSISWARNING("Unable to read the sof file",filename.c_str());
					SOLIPSISWARNING("Error returned bu TinyXML",doc.ErrorDesc());
				}
				else
                {
                    Object3D *newObject3D = createObjectWithXML(doc, texturepath, pos) ;
                    loadedObjects.push_back(newObject3D);
                }
			}
		}

		//create all relation Parent-Children :
		for( int i=0 ; i<zz.getNbFile() ; i++)	//for each XML files
		{
			Path currentFileName ( zz.getName(i) ) ;
			string ext =  currentFileName.getExtension() ;

			if ( (strcmp ( currentFileName.getExtension().c_str() , "xml")) == 0 )
			{
				//we find a XML file, so we open it : 
				FileBuffer buff = zz.readFile( zz.getName(i) );
				TiXmlDocument doc;
				doc.Parse(buff.getBufferFormatedToText().c_str());
				
				if (doc.Error())
				{
					SOLIPSISWARNING("Unable to read the sof file",filename.c_str());
					SOLIPSISWARNING("Error returned bu TinyXML",doc.ErrorDesc());
				}
				else	//we test if this object has got parent
				{
					TiXmlElement *e = doc.RootElement()->FirstChildElement("properties");
					String parentName ;
					String currentObjName ;
					Object3D * currentObj = 0;
					Object3D * objParent = 0;
					from_string(e->FirstChildElement("objparent")->Attribute("Name"),parentName);
					from_string(e->FirstChildElement("objname")->Attribute("Name"),currentObjName);
					
					if(strcmp (parentName.c_str(), "NULL") != 0)	//if there is a parent 
					{
						currentObj = mSelection->get3DObject(currentObjName);
						objParent = mSelection->get3DObject(parentName);
						objParent->linkObject( currentObj, mSceneManager) ;
						objParent->showBoundingBox(false);
					}

					currentObj = mSelection->get3DObject(currentObjName);
					if (currentObj) 
					{
						Vector3 currentPos = currentObj->getPosition(true);
						if( pos != Vector3::ZERO )
						{
							// place this object where the avatar is
							if( objParent == 0 )
								currentPos = pos;
						}
						currentObj->setPosition( currentPos );
					}
				}	
			}
		}
	}
	else
	{
#ifdef WIN32
		std::string fileName = "File not found [";
		fileName += filename;
		fileName += "]";
		MessageBox(NULL,fileName.c_str(),"Information",MB_OK | MB_ICONINFORMATION); 
#else
		std::cerr << " You have to select an object3D " << std::endl;
#endif
		return false;	
	}

	// Go back to the main directory
	_chdir(mExecPath.c_str());

	return true;
}

/// Import a mesh file to a XML SOLIPSIS file (.sof)
bool Modeler::XMLImport(const String& filename, Vector3 pos)
{
    String filenameToLoad = filename;
    if (filenameToLoad.empty())
    {
		char *fileToLoad = FileBrowser::displayWindowForLoading( 
			" Ogre Mesh File,(*.mesh)\0*.mesh\0 3D Studio File,(*.3ds)\0*.3ds\0 Google SketchUp File,(*.skp)\0*.skp\0", string("") ); 
        if (fileToLoad != 0)
            filenameToLoad = fileToLoad;
    }

    // Go back to the main directory
	_chdir(mExecPath.c_str());

    if (!filenameToLoad.empty())
	{
		Path FilePath (filenameToLoad) ;
		Ogre::String ext = FilePath.getExtension() ;
		String entityName = FilePath.getLastFileName(false);

        //try {
        //    ResourceGroupManager::getSingleton().addResourceLocation(FilePath.getFormatedRootPath(), "FileSystem");//, name + "Resources");
        //}
        //catch (Ogre::Exception e)
        //{}

	    static int num = -1;
	    char name[31];
	    sprintf(name, "Imported%.3u",++num);

        Entity* entity = 0;
		if (!mSceneManager->hasEntity(entityName))
		{
            //ResourceGroupManager::getSingleton().addResourceLocation("C:\\3dsfiles\\", "FileSystem");
			if(ext == "mesh")
				entity = mSceneManager->createEntity( entityName, FilePath.getLastFileName(true) );
				
			else if(ext == "3ds")
			{
				entity = Plugin_3ds::createEntityFrom3ds(entityName,filenameToLoad,mSceneManager);
			}
			else if(ext == "skp")
			{
				entity = Plugin_skp::createEntityFromskp(entityName,filenameToLoad,mSceneManager);
			}
		}
        entity = mSceneManager->getEntity(entityName)->clone(name);
	    SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
#ifdef SHADOWS
	    entity->setCastShadows(true);
#endif
        entity->setQueryFlags(Navigator::QFObject);
	    node->attachObject(entity);
		Vector3 size = entity->getBoundingBox().getSize();

		Ogre::Real mNormalise = (size.x>=size.y ? size.x : size.y)>=size.z ? (size.x>=size.y?size.x:size.y) : size.z;
		node->scale(4.0/mNormalise,4.0/mNormalise,4.0/mNormalise);//standardize the models loaded.


		//Object3DOther* obj = new Object3DOther( String(name), node );
		//mSelection->add3DObject(obj);
		//obj->mCentreSelection = pos;

	    node->setPosition(pos);
    }
	return true;
}

/// Save to a XML SOLIPSIS file (.sof)
bool Modeler::XMLSave(bool all, const char* pathToSave)
{
	MyZipArchive* zz;

	// Go back to the main directory
	_chdir(mExecPath.c_str());

	//Create Path :
    std::string strCompleteFileName ( std::string(pathToSave) + "\\solTmpObject.sof" );
	Path FilePath (	strCompleteFileName );

	//Get only path
	size_t nameSizeChar = strCompleteFileName.find_last_of( '\\' );
	//size_t nameSizeChar = strCompleteFileName.find_last_of( '/' );
	std::string directory (strCompleteFileName, 0, nameSizeChar+1);

	//get only name of file (without extension)
	size_t extPos = strCompleteFileName.find_last_of( '.' );
//		std::string name (FilePath.getLastFileName(false) , nameSizeChar+1, FilePath.getLastFileName(false).length());
	std::string name (FilePath.getLastFileName(false));

	//Save all objects in this scene
	if (!isSelectionEmpty())
	{
		Object3DPtrList listObj, tmp1, tmp2;
        Object3D* obj = mSelection->getFirstSelectedObject();
        while (obj != 0)
        {
            tmp1.clear();
		    tmp1.push_back( obj );
		    while( !tmp1.empty() )
		    {
			    Object3DPtrList::iterator itList;
			    for( itList=tmp1.begin(); itList!=tmp1.end(); itList++)
			    {
                    Object3DPtrList::iterator itListCheck;
		            for(itListCheck=listObj.begin(); itListCheck!=listObj.end(); itListCheck++)
                        if (*itListCheck == *itList)
                            break;
                    if (itListCheck == listObj.end())
                        listObj.push_back( (*itList) );
				    tmp2.push_back( (*itList) );
			    }
			    tmp1.clear();
			    for( itList=tmp2.begin(); itList!=tmp2.end(); itList++)
			    {
				    std::vector<Object3D *> *listChild = (*itList)->getChildren();
				    if( listChild != 0 )
				    {
					    std::vector<Object3D *>::iterator it;
					    for(it=listChild->begin() ; it != listChild->end() ; it++ )
						    tmp1.push_back( (*it) );
				    }
			    }
			    tmp2.clear();
		    }
            obj = mSelection->getNextSelectedObject();
        }

		//Save SOF :
		Ogre::String fileZipToSave = directory + name + Ogre::String(".sof"); 
		zz = new MyZipArchive(fileZipToSave.c_str());

		if( zz->isArchivePresent() )	//if this archive is already present ...
			for (int i=zz->getNbFile(); i>=0  ; i--)	//...remove all files :
				zz->removeFile(zz->getName(i)) ;

		Object3DPtrList::iterator itr ;
		for(itr=listObj.begin() ; itr != listObj.end() ; itr++ )
		{
			// Update command list with the last called 
			updateCommand(Object3D::NONE, (*itr) );

			//Save object in XML :
			Ogre::String fileToSave = directory + (*itr)->getName() + Ogre::String(".xml");
			(*itr)->saveToFile(fileToSave.c_str());
			zz->writeFile(fileToSave);

            if (!(*itr)->mCommandList.empty())
            {
    			(*itr)->mCommandList.pop_back();
				list<Object3D::TCommand>::iterator cmd = (*itr)->mCommandList.end();
                cmd--;
			    const Object3D::Command command = (*cmd).first;
			    (*itr)->mCommandLast = command;
            }

			//Save textures :
#ifdef WIN32
			CreateDirectory( "solTmpTexture", NULL );
#else
			system( "md solTmpTexture" );
#endif
			std::string texturePath ;
			for (int i=1; i< (*itr)->getMaterialManager()->getNbTexture(); i++)	//begin to 1 to do not save the default texture !
			{
				texturePath = (*itr)->getMaterialManager()->getTexture(i)->getName();

				Path path(texturePath);
				size_t nameSizeChar = path.getFormatedPath().find_last_of( '\\' );
				std::string fileName (path.getFormatedPath(), nameSizeChar+1, path.getFormatedPath().length() );

				TexturePtr Texture = TextureManager::getSingleton().getByName(texturePath);
                // Extended texture
                TextureExtParamsMap *textureExtParamsMap = (*itr)->getMaterialManager()->getTextureExtParamsMap(Texture);
                if (textureExtParamsMap == 0)
                {
    				String str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);
				    std::string newFile( "solTmpTexture\\" + fileName );
    				
				    Ogre::Image image;
				    image.load( texturePath, str);
				    image.save( newFile );

				    if ( ! zz->isFilePresent( texturePath ) )
					    zz->writeFile( newFile );

				    SOLdeleteFile( newFile.c_str() );
                }
			}
#ifdef WIN32
//				RemoveDirectory( "solTmpTexture" );
#else
//				system( "rm solTmpTexture -r" );
#endif		
			SOLdeleteFile(fileToSave.c_str());
		}

        if (mModelerCallbacks != 0)
            mModelerCallbacks->OnObject3DListSave(fileZipToSave, listObj);

		listObj.clear();
	}

	// Go back to the main directory
	_chdir(mExecPath.c_str());

//#ifdef WIN32
//	MessageBox(NULL,"Handle File SAVE","Information",MB_OK | MB_ICONINFORMATION); 
//#else
//	std::cerr << " Handle File SAVE " << std::endl;
//#endif
	return true;
}

/// Create a new Object3D with a file XML
Object3D * Modeler::createObjectWithXML(TiXmlDocument doc, string path, Vector3 pos)
{
	Ogre::String primType = doc.RootElement()->FirstChildElement("model")->FirstChildElement("primitive")->Attribute("Name");
	Object3D::Type type = objectStringToType(primType);

	switch (type) 
	{
		case Object3D::BOX :
			createBox(pos);
			break;
		case Object3D::CORNER :
			createCorner(pos);
			break;
		case Object3D::PYRAMID :
			createPyramid(pos);
			break;
		case Object3D::PRISM :
			createPrism(pos);
			break;
		case Object3D::CYLINDER :
			createCylinder(pos);
			break;
		case Object3D::HALF_CYLINDER :
			createHalfCyl(pos);
			break;
		case Object3D::CONE :
			createCone(pos);
			break;
		case Object3D::HALF_CONE :
			createHalfCone(pos);
			break;
		case Object3D::SPHERE :
			createSphere(pos);
			break;
		case Object3D::HALF_SPHERE :
			createHalfSphere(pos);
			break;
		case Object3D::RING :
			createRing(pos);
			break;
		case Object3D::TORUS :
			createTorus(pos);
			break;
		case Object3D::TUBE :
			createTube(pos);
			break;
	}

	Object3D * newObject ;
	newObject = mSelection->geLastAddedObject();

	//Test the name of this object :
		//get the name of the new object
	TiXmlElement *XMLfile = doc.RootElement()->FirstChildElement("properties");
	String testName = XMLfile->FirstChildElement("objname")->Attribute("Name");
		//search if an object has already this name 
	Object3D * ObjectWithSameName = mSelection->get3DObject( testName ) ;
	if ( ObjectWithSameName != 0 )
	{		
		SOLIPSISWARNING("ERROR when open file, this name already exists. The object are automaticly renamed.",testName.c_str());
		
		//Make a new name for the object :
		do 
		{
			testName += "_" ;
		}
		while(mSelection->get3DObject( testName ) != 0) ;

		//rename the old object : 
		ObjectWithSameName->setName( testName );
		//We rename the object already present in the scene, because in the XML file,
		//	children have recover their parent with their name ! So if we change the name
		//	of the new object, we won't assign correct children
	}

	newObject->loadFromFile(doc, path.c_str());


	// Go back to the main directory
	_chdir(mExecPath.c_str());

	return newObject ;
}

/// Dedicated callback texture loader
TexturePtr Modeler::loadTexture(ModifiedMaterialManager* modifiedMaterialManager, Entity* entity, const String& name, const TextureExtParamsMap& textureExtParamsMap)
{
    TexturePtr texture;
    if (textureExtParamsMap.empty())
        texture = TextureManager::getSingleton().load(name , ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    else
    {
        ModifiedMaterial* modifiedMaterial = modifiedMaterialManager->getModifiedMaterial();
        String mtlName = modifiedMaterial->getOwner()->getName();
        TextureExtParamsMap::const_iterator it = textureExtParamsMap.find("plugin");
        if (it == textureExtParamsMap.end())
            return texture;
        String plugin = it->second;
        if (plugin == "www")
        {
            String url;
            int width , height, fps;
            TextureExtParamsMap::const_iterator it;
            it = textureExtParamsMap.find("url");
            if (it == textureExtParamsMap.end())
                return texture;
            url = it->second;
            it = textureExtParamsMap.find("width");
            if (it == textureExtParamsMap.end())
                return texture;
            width = atoi(it->second.c_str());
            it = textureExtParamsMap.find("height");
            if (it == textureExtParamsMap.end())
                return texture;
            height = atoi(it->second.c_str());
            it = textureExtParamsMap.find("frames_per_second");
            if (it == textureExtParamsMap.end())
                return texture;
            fps = atoi(it->second.c_str());
            NaviLibrary::Navi* naviWWWTexture = NaviLibrary::NaviManager::Get().createNaviMaterial("WWW_" + entity->getName(), url, width, height, FO_ANISOTROPIC, mtlName);
            naviWWWTexture->show(true);
            naviWWWTexture->setMaxUPS(fps);
            naviWWWTexture->setForceMaxUpdate(fps != 0);
            naviWWWTexture->setOpacity(1.0f);
        }
        else
        {
            ExternalTextureSourceManager::getSingleton().setCurrentPlugIn(plugin);
            ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource(plugin);
            for(TextureExtParamsMap::const_iterator it=textureExtParamsMap.begin();it!=textureExtParamsMap.end();++it)
                vlcExtTextSrc->setParameter(it->first, it->second);
            vlcExtTextSrc->createDefinedTexture(mtlName);
        }
        // here we refresh internal variables on tech, pass, text unit because
        // the texture source plugin maybe updated them
        modifiedMaterial->refreshTechPassTextUnit();
        // addding specific query flags
        it = textureExtParamsMap.find("query_flags");
        if (it != textureExtParamsMap.end())
            entity->addQueryFlags(StringConverter::parseUnsignedInt(it->second));
        // retrieving the texture from the refreshed texture unit of the material
        texture = TextureManager::getSingleton().getByName(modifiedMaterial->getTextureName());
    }
    return texture;
}

void Modeler::releaseTexture(ModifiedMaterialManager* modifiedMaterialManager, const String& name, const TextureExtParamsMap& textureExtParamsMap)
{
    if (textureExtParamsMap.empty())
        return;
    ModifiedMaterial* modifiedMaterial = modifiedMaterialManager->getModifiedMaterial();
    String mtlName = modifiedMaterial->getOwner()->getName();
    TextureExtParamsMap::const_iterator it = textureExtParamsMap.find("plugin");
    if (it == textureExtParamsMap.end())
        return;
    String plugin = it->second;
    if (plugin == "www")
    {
        NaviManager &naviMgr = NaviLibrary::NaviManager::Get();
        Navi *navi = naviMgr.getNaviFromMtlName(mtlName);
        if (navi == 0)
    		SOLIPSISWARNING("ERROR when releasing navi texture. The navi cannot be retrieved from the material name.", "");
        else
            naviMgr.destroyNavi(navi->getName());
    }
    else
    {
        ExternalTextureSourceManager::getSingleton().destroyAdvancedTexture(mtlName);
    }
}

/// Update the command list
bool Modeler::updateCommand(Object3D::Command pCommand, Object3D* pObject)
{
	Object3D::TCommand cmdNew;
	cmdNew.first = pCommand;
	cmdNew.second = Vector3(-1,-1,-1);

	Object3D::Command cmdOld;
	return pObject->addCommand( cmdNew, cmdOld );
}

} // namespace Solipsis