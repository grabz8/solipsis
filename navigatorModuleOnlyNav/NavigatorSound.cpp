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
#include <PhonetizerManager.h>
#include <VoiceEngineManager.h>
#include <CTLog.h>
#include <CTIO.h>

using namespace CommonTools;

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
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "NavigatorSound::initialize() Initializing Sound System ...");
    result = FMOD::System_Create(&mSoundSystem);
    if (!fmodErrorCheck(result))
        return false;
    result = mSoundSystem->getVersion(&version);
    if (!fmodErrorCheck(result))
        return false;
    if (version < FMOD_VERSION)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorSound::initialize() FMOD error: You are using an old version of FMOD %08x. This program requires version %08x", version, FMOD_VERSION);
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

	// Create the Phonetizer
	{
		LOGHANDLER_LOGF(LogHandler::VL_INFO, "NavigatorSound::initialize() Initializing the Phonetizer ...");
		PhonetizerManager::getSingleton().selectPhonetizer("Dummy Phonetizer");
		IPhonetizer* pPhonetizer = PhonetizerManager::getSingleton().getSelectedPhonetizer();
		if (pPhonetizer == 0)
		{
			LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorSound::initialize() Could not find the Dummy Phonetizer");
			return false;
		}
	}

    // Create Voice engine
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "NavigatorSound::initialize() Initializing Voice Engine ...");
    VoiceEngineManager::getSingleton().selectEngine("FMod/Speex engine");
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "NavigatorSound::initialize() Could not find the FMod/Speex voice engine, it won t be supported !");
    else
        if (!voiceEngine->initSoundSystem(mSoundSystem))
        {
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorSound::initialize() Could not initialize the voice engine");
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
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorSound::fmodErrorCheck() FMOD error: (%d) %s", result, FMOD_ErrorString(result));
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis