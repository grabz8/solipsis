#include "TokamakPlugin.h"
#include "PhysicsEngineManager.h"

using namespace Solipsis;

const String sPluginName = "Tokamak Physics Engine";

//-------------------------------------------------------------------------------------
TokamakPlugin::TokamakPlugin() :
    mEngine(0)
{
}

//-------------------------------------------------------------------------------------
const String& TokamakPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void TokamakPlugin::install()
{
    mEngine = new TokamakEngine();
    PhysicsEngineManager::getSingleton().addEngine(mEngine);
}

//-------------------------------------------------------------------------------------
void TokamakPlugin::initialise()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void TokamakPlugin::shutdown()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void TokamakPlugin::uninstall()
{
    if (mEngine != 0)
    {
        if (PhysicsEngineManager::getSingletonPtr())
            PhysicsEngineManager::getSingleton().removeEngine(mEngine);
        delete mEngine;
    }
}
