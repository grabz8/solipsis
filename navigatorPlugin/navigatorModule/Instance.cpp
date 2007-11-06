#include "Instance.h"

using namespace NavigatorModule;

OgreApplication* Instance::ms_OgreApplication = 0;

/*pthread_once_t Instance::ms_TlsKeyOnce = PTHREAD_ONCE_INIT;
pthread_key_t Instance::ms_TlsKey = NAVMODINSTANCE_TLS_NOKEY;
*/
Instance::Instance() :
    mIWindow(0),
    mStopRequested(false),
    mFrameListener(0),
    mSceneMgr(0),
    mWindow(0),
    mCamera(0)
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

    return true;
}

bool Instance::run()
{
    while (!mStopRequested)
    {
        try
        {
            ms_OgreApplication->getRoot()->getRenderSystem()->_initRenderTargets();
            while (ms_OgreApplication->getRoot()->renderOneFrame())
                ;
        }
        catch (...)
        {
            // clean up
            ms_OgreApplication->finalize();
            throw;
        }
        // clean up
        ms_OgreApplication->finalize();
    }

    return true;
}

void Instance::requestTerminate()
{
    mStopRequested = true;
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
void Instance::registerFrameListener()
{
    if (mFrameListener != 0)
        Root::getSingletonPtr()->addFrameListener(mFrameListener);
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
