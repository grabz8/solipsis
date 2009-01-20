/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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

#ifndef FModSpeexVoipHandler_H
#define FModSpeexVoipHandler_H

#include "voiceformat.h"
#include "voicesource.h"
// GREG BEGIN
//#include "voicenet.h"
// GREG END
#include "voiceheader.h"

// GREG BEGIN
/*#include <apr-1/apr_thread_proc.h>
#include <apr-1/apr_thread_mutex.h>*/
// GREG END
#include "pthread.h"

#include <boost/pool/pool.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <Socket.h>
#include <XmlDatasBasicTypes.h>

namespace FMOD
{
    class System;
    class Sound;
}

class VoiceBuffer;
class VoiceCodec;
struct VoicePacketHeader;

namespace Solipsis
{
	class IVoicePacketListener;
    class IVoiceEngineLogger;
}

/**
 *  A singleton class for handling voip communication
 */
class FModSpeexVoipHandler
{
public:
    typedef boost::shared_ptr<VoiceCodec> CodecPtr;

    /**
     *  @return VoiceEngine singleton instance
     */
    static FModSpeexVoipHandler* getInstance();

    /**
     * Constructor Perform the initialization with the sound system
     *  @param system External FMOD sound system or 0 if not available
     *  @param networkChunkSize Size of a network chunk, i.e. the amount of data that will be buffered before
     *                          it's sent to the server. The size is specified in PCM samples.
     *  @param bufferFrameCount The number of frames to hold in record buffer (buffer sample count = frequency * frame count)
     *  @param frequency Frequency of audio to record (affects only the default SPEEX codec)
     *  @param silenceLevel The silence level between 0 (silent) and 128 (loud)
     *  @param silenceLatencySec The silence latency during sound is sent/processed since last time silence was broken
     */
	FModSpeexVoipHandler(FMOD::System* system,
        Solipsis::IVoicePacketListener* pVoicePacketListener,
        size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000,
        float silenceLevel = 5.0f, unsigned int silenceLatencySec = 5);

    ~FModSpeexVoipHandler();

    /**
     * Connects to a voice chat server
     *  @param host The voice server hostname
     *  @param port The voice server port
     *  @param voiceId The unique user identifier connecting to the voice server, used to identify the sound source
     *  @return TRUE if successfully connected, FALSE otherwise
     */
	bool connect(const char* host, unsigned short port, const Solipsis::EntityUID& voiceId);

    /**
     * Closes the connection to a voice server if connected
     */
    void disconnect();

    /**
     * Returns true if engine is connected to the voice server.
     *  @return TRUE if engine is connected, FALSE otherwise
     */
    bool isConnected();

    /**
     * Updates the internal sound system. This should be called once
     * per frame even if an external sound system is supplied.
     */
    void update();

    /**
     * Set the silence detection parameters.
     *  @param port The silence level between 0 (silent) and 128 (loud)
     *  @param port The silence latency during sound is sent/processed since last time silence was broken
     */
	void setSilenceParams(float silenceLevel = 5.0f, unsigned int silenceLatencySec = 5);

    /**
     * Starts recording audio through an input device. The recorded
     * data will be automatically sent to the voice server.
     */
    void startRecording();

    /**
     * Stops recording audio and sending data to the voice server.
     */
    void stopRecording();

    /**
     * Returns true if engine is recording
     *  @return TRUE if the voice engine is currently recording audio, FALSE otherwise
     */
    bool isRecording() const;


    /**
     * Adds a new voice codec to the engine that can be used to
     * encode/decode incoming audio. If you want to encode outgoing
     * audio using this codec you can use setRecordCodec.
     */
    void addCodec(CodecPtr& codec);

    /**
     * Sets the codec used to encode outgoing audio data. A new
     * recording buffer will be created with the preferences of
     * the codec.
     *  @return TRUE on success, FALSE on failure
     */
    bool setRecordCodec(CodecPtr& codec);


//    void updateListener(float* pos, float* dir, float* vel);
	void updateAvatar(const Solipsis::EntityUID& id, float* pos, float* dir, float* vel);

    void setEnabled(bool enabled);

    /// log a message
    void logMessage(const std::string& message);

