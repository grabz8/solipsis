#include "OgreApplication.h"
#include "NaviManager.h"

#define RESSOURCE_FILE_NAME "resources.cfg"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgreApplication::OgreApplication() :
    mRoot(0)
{
}

//-------------------------------------------------------------------------------------
OgreApplication::~OgreApplication()
{
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
bool OgreApplication::initialize(bool configManagedByOgre, String windowTitle)
{
    mRoot = new Root();
    if (mRoot == 0)
        return false;

    if (configManagedByOgre)
    {
        // Show the configuration dialog and initialize the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
        if (mRoot->restoreConfig() || mRoot->showConfigDialog())
            // If returned true, user clicked OK so initialize
            ;
        else
            return false;
    }
    else
    {
	    Ogre::RenderSystem *currentRenderSystem = NULL;
	    Ogre::RenderSystemList *rl = mRoot->getAvailableRenderers();
	    Ogre::String str;
	    for (Ogre::RenderSystemList::iterator it = rl->begin(); it != rl->end(); ++it)
        {
		    currentRenderSystem = (*it);
		    str = currentRenderSystem->getName().c_str();
		    if ((int)str.find("3D9") > 0)
			    break;
        }
	    if (currentRenderSystem == NULL)
            return false;
	    // preserve the floating point precision
	    currentRenderSystem->setConfigOption("Floating-point mode", "Consistent");
	    try 
	    {
		    mRoot->setRenderSystem(currentRenderSystem);
        }
        catch (Ogre::Exception& e)
        {
            return false;
        }
    }
    mRoot->initialise(configManagedByOgre, windowTitle);

    addResourceLocations();

    // Create any resource listeners (for loading screens)
    createResourceListener();

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreApplication::finalize()
{
    delete mRoot;

    return true;
}

//-------------------------------------------------------------------------------------
