#include "ModuleGUI.h"

ModuleGUI::ModuleGUI(SceneManager* pSceneMgr, Camera* pCamera)
{
	mSceneMgr = pSceneMgr;
	mCamera = pCamera;
}

//-------------------------------------------------------------------------------------
ModuleGUI::~ModuleGUI()
{
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::createGUI( RenderWindow* pWindow )
{
	/* ### EXAMPLE ###
	// Set up GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(pWindow, 
		Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);
	mGUISystem = new CEGUI::System(mGUIRenderer);

	// This is where your CEGUI log is created. It is currently set to Informative. There are four settings: Standard, Errors, Informative and Insane.
	CEGUI::Logger::getSingleton().setLoggingLevel(
		CEGUI::Informative);

	// Creates a new CEGUI System, using the 'Taharez Look' for the scheme and mouse cursor, with 'BlueHighway-12' for the font.
	CEGUI::SchemeManager::getSingleton().loadScheme(
		(CEGUI::utf8*)"TaharezLookSkin.scheme");
	mGUISystem->setDefaultMouseCursor(
		(CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem->setDefaultFont(
		(CEGUI::utf8*)"BlueHighway-12");

	CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().loadWindowLayout(
		(CEGUI::utf8*)"mii.layout"); 
	mGUISystem->setGUISheet(sheet);
	*/
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::setupEventHandlers(void)
{
	/* ### EXAMPLE ###
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	wmgr.getWindow((CEGUI::utf8*)"cmdCreateBox")->subscribeEvent(
		CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&MiiApp::handleQuit, this));
	*/
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::handleQuit(const CEGUI::EventArgs& e)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::mouseMoved( const OIS::MouseEvent &e )
{
	/* ### EXAMPLE ###
	static_cast<MiiFrameListener*>(mFrameListener)->requestShutdown();
	*/
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::keyPressed( const OIS::KeyEvent &e )
{
	return true;
}

//-------------------------------------------------------------------------------------
bool ModuleGUI::keyReleased( const OIS::KeyEvent &e )
{
	return true;
}
