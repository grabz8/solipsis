#include "NavigatorSound.h"
#include "VoiceEngineManager.h"
#include "OgreHelpers.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
NavigatorSound::NavigatorSound() :
    mSoundSystem(0)
{
}

//-------------------------------------------------------------------------------------
NavigatorSound::~NavigatorSound()
{
    shutdown();
}

//-------------------------------------------------------------------------------------
bool NavigatorSound::initialize()
{
    FMOD_RESULT result;
    unsigned int version;

    // Create a System object and initialize.
    OGRE_LOG("NavigatorSound::initialize() Initializing Sound System ...");
    result = FMOD::System_Create(&mSoundSystem);
    if (!fmodErrorCheck(result))
        return false;
    result = mSoundSystem->getVersion(&version);
    if (!fmodErrorCheck(result))
        return false;
    if (version < FMOD_VERSION)
    {
        char versionHexStr[32];
        char FMOD_VERSIONHexStr[32];
        _snprintf(versionHexStr, sizeof(versionHexStr)-1, "%08x", version);
        _snprintf(FMOD_VERSIONHexStr, sizeof(FMOD_VERSIONHexStr)-1, "%08x", FMOD_VERSION);
        OGRE_LOG("NavigatorSound::initialize() FMOD error: You are using an old version of FMOD " + Ogre::String(versionHexStr) + ". This program requires " + Ogre::String(FMOD_VERSIONHexStr));
        return false;
    }
    result = mSoundSystem->init(1, FMOD_INIT_NORMAL, 0);
    if (!fmodErrorCheck(result))
        return false;

    // Create Voice engine
    OGRE_LOG("NavigatorSound::initialize() Initializing Voice Engine ...");
    VoiceEngineManager::getSingleton().selectEngine("FMod/Speex engine");
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
    {
        OGRE_LOG("NavigatorSound::initialize() Could not find the FMod/Speex voice engine");
        return false;
    }
    if (!voiceEngine->initSoundSystem(mSoundSystem))
    {
        OGRE_LOG("NavigatorSound::initialize() Could not initialize the voice engine");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorSound::shutdown()
{
    // Shutdown voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine != 0)
        voiceEngine->shutdownSoundSystem();

    // Shutdown sound system
    if (mSoundSystem != 0)
    {
        FMOD_RESULT result;
        result = mSoundSystem->close();
        fmodErrorCheck(result);
        result = mSoundSystem->release();
        fmodErrorCheck(result);
        mSoundSystem = 0;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorSound::update()
{
    // Update voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine != 0)
        voiceEngine->update();

    // Update sound system
    if (mSoundSystem != 0)
    {
        FMOD_RESULT result;
        result = mSoundSystem->update();
        fmodErrorCheck(result);
    }
}

//-------------------------------------------------------------------------------------
bool NavigatorSound::fmodErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        OGRE_LOG("NavigatorSound::fmodErrorCheck() FMOD error: (" + StringConverter::toString(result) + ") " + Ogre::String(FMOD_ErrorString(result)));
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis