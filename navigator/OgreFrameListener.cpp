#include "OgreFrameListener.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------

// Constructor takes a RenderWindow because it uses that to determine input context
OgreFrameListener::OgreFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr) :
        mCamera(cam), mTranslateVector(Vector3::ZERO), mWindow(win), mStatsOn(true), mNumScreenShots(0),
        mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
        mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36), mDebugOverlay(0),
        mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0) {

        using namespace OIS;

#if OGRE_DEBUG_MODE == 1
        //Debug overlay
        mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

        mRotateSpeed = 36;
        mMoveSpeed = 100;

        mStatsOn = true;
        mNumScreenShots = 0;
        mTimeUntilNextToggle = 0;
        mSceneDetailIndex = 0;
        mMoveScale = 0.0f;
        mRotScale = 0.0f;
        mTranslateVector = Vector3::ZERO;
        mAniso = 1;
        mFiltering = TFO_BILINEAR;
        showDebugOverlay(true);
#endif

        OGRE_LOG("*** Initializing OIS ***");

        ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        mInputManager = InputManager::createInputSystem(pl);

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject(OISKeyboard, true));
        mMouse = static_cast<Mouse*>(mInputManager->createInputObject(OISMouse, true));
        try {
            mJoy = static_cast<JoyStick*>(mInputManager->createInputObject(OISJoyStick, true));
        }
        catch(...) {
            mJoy = 0;
        }

        //Set initial mouse clipping size
        windowResized(mWindow);

#if OGRE_DEBUG_MODE == 1
        showDebugOverlay(true);
#endif


        // Populate the camera and scene manager containers
        mCamNode = cam->getParentSceneNode()->getParentSceneNode();

        mSceneMgr = sceneMgr;

        mRotate = 0.13;
        mMove = 250;
        mDirection = Vector3::ZERO;
        mContinue = true; // continue rendering

        //Register as a Window listener
        WindowEventUtilities::addWindowEventListener(mWindow, this);

        //OIS Listeners
        mMouse->setEventCallback(this);
        mKeyboard->setEventCallback(this);

    }

//-------------------------------------------------------------------------------------

    OgreFrameListener::~OgreFrameListener() {
        //Remove ourself as a Window listener
        WindowEventUtilities::removeWindowEventListener(mWindow, this);
        windowClosed(mWindow);
    }

//-------------------------------------------------------------------------------------



    //-----------------------------------------------------------------------------//
    //--                                                                         --//
    //--                           FrameListener                                 --//
    //--                                                                         --//
    //-----------------------------------------------------------------------------// 



//-------------------------------------------------------------------------------------

    // Override frameStarted event to process that (don't care about frameEnded)
    bool OgreFrameListener::frameStarted(const FrameEvent& evt)
    {
       if (mMouse)
           mMouse->capture();
       if (mKeyboard)
           mKeyboard->capture();

        return mContinue;
    }

//-------------------------------------------------------------------------------------

    bool OgreFrameListener::frameEnded(const FrameEvent& evt)
    {
#if OGRE_DEBUG_MODE == 1
        updateStats();
#endif
        return true;
    }

//-------------------------------------------------------------------------------------



   //-----------------------------------------------------------------------------//
   //--                                                                         --//
   //--                           OIS::MouseListener                            --//
   //--                                                                         --//
   //-----------------------------------------------------------------------------// 



//-------------------------------------------------------------------------------------

   bool OgreFrameListener::mouseMoved(const OIS::MouseEvent &e) {

       using namespace OIS;

       mCamNode->yaw(Degree(-mRotate * e.state.X.rel));
       mCamNode->getChild(0)->pitch(Degree(-mRotate * e.state.Y.rel));

       return true;
   }


//-------------------------------------------------------------------------------------

   bool OgreFrameListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {
       return true;
   }

//-------------------------------------------------------------------------------------

   bool OgreFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { 
       return true;
   }

//-------------------------------------------------------------------------------------


   //-----------------------------------------------------------------------------//
   //--                                                                         --//
   //--                           OIS::KeyListener                              --//
   //--                                                                         --//
   //-----------------------------------------------------------------------------// 



//-------------------------------------------------------------------------------------

   bool OgreFrameListener::keyPressed(const OIS::KeyEvent &e) { 
       using namespace OIS;
       switch (e.key) {
           case KC_ESCAPE: 
               mContinue = false;
               break;
       }
       return true;
   }

//-------------------------------------------------------------------------------------
   bool OgreFrameListener::keyReleased(const OIS::KeyEvent &e) {
       using namespace OIS;
/*       switch (e.key)
       {
       default:
           break;
       }*/
       return true;
   }

//-------------------------------------------------------------------------------------



    //-----------------------------------------------------------------------------//
    //--                                                                         --//
    //--                           WindowEventListener                           --//
    //--                                                                         --//
    //-----------------------------------------------------------------------------// 



//-------------------------------------------------------------------------------------


    //Adjust mouse clipping area
    void OgreFrameListener::windowResized(RenderWindow* rw)
    {
        unsigned int width, height, depth;
        int left, top;
        rw->getMetrics(width, height, depth, left, top);

        const OIS::MouseState &ms = mMouse->getMouseState();
        ms.width = width;
        ms.height = height;
    }

