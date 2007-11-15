#include "Instance.h"
#include "Platform.h"

using namespace NavigatorModule;

OgreApplication* Instance::ms_OgreApplication = 0;

/*pthread_once_t Instance::ms_TlsKeyOnce = PTHREAD_ONCE_INIT;
pthread_key_t Instance::ms_TlsKey = NAVMODINSTANCE_TLS_NOKEY;
*/
Instance::Instance() :
    mIWindow(0),
    mReady(false),
    mTermRequested(false),
    mFrameListener(0),
    mSceneMgr(0),
    mWindow(0),
    mCamera(0),
    mNaviMutex(PTHREAD_MUTEX_INITIALIZER),
    mMouseMutex(PTHREAD_MUTEX_INITIALIZER),
    mLastMouseMovedValid(false)
{
    initialize();
}

Instance::~Instance()
{
    finalize();
}

bool Instance::initialize()
{
    return true;
}

bool Instance::finalize()
{
    // Destroy the scene
    destroyScene();

    // Destroy GUI
    destroyGUI();

    try
    {
        if (ms_OgreApplication->unregisterInstance(this) == 0)
        {
            ms_OgreApplication->finalize();
            delete ms_OgreApplication;
            ms_OgreApplication = NULL;
        }
    }
    catch( Ogre::Exception&)
    {
    }
    catch(...)
    {
        // catch everything
    }

    delete mFrameListener;

    return true;
}

bool Instance::setWindow(IWindow* w)
{
    mReady = false;
    assert(w != 0);

    mIWindow = w;

    if (ms_OgreApplication == 0)
    {
        ms_OgreApplication = new OgreApplication();
        assert(ms_OgreApplication != 0);
        if (!ms_OgreApplication->initialize())
            return false;
    }
    if (ms_OgreApplication != 0)
        ms_OgreApplication->registerInstance(this, w);

    Ogre::NameValuePairList misc;
    misc["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned int)(mIWindow->getHandle()));
    misc["vsync"] = "true";
    misc["FSAA"] = "0";
    char strName[64];
    sprintf(strName, "Navigator");
    try
    {
        mWindow = ms_OgreApplication->getRoot()->createRenderWindow(strName, mIWindow->getWidth(), mIWindow->getHeight(), false, &misc);
    }
    catch (Ogre::Exception& e)
    {
        return false;
    }
    ms_OgreApplication->initialize2();

    // if we cannot initialise Ogre, just abandon the whole deal
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
    createGUI();

    createFrameListener();
    registerFrameListener();

    mReady = true;

    return true;
}

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

bool Instance::run()
{
    bool _initRenderTargetsCalled = false;
    while (!mTermRequested)
    {
        try
        {
            if (!mReady)
            {
                Platform::sleep(1000);
                continue;
            }

		    // process events
            if (!handleEvents())
                requestTerminate();
            if (mTermRequested) break;

            // render
            if (!_initRenderTargetsCalled)
            {
                ms_OgreApplication->getRoot()->getRenderSystem()->_initRenderTargets();
                _initRenderTargetsCalled = true;
            }
            ms_OgreApplication->getRoot()->renderOneFrame();
        }
        catch (...)
        {
            // clean up
//            ms_OgreApplication->finalize();
//            throw;
        }
    }
    // clean up
//    ms_OgreApplication->finalize();

    return true;
}

void Instance::requestTerminate()
{
/*    mTermRequested = true;
    while (mTermRequested)
        Platform::sleep(100);*/
    Evt termEvt;
    termEvt.mType = ETTermRequested;
    processEvent(Event(0, &termEvt));
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
    // Look back along -Z
    mCamera->lookAt(Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);
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
    // Initializing Navi
    NaviLibrary::NaviManager::Get().Startup(mWindow);

    return true;
}

//-------------------------------------------------------------------------------------
void Instance::destroyGUI() {
    // Finalizing Navi
    NaviLibrary::NaviManager::Get().Shutdown();
}

//-------------------------------------------------------------------------------------
void Instance::createViewports()
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

//-------------------------------------------------------------------------------------
void Instance::registerFrameListener()
{
    if (mFrameListener != 0)
        Root::getSingletonPtr()->addFrameListener(mFrameListener);
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
            handleEvent(*evt);
            mEventQueue.removeHead();
        }
    }

    return true;
}
