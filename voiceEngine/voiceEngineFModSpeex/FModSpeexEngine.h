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

#ifndef __FModSpeexEngine_h__
#define __FModSpeexEngine_h__

#include <IVoiceEngine.h>
#include <FModSpeexVoipHandler.h>
#include <IVoicePacketListener.h>

namespace Solipsis {

/** This class manages the FModSpeex engine.
*/
class FModSpeexEngine : public IVoiceEngine, public IVoicePacketListener
{
private:
    /// Logging instance
    IVoiceEngineLogger* mLogger;

    /// Voice engine
    FModSpeexVoipHandler *mVoiceEngine;

	std::map< std::string, IVoicePacketListener* > mAvatarUidToVoicePacketListener;	///< associates a voice packet listener to an avatar. Note that these are just references to listeners, the listeners are owned by something else.

public:
    FModSpeexEngine() : mVoiceEngine(0) {}

	//! @name IVoiceEngine implementation
	//!	@{
		/// @copydoc IVoiceEngine::getName
		virtual const std::string& getName() const;

		/// @copydoc IVoiceEngine::init
		virtual bool init();

		/// @copydoc IVoiceEngine::shutdown
		virtual bool shutdown();

		/// @copydoc IVoiceEngine::createScene
		virtual bool initSoundSystem(FMOD::System* system, size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000);

		/// @copydoc IVoiceEngine::createScene
		virtual bool shutdownSoundSystem();

		/// @copydoc IVoiceEngine::createScene
		virtual bool connect(const char* host, int port, const Solipsis::EntityUID & id);

		/// @copydoc IVoiceEngine::createScene
		virtual void disconnect();

		/// @copydoc IVoiceEngine::createScene
		virtual void update();

		/// @copydoc IVoiceEngine::createScene
		virtual void startRecording();

		/// @copydoc IVoiceEngine::createScene
		virtual void stopRecording();

		/// @copydoc IVoiceEngine::createScene
		virtual bool isRecording();

		/// @copydoc IVoiceEngine::setLogger
		virtual void setLogger(IVoiceEngineLogger* logger) { mLogger = logger; }

		virtual void addVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener );
		virtual void removeVoicePacketListener( const std::string & talkingAvatarUid, IVoicePacketListener* pVoicePacketListener );

	//! @}
	
    /// log a message
    inline void logMessage(const std::string& message) { if (mLogger != 0) mLogger->logMessage(message); }

	//! @name IVoicePacketListener implementation
	//!	@{
		virtual void onVoicePacketReception( VoicePacket* pVoicePacket );
	//! @}
};

} // namespace Solipsis

#endif // #ifndef __FModSpeexEngine_h__