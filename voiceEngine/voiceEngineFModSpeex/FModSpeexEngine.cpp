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

#include "FModSpeexEngine.h"

#include "fmod.hpp"

#include <FModSpeexVoipHandler.h>
#include <VoicePacket.h>

using namespace Solipsis;

const std::string sVoiceEngineName = "FMod/Speex engine";

//-------------------------------------------------------------------------------------
const std::string& FModSpeexEngine::getName() const
{
    return sVoiceEngineName;
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::init()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::shutdown()
{
    return true;
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::initSoundSystem(FMOD::System* system,
                                      size_t networkChunkSizePCM, unsigned int bufferFrameCount, unsigned int frequency,
                                      float silenceLevel, unsigned int silenceLatencySec)
{
    if (mVoiceEngine != 0)
    {
        logMessage("Shutting down previous sound system ...");
        shutdownSoundSystem();
    }
    mVoiceEngine = 0;

	mVoiceEngine = new FModSpeexVoipHandler(system, this, networkChunkSizePCM, bufferFrameCount, frequency, silenceLevel, silenceLatencySec);
    if (mVoiceEngine == 0)
    {
        logMessage("Could not create the voice engine");
        return false;
    }
    mVoiceEngine->setLogger(this); 

    return true;
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::shutdownSoundSystem()
{
    if (mVoiceEngine == 0)
        return false;

    mVoiceEngine->disconnect();

    // Shut down
    delete mVoiceEngine;
    mVoiceEngine = 0;

    return true;
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::connect(const char* host, unsigned short port, const EntityUID& voiceId)
{
    if (mVoiceEngine == 0)
        return false;

    return mVoiceEngine->connect(host, port, voiceId);
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::disconnect()
{
    mVoiceEngine->disconnect();
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::isConnected()
{
    return mVoiceEngine->isConnected();
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::update()
{
    mVoiceEngine->update();
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::setSilenceParams(float silenceLevel, unsigned int silenceLatencySec)
{
    mVoiceEngine->setSilenceParams(silenceLevel, silenceLatencySec);
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::startRecording()
{
    mVoiceEngine->startRecording();
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::stopRecording()
{
    mVoiceEngine->stopRecording();
}

//-------------------------------------------------------------------------------------
bool FModSpeexEngine::isRecording()
{
    return mVoiceEngine->isRecording();
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::updateAvatar(const EntityUID& voiceId, float* pos, float* dir, float* vel)
{
    mVoiceEngine->updateAvatar(voiceId, pos, dir, vel);
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::addVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener )
{
	assert( mAvatarUidToVoicePacketListener.find(talkingAvatarUid) == mAvatarUidToVoicePacketListener.end() );
	mAvatarUidToVoicePacketListener[ talkingAvatarUid ] = pVoicePacketListener;
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::removeVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener )
{
	assert( mAvatarUidToVoicePacketListener.find(talkingAvatarUid) != mAvatarUidToVoicePacketListener.end() );
	mAvatarUidToVoicePacketListener.erase( talkingAvatarUid );
}

//-------------------------------------------------------------------------------------
void FModSpeexEngine::onVoicePacketReception( VoicePacket* pVoicePacket )
{
	// we received a voice packet from FModSpeexVoipHandler, forward it to the listeners associated to the uid of the talking avatar
	const std::string & talkingAvatarUid = pVoicePacket->getTalkingAvatarUid();
	assert( mAvatarUidToVoicePacketListener.find(talkingAvatarUid) != mAvatarUidToVoicePacketListener.end() );
	mAvatarUidToVoicePacketListener[ talkingAvatarUid ]->onVoicePacketReception( pVoicePacket );
}

//-------------------------------------------------------------------------------------
