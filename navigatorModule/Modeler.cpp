/**
	\file 
		Modeler.cpp
	\brief
		Solipsis Imperative Intuitive Modeler
	\author
		ARTEFACTO 2007 - Stéphane CHAPLAIN
*/

#include "Modeler.h"
#include "../MdlrTools/include/Selection.h"

namespace Solipsis {

Modeler::Modeler(SceneManager* pSceneMgr, Camera* pCamera)
{
	mSceneManager = pSceneMgr;
	mCamera = pCamera;
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
	mSelection = NULL;

}

bool Modeler::init(Avatar * playerAvatar)
{
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

	// Init generic primitives to clone when the creation will be called
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

	return true; // Success
}


bool Modeler::createBox(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Box%.3u",++num);

	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DBox* obj = new Object3DBox( String(name), node );
	mSelection->add3DObject(obj);
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
	node->attachObject( entity );

	Object3DCorner* obj = new Object3DCorner( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DPyramid* obj = new Object3DPyramid( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DPrism* obj = new Object3DPrism( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DCylinder* obj = new Object3DCylinder( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DHalfCylinder* obj = new Object3DHalfCylinder( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DCone* obj = new Object3DCone( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DHalfCone* obj = new Object3DHalfCone( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DSphere* obj = new Object3DSphere( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DHalfSphere* obj = new Object3DHalfSphere( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DTorus* obj = new Object3DTorus( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DTube* obj = new Object3DTube( String(name), node );
	mSelection->add3DObject(obj);

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
	node->attachObject( entity );

	Object3DRing* obj = new Object3DRing( String(name), node );
	mSelection->add3DObject(obj);

	node->setPosition(player_pos);
	return true;
}

/// Create a mesh. 
bool Modeler::createMesh(Vector3	&player_pos)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Box%.3u",++num);

	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneManager->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneManager->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DOther* obj = new Object3DOther( String(name), node );
	mSelection->add3DObject(obj);

	node->setPosition(player_pos);
	return true;
}


} // namespace Solipsis