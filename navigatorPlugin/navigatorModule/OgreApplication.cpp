#include "OgreApplication.h"
#include "NaviManager.h"

#define RESSOURCE_FILE_NAME "resources.cfg"

//-------------------------------------------------------------------------------------
OgreApplication::OgreApplication() :
    mRoot(0),
    mNumInstances(0)
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
bool OgreApplication::initialize()
{
    mRoot = new Root();

    addResourceLocations();

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Initialise resources
    initResources();

    return true;
};

//-------------------------------------------------------------------------------------
bool OgreApplication::finalize()
{
    assert(mNumInstances == 0);

    delete mRoot;

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreApplication::registerInstance(NavigatorModule::Instance* instance, NavigatorModule::IWindow* window)
{
    mNumInstances++;

    return true;
}

//-------------------------------------------------------------------------------------
unsigned int OgreApplication::unregisterInstance(NavigatorModule::Instance* instance)
{
    assert(mNumInstances > 0);
    mNumInstances--;
    return mNumInstances;
}
