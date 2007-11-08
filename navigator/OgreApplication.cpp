#include "OgreApplication.h"
#include "NaviManager.h"

#define RESSOURCE_FILE_NAME "resources.cfg"

//-------------------------------------------------------------------------------------
OgreApplication::OgreApplication(const char* title) :
    mWindowTitle(title),
    mFrameListener(0),
    mSceneMgr(0),
    mWindow(0),
    mCamera(0)
{
}

//-------------------------------------------------------------------------------------
OgreApplication::~OgreApplication()
{
    delete Root::getSingletonPtr();
    delete mFrameListener;
}

//-------------------------------------------------------------------------------------
bool OgreApplication::initOgreCore()
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if (Root::getSingletonPtr()->restoreConfig() || Root::getSingletonPtr()->showConfigDialog())
        {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'

        if ((mWindowTitle.empty()) || (mWindowTitle.compare("")==0))
            mWindow = Root::getSingletonPtr()->initialise(true);
        else
            mWindow = Root::getSingletonPtr()->initialise(true, mWindowTitle);
        return true;
    }
    else
    {
        return false;
    }

    /* To do it by yourself
    selectedRenderSystem->setConfigOption("Full Screen","No");  
    selectedRenderSystem->setConfigOption("Video Mode","800 x 600 @ 16-bit colour");
    selectedRenderSystem->setConfigOption("Allow NVPerfHUD","No");
    selectedRenderSystem->setConfigOption("Anti aliasing","None");
    selectedRenderSystem->setConfigOption("Floating-point mode","Fastest");
    selectedRenderSystem->setConfigOption("Rendering Device","RADEON 9200");
    selectedRenderSystem->setConfigOption("VSync","No");
    */
}

//-------------------------------------------------------------------------------------
bool OgreApplication::initPostOgreCore()
{
    return true;
}

//-------------------------------------------------------------------------------------
void OgreApplication::createSceneManager()
{
    // Create the SceneManager, in this case a generic one
    mSceneMgr = Root::getSingletonPtr()->createSceneManager(ST_GENERIC);
}

//-------------------------------------------------------------------------------------
void OgreApplication::createCamera()
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
void OgreApplication::createFrameListener()
{
    mFrameListener = 0; //Override it if needed
}

//-------------------------------------------------------------------------------------
void OgreApplication::registerFrameListener()
{
    if (mFrameListener != 0)
        Root::getSingletonPtr()->addFrameListener(mFrameListener);
}

//-------------------------------------------------------------------------------------
void OgreApplication::destroyScene()
{
}

//-------------------------------------------------------------------------------------
bool OgreApplication::createGUI() {
    // Initializing Navi
    NaviLibrary::NaviManager::Get().Startup(mWindow);

    return true;
}

//-------------------------------------------------------------------------------------
void OgreApplication::destroyGUI() {
    // Finalizing Navi
    NaviLibrary::NaviManager::Get().Shutdown();
}

//-------------------------------------------------------------------------------------
void OgreApplication::createViewports()
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
}

//-------------------------------------------------------------------------------------
void OgreApplication::addResourceLocations()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

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
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

//-------------------------------------------------------------------------------------
void OgreApplication::createResourceListener()
{

}

//-------------------------------------------------------------------------------------
void OgreApplication::initResources()
{
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-------------------------------------------------------------------------------------
void OgreApplication::go()
{
    try
    {
        if (!initialise())
        {
            // clean up
            shutdown();
            return;
        }

        Root::getSingletonPtr()->startRendering();
    }
    catch (...)
    {
        // clean up
        shutdown();
        throw;
    }
    // clean up
    shutdown();
}

//-------------------------------------------------------------------------------------
bool OgreApplication::initialise()
{
    new Root();

    addResourceLocations();

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

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Initialise resources
    initResources();

    // Create the scene
    createScene();

    // Create GUI
    if (!createGUI())
        return false;

    createFrameListener();
    registerFrameListener();

    return true;
};

//-------------------------------------------------------------------------------------
bool OgreApplication::shutdown()
{
    // Destroy the scene
    destroyScene();

    // Destroy GUI
    destroyGUI();

    return true;
}

//-------------------------------------------------------------------------------------
SceneManager* OgreApplication::getSceneMgrPtr() {
    return mSceneMgr;
}

//-------------------------------------------------------------------------------------
RenderWindow* OgreApplication::getRenderWindowPtr() {
    return mWindow;
}

//-------------------------------------------------------------------------------------
Camera* OgreApplication::getCameraPtr() {
    return mCamera;
}