//-------------------------------------------------------------------------------------

    //Unattach OIS before window shutdown (very important under Linux)
    void OgreFrameListener::windowClosed(RenderWindow* rw)
    {
        //Only close for window that created OIS (the main window in these demos)
        if (rw == mWindow)
        {
            if (mInputManager)
            {
                mInputManager->destroyInputObject(mMouse);
                mInputManager->destroyInputObject(mKeyboard);
                mInputManager->destroyInputObject(mJoy);

                OIS::InputManager::destroyInputSystem(mInputManager);
                mInputManager = 0;
            }
        }
    }

//-------------------------------------------------------------------------------------
void OgreFrameListener::pushOIS(OIS::KeyListener* keyListener, OIS::MouseListener* mouseListener)
{
    keyListenersStack.push(mKeyboard->getEventCallback());
    mouseListenersStack.push(mMouse->getEventCallback());
    mKeyboard->setEventCallback(keyListener);
    mMouse->setEventCallback(mouseListener);
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::popOIS()
{
    mKeyboard->setEventCallback(keyListenersStack.top());
    mMouse->setEventCallback(mouseListenersStack.top());
    keyListenersStack.pop();
    mouseListenersStack.pop();
}

//-------------------------------------------------------------------------------------
   
 
    void OgreFrameListener::showDebugOverlay(bool show) {
        if (mDebugOverlay) {
            if (show)
                mDebugOverlay->show();
            else
                mDebugOverlay->hide();
        }
    }

//-------------------------------------------------------------------------------------

void OgreFrameListener::requestShutDown() {
    mContinue = false;
}

//-------------------------------------------------------------------------------------

    void OgreFrameListener::updateStats(void) {

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
                + " " + StringConverter::toString(stats.bestFrameTime) + " ms");
            guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
                + " " + StringConverter::toString(stats.worstFrameTime) + " ms");

            OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
            guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

            OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
            guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

            OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
            guiDbg->setCaption(mDebugText);
        }
        catch(...) { /* ignore */ }
    }

/*
pushOIS, popOIS could be used to switch to another Ogre module
with its own listeners (frame, keyboard, mouse) and its scene manager,
you have to destroy/recreate camera+viewports to switch between scene managers

Module::Module(Navigator* navigator) :
    mNavigator(navigator),
    mSceneMgr(0),
    mCamera(0) {}
Module::~Module() {}
bool Module::initialise()
{
    createSceneManager();
    createCamera();
    createViewports();

    // Create the scene
    createScene();

    return true;
}
bool Module::shutdown()
{
    // Destroy the scene
    destroyScene();

    destroyViewports();
    destroyCamera();
    destroySceneManager();

    return true;
}
Navigator* Module::getNavigator()
{
    return mNavigator;
}
void Module::createSceneManager()
{
    // Create the SceneManager, in this case a generic one
    mSceneMgr = Root::getSingletonPtr()->createSceneManager(ST_GENERIC);
}
void Module::destroySceneManager()
{
    // Destroy the SceneManager
    Root::getSingletonPtr()->destroySceneManager(mSceneMgr);
    mSceneMgr = 0;
}
void Module::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("ModuleCam");
    // Position it at 500 in Z direction
    mCamera->setPosition(Vector3(0,0,0));
    // Look back along -Z
    mCamera->lookAt(Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);
}
void Module::destroyCamera()
{
    mSceneMgr->destroyCamera(mCamera);
}
void Module::createViewports()
{
    // Create one viewport, entire window
    Viewport* vp = mNavigator->getRenderWindowPtr()->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
}
void Module::destroyViewports()
{
    mNavigator->getRenderWindowPtr()->removeViewport(mCamera->getViewport()->getZOrder());
}
void Module::createScene() {}
void Module::destroyScene() {}

SWITCH
    // Add module listener
    Root::getSingletonPtr()->addFrameListener(mModuleFrameListener);
    mFrameListener->pushOIS(mModuleFrameListener, mModuleFrameListener);

    // Hide scene
    ((NavigatorFrameListener*)mFrameListener)->detachCamera();
    // Save camera
    mSavedCameraPos = mCamera->getPosition();
    mSavedCameraQuat = mCamera->getOrientation();
    mSavedCameraNearClipDistance = mCamera->getNearClipDistance();
    mWindow->removeViewport(mCamera->getViewport()->getZOrder());
    mSceneMgr->destroyCamera(mCamera);

    mModule->initialise();
BACK
    mModule->shutdown();

    // Display scene
    // Create the camera
    mCamera = mSceneMgr->createCamera("UserCam");
    mCamera->setPosition(mSavedCameraPos);
    mCamera->setOrientation(mSavedCameraQuat);
    mCamera->setNearClipDistance(mSavedCameraNearClipDistance);
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
    mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
    mFrameListener->setCamera(mCamera);
    ((NavigatorFrameListener*)mFrameListener)->attachCamera();

    // Remove module listener
    mFrameListener->popOIS();
    Root::getSingletonPtr()->removeFrameListener(mModuleFrameListener);
*/