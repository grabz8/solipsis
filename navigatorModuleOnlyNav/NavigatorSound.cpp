/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "NavigatorSound.h"
#include "VoiceEngineManager.h"
#include "OgreHelpers.h"
#include <CTIO.h>

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
    FMOD_ADVANCEDSETTINGS settings;
    memset(&settings, 0, sizeof(FMOD_ADVANCEDSETTINGS));
    settings.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);
    std::string cwd = CommonTools::IO::getCWD();
    settings.debugLogFilename = (char*)cwd.c_str();
    result = mSoundSystem->setAdvancedSettings(&settings);
    if (!fmodErrorCheck(result))
        return false;
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