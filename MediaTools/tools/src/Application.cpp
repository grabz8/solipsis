#include "Application.h"

//-------------------------------------------------------------------------------------
Application::Application()
	: mRoot(0),
	mTranslateVector(Vector3::ZERO), mStatsOn(true), mNumScreenShots(0),
	mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
	mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36), mDebugOverlay(0),
	mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
{
}

//-------------------------------------------------------------------------------------
Application::~Application()
{
	//Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);

	delete mRoot;
}

//-------------------------------------------------------------------------------------
void Application::go()
{
	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}

//-------------------------------------------------------------------------------------
bool Application::setup()
{
	// if we cannot initialise Ogre, just abandon the whole deal
	if ( !configure() ) 
		return false;

	createSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	createResourceListener();

	// Initialise resources
	initResources();

	// Create the scene
	createScene();

	// Create the frame listener
	createFrameListener();

	return true;
}

//-------------------------------------------------------------------------------------
bool Application::configure()
{
	// Try to restore the old config first...
	if (mRoot->restoreConfig())
	{
		mWindow = mRoot->initialise(true);
		return true;
	}

	// Show the configuration dialog and initialise the system
	if(mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true);
		return true;
	}
	else
	{
		mRoot->shutdown();
		return false;
	}
}

//-------------------------------------------------------------------------------------
void Application::createSceneManager()
{
	// Create the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC);
}

//-------------------------------------------------------------------------------------
void Application::createCamera()
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Vector3(0,0,0));
	// Look back along -Z
	mCamera->lookAt(Vector3(0,0,0));
	mCamera->setNearClipDistance(5);
}

//-------------------------------------------------------------------------------------
void Application::createViewports()
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

//-------------------------------------------------------------------------------------
void Application::createResourceListener()
{
}

//-------------------------------------------------------------------------------------
void Application::initResources()
{
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-------------------------------------------------------------------------------------
void Application::destroyScene()
{
}

//-------------------------------------------------------------------------------------
bool Application::frameStarted(const FrameEvent& evt)
{
	// ..
	if(!this->construct(evt))
		return false;

	// input
	if( mMouse )
		mMouse->capture();
	if( mKeyboard ) 
		mKeyboard->capture();

	mCamNode->translate( mCamNode->getOrientation() *
		mCamNode->getChild( 0 )->getOrientation() *		// comment this line ?
		mDirection * evt.timeSinceLastFrame );

	return mContinue;
}

//-------------------------------------------------------------------------------------
bool Application::frameEnded(const FrameEvent& evt)
{
	updateStats();
	return true;
}

//-------------------------------------------------------------------------------------
void Application::createFrameListener()
{
//	mFrameListener = this::FrameListener();
//	mRoot->addFrameListener(mFrameListener);

	mRoot->addFrameListener(this);

	using namespace OIS;

	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, true ));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, true ));
	
	// Delete these two lines and uncomment the try.... catch to re-enable joystick detection
	LogManager::getSingletonPtr()->logMessage("Solipsis : No joystick available at the moment in the MII App");
	mJoy = 0;	
	/*try {
		mJoy = static_cast<JoyStick*>(mInputManager->createInputObject( OISJoyStick, false ));
	}
	catch(...) {
		mJoy = 0;
	}*/

	//Set initial mouse clipping size
	windowResized(mWindow);

#ifdef _DEBUG
    showDebugOverlay(true);
#endif

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	// Populate the camera and scene manager containers
	mCamNode = mCamera->getParentSceneNode( )->getParentSceneNode( );

	// set the rotation and move speed
	mRotate = 0.15;
	mMove = 250;
	mDirection = Vector3::ZERO;

	// continue rendering
	mContinue = true;

	// register the MiiFrameListener as the listener by calling the setEventCallback method on these input objects
	//mEventProcessor->addKeyListener( this );
	mMouse->setEventCallback( this );
	mKeyboard->setEventCallback( this );

	// CEGUI
//	mGUIRenderer = (CEGUI::OgreCEGUIRenderer *)renderer;
//	mShutdownRequested = false;
}

//-------------------------------------------------------------------------------------
bool Application::mouseMoved( const OIS::MouseEvent &e ) { return true; }
bool Application::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ){return true; }
bool Application::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) { return true; }
bool Application::keyPressed( const OIS::KeyEvent &e ) { return true; }
bool Application::keyReleased( const OIS::KeyEvent &e ) { return true; }
//-------------------------------------------------------------------------------------
bool Application::construct(const FrameEvent& evt)
{
	using namespace OIS;

	if(mWindow->isClosed())	return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	if( mJoy ) mJoy->capture();

	bool buffJ = (mJoy) ? mJoy->buffered() : true;

	//Check if one of the devices is not buffered
	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
	{
		// one of the input modes is immediate, so setup what is needed for immediate movement
		if (mTimeUntilNextToggle >= 0)
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			mMoveScale = 1;
			mRotScale = 0.1;
		}
		// Otherwise scale movement units by time passed since last frame
		else
		{
			// Move about 100 units per second,
			mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
			// Take about 10 seconds for full rotation
			mRotScale = mRotateSpeed * evt.timeSinceLastFrame;
		}
		mRotX = 0;
		mRotY = 0;
		mTranslateVector = Ogre::Vector3::ZERO;
	}

	//Check to see which device is not buffered, and handle it
/*	if( !mKeyboard->buffered() )
		if( processUnbufferedKeyInput(evt) == false )
			return false;
	if( !mMouse->buffered() )
		if( processUnbufferedMouseInput(evt) == false )
			return false;

	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
		moveCamera();
*/
	return true;
}

void Application::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }
}

//Adjust mouse clipping area
void Application::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void Application::windowClosed(RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );
			mInputManager->destroyInputObject( mJoy );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

void Application::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}