#include "ODEPlugin.h"
#include "PhysicsEngineManager.h"

using namespace Solipsis;

const String sPluginName = "ODE Physics Engine";

//-------------------------------------------------------------------------------------
ODEPlugin::ODEPlugin() :
    mEngine(0)
{
}

//-------------------------------------------------------------------------------------
const String& ODEPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void ODEPlugin::install()
{
    mEngine = new ODEEngine();
    PhysicsEngineManager::getSingleton().addEngine(mEngine);
}

//-------------------------------------------------------------------------------------
void ODEPlugin::initialise()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void ODEPlugin::shutdown()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void ODEPlugin::uninstall()
{
    if (mEngine != 0)
    {
        if (PhysicsEngineManager::getSingletonPtr())
            PhysicsEngineManager::getSingleton().removeEngine(mEngine);
        delete mEngine;
    }
}