    /**
     * setLogger.
     *  @param logger The logger instance
     */
    void setLogger(Solipsis::IVoiceEngineLogger* logger) { mLogger = logger; }

private:
	int sendLogin(const Solipsis::EntityUID& voiceId);
    int sendRawAudio(unsigned int from, unsigned int to);
	/**
		@brief	sends audio data over the network

		@param	from	the index of the first sample to send in the recording buffer
		@param	to		index of the sample following the last sample to send in the recording buffer
		@return	the number of samples sent over the network
	*/
    int sendAudioFrames(unsigned int from, unsigned int to);
    int sendEnableVOIP(bool enabled);

	/**
		@brief	encodes a portion of the recording buffer

		@param	from	the index of the first sample to encode in the recording buffer
		@param	to		index of the sample following the last sample to encode in the recording buffer
		@param	buffer	pointer to the buffer receiving the encoded audio data
		@return	the encoded size in bytes
	*/
    unsigned short encodeAudioFrame(unsigned int from, unsigned int to, char* buffer);
    unsigned short encodeAudioFrame(char* data, unsigned int from, unsigned int to, char* buffer);

	/**
		@return	the number of bytes read from the socket
	*/
    int recvAudioFrames(unsigned int expectedSize);
	/**
		@return	the number of bytes read from the socket
	*/
    int recvVoiceHeader(VoicePacketHeader* header);

// GREG BEGIN
//    static void* APR_THREAD_FUNC receiveThread(apr_thread_t* thread, void* param);
    static void *receiveThread(void* param);
// GREG END

    VoiceBuffer* newAvatar(const Solipsis::EntityUID& id, VoiceCodec* codec);
    void removeAvatar(const Solipsis::EntityUID& id);

	VoiceBuffer* getAvatarVoiceBuffer(const Solipsis::EntityUID& id) const;

// GREG BEGIN
    bool silenceDetected(unsigned int from, unsigned int to);
// GREG END

private:
    static FModSpeexVoipHandler* mInstance;

    /// Logging instance
    Solipsis::IVoiceEngineLogger* mLogger;

    FMOD::System* mSystem;
    FMOD::Sound* mRecordSound;

    // Encoded audio format (i.e. codec id)
    int mEncodeFormat;

    // Audio frequency
    unsigned int mRecordSampleRate;

    // Audio sample size (in bytes)
    unsigned short mRecordSampleSize;

    // The number of frames to hold in record buffer
    unsigned int mBufferFrameCount;

    // The number of samples to record before sending data to the network
    size_t mNetworkChunkSize;

    // The position in the record buffer from where we last sent data (in PCM samples)
    unsigned int mLastRecordPos;
// GREG BEGIN
#ifdef TEST_PLAY_SOUND
    unsigned int mLastPos;
#endif
// GREG END

// GREG BEGIN
    // Silence detection level
    float mSilenceLevel;
    // Silence latency in seconds (minimal duration sending audio after last sound detected)
    unsigned int mSilenceLatencySec;
    // Silence detected, no audio sent
    bool mSilence;
    // Time of the last sound detected in seconds
    time_t mLastSoundDetectedTimeSec;
// GREG END

    bool mRecording;
    bool mUseExternalSystem;
    bool mRun;

    Socket mSock;

// GREG BEGIN
//    apr_thread_t* mReceiveThread;
//    apr_pool_t* mAprPool;
    pthread_t mReceiveThread;
// GREG END

    typedef std::map<int, CodecPtr> CodecMap;
    CodecMap mCodecs;
    int mSupportedFormats;

//    VoiceSource mSelfListener;		///< the motion state of the main avatar (ourselves). It is used to work out how we hear other sound sources (eg. are they far away ?)

// GREG BEGIN
//    apr_thread_mutex_t* mAvatarMutex;
    pthread_mutex_t mAvatarMutex;
// GREG END

	typedef std::map<Solipsis::EntityUID, VoiceBuffer*> BufferMap;
    BufferMap mBuffers;

    typedef std::map<Solipsis::EntityUID, VoiceSource> SourceMap;
    SourceMap mSources;

    // We need two pools because send/receive can happen at the same time
    boost::pool<> mReceivePool;
    boost::pool<> mSendPool;

    bool mEnabled;

	Solipsis::IVoicePacketListener*	mVoicePacketListener;	///< reference to the listener that is listening for voice packets

};  //  class VoiceEngine

#endif  //  FModSpeexVoipHandler_H
