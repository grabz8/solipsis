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

// ----------------------------------------------------------------------------
//
// TODO:
//  - Add support for multiple servers at the same time (and find out to which
//    one the local avatar belongs)
//  - Maybe VoiceBuffer and VoiceSource should be merged?
//  - Simplify avatar position updates (get rid of per avatar mutex locks etc.)
//  - Add VAD & DTX support
//
// ----------------------------------------------------------------------------

#include "FModSpeexVoipHandler.h"
#include "voicecodec.h"
#include "voiceheader.h"
#include "voicebuffer.h"
#include "voicecodecspeex.h"

// GREG BEGIN
//#include <fmodex/fmod.hpp>
#include <fmod.hpp>
// GREG END
#include <boost/pool/singleton_pool.hpp>

#include <iostream>
#include <IVoiceEngine.h>
#include <VoicePacket.h>
#include <IVoicePacketListener.h>
#include <AudioSequence.h>
#include <PhonetizerManager.h>
#include <IPhonetizer.h>
#include <PhonemeSequence.h>
#include <SimpleVoiceEngineProtocol.h>

using Solipsis::VoicePacket;
using Solipsis::AudioSequence;
using Solipsis::PhonetizerManager;
using Solipsis::PhonemeSequence;

// ----------------------------------------------------------------------------
// Helper functions

#define velogs std::cout << "[VOIP] "
#define veendl std::endl

using Solipsis::EntityUID;

namespace Solipsis
{
	/**
		@brief	class that provides access to sockets through stl stream mechanism

		@note	see http://spec.winprog.org/streams/ for details
	*/
	class SocketStreamBuffer : public std::streambuf
	{
	public:
		typedef std::char_traits<char> CharTraits;
		SocketStreamBuffer( Socket & socket ) : mSocket (socket) {}
	protected:
		//! @name	streambuf interface implementation
		//!	@{
			/**
				@brief	called whenever the put buffer is full
			*/
			virtual int_type overflow(int_type c = CharTraits::eof() )
			{
				if(c == CharTraits::eof())
				{
					return CharTraits::not_eof(c);
				}
				else
				{
					char ch = CharTraits::to_char_type(c);
					mSocket.send( &ch, 1 );
					return c;
				}
			}
			/**
				@brief	called whenever the get buffer is empty	
			*/
			virtual int_type uflow( void )
			{
				char c;
				unsigned int numBytesReceived = mSocket.receive( &c, sizeof(char) );
				assert(numBytesReceived == sizeof(char));
				return c;
			}
		//!	@}
	private:
		

		Socket & mSocket;
	};

	class SocketOStream : public std::ostream
	{
	public:
		SocketOStream( Socket & socket )
			: std::ostream(&mSocketStreamBuffer)
			, mSocketStreamBuffer(socket)
		{
		}
		virtual ~SocketOStream( void ) {}

		/*
		void writeFloat(const float & floatValue);
		{
			write( &floatValue, sizeof(float) );
		}

		void writeU8(const unsigned char & u8Value);
		{
			assert(sizeof(unsigned char) == 1);
			write( &u8Value, sizeof(unsigned char) );
		}

		void writeU16(const unsigned short & u16Value);
		{
			assert(sizeof(unsigned short) == 2);
			write( &u16Value, sizeof(unsigned short) );
		}

		void writeU32(const unsigned int & u32Value);
		{
			assert(sizeof(unsigned int) == 4);
			write( &u32Value, sizeof(unsigned int) );
		}
		*/

	private:
		SocketStreamBuffer	mSocketStreamBuffer;
	};

	class SocketIStream : public std::istream
	{
	public:
		SocketIStream( Socket & socket )
			: std::istream(&mSocketStreamBuffer)
			, mSocketStreamBuffer(socket)
		{
		}
		virtual ~SocketIStream( void ) {}

	private:
		SocketStreamBuffer	mSocketStreamBuffer;
	};

}


FMOD_SOUND_FORMAT voiceFormatToFmod(VoiceFormat format)
{
    switch(format)
    {
        case VF_PCM8: return FMOD_SOUND_FORMAT_PCM8;
        case VF_PCM16: return FMOD_SOUND_FORMAT_PCM16;
        case VF_PCM32: return FMOD_SOUND_FORMAT_PCM32;
    }

    return FMOD_SOUND_FORMAT_NONE;
}

unsigned short sampleSizeFromVoiceFormat(VoiceFormat format)
{
    switch(format)
    {
        case VF_PCM8: return sizeof(char);
        case VF_PCM16: return sizeof(short);
        case VF_PCM32: return sizeof(int);
    }

    return 0;
}


// ----------------------------------------------------------------------------
// Voice engine implementation

FModSpeexVoipHandler* FModSpeexVoipHandler::mInstance = 0;

typedef boost::singleton_pool<VoiceBuffer, sizeof(VoiceBuffer)> VoiceBufferPool;
typedef boost::singleton_pool<VoiceFrameHeader, sizeof(VoiceFrameHeader)> FrameHeaderPool;


FModSpeexVoipHandler* FModSpeexVoipHandler::getInstance()
{
    return mInstance;
}

FModSpeexVoipHandler::FModSpeexVoipHandler(FMOD::System* system,
                                           Solipsis::IVoicePacketListener* pVoicePacketListener,
                                           size_t networkChunkSizePCM, unsigned int bufferFrameCount, unsigned int frequency,
                                           float silenceLevel, unsigned int silenceLatencySec)
    : mSystem(system)
    , mRecordSound(0)
    , mRecordSampleRate(frequency)
    , mBufferFrameCount(bufferFrameCount)
    , mRecordSampleSize(0)
    , mRecording(false)
// GREG BEGIN
    , mSilenceLevel(silenceLevel)
    , mSilenceLatencySec(silenceLatencySec)
    , mSilence(true)
    , mLastSoundDetectedTimeSec(0)
