/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "Prerequisites.h"
#include "OgreFrameListener.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgreFrameListener::OgreFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr) :
    mCamera(cam), mTranslateVector(Vector3::ZERO), mWindow(win), mStatsOn(true), mNumScreenShots(0),
    mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
    mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36), mDebugOverlay(0),
    mKeyboardListener(0), mMouseListener(0)
{
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

#if OGRE_DEBUG_MODE == 1
    showDebugOverlay(true);
#endif

    // Populate the camera and scene manager containers
    if ((cam != 0) && (cam->getParentSceneNode() != 0))
        mCamNode = cam->getParentSceneNode()->getParentSceneNode();

    mSceneMgr = sceneMgr;

    mRotate = 0.13;
    mMove = 250;
    mDirection = Vector3::ZERO;
    mContinue = true; // continue rendering
}

//-------------------------------------------------------------------------------------
OgreFrameListener::~OgreFrameListener()
{
}

//-------------------------------------------------------------------------------------
bool OgreFrameListener::frameStarted(const FrameEvent& evt)
{
    // Override frameStarted event to process that (don't care about frameEnded)
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
bool OgreFrameListener::keyPressed(const KeyboardEvt& evt)
{ 
    switch (evt.mKey)
    {
    //case KC_A:        // removed to control the avatar's moves
    case KC_ESCAPE: 
        mContinue = false;
        break;
    }
    return true;
}

//-------------------------------------------------------------------------------------
bool OgreFrameListener::keyReleased(const KeyboardEvt& evt)
{ 
    return true;
}

//-------------------------------------------------------------------------------------
bool OgreFrameListener::mouseMoved(const MouseEvt& evt)
{
    if (mCamNode == 0)
        return true;

    mCamNode->yaw(Degree(-mRotate*evt.mState.mXrel));
    mCamNode->getChild(0)->pitch(Degree(-mRotate*evt.mState.mYrel));

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreFrameListener::mousePressed(const MouseEvt& evt)
{
    return true;
}

//-------------------------------------------------------------------------------------
bool OgreFrameListener::mouseReleased(const MouseEvt& evt)
{ 
    return true;
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::pushListeners(KeyboardEventListener* keyListener, MouseEventListener* mouseListener)
{
    mKeyListenersStack.push(mKeyboardListener);
    mMouseListenersStack.push(mMouseListener);
    mKeyboardListener = keyListener;
    mMouseListener = mouseListener;
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::popListeners()
{
    mKeyboardListener = mKeyListenersStack.top();
    mMouseListener = mMouseListenersStack.top();
    mKeyListenersStack.pop();
    mMouseListenersStack.pop();
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::showDebugOverlay(bool show)
{
    if (mDebugOverlay) {
        if (show)
            mDebugOverlay->show();
        else
            mDebugOverlay->hide();
    }
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::requestShutDown()
{
    mContinue = false;
}

//-------------------------------------------------------------------------------------
void OgreFrameListener::updateStats(void)
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

//-------------------------------------------------------------------------------------

/*
pushListeners, popListeners could be used to switch to another Ogre module
with its own listeners (frame, keyboard, mouse) and its scene manager,
you have to destroy/recreate camera+viewports to switch between scene managers

Module::Module(Navigator* navigator) :
    mNavigator(navigator),
    mSceneMgr(0),
    mCamera(0) {}
Module::~Module() {}
bool Module::initialize()
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
    mFrameListener->pushListeners(mModuleFrameListener, mModuleFrameListener);

    // Hide scene
    ((NavigatorFrameListener*)mFrameListener)->detachCamera();
    // Save camera
    mSavedCameraPos = mCamera->getPosition();
    mSavedCameraQuat = mCamera->getOrientation();
    mSavedCameraNearClipDistance = mCamera->getNearClipDistance();
    mWindow->removeViewport(mCamera->getViewport()->getZOrder());
    mSceneMgr->destroyCamera(mCamera);

    mModule->initialize();
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
    mFrameListener->popListeners();
    Root::getSingletonPtr()->removeFrameListener(mModuleFrameListener);
*/