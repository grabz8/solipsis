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

#include "DllExport.h"
#include <string>
#include <EntityUID.h>

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

/** This class represents a generic Voice engine.
*/
class VOICEENGINE_EXPORT IVoiceEngine
{
public:
    IVoiceEngine() {}
    virtual ~IVoiceEngine() {}

    /** Get the name of the voice engine.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
	virtual const std::string& getName() const = 0;

    /** Perform the initialization. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool init() = 0;

    /** Perform any tasks when the system is shut down.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdown() = 0;

    /** Perform the initialization with the sound system. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool initSoundSystem(FMOD::System* system, size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000) = 0;

    /** Perform the finalization with the sound system. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdownSoundSystem() = 0;

    /** Connect to a voice server.
		@remarks An implementation must be supplied for this method.
		@param	the id of the avatar connecting to the voice server. It is used to uniquely identify the sound sources and dispatch
    */
    virtual bool connect(const char* host, int port, const EntityUID & voiceId) = 0;

    /** Disconnect from the voice server.
    @remarks An implementation must be supplied for this method.
    */
    virtual void disconnect() = 0;

    /** Update the voice engine, this should be called once per frame.
    @remarks An implementation must be supplied for this method.
    */
    virtual void update() = 0;

    /** Start recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void startRecording() = 0;

    /** Stop recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void stopRecording() = 0;

    /** Returns true if engine is recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool isRecording() = 0;

	/**
		@brief	adds a listener that will be informed when a voice packet emitted by the given talking avatar is received
	*/
	virtual void addVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener ) = 0 ;

	/**
		@brief	removes a listener for voice packets
	*/
	virtual void removeVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener ) = 0 ;
};

} // namespace Solipsis

#endif // #ifndef __IVoiceEngine_h__