// GREG END
    , mNetworkChunkSize(networkChunkSizePCM)
    , mUseExternalSystem(true)
    , mRun(false)
    , mLastRecordPos(0)
    , mSock()
// GREG BEGIN
//    , mReceiveThread(0)
//    , mAprPool(0)
    , mAvatarMutex(PTHREAD_MUTEX_INITIALIZER)
// GREG END
    , mSupportedFormats(VF_PCM8 | VF_PCM16 | VF_PCM32)
    , mReceivePool(1, 40960)
    , mSendPool(1, 40960)
    , mEnabled(true)
	, mVoicePacketListener( pVoicePacketListener )
{
    if (mSystem == 0)
    {
        mUseExternalSystem = false;
        if (FMOD::System_Create(&mSystem) != FMOD_OK)
        {
            velogs << "Error: Unable to create FMOD Ex sound system" << veendl;
            return;
        }

        unsigned int version;
        mSystem->getVersion(&version);
        if (version < FMOD_VERSION)
        {
            velogs << "Error: You are using an old version of FMOD Ex" << veendl;
            mSystem->release();
            mSystem = 0;
            return;
        }

        if (mSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT) != FMOD_OK)
            velogs << "Error: Unable to set output" << veendl;
        if (mSystem->setRecordDriver(0) != FMOD_OK)
            velogs << "Error: Unable to set record driver" << veendl;
        if (mSystem->init(32, FMOD_INIT_NORMAL, 0) != FMOD_OK)
            velogs << "Error: Unable to initialize FMOD EX sound system" << veendl;
    }

    mInstance = this;

// GREG BEGIN
    mReceiveThread.p = 0;
//    apr_pool_create(&mAprPool, NULL);
//    apr_thread_mutex_create(&mAvatarMutex, APR_THREAD_MUTEX_DEFAULT, mAprPool);
// GREG END

    CodecPtr speexCodec(new VoiceCodecSpeex(frequency));
    setRecordCodec(speexCodec);
}

FModSpeexVoipHandler::~FModSpeexVoipHandler()
{
    disconnect();

    for (BufferMap::iterator i = mBuffers.begin(); i != mBuffers.end(); ++i)
    {
        if (i->second)
        {
            i->second->~VoiceBuffer();
            VoiceBufferPool::free(i->second);
        }
    }

    if (mRecordSound) mRecordSound->release();
    mRecordSound = 0;

// GREG BEGIN
//    if (mSystem) mSystem->release();
//    mSystem = 0;
    if (!mUseExternalSystem)
    {
        if (mSystem) mSystem->release();
        mSystem = 0;
    }
// GREG END

// GREG BEGIN
/*    if (mAvatarMutex) apr_thread_mutex_destroy(mAvatarMutex);
    mAvatarMutex = 0;

    if (mAprPool) apr_pool_destroy(mAprPool);
    mAprPool = 0;*/
// GREG END

    mInstance = 0;
}

// -----------------------------------------------------------------------------
// Codec methods

void FModSpeexVoipHandler::addCodec(CodecPtr& codec)
{
    assert(codec.get() != 0);
    int format = codec->getEncodeFormat();
    mCodecs.insert(std::make_pair(format, codec));
    mSupportedFormats |= format;
}

bool FModSpeexVoipHandler::setRecordCodec(CodecPtr& codec)
{
    CodecMap::const_iterator i = mCodecs.find(codec->getEncodeFormat());
    if (i == mCodecs.end())
    {
        addCodec(codec);
        i = mCodecs.find(codec->getEncodeFormat());
        if (i == mCodecs.end())
            velogs << "Error adding new voice codec!" << veendl;
    }

    if (mRecordSound)
    {
        mRecordSound->release();
        mRecordSound = 0;
    }

    mEncodeFormat = i->second->getEncodeFormat();
    VoiceFormat format = i->second->getDecodeFormat();

    mRecordSampleSize = sampleSizeFromVoiceFormat(format);
    mRecordSampleRate = i->second->getSampleRate();

    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels = 1;
    exinfo.format = voiceFormatToFmod(format);
    exinfo.defaultfrequency = mRecordSampleRate;
    exinfo.length = mBufferFrameCount * exinfo.defaultfrequency * mRecordSampleSize;

// GREG BEGIN
#ifdef TEST_PLAY_SOUND
    mLastPos = 0;
    FMOD_RESULT fmodResult = mSystem->createSound("C:\\Acer\\AcerTour\\Music\\main.wav", FMOD_LOOP_OFF | FMOD_2D | FMOD_SOFTWARE, 0, &mRecordSound);
//    FMOD_RESULT fmodResult = mSystem->createSound("D:\\devpt\\gnet\\world\\sounds\\mp3s\\JerryC-Canon-Rock.mp3", FMOD_LOOP_OFF | FMOD_2D | FMOD_SOFTWARE, 0, &mRecordSound);
    if (fmodResult == FMOD_OK)
        return true;
#else
    if (mSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &mRecordSound) == FMOD_OK)
        return true;
#endif
// GREG END

    return false;
}

unsigned short FModSpeexVoipHandler::encodeAudioFrame(unsigned int from, unsigned int to, char* buffer)
{
    unsigned short outSize1 = 0;
    unsigned short outSize2 = 0;

    if (to > from)
    {
        CodecMap::const_iterator i = mCodecs.find(mEncodeFormat);
        if (i != mCodecs.end())
        {
            // A codec was found, encode data

            void* ptr1, *ptr2;
            unsigned int len1, len2;

            mRecordSound->lock(from * mRecordSampleSize, (to - from) * mRecordSampleSize, &ptr1, &ptr2, &len1, &len2);
            if (len1 > 0)
            {
                outSize1 = i->second->encode((const char*)ptr1, len1, buffer, len1);

                if (len2 > 0)
                {
                    outSize2 = i->second->encode((const char*)ptr2, len2, &buffer[outSize1], len2);
                }
            }
            mRecordSound->unlock(ptr1, ptr2, len1, len2);
        }
    }

    return outSize1 + outSize2;
}

