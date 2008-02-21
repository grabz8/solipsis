#include "PhysXPlugin.h"
#include "PhysicsEngineManager.h"

using namespace Solipsis;

const String sPluginName = "PhysX Physics Engine";

//-------------------------------------------------------------------------------------
PhysXPlugin::PhysXPlugin() :
    mEngine(0)
{
}

//-------------------------------------------------------------------------------------
const String& PhysXPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void PhysXPlugin::install()
{
    mEngine = new PhysXEngine();
    PhysicsEngineManager::getSingleton().addEngine(mEngine);
}

//-------------------------------------------------------------------------------------
void PhysXPlugin::initialise()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void PhysXPlugin::shutdown()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void PhysXPlugin::uninstall()
{
    if (mEngine != 0)
    {
        if (PhysicsEngineManager::getSingletonPtr())
            PhysicsEngineManager::getSingleton().removeEngine(mEngine);
        delete mEngine;
    }
}
