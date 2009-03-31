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
#include "Instance.h"
#include "MainApplication/AutoCreatedWindow.h"
#include "OgreTools/OgreHelpers.h"
#include "CTLog.h"

using namespace Solipsis;
using namespace CommonTools;

/*pthread_once_t Instance::ms_TlsKeyOnce = PTHREAD_ONCE_INIT;
pthread_key_t Instance::ms_TlsKey = INSTANCE_TLS_NOKEY;
*/

//-------------------------------------------------------------------------------------
Instance::Instance(const String name, IApplication* application) :
    mName(name),
    mIWindow(0),
    mAutoCreatedWindow(false),
    mStartMutex(PTHREAD_MUTEX_INITIALIZER),
    mTermRequested(false),
    mMouseMutex(PTHREAD_MUTEX_INITIALIZER),
    mLastMouseMovedValid(false),
    mFrameListener(0),
    mSceneMgr(0),
    mWindow(0),
    mCamera(0),
    mNaviSupported(true)
{
    mOgreApplication = (OgreApplication*)application;
}

//-------------------------------------------------------------------------------------
Instance::~Instance()
{
    finalize();
}

//-------------------------------------------------------------------------------------
bool Instance::setWindow(IWindow* w)
{
    if (mOgreApplication == 0)
        return false;
    if ((mIWindow != 0) || (mAutoCreatedWindow && (mIWindow == 0)))
        return false;

    mAutoCreatedWindow = (w == 0);
    mIWindow = w;
    pthread_mutex_unlock(&mStartMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::_setWindow()
{
    if (mOgreApplication == 0)
        return false;

    mOgreApplication->lock();
    if (mAutoCreatedWindow)
    {
        mIWindow = new AutoCreatedWindow(this);
        mWindow = mOgreApplication->getRoot()->getAutoCreatedWindow();
        if (mWindow != 0)
            ((AutoCreatedWindow*)mIWindow)->initialize();
    }
    else
    {
        Ogre::NameValuePairList misc;
        misc["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned int)(mIWindow->getHandle()));
        misc["vsync"] = "true";
        misc["FSAA"] = "0";
        try
        {
            mWindow = mOgreApplication->getRoot()->createRenderWindow(mName, mIWindow->getWidth(), mIWindow->getHeight(), false, &misc);
        }
        catch (Ogre::Exception& e)
        {
            OGRE_LOG("Instance::_setWindow caught Ogre::Exception " + e.getFullDescription());
            mWindow = 0;
        }
    }
    mOgreApplication->unlock();
    if (mWindow == 0)
        return false;

    try {
        // Initialize resources
        mOgreApplication->initResources();

        initialize();
    }
    catch (Ogre::Exception& e)
    {
        throw "Instance::_setWindow() Unable to initialize instance, OGRE exception: " + e.getDescription();
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::processEvent(const Event& evt)
{
    if (evt.getEvt().mType == ETMouseMoved)
    {
        pthread_mutex_lock(&mMouseMutex);
        mLastMouseMovedEvent = evt;
        if (!mLastMouseMovedValid)
        {
            mEventQueue.addTail(evt);
            mLastMouseMovedValid = true;
        }
        pthread_mutex_unlock(&mMouseMutex);
    }
    else
        mEventQueue.addTail(evt);

    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::run()
{
    bool _initRenderTargetsCalled = false;

    // wait until the window is set
    pthread_mutex_lock(&mStartMutex);
    // termination requested before setting window ?
    if (mTermRequested)
        return true;

    // set the window
    if (!_setWindow())
        return false;

    while (!mTermRequested)
    {
        try
        {
		    // process events
            if (!handleEvents())
                requestTerminate();
            if (mTermRequested) break;

            mOgreApplication->lock();

            // render
            if (!_initRenderTargetsCalled)
            {
                mOgreApplication->getRoot()->getRenderSystem()->_initRenderTargets();
                _initRenderTargetsCalled = true;
            }

            // if we auto-create the window then rendering loop is in the same thread
            // and we have to pump window messages
            if (mAutoCreatedWindow)
                WindowEventUtilities::messagePump();

            // render the current frame
            if (!mOgreApplication->getRoot()->renderOneFrame())
                requestTerminate();

            mOgreApplication->unlock();
        }
        catch (Ogre::Exception e)
        {
            LOGHANDLER_LOG(LogHandler::VL_CRITICAL, "Caught an Ogre exception, " + e.getFullDescription());
            // Hum hum TODO add a good Solipsis::exception class
             requestTerminate();
            mOgreApplication->unlock();
        }
        catch (std::exception e)
        {
            LOGHANDLER_LOG(LogHandler::VL_CRITICAL, "Caught an std::exception, " + String(e.what()));
            // Hum hum TODO add a good Solipsis::exception class
             requestTerminate();
            mOgreApplication->unlock();
        }
        catch (...)
        {
            // Hum hum TODO add a good Solipsis::exception class
            mOgreApplication->unlock();
            LOGHANDLER_LOG(LogHandler::VL_CRITICAL, "Caught an exception, ");
             requestTerminate();
            mOgreApplication->unlock();
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
void Instance::requestTerminate()
{
    Evt termEvt;
    termEvt.mType = ETTermRequested;
    processEvent(Event(0, &termEvt));

    // in case the setWindow was never called
    pthread_mutex_unlock(&mStartMutex);
}

//-------------------------------------------------------------------------------------
SceneManager* Instance::getSceneMgrPtr() {
    return mSceneMgr;
}

//-------------------------------------------------------------------------------------
RenderWindow* Instance::getRenderWindowPtr() {
    return mWindow;
}

//-------------------------------------------------------------------------------------
Camera* Instance::getCameraPtr() {
    return mCamera;
}

//-------------------------------------------------------------------------------------
bool Instance::initialize()
{
    // if we cannot initialize Ogre, just abandon the whole deal
    if (!initOgreCore())
        return false;
    if (!initPostOgreCore())
        return false;

    createSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(99999);

    // Create the scene
    createScene();

    // Create GUI
    if (!createGUI())
        return false;

    createFrameListener();
    registerFrameListener();

    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::finalize()
{
    unregisterFrameListener();
    delete mFrameListener;

    // Destroy GUI
    destroyGUI();

    // Destroy the scene
    destroyScene();

    if (mSceneMgr != 0)
    {
        // Destroy the camera
        if (mCamera != 0)
            mSceneMgr->destroyCamera("UserCam");
        mSceneMgr->clearScene();
    }
//    MeshManager::getSingleton().unloadAll(); // To look for unfreed meshes resources

    if (!mAutoCreatedWindow)
        mOgreApplication->getRoot()->detachRenderTarget(mWindow);

    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::initOgreCore()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::initPostOgreCore()
{
    return true;
}

//-------------------------------------------------------------------------------------
void Instance::createSceneManager()
{
    // Create the SceneManager, in this case a generic one
    mSceneMgr = Root::getSingletonPtr()->createSceneManager(ST_GENERIC);
}

//-------------------------------------------------------------------------------------
void Instance::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("UserCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Vector3(0,0,0));
    mCamera->setOrientation(Quaternion::IDENTITY);
    // Look back along -Z
    mCamera->lookAt(Vector3(0,0,-1));
    mCamera->setNearClipDistance(0.1f);
}

//-------------------------------------------------------------------------------------
void Instance::createFrameListener()
{
    mFrameListener = 0; //Override it if needed
}

//-------------------------------------------------------------------------------------
void Instance::destroyScene()
{
}

//-------------------------------------------------------------------------------------
bool Instance::createGUI() {
    // Navi supported ?
    char* naviSupport = ::getenv(NAVI_SUPPORT_ENV);
    OGRE_LOG(std::string(NAVI_SUPPORT_ENV": ") + ((naviSupport != 0) ? std::string(naviSupport) : std::string("not defined")))
    if ((naviSupport == 0) ||
        ((naviSupport != 0) && (_stricmp(naviSupport, "no") == 0)))
        mNaviSupported = false;

    if (!mNaviSupported)
        return true;

    return true;
}

//-------------------------------------------------------------------------------------
void Instance::destroyGUI() {
    if (!mNaviSupported)
        return;
}

//-------------------------------------------------------------------------------------
void Instance::createViewports()
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue::Black);

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
}

//-------------------------------------------------------------------------------------
void Instance::registerFrameListener()
{
    if (mFrameListener != 0)
        Root::getSingletonPtr()->addFrameListener(mFrameListener);
}

//-------------------------------------------------------------------------------------
void Instance::unregisterFrameListener()
{
    if (mFrameListener != 0)
        Root::getSingletonPtr()->removeFrameListener(mFrameListener);
}

//-------------------------------------------------------------------------------------
bool Instance::handleEvent(const Event& evt)
{
    switch (evt.getEvt().mType)
    {
    case ETTermRequested:
        mTermRequested = true;
        break;

    case ETKeyPressed:
        if (mFrameListener != 0)
            mFrameListener->keyPressed((const KeyboardEvt&)evt.getEvt());
        break;
    case ETKeyReleased:
        if (mFrameListener != 0)
            mFrameListener->keyReleased((const KeyboardEvt&)evt.getEvt());
        break;

    case ETMousePressed:
        if (mFrameListener != 0)
            mFrameListener->mousePressed((const MouseEvt&)evt.getEvt());
        break;
    case ETMouseReleased:
        if (mFrameListener != 0)
            mFrameListener->mouseReleased((const MouseEvt&)evt.getEvt());
        break;
    case ETMouseMoved:
        if (mFrameListener != 0)
            mFrameListener->mouseMoved((const MouseEvt&)evt.getEvt());
        break;
    }
    return true;
}

//-------------------------------------------------------------------------------------
bool Instance::handleEvents()
{
    while (!mEventQueue.isEmpty()) {
        Event* evt = mEventQueue.getHead();
        if (evt == 0)
            return true;
        if (evt->getEvt().mType == ETMouseMoved)
        {
            pthread_mutex_lock(&mMouseMutex);
            Event evt = mLastMouseMovedEvent;
            mEventQueue.removeHead();
            mLastMouseMovedValid = false;
            pthread_mutex_unlock(&mMouseMutex);
            handleEvent(evt);
        }
        else
        {
            mEventQueue.removeHead();
            handleEvent(*evt);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