unsigned short FModSpeexVoipHandler::encodeAudioFrame(char* data, unsigned int from, unsigned int to, char* buffer)
{
    if (to > from)
    {
        CodecMap::const_iterator i = mCodecs.find(mEncodeFormat);
        if (i != mCodecs.end())
        {
            unsigned int len = (to-from)*mRecordSampleSize;
            return i->second->encode((const char*)data, len, buffer, len);
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Recording methods

void FModSpeexVoipHandler::setSilenceParams(float silenceLevel, unsigned int silenceLatencySec)
{
    mSilenceLevel = silenceLevel;
    mSilenceLatencySec = silenceLatencySec;
}

void FModSpeexVoipHandler::startRecording()
{
    if (!mEnabled) return;

    assert(mSystem != 0);
    assert(mRecordSound != 0);

// GREG BEGIN
#ifdef TEST_PLAY_SOUND
    mLastRecordPos = 0;
#else
    if (mSystem->recordStart(mRecordSound, true) != FMOD_OK)
    {
        velogs << "Error: Failed to start recording" << veendl;
        return;
    }
#endif
// GREG END

    mRecording = true;
}

void FModSpeexVoipHandler::stopRecording()
{
    if (!mEnabled) return;

    assert(mSystem != 0);
    assert(mRecordSound != 0);

// GREG BEGIN
#ifdef TEST_PLAY_SOUND
#else
    if (mSystem->recordStop() != FMOD_OK)
    {
        velogs << "Error: Failed to stop recording" << veendl;
        return;
    }
#endif
// GREG END

    mRecording = false;
}

bool FModSpeexVoipHandler::isRecording() const
{
    return mRecording;
}

// -----------------------------------------------------------------------------
// Update methods

void FModSpeexVoipHandler::update()
{
    // Update avatar voices
	// play the sounds of the speeches of the avatars
//     if (apr_thread_mutex_trylock(mAvatarMutex) != APR_EBUSY)
    {
// GREG BEGIN
//        apr_thread_mutex_lock(mAvatarMutex);
        pthread_mutex_lock(&mAvatarMutex);
// GREG END
        for (BufferMap::const_iterator i = mBuffers.begin(); i != mBuffers.end(); ++i)
        {
			const Solipsis::EntityUID voiceUuid = i->first;
			VoiceBuffer* pVoiceBuffer = i->second;
            unsigned int lastpos = pVoiceBuffer->getLastPosition();
            bool playing = pVoiceBuffer->isPlaying();

            unsigned int playbackOffset = (unsigned int)mNetworkChunkSize;
            if (playing)
            {
                unsigned int pos = 0;
                pVoiceBuffer->getChannel()->getPosition(&pos, FMOD_TIMEUNIT_PCM);
                if ((pos > lastpos) && ((pos - lastpos) < playbackOffset))
                {
					// this means that the player is starving (doesn't receive audio data quick enough so that it arrives before we need it)
                    velogs << "Warning: No voice playback data available (pos:"
                           << pos << " lastPos:" << lastpos << " offset:" << playbackOffset << ")" << veendl;
                    pVoiceBuffer->stop();

                    AvatarHandlerMap::iterator ahi = mAvatarHandlers.find(voiceUuid);
                    if (ahi != mAvatarHandlers.end())
                        ahi->second->onTalking(false);
                }
            }

            if ((lastpos >= playbackOffset) && !playing)
            {
				// we received enough data to start playing the sound
                velogs << "Starting playback" << veendl;
                pVoiceBuffer->play(mSystem);

                AvatarHandlerMap::iterator ahi = mAvatarHandlers.find(voiceUuid);
                if (ahi != mAvatarHandlers.end())
                    ahi->second->onTalking(true);
            }

			// update the way the sound is played, depending on the position (and velocity) of the speaking avatar
            SourceMap::iterator sourceIt = mSources.find(voiceUuid);
            if (sourceIt != mSources.end())
            {
                VoiceSource& source = sourceIt->second;
                if ((!playing && pVoiceBuffer->isPlaying()) ||
                    (playing && source.isPosDirVelUpdated()))
                {
                    FMOD_VECTOR pos, vel;
                    source.getPosition(&pos.x, &pos.y, &pos.z);
                    source.getVelocity(&vel.x, &vel.y, &vel.z);
                    pVoiceBuffer->getChannel()->set3DAttributes(&pos, &vel);
                    source.resetPosDirVelUpdated();
                }
                if (source.isDistancesUpdated())
                {
                    float minDist, maxDist;
                    source.getDistances(&minDist, &maxDist);
                    pVoiceBuffer->getSound()->set3DMinMaxDistance(minDist, maxDist);
                    source.resetDistancesUpdated();
                }
            }
        }

// GREG BEGIN
//        apr_thread_mutex_unlock(mAvatarMutex);
        pthread_mutex_unlock(&mAvatarMutex);
// GREG END
    }

    if (!isRecording() || !mEnabled)
        return;

    unsigned int length = mRecordSampleRate * mBufferFrameCount;

    unsigned int recordpos;
// GREG BEGIN
#ifdef TEST_PLAY_SOUND
//    if (mLastPos <= mNetworkChunkSize*2) // only first packet
        mLastPos += 1000;
    recordpos = mLastPos;
    unsigned int totalLength;
    mRecordSound->getLength(&totalLength, FMOD_TIMEUNIT_RAWBYTES);
    if (recordpos >= totalLength)
        recordpos = 0;
#else
    if (mSystem->getRecordPosition(&recordpos) != FMOD_OK)
        return;
#endif
// GREG END

// GREG BEGIN
    // Silence detection
    time_t now;
    time(&now);
    if (recordpos >= mLastRecordPos)
    {
        if (!silenceDetected(mLastRecordPos, recordpos))
            mLastSoundDetectedTimeSec = now;
    }
    else
    {
        if (!silenceDetected(mLastRecordPos, length))
            mLastSoundDetectedTimeSec = now;
    }
    if (now - mLastSoundDetectedTimeSec > mSilenceLatencySec)
        mSilence = true;
    else
        mSilence = false;
    if (mSilence)
    {
        mLastRecordPos = recordpos;
        return;
    }
// GREG END

    // If we have recorded mNetworkChunkSize since the last sent frame
    // send a new audio chunk to the network
    if ((recordpos >= mNetworkChunkSize) && ((recordpos - mNetworkChunkSize) >= mLastRecordPos))
    {
        mLastRecordPos += sendAudioFrames(mLastRecordPos, recordpos);
        if (mLastRecordPos >= length) mLastRecordPos = mLastRecordPos-length;
    }

    // If the recording has restarted from the beginning

    if (mLastRecordPos > recordpos)
    {
        if ((mLastRecordPos < length) && (mLastRecordPos > length-mNetworkChunkSize))
            sendAudioFrames(mLastRecordPos, length);

        mLastRecordPos = 0;
    }
}

// -----------------------------------------------------------------------------
// Avatar methods

void FModSpeexVoipHandler::updateRecordingAvatar(float* pos, float* dir, float* vel, float* dist)
{
    mRecordingSource.setPosition(pos);
    mRecordingSource.setDirection(dir);
    mRecordingSource.setVelocity(vel);
    mRecordingSource.setDistances(dist);
}

VoiceBuffer* FModSpeexVoipHandler::newAvatar(const EntityUID& id, VoiceCodec* codec)
{
    // Create avatar sound
    logMessage("FModSpeexVoipHandler::newAvatar(" + id + ")");

    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels = 1;
    exinfo.format = voiceFormatToFmod(codec->getDecodeFormat());
    exinfo.defaultfrequency = codec->getSampleRate();
    exinfo.length = mBufferFrameCount * exinfo.defaultfrequency * sampleSizeFromVoiceFormat(codec->getDecodeFormat());

    FMOD::Sound* sound = 0;
    if (mSystem->createSound(0, FMOD_3D | FMOD_OPENUSER | FMOD_LOOP_NORMAL, &exinfo, &sound) != FMOD_OK)
    {
        return 0;
    }

    // Set initial distances
    sound->set3DMinMaxDistance(1.0f, 100.0f);

    // Create voice buffer and source
    VoiceBuffer* buffer = new(VoiceBufferPool::malloc()) VoiceBuffer(sound);
// GREG BEGIN
//    apr_thread_mutex_lock(mAvatarMutex);
    pthread_mutex_lock(&mAvatarMutex);
// GREG END
    mBuffers.insert(std::make_pair(id, buffer));
    mSources.insert(std::make_pair(id, VoiceSource()));

    AvatarHandlerMap::iterator ahi = mAvatarHandlers.find(id);
    if (ahi != mAvatarHandlers.end())
        ahi->second->onVoiceCreation();
// GREG BEGIN
//    apr_thread_mutex_unlock(mAvatarMutex);
    pthread_mutex_unlock(&mAvatarMutex);
// GREG END

    // Create voice decoders

    codec->allocDecoder(id);
//     for (CodecMap::iterator i = mCodecs.begin(); i != mCodecs.end(); ++i)
//     {
//         i->second->allocDecoder(id);
//     }

    return buffer;
}

void FModSpeexVoipHandler::removeAvatar(const EntityUID& id)
{
    // Release voice buffer and source
    logMessage("FModSpeexVoipHandler::removeAvatar(" + id + ")");

// GREG BEGIN
//    apr_thread_mutex_lock(mAvatarMutex);
    pthread_mutex_lock(&mAvatarMutex);
// GREG END

    AvatarHandlerMap::iterator ahi = mAvatarHandlers.find(id);
    if (ahi != mAvatarHandlers.end())
        ahi->second->onVoiceDestruction();

    BufferMap::iterator b = mBuffers.find(id);
    if (b != mBuffers.end())
    {
        if (b->second)
        {
            b->second->~VoiceBuffer();
            VoiceBufferPool::free(b->second);
            b->second = 0;
        }
        mBuffers.erase(b);
    }

    SourceMap::iterator s = mSources.find(id);
    if (s != mSources.end())
    {
        mSources.erase(s);
    }

// GREG BEGIN
//    apr_thread_mutex_unlock(mAvatarMutex);
    pthread_mutex_unlock(&mAvatarMutex);
// GREG END

    // Release voice decoders

    for (CodecMap::iterator i = mCodecs.begin(); i != mCodecs.end(); ++i)
    {
        if (i->second->hasDecoder(id))
            i->second->releaseDecoder(id);
    }
}

VoiceBuffer* FModSpeexVoipHandler::getAvatarVoiceBuffer(const Solipsis::EntityUID& id) const
{
    BufferMap::const_iterator i = mBuffers.find(id);
    if (i == mBuffers.end())
        return 0;

    return i->second;
}

// -----------------------------------------------------------------------------
// Network methods

bool FModSpeexVoipHandler::connect(const char* host, unsigned short port, const EntityUID& voiceId)
{
	if (mSock.isValid())
        disconnect();

// GREG BEGIN
	Socket::initialize();
// GREG END
    if (mSock.create())
    {
		if (mSock.connect(std::string(host), port))
        {
            sendLogin(voiceId);
            sendEnableVOIP(mEnabled);
            mRun = true;

// GREG BEGIN
//            apr_thread_create(&mReceiveThread, NULL, receiveThread, (void*)this, mAprPool);
            pthread_create(&mReceiveThread, NULL, receiveThread, (void*)this);
// GREG END
            return true;
        }

        mRun = false;
		mSock.close();
    }

    return false;
}

void FModSpeexVoipHandler::disconnect()
{
	mSock.close();

    mRun = false;
// GREG BEGIN
//    if (mReceiveThread)
    if (mReceiveThread.p)
// GREG END
    {
// GREG BEGIN
//        apr_status_t ret = 0;
//        apr_thread_join(&ret, mReceiveThread);
        int ret = pthread_join(mReceiveThread, NULL);
// GREG BEGIN
        if (ret != 0)
        {
            velogs << "Warning: Unable to shut down receive thread" << veendl;
        }
    }
// GREG BEGIN
//    mReceiveThread = 0;
    mReceiveThread.p = 0;
// GREG END
}

bool FModSpeexVoipHandler::isConnected()
{
    return mRun;
}

int FModSpeexVoipHandler::recvVoiceHeader(VoicePacketHeader* header)
{
    int received = 0;
    int headerSize = sizeof(VoicePacketHeader);
    while (received < headerSize)
    {
		int bytesReceived = mSock.receive( &((char*)header)[received], headerSize-received);
        received += bytesReceived;
        if (bytesReceived <= 0) return bytesReceived;
    }

    return received;
}

int FModSpeexVoipHandler::sendLogin(const EntityUID& voiceId)
{
    int sent = 0;
	int voiceIdSerializedSize = (int)voiceId.size() + sizeof(unsigned int);
	sent += SimpleVoiceEngineProtocol::sendPacketHeader(mSock, VP_LOGIN, voiceIdSerializedSize + sizeof(int));
	unsigned int voiceIdSize = (unsigned int)voiceId.size();
	sent += mSock.send((const char*)&voiceIdSize, sizeof(unsigned int));
	sent += mSock.send((const char*)voiceId.c_str(), (int)voiceIdSize);
    sent += mSock.send((const char*)&mSupportedFormats, sizeof(mSupportedFormats));
    return sent;
}

int FModSpeexVoipHandler::sendEnableVOIP(bool enabled)
{
    int sent = 0;
    sent += SimpleVoiceEngineProtocol::sendPacketHeader(mSock, VP_ENABLE_VOIP, sizeof(char));
    char val = enabled ? 1 : 0;
    sent += mSock.send(&val, sizeof(char));
    return sent;
}

// GREG BEGIN
bool FModSpeexVoipHandler::silenceDetected(unsigned int from, unsigned int to)
{
//	printf( "FModSpeexVoipHandler::silenceDetected : start\n" );

    assert(to >= from);

    // we will compute the surface of the spectrum bounded between -127..127
    unsigned int sampleCount = (to - from)/mRecordSampleSize;
    if (sampleCount < 1) return true;
#define SILENCE_DETECTION_SAMPLE_RATE 1000.0f
    unsigned int samplesInc = (unsigned int)((float)mRecordSampleRate/SILENCE_DETECTION_SAMPLE_RATE);
    if (samplesInc == 0)
        samplesInc = 1;

	void* pptr1, *pptr2;
	unsigned int plen1, plen2;

	mRecordSound->lock(from*mRecordSampleSize, sampleCount*mRecordSampleSize, &pptr1, &pptr2, &plen1, &plen2);
    unsigned int surface = 0;
	unsigned int len1, len2;
    len1 = plen1; len2 = plen2;
    switch (mRecordSampleSize)
    {
        case sizeof(char):
            {
                for (char *ptr1 = (char*)pptr1; (char*)ptr1 < (char*)pptr1 + len1; ptr1 += samplesInc)
                    surface += (unsigned int)((*ptr1 > 0) ? (*ptr1) : -(*ptr1))>>8;
                for (char *ptr2 = (char*)pptr2; (char*)ptr2 < (char*)pptr2 + len2; ptr2 += samplesInc)
                    surface += (unsigned int)((*ptr2 > 0) ? (*ptr2) : -(*ptr2))>>8;
            }
            break;
        case sizeof(short):
            {
                for (short *ptr1 = (short*)pptr1; (char*)ptr1 < (char*)pptr1 + len1; ptr1 += samplesInc)
                    surface += (unsigned int)((*ptr1 > 0) ? (*ptr1) : -(*ptr1))>>8;
                for (short *ptr2 = (short*)pptr2; (char*)ptr2 < (char*)pptr2 + len2; ptr2 += samplesInc)
                    surface += (unsigned int)((*ptr2 > 0) ? (*ptr2) : -(*ptr2))>>8;
            }
            break;
        case sizeof(int):
            {
                for (int *ptr1 = (int*)pptr1; (char*)ptr1 < (char*)pptr1 + len1; ptr1 += samplesInc)
                    surface += (unsigned int)((*ptr1 > 0) ? (*ptr1) : -(*ptr1))>>8;
                for (int *ptr2 = (int*)pptr2; (char*)ptr2 < (char*)pptr2 + len2; ptr2 += samplesInc)
                    surface += (unsigned int)((*ptr2 > 0) ? (*ptr2) : -(*ptr2))>>8;
            }
            break;
    }
	mRecordSound->unlock(pptr1, pptr2, plen1, plen2);

    // the sum (surface) is now computed relative to the number of samples taken
    // max surface of 1 sample = 127
    // max surface = (sampleCount/samplesInc)*127
    // 0 <= surfaceRel <= 127
    float surfaceRel = (float)surface/(float)(sampleCount/samplesInc);
//#ifdef _DEBUG
//    char tmp[256];
//    _snprintf(tmp, sizeof(tmp) - 1, "FModSpeexVoipHandler::silenceDetected() %.8f", surfaceRel );
//    logMessage(tmp);
//#endif
    return (surfaceRel < mSilenceLevel);
}
// GREG END

int FModSpeexVoipHandler::sendAudioFrames(unsigned int from, unsigned int to)
{
	printf( "FModSpeexVoipHandler::sendAudioFrames : start\n" );

    assert(to >= from);
// GREG BEGIN
    // Speex sometimes crash when using big audio buffer close to 64Kb
    // so we ensure buffers won t exceed 32Kb
    if (to - from > 32768)
    {
        char tmp[256];
        _snprintf(tmp, sizeof(tmp) - 1, "FModSpeexVoipHandler::sendAudioFrames() too much audio datas %d (>32768), keeping last 32768 bytes.", to - from);
        logMessage(tmp);
        from = to - 32768;
    }
// GREG END

    VoicePacketHeader header;
    header.format = mEncodeFormat;

    CodecMap::const_iterator i = mCodecs.find(mEncodeFormat);
    if (i != mCodecs.end())
    {
		VoiceCodec* pCodec = i->second.get();
        // Encoded audio, encode in frames equal to codec frame size and send all available frames
        // as a chunk as big as possible
        unsigned short frameSizePCM = pCodec->getFrameSizePCM();
        unsigned int chunkSize = to - from;
        if (chunkSize >= frameSizePCM)
        {
            // Calculate frame and sample count
            unsigned short numFrames = chunkSize/frameSizePCM;
            unsigned int sampleCount = numFrames*frameSizePCM;

            bool constantFrameSize = pCodec->isEncodedSizeConstant();

            // Allocate only one frame header if frame size is constant
            size_t headerCount = constantFrameSize ? 1 : numFrames;
            VoiceFrameHeader* frameHeaders = (VoiceFrameHeader*)FrameHeaderPool::ordered_malloc(headerCount);

            // Encode audio frames to a temporary buffer

			// the size of the encoding buffer is made the same as the size of the raw audio to make sure we have enough room
			size_t uncompressedAudioSize = numFrames * frameSizePCM * mRecordSampleSize;
            size_t encodingBufferSize = uncompressedAudioSize;
            char* encBuffer = (char*)mSendPool.ordered_malloc(encodingBufferSize);

            //
            // TODO: Which is faster: multiple locks/unlocks or a memcpy to a temporary buffer and only one lock/unlock?
            //
//             char* audioBuffer = (char*)mSendPool.ordered_malloc(bufferSize);
//             void* ptr1, *ptr2;
//             unsigned int len1, len2;
//             mRecordSound->lock(from * mRecordSampleSize, sampleCount * mRecordSampleSize, &ptr1, &ptr2, &len1, &len2);
//             if (len1 > 0)
//             {
//                 memcpy(audioBuffer, ptr1, len1);
//                 if (len2 > 0) memcpy(&audioBuffer[len1], ptr2, len2);
//             }
//             mRecordSound->unlock(ptr1, ptr2, len1, len2);

            unsigned short encodedSize = 0;
            unsigned int lastFrom = from;
            unsigned int numFramesSkipped = 0;
            for (int i = 0; i<numFrames; ++i)
            {
                unsigned short frameEncodedSize = encodeAudioFrame(lastFrom, lastFrom+frameSizePCM, &encBuffer[encodedSize]);
//                 unsigned short encodedSize = encodeAudioFrame(&audioBuffer[i*frameSizePCM*mRecordSampleSize],
//                                                    lastFrom, lastFrom+frameSizePCM, &encBuffer[size]);
                lastFrom += frameSizePCM;
                if (frameEncodedSize == 0)
                {
					// how can this happen ?
                    ++numFramesSkipped;
                    continue;
                }

                if (constantFrameSize)
                    frameHeaders->frameSize = frameEncodedSize;
                else if (frameHeaders->frameSize == 0)
                    frameHeaders[i-numFramesSkipped].frameSize = frameEncodedSize;

                encodedSize += frameEncodedSize;
            }

            numFrames -= numFramesSkipped;

            header.frames = numFrames;
            header.decodedAudioSize = numFrames*frameSizePCM*mRecordSampleSize;
            header.audioSize = encodedSize;

            unsigned int frameHeadersSize = (unsigned int)sizeof(VoiceFrameHeader)*headerCount;

			// generate the phone sequence from the audio
			PhonemeSequence* pPhonemeSequence;
			{
				assert( mRecordSampleSize == 2 ); // make sure the raw audio data is available as PCM16 format

				short* pAudioData = new short [sampleCount];

				void* ptr1, *ptr2;
				unsigned int len1, len2;

				mRecordSound->lock(from * mRecordSampleSize, sampleCount * mRecordSampleSize, &ptr1, &ptr2, &len1, &len2);
				assert((len1+len2) == sampleCount * mRecordSampleSize);
				if (len1 > 0)
				{
					memcpy( pAudioData, ptr1, len1 );

					if (len2 > 0)
					{
						memcpy( &pAudioData[len1], ptr2, len2 );
					}
				}
				mRecordSound->unlock(ptr1, ptr2, len1, len2);

				AudioSequence audioSequence = AudioSequence( pCodec->getSampleRate(), sampleCount, pAudioData, false );
				pPhonemeSequence = PhonetizerManager::getSingleton().getSelectedPhonetizer()->audioToPhonemes( audioSequence );
				delete [] pAudioData;
			}

            // Add 3D sound properties into header
            header.flags = VPF_NONE;
            unsigned int flagsSize = 0;
            float flagsDatas[3*3 + 2];
            float *pflagsDatas = flagsDatas;
            if (mRecordingSource.isPosDirVelUpdated())
            {
                header.flags |= VPF_POSDIRVEL;
                flagsSize += 3*3*sizeof(float);
                mRecordingSource.getPosition(pflagsDatas); pflagsDatas += 3;
                mRecordingSource.getDirection(pflagsDatas); pflagsDatas += 3;
                mRecordingSource.getVelocity(pflagsDatas); pflagsDatas += 3;
                mRecordingSource.resetPosDirVelUpdated();
            }
            if (mRecordingSource.isDistancesUpdated())
            {
                header.flags |= VPF_DISTANCES;
                flagsSize += 2*sizeof(float);
                mRecordingSource.getDistances(pflagsDatas); pflagsDatas += 2;
                mRecordingSource.resetDistancesUpdated();
            }

            // Send headers and audio data to server

            if (header.audioSize != 0)
            {
                SimpleVoiceEngineProtocol::sendPacketHeader(mSock, VP_AUDIO_TO_SERVER, sizeof(header) + flagsSize + frameHeadersSize + header.audioSize + pPhonemeSequence->getSerializedSize());
                mSock.send((const char*)&header, sizeof(header));
                if (flagsSize > 0)
                    mSock.send((const char*)&flagsDatas, flagsSize);
                mSock.send((const char*)frameHeaders, frameHeadersSize);
                mSock.send(encBuffer, header.audioSize);
            }

			// send phonemes
			{
				Solipsis::SocketOStream stream( mSock );
				pPhonemeSequence->serialize( stream );
			}
			//std::string talkingAvatarUid = "SAmLRc8iixAu66ssbS-ke0F_00000000";

            // Free temporary buffers

            FrameHeaderPool::ordered_free(frameHeaders, headerCount);
            frameHeaders = 0;

            mSendPool.ordered_free(encBuffer, encodingBufferSize);
            encBuffer = 0;
//             mSendPool.ordered_free(audioBuffer, bufferSize);
//             audioBuffer = 0;
			printf( "FModSpeexVoipHandler::sendAudioFrames : end\n" );

            return sampleCount;
        }
		else
		{
			// not enough data to send (data to send is smaller than a frame)... we don't send anything for now
		}
    }
	else
	{
		assert(false); // unable to find the encoder
	}
	printf( "FModSpeexVoipHandler::sendAudioFrames : end (alternative)\n" );
    return 0;
}

int FModSpeexVoipHandler::recvAudioFrames(unsigned int expectedSize)
{
	printf( "FModSpeexVoipHandler::recvAudioFrames : start\n" );

    int size = 0;
    int received = 0;

	Solipsis::EntityUID avatarId;
	received = SimpleVoiceEngineProtocol::receiveVoiceUid(mSock, avatarId);
    size += received;
    if (received <= 0) return size;

    // Read audio header

    VoicePacketHeader header;
    received = recvVoiceHeader(&header);
    size += received;
    if (received <= 0) return size;

    // Check for codec format and do a dummy receive if the codec is unknown

    received = 0;
    CodecMap::const_iterator codecIt = mCodecs.find(header.format);
    if (codecIt == mCodecs.end())
    {
        velogs << "Warning: Avatar has an unknown voice codec (id:" << header.format << ")" << veendl;
        char* data = (char*)mReceivePool.ordered_malloc(expectedSize - size);
		size += mSock.receive(data, expectedSize - size);
        mReceivePool.ordered_free(data, expectedSize - size);
        return size;
    }
	VoiceCodec* pCodec = codecIt->second.get();

    // Create new avatar if not found

    VoiceBuffer* voice = getAvatarVoiceBuffer(avatarId);
    if (voice == 0)
    {
        voice = newAvatar(avatarId, pCodec);
        if (!voice)
            return size;
    }

    // Get 3D sound
    if (header.flags != VPF_NONE)
    {
        unsigned int flagsSize = 0;
        if (header.flags & VPF_POSDIRVEL) flagsSize += 3*3*sizeof(float);
        if (header.flags & VPF_DISTANCES) flagsSize += 2*sizeof(float);
        float flagsDatas[3*3 + 2];
        float *pflagsDatas = flagsDatas;
        received = 0;
        while (received < flagsSize)
        {
			int packetBytes = mSock.receive(&((char*)flagsDatas)[received], flagsSize-received);
            size += packetBytes;
            received += packetBytes;
            if (packetBytes <= 0) return size;
        }
        SourceMap::iterator sourceIt = mSources.find(avatarId);
        if (sourceIt != mSources.end())
        {
            if (header.flags & VPF_POSDIRVEL)
            {
                sourceIt->second.setPosition(pflagsDatas); pflagsDatas += 3;
                sourceIt->second.setDirection(pflagsDatas); pflagsDatas += 3;
                sourceIt->second.setVelocity(pflagsDatas); pflagsDatas += 3;
            }
            if (header.flags & VPF_DISTANCES)
                sourceIt->second.setDistances(pflagsDatas); pflagsDatas += 2;
        }
    }

    // Read frame headers

    bool constantFrameSize = pCodec->isEncodedSizeConstant();
    int headerCount = constantFrameSize ? 1 : header.frames;
    VoiceFrameHeader* frameHeaders = (VoiceFrameHeader*)FrameHeaderPool::ordered_malloc(headerCount);

    for (int h = 0; h < headerCount; ++h)
    {
        received = 0;
        while (received < sizeof(VoiceFrameHeader))
        {
			int packetBytes = mSock.receive(&((char*)&frameHeaders[h])[received],
                                                 sizeof(VoiceFrameHeader)-received);
            size += packetBytes;
            received += packetBytes;
            if (packetBytes <= 0) return size;
        }
    }

	printf( "FModSpeexVoipHandler::recvAudioFrames : after reading frame headers\n" );

    // Read encoded audio data

    received = 0;
    char* data = (char*)mReceivePool.ordered_malloc(header.audioSize);
    while (received < header.audioSize)
    {
		int bytesReceived = mSock.receive(&data[received], header.audioSize-received);
        received += bytesReceived;
        size += bytesReceived;
        if (bytesReceived <= 0) return size;
    }

	printf( "FModSpeexVoipHandler::recvAudioFrames : after reading encoded audio data\n" );
	/*
	{
		char c;
		unsigned int numBytesReceived = mSock.receive( &c, sizeof(char) );
	}
	*/

	// read phonemes
	Solipsis::SocketIStream inputStream( mSock );
	PhonemeSequence* pPhonemeSequence = PhonemeSequence::createFromStream( inputStream );
	printf( "FModSpeexVoipHandler::recvAudioFrames : after creation of phonemes from stream\n" );
	assert( pPhonemeSequence );

	size += pPhonemeSequence->getSerializedSize();

    // Decode audio data

    char* audioData = 0;
    unsigned short decodedSize = 0;

//     // Test sound to detect jittering
//     unsigned int lastPos = voice->getLastPosition();
//     decodedSize = header.decodedAudioSize;
//     audioData = (char*)mReceivePool.ordered_malloc(header.decodedAudioSize);
//     short* ad = (short*)audioData;
//     for (int i = 0; i < decodedSize/2; ++i)
//     {
//         ad[i] = (short)(sinf((i+lastPos)/10.0f)*32767);
//     }

    unsigned short sampleSize = sampleSizeFromVoiceFormat(pCodec->getDecodeFormat());
    audioData = (char*)mReceivePool.ordered_malloc(header.decodedAudioSize);

    unsigned short frameSizeDecoded = pCodec->getFrameSizePCM() * sampleSize;
    size_t offset = 0;
    for (unsigned short f = 0; f < header.frames; ++f)
    {
        // Get encoded frame size
        unsigned short frameSizeEncoded = constantFrameSize ? frameHeaders->frameSize : frameHeaders[f].frameSize;

        // Decode encoded frame to audio buffer
        decodedSize += pCodec->decode(avatarId, &data[offset], frameSizeEncoded,
                                             &audioData[f*frameSizeDecoded], frameSizeDecoded);
        offset += frameSizeEncoded;
    }

    FMOD::Sound* sound = voice->getSound();

    unsigned int len = 0;
    sound->getLength(&len, FMOD_TIMEUNIT_PCM);

    unsigned int lastPos = voice->getLastPosition();

    void* ptr1, *ptr2;
    unsigned int len1, len2;
    sound->lock(lastPos * sampleSize, decodedSize, &ptr1, &ptr2, &len1, &len2);

    if (len1 + len2 == decodedSize)
    {
        // Copy audio data

        if (len1 > 0)
        {
            memcpy(ptr1, audioData, len1);
            if (len2 > 0) memcpy(ptr2, &audioData[len1], len2);
        }
        else if (len2 > 0)
        {
            memcpy(ptr2, audioData, len2);
        }

        lastPos += decodedSize / sampleSize;
        if (lastPos >= len)
            lastPos = lastPos - len;
    }
    else
    {
        velogs << "Error: Invalid audio data received - " << decodedSize << ", "
                << len1 << "+" << len2 << "/" << len << veendl;
    }

    sound->unlock(ptr1, ptr2, len1, len2);
    voice->setLastPosition(lastPos);

	{
		// send phonemes

		VoicePacket* pVoicePacket = new VoicePacket( avatarId );


		assert( (decodedSize % sampleSize) == 0 );
		pVoicePacket->setNumAudioSamples( decodedSize / sampleSize );
		pVoicePacket->setAudioFrequency( pCodec->getSampleRate() );

		pVoicePacket->setPhonemeSequence( pPhonemeSequence );
		if(mVoicePacketListener)
		{
			mVoicePacketListener->onVoicePacketReception( pVoicePacket );
		}
		else
		{
			delete pVoicePacket;
		}
	}

    // Free audio headers and data

    mReceivePool.ordered_free(data, header.audioSize);
    mReceivePool.ordered_free(audioData, header.decodedAudioSize);
    FrameHeaderPool::ordered_free(frameHeaders, constantFrameSize ? 1 : header.frames);


	printf( "FModSpeexVoipHandler::recvAudioFrames : end\n" );
    return size;
}

// -----------------------------------------------------------------------------
// Network data receiver thread

// GREG BEGIN
//void* APR_THREAD_FUNC VoiceEngine::receiveThread(apr_thread_t* thread, void* param)
void *FModSpeexVoipHandler::receiveThread(void* param)
// GREG END
{
    FModSpeexVoipHandler* ve = (FModSpeexVoipHandler*)param;

    while (ve->mRun)
    {
        VoicePacketType packetType;
        unsigned int packetSize;
		if (SimpleVoiceEngineProtocol::receivePacketHeader(ve->mSock, &packetType, &packetSize) <= 0)
            return 0;

        switch (packetType)
        {
            case VP_AUDIO_TO_CLIENT:
                {
                    int receivedSize = ve->recvAudioFrames(packetSize);
                    if (receivedSize <= 0)
                        return 0;

                    if (receivedSize != packetSize)
                    {
                        velogs << "Warning: receivedSize != packetSize ("
                                << receivedSize << "/" << packetSize << ")" << veendl;
                    }
                    break;
                }

            default:
                velogs << "Unknown packet header received: type " << packetType << ", size " << packetSize
                        << ". Shutting down receive thread..." << veendl;
                return 0;
        }
    }

    return 0;
}

// ----------------------------------------------------------------------------

void FModSpeexVoipHandler::setEnabled(bool enabled)
{
    mEnabled = enabled;
	if (mSock.isValid())
        sendEnableVOIP(mEnabled);
}

// ----------------------------------------------------------------------------

void FModSpeexVoipHandler::logMessage(const std::string& message)
{
    if (mLogger != 0)
        mLogger->logMessage(message);
}

// ----------------------------------------------------------------------------

void FModSpeexVoipHandler::setAvatarHandler(const Solipsis::EntityUID& voiceId, Solipsis::IVoiceEngineAvatarHandler* avatarHandler)
{
	assert(mAvatarHandlers.find(voiceId) == mAvatarHandlers.end());
    mAvatarHandlers[voiceId] = avatarHandler;
}

// ----------------------------------------------------------------------------

void FModSpeexVoipHandler::removeAvatarHandler(const Solipsis::EntityUID& voiceId)
{
	assert(mAvatarHandlers.find(voiceId) != mAvatarHandlers.end());
	mAvatarHandlers.erase(voiceId);
}

// ----------------------------------------------------------------------------
