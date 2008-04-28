#include "FModSpeexPlugin.h"
#include "VoiceEngineManager.h"

using namespace Solipsis;

const String sPluginName = "FMod/Speex Voice Engine";

//-------------------------------------------------------------------------------------
FModSpeexPlugin::FModSpeexPlugin() :
    mEngine(0)
{
}

//-------------------------------------------------------------------------------------
const String& FModSpeexPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void FModSpeexPlugin::install()
{
    mEngine = new FModSpeexEngine();
    VoiceEngineManager::getSingleton().addEngine(mEngine);
}

//-------------------------------------------------------------------------------------
void FModSpeexPlugin::initialise()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void FModSpeexPlugin::shutdown()
{
    // nothing to do
}

//-------------------------------------------------------------------------------------
void FModSpeexPlugin::uninstall()
{
    if (mEngine != 0)
    {
        if (VoiceEngineManager::getSingletonPtr())
            VoiceEngineManager::getSingleton().removeEngine(mEngine);
        delete mEngine;
    }
}
