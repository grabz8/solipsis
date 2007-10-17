#include "MiiApp.h"
#include "Selection.h"
#include "Transformations.h"
#include "Camera.h"
#include "MiiModule.h"

//-------------------------------------------------------------------------------------
MiiApp::MiiApp(void)
{
	// Setup default variables

	mKey_LCTRL = false;
	mKey_LALT = false;

	//mouse's parameters :
	mMouse_Speed = 1.3 ;

	mSelection = NULL;
	mMustDeleteConfigFile = false;
}

//-------------------------------------------------------------------------------------
MiiApp::~MiiApp()
{
	if (mSelection)
		delete mSelection;
	mRoot->saveConfig();
}

void MiiApp::init(const char* resName)
{
	String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB

#ifdef _DEBUG
	pluginsPath = mResourcePath + "plugins.cfg";
#else
	pluginsPath = mResourcePath + "pluginsR.cfg";
#endif

#endif

	mRoot = new Root(pluginsPath, 
		mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

	// Load resource paths from config file
	ConfigFile cf;
	cf.load(resName);

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app,
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location
			ResourceGroupManager::getSingleton().addResourceLocation(
				String(macBundlePath() + "/" + archName), typeName, secName);
#else
			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
#endif
		}
	}
}

//-------------------------------------------------------------------------------------
void MiiApp::createScene(void)
{
	Entity *ent = NULL;
	SceneNode *node = NULL;

	mSceneMgr->setAmbientLight( ColourValue( 0.5, 0.5, 0.5 ) );
	
	//---------------------------------------------------------------------------------------
	//		Light :
	//---------------------------------------------------------------------------------------
	Light *light = mSceneMgr->createLight( "Light1" );
	light->setType( Light::LT_POINT );
	light->setPosition( Vector3(0, 750, 1000) );
	light->setDiffuseColour( ColourValue( 0.8, 0.8, 0.8 ) );
	light->setSpecularColour( ColourValue( 0.8, 0.8, 0.8 ));

	//---------------------------------------------------------------------------------------
	//		Selection :
	//---------------------------------------------------------------------------------------
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeCentreRotation");
	SceneNode * node2 = node->createChildSceneNode("NodeCentreObject");
	mSelection = new Selection (node, node2) ;

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeSelection");	//for find all objects selected



	//---------------------------------------------------------------------------------------
	//		Camera
	//---------------------------------------------------------------------------------------
	mCameraManagement = new CCamera (mCamera, mSceneMgr);
	

	//---------------------------------------------------------------------------------------
	//		Add a ground plane :
	//---------------------------------------------------------------------------------------
	ent = mSceneMgr->createEntity( "Plane", "Prefab_Plane" );
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode( "PlaneNode" );
	node->attachObject( ent );

	Object3DOther* obj = new Object3DOther("Plane", node );
	mSelection->add3DObject(obj,false);

	node->pitch(Degree(-90));
	node->scale(2,2,2);

	ent->setVisible(true);

	//---------------------------------------------------------------------------------------
	//		Other
	//---------------------------------------------------------------------------------------

	// Create RaySceneQuery
	mRaySceneQuery = mSceneMgr->createRayQuery( Ray() );

	// setup GUI system
	mii = new MiiModule(mSceneMgr, mCamera,mSelection);
	mii->createGUI(mWindow);

	//create and put gizmos in the centre of selection
	node = mSceneMgr->getSceneNode("NodeSelection");
	mSelection->mTransformation->createGizmos( node , mSceneMgr, mCamera );	

	mii->setupEventHandlers();
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Tools/DispConfiguration")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiApp::reconfigureDisplay, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/File/Exit")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiApp::handleFileExit, this));

	mCamera->getParentSceneNode()->attachObject( light );
}

//-------------------------------------------------------------------------------------
void MiiApp::destroyScene(void)
{
	// We created the query, and we are also responsible for deleting it.
	delete mRaySceneQuery;
}

//-------------------------------------------------------------------------------------
bool MiiApp::frameStarted(const FrameEvent& evt)
{
	return Application::frameStarted(evt);
}

#include "OgreConfigDialog.h"

//-------------------------------------------------------------------------------------
bool MiiApp::reconfigureDisplay(const CEGUI::EventArgs& e)
{
#ifdef WIN32
	MessageBox(NULL,"Ogre will ask you how you want to configure it the next time you start","Information", MB_OK | MB_ICONINFORMATION);
#else
	std::cerr << "Ogre will ask you how you want to configure it the next time you start..." << endl;
#endif 
	mMustDeleteConfigFile = true;
	return false;
}

//-------------------------------------------------------------------------------------
bool MiiApp::handleFileExit(const CEGUI::EventArgs& e)
{
	mRoot->queueEndRendering();
	return true;
}