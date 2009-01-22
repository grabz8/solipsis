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

#ifndef __IVoiceEngine_h__
#define __IVoiceEngine_h__

#include <string>
#include <XmlDatasBasicTypes.h>
#include "DllExport.h"

namespace FMOD
{
    class System;
}

namespace Solipsis {

class IVoicePacketListener;

/** This class provide logging capacities interface.
 */
class IVoiceEngineLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class provide voice engine avatar events interface.
 */
class IVoiceEngineAvatarHandler
{
public:
    /** Called when avatar voice is created.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onVoiceCreation() {}

    /** Called when avatar voice is destroyed.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onVoiceDestruction() {}

    /** Called when avatar is talking or not.
    @param talking TRUE if avatar is speaking, FALSE otherwise
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onTalking(bool talking) {}
};

/** This class represents a generic Voice engine.
*/
class VOICEENGINE_EXPORT IVoiceEngine
{
public:
    IVoiceEngine() {}
    virtual ~IVoiceEngine() {}

    /** Get the name of the voice engine.
    @return The name of the voice engine
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
	virtual const std::string& getName() const = 0;

    /** Perform the initialization. 
    @return TRUE if successfully connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool init() = 0;

    /** Perform any tasks when the system is shut down.
    @return TRUE if successfully connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdown() = 0;

    /** Perform the initialization with the sound system. 
    @param system External FMOD sound system or 0 if not available
    @param networkChunkSize Size of a network chunk, i.e. the amount of data that will be buffered before
                            it's sent to the server. The size is specified in PCM samples.
    @param bufferFrameCount The number of frames to hold in record buffer (buffer sample count = frequency * frame count)
    @param frequency Frequency of audio to record (affects only the default SPEEX codec)
    @param silenceLevel The silence level between 0 (silent) and 128 (loud)
    @param silenceLatencySec The silence latency during sound is sent/processed since last time silence was broken
    @return TRUE if successfully connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool initSoundSystem(FMOD::System* system,
        size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000,
        float silenceLevel = 5.0f, unsigned int silenceLatencySec = 5) = 0;

    /** Perform the finalization with the sound system. 
    @return TRUE if successfully connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdownSoundSystem() = 0;

    /** Connect to a voice server.
    @param host The voice server hostname
    @param port The voice server port
    @param voiceId The unique user identifier connecting to the voice server, used to identify the sound source
    @return TRUE if successfully connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool connect(const char* host, unsigned short port, const EntityUID& voiceId) = 0;

    /** Disconnect from the voice server.
    @remarks An implementation must be supplied for this method.
    */
    virtual void disconnect() = 0;

    /** Returns true if engine is connected to the voice server.
    @return TRUE if engine is connected, FALSE otherwise
    @remarks An implementation must be supplied for this method.
    */
    virtual bool isConnected() = 0;

    /** Update the voice engine, this should be called once per frame.
    @remarks An implementation must be supplied for this method.
    */
    virtual void update() = 0;

    /** Set the silence detection parameters.
    @param silenceLevel The silence level between 0 (silent) and 128 (loud)
    @param silenceLatencySec The silence latency during sound is sent/processed since last time silence was broken
    @remarks An implementation must be supplied for this method.
    */
	virtual void setSilenceParams(float silenceLevel = 5.0f, unsigned int silenceLatencySec = 5) = 0;

    /** Start recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void startRecording() = 0;

    /** Stop recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void stopRecording() = 0;

    /** Returns true if engine is recording.
    @return TRUE if engine is recording
    @remarks An implementation must be supplied for this method.
    */
    virtual bool isRecording() = 0;

    /** Update sound position/direction/velocity/3D sound distances of an avatar.
    @remarks An implementation must be supplied for this method.
    */
    virtual void updateRecordingAvatar(float* pos, float* dir, float* vel, float* dist) = 0;

    /** Adds a listener that will be informed when a voice packet emitted by the given talking avatar is received.
    @param voiceId The unique user identifier connecting to the voice server
    @param voicePacketListener The VoicePacketListener implementation
    @remarks An implementation must be supplied for this method.
    */
	virtual void addVoicePacketListener(const EntityUID& voiceId, IVoicePacketListener* voicePacketListener) = 0 ;

    /** Removes a listener for voice packets.
    @param voiceId The unique user identifier connecting to the voice server
    @remarks An implementation must be supplied for this method.
    */
	virtual void removeVoicePacketListener(const EntityUID& voiceId) = 0 ;

    /** setAvatarHandler.
    @remarks An implementation must be supplied for this method.
    @param voiceId The unique user identifier connecting to the voice server
    @param avatarHandler The avatar handler instance
    */
    virtual void setAvatarHandler(const EntityUID& voiceId, IVoiceEngineAvatarHandler* avatarHandler) = 0;

    /** removeAvatarHandler.
    @remarks An implementation must be supplied for this method.
    @param voiceId The unique user identifier connecting to the voice server
    */
    virtual void removeAvatarHandler(const EntityUID& voiceId) = 0;

    /** setLogger.
    @remarks An implementation must be supplied for this method.
    @param logger The logger instance
    */
    virtual void setLogger(IVoiceEngineLogger* logger) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IVoiceEngine_h__