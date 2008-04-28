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

#include "voiceengine.h"
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


// ----------------------------------------------------------------------------
// Helper functions

#define velogs std::cout << "[VOIP] "
#define veendl std::endl

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

VoiceEngine* VoiceEngine::mInstance = 0;

typedef boost::singleton_pool<VoiceBuffer, sizeof(VoiceBuffer)> VoiceBufferPool;
typedef boost::singleton_pool<VoiceFrameHeader, sizeof(VoiceFrameHeader)> FrameHeaderPool;


VoiceEngine* VoiceEngine::getInstance()
{
    return mInstance;
}

VoiceEngine::VoiceEngine(FMOD::System* system, size_t networkChunkSizePCM,
                             unsigned int bufferFrameCount, unsigned int frequency)
    : mSystem(system)
    , mRecordSound(0)
    , mRecordSampleRate(frequency)
    , mBufferFrameCount(bufferFrameCount)
    , mRecordSampleSize(0)
    , mRecording(false)
    , mNetworkChunkSize(networkChunkSizePCM)
    , mUseExternalSystem(true)
    , mRun(true)
    , mLastRecordPos(0)
    , mSock(VE_INVALID_SOCKET)
// GREG BEGIN
//    , mReceiveThread(0)
//    , mAprPool(0)
    , mAvatarMutex(PTHREAD_MUTEX_INITIALIZER)
// GREG END
    , mSupportedFormats(VF_PCM8 | VF_PCM16 | VF_PCM32)
    , mReceivePool(1, 40960)
    , mSendPool(1, 40960)
    , mEnabled(true)
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

VoiceEngine::~VoiceEngine()
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

void VoiceEngine::addCodec(CodecPtr& codec)
{
    assert(codec.get() != 0);
    int format = codec->getEncodeFormat();
    mCodecs.insert(std::make_pair(format, codec));
    mSupportedFormats |= format;
}

bool VoiceEngine::setRecordCodec(CodecPtr& codec)
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

unsigned short VoiceEngine::encodeAudioFrame(unsigned int from, unsigned int to, char* buffer)
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

unsigned short VoiceEngine::encodeAudioFrame(char* data, unsigned int from, unsigned int to, char* buffer)
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

void VoiceEngine::startRecording()
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

void VoiceEngine::stopRecording()
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

bool VoiceEngine::isRecording() const
{
    return mRecording;
}

// -----------------------------------------------------------------------------
// Update methods

void VoiceEngine::update()
{
    // Update avatar voices
//     if (apr_thread_mutex_trylock(mAvatarMutex) != APR_EBUSY)
    {
// GREG BEGIN
//        apr_thread_mutex_lock(mAvatarMutex);
        pthread_mutex_lock(&mAvatarMutex);
// GREG END
        for (BufferMap::const_iterator i = mBuffers.begin(); i != mBuffers.end(); ++i)
        {
            unsigned int lastpos = i->second->getLastPosition();
            bool playing = i->second->isPlaying();

            unsigned int playbackOffset = (unsigned int)mNetworkChunkSize;
            if (playing)
            {
                unsigned int pos = 0;
                i->second->getChannel()->getPosition(&pos, FMOD_TIMEUNIT_PCM);
                if ((pos > lastpos) && ((pos - lastpos) < playbackOffset))
                {
                    velogs << "Warning: No voice playback data available (pos:"
                           << pos << " lastPos:" << lastpos << " offset:" << playbackOffset << ")" << veendl;
                    i->second->stop();
                }
            }

            if ((lastpos >= playbackOffset) && !playing)
            {
                velogs << "Starting playback" << veendl;
                i->second->play(mSystem);
            }

            if (playing)
            {
                SourceMap::const_iterator j = mSources.find(i->first);
                if (j != mSources.end())
                {
                    const VoiceSource& source = j->second;

                    FMOD_VECTOR pos;
                    source.getPosition(&pos.x, &pos.z, &pos.y);
                    pos.x = -pos.x;
                    pos.z = -pos.z;

                    FMOD_VECTOR vel;
                    source.getVelocity(&vel.x, &vel.z, &vel.y);
                    vel.x = -vel.x;
                    vel.z = -vel.z;

                    i->second->getChannel()->set3DAttributes(&pos, &vel);
                }
            }
        }

// GREG BEGIN
//        apr_thread_mutex_unlock(mAvatarMutex);
        pthread_mutex_unlock(&mAvatarMutex);
// GREG END
    }

    // Update fmod system if we have created it
    if (!mUseExternalSystem)
    {
        FMOD_VECTOR up = {0.0f, 1.0f, 0.0f};

        FMOD_VECTOR pos;
        mSelfListener.getPosition(&pos.x, &pos.z, &pos.y);
        pos.x = -pos.x;
        pos.z = -pos.z;

        FMOD_VECTOR vel;
        mSelfListener.getVelocity(&vel.x, &vel.z, &vel.y);
        vel.x = -vel.x;
        vel.z = -vel.z;

        FMOD_VECTOR dir;
        mSelfListener.getDirection(&dir.x, &dir.z, &dir.y);
        dir.x = -dir.x;
        dir.z = -dir.z;

        mSystem->set3DListenerAttributes(0, &pos, &vel, &dir, &up);
        mSystem->update();
    }

    if (!isRecording() && !mEnabled)
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

void VoiceEngine::updateListener(float* pos, float* dir, float* vel)
{
    mSelfListener.setPosition(pos[0], pos[1], pos[2]);
    mSelfListener.setDirection(dir[0], dir[1], dir[2]);
    mSelfListener.setVelocity(vel[0], vel[1], vel[2]);
}

// -----------------------------------------------------------------------------
// Avatar methods

void VoiceEngine::updateAvatar(const VoiceUUID& id, float* pos, float* dir, float* vel)
{
// GREG BEGIN
//    apr_thread_mutex_lock(mAvatarMutex);
    pthread_mutex_lock(&mAvatarMutex);
// GREG END

    SourceMap::iterator i = mSources.find(id);
    if (i == mSources.end())
    {
// GREG BEGIN
//        apr_thread_mutex_unlock(mAvatarMutex);
        pthread_mutex_unlock(&mAvatarMutex);
// GREG END
        return;
    }

    i->second.setPosition(pos[0], pos[1], pos[2]);
    i->second.setDirection(dir[0], dir[1], dir[2]);
    i->second.setVelocity(vel[0], vel[1], vel[2]);

// GREG BEGIN
//    apr_thread_mutex_unlock(mAvatarMutex);
    pthread_mutex_unlock(&mAvatarMutex);
// GREG END
}

VoiceBuffer* VoiceEngine::newAvatar(const VoiceUUID& id, VoiceCodec* codec)
{
    // Create avatar sound

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

    sound->set3DMinMaxDistance(5, 10000.0f);

    // Create voice buffer and source

    VoiceBuffer* buffer = new(VoiceBufferPool::malloc()) VoiceBuffer(sound);
// GREG BEGIN
//    apr_thread_mutex_lock(mAvatarMutex);
    pthread_mutex_lock(&mAvatarMutex);
// GREG END
    mBuffers.insert(std::make_pair(id, buffer));
    mSources.insert(std::make_pair(id, VoiceSource()));
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

void VoiceEngine::removeAvatar(const VoiceUUID &id)
{
    // Release voice buffer and source

// GREG BEGIN
//    apr_thread_mutex_lock(mAvatarMutex);
    pthread_mutex_lock(&mAvatarMutex);
// GREG END

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

VoiceBuffer* VoiceEngine::getAvatarVoiceBuffer(const VoiceUUID& id) const
{
    BufferMap::const_iterator i = mBuffers.find(id);
    if (i == mBuffers.end())
        return 0;

    return i->second;
}

// -----------------------------------------------------------------------------
// Network methods

bool VoiceEngine::connect(const char* host, int port, const VoiceUUID& id)
{
    if (mSock != VE_INVALID_SOCKET)
        disconnect();

// GREG BEGIN
    initWinSock();
// GREG END
    if (ve_create_socket_tcp(mSock))
    {
        if (ve_connect_socket_tcp(mSock, host, port))
        {
            sendLogin(id);
            sendEnableVOIP(mEnabled);
            mRun = true;

// GREG BEGIN
//            apr_thread_create(&mReceiveThread, NULL, receiveThread, (void*)this, mAprPool);
            pthread_create(&mReceiveThread, NULL, receiveThread, (void*)this);
// GREG END
            return true;
        }

        mRun = false;
        ve_destroy_socket_tcp(mSock);
    }

    return false;
}

void VoiceEngine::disconnect()
{
    ve_destroy_socket_tcp(mSock);

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

int VoiceEngine::sendPacketHeader(char type, unsigned int size)
{
    char packet[sizeof(char)+sizeof(unsigned int)];
    memcpy(packet, &type, sizeof(char));
    memcpy(&packet[sizeof(char)], &size, sizeof(unsigned int));
    return ve_send_packet_tcp(mSock, packet, sizeof(packet));
}

int VoiceEngine::recvPacketHeader(char* type, unsigned int* size)
{
    int received = 0;
    received = ve_receive_packet_tcp(mSock, type, sizeof(char));
    if (received <= 0) return received;
    received = 0;
    while (received < sizeof(unsigned int))
        received += ve_receive_packet_tcp(mSock, &((char*)size)[received], sizeof(unsigned int)-received);
    return received + sizeof(char);
}

int VoiceEngine::recvVoiceHeader(VoicePacketHeader* header)
{
    int received = 0;
    int headerSize = sizeof(VoicePacketHeader);
    while (received < headerSize)
    {
        int bytesReceived = ve_receive_packet_tcp(mSock, &((char*)header)[received], headerSize-received);
        received += bytesReceived;
        if (bytesReceived <= 0) return bytesReceived;
    }

    return received;
}

int VoiceEngine::recvUUID(VoiceUUID* id)
{
    int received = 0;
    while (received < sizeof(VoiceUUID))
        received += ve_receive_packet_tcp(mSock, &((char*)id)[received], sizeof(VoiceUUID)-received);

    return received;
}

int VoiceEngine::sendLogin(const VoiceUUID& id)
{
    int sent = 0;
    sent += sendPacketHeader(VP_LOGIN, sizeof(VoiceUUID) + sizeof(int));
    sent += ve_send_packet_tcp(mSock, (const char*)&id, sizeof(VoiceUUID));
    sent += ve_send_packet_tcp(mSock, (const char*)&mSupportedFormats, sizeof(mSupportedFormats));
    return sent;
}

int VoiceEngine::sendEnableVOIP(bool enabled)
{
    int sent = 0;
    sent += sendPacketHeader(VP_ENABLE_VOIP, sizeof(char));
    char val = enabled ? 1 : 0;
    sent += ve_send_packet_tcp(mSock, &val, sizeof(char));
    return sent;
}

int VoiceEngine::sendAudioFrames(unsigned int from, unsigned int to)
{
    assert(to >= from);

    VoicePacketHeader header;
    header.format = mEncodeFormat;

    CodecMap::const_iterator i = mCodecs.find(mEncodeFormat);
    if (i != mCodecs.end())
    {
        // Encoded audio, encode in frames equal to codec frame size and send all available frames
        // as a chunk as big as possible
        unsigned short frameSizePCM = i->second->getFrameSizePCM();
        unsigned int chunkSize = to - from;
        if (chunkSize >= frameSizePCM)
        {
            // Calculate frame and sample count
            unsigned short frames = chunkSize/frameSizePCM;
            unsigned int sampleCount = frames*frameSizePCM;

            bool constantFrameSize = i->second->isEncodedSizeConstant();

            // Allocate only one frame header if frame size is constant
            size_t headerCount = constantFrameSize ? 1 : frames;
            VoiceFrameHeader* frameHeaders = (VoiceFrameHeader*)FrameHeaderPool::ordered_malloc(headerCount);

            // Encode audio frames to a temporary buffer

            size_t bufferSize = frames * frameSizePCM * mRecordSampleSize;
            char* encBuffer = (char*)mSendPool.ordered_malloc(bufferSize);

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

            unsigned short size = 0;
            unsigned int lastFrom = from;
            unsigned int framesSkipped = 0;
            for (int i = 0; i<frames; ++i)
            {
                unsigned short encodedSize = encodeAudioFrame(lastFrom, lastFrom+frameSizePCM, &encBuffer[size]);
//                 unsigned short encodedSize = encodeAudioFrame(&audioBuffer[i*frameSizePCM*mRecordSampleSize],
//                                                    lastFrom, lastFrom+frameSizePCM, &encBuffer[size]);
                lastFrom += frameSizePCM;
                if (encodedSize == 0)
                {
                    ++framesSkipped;
                    continue;
                }

                if (constantFrameSize)
                    frameHeaders->frameSize = encodedSize;
                else if (frameHeaders->frameSize == 0)
                    frameHeaders[i-framesSkipped].frameSize = encodedSize;

                size += encodedSize;
            }

            frames -= framesSkipped;

            header.frames = frames;
            header.decodedAudioSize = frames*frameSizePCM*mRecordSampleSize;
            header.audioSize = size;

            unsigned int frameHeadersSize = (unsigned int)sizeof(VoiceFrameHeader)*headerCount;

            // Send headers and audio data to server

            if (header.audioSize != 0)
            {
                sendPacketHeader(VP_AUDIO_TO_SERVER, sizeof(header) + frameHeadersSize + header.audioSize);
                ve_send_packet_tcp(mSock, (const char*)&header, sizeof(header));
                ve_send_packet_tcp(mSock, (const char*)frameHeaders, frameHeadersSize);
                ve_send_packet_tcp(mSock, encBuffer, header.audioSize);
            }

            // Free temporary buffers

            FrameHeaderPool::ordered_free(frameHeaders, headerCount);
            frameHeaders = 0;

            mSendPool.ordered_free(encBuffer, bufferSize);
            encBuffer = 0;
//             mSendPool.ordered_free(audioBuffer, bufferSize);
//             audioBuffer = 0;

            return sampleCount;
        }
    }

    return 0;
}

int VoiceEngine::recvAudioFrames(unsigned int expectedSize)
{
    int size = 0;
    int received = 0;

    VoiceUUID avatarId;
    received = recvUUID(&avatarId);
    size += received;
    if (received <= 0) return size;

    // Read audio header

    VoicePacketHeader header;
    received = recvVoiceHeader(&header);
    size += received;
    if (received <= 0) return size;

    // Check for codec format and do a dummy receive if the codec is unknown

    received = 0;
    CodecMap::const_iterator codec = mCodecs.find(header.format);
    if (codec == mCodecs.end())
    {
        velogs << "Warning: Avatar has an unknown voice codec (id:" << header.format << ")" << veendl;
        char* data = (char*)mReceivePool.ordered_malloc(expectedSize - size);
        size += ve_receive_packet_tcp(mSock, data, expectedSize - size);
        mReceivePool.ordered_free(data, expectedSize - size);
        return size;
    }

    // Create new avatar if not found

    VoiceBuffer* voice = getAvatarVoiceBuffer(avatarId);
    if (voice == 0)
    {
        voice = newAvatar(avatarId, codec->second.get());
        if (!voice)
            return size;
    }

    // Read frame headers

    bool constantFrameSize = codec->second->isEncodedSizeConstant();
    int headerCount = constantFrameSize ? 1 : header.frames;
    VoiceFrameHeader* frameHeaders = (VoiceFrameHeader*)FrameHeaderPool::ordered_malloc(headerCount);

    for (int h = 0; h < headerCount; ++h)
    {
        received = 0;
        while (received < sizeof(VoiceFrameHeader))
        {
            int packetBytes = ve_receive_packet_tcp(mSock, &((char*)&frameHeaders[h])[received],
                                                 sizeof(VoiceFrameHeader)-received);
            size += packetBytes;
            received += packetBytes;
            if (packetBytes <= 0) return size;
        }
    }

    // Read audio data

    received = 0;
    char* data = (char*)mReceivePool.ordered_malloc(header.audioSize);
    while (received < header.audioSize)
    {
        int bytesReceived = ve_receive_packet_tcp(mSock, &data[received], header.audioSize-received);
        received += bytesReceived;
        size += bytesReceived;
        if (bytesReceived <= 0) return size;
    }

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

    unsigned short sampleSize = sampleSizeFromVoiceFormat(codec->second->getDecodeFormat());
    audioData = (char*)mReceivePool.ordered_malloc(header.decodedAudioSize);
    unsigned short frameSizeDecoded = codec->second->getFrameSizePCM() * sampleSize;
    size_t offset = 0;
    for (unsigned short f = 0; f < header.frames; ++f)
    {
        // Get encoded frame size
        unsigned short frameSizeEncoded = constantFrameSize ? frameHeaders->frameSize : frameHeaders[f].frameSize;

        // Decode encoded frame to audio buffer
        decodedSize += codec->second->decode(avatarId, &data[offset], frameSizeEncoded,
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

    // Free audio headers and data

    mReceivePool.ordered_free(data, header.audioSize);
    mReceivePool.ordered_free(audioData, header.decodedAudioSize);
    FrameHeaderPool::ordered_free(frameHeaders, constantFrameSize ? 1 : header.frames);

    return size;
}

// -----------------------------------------------------------------------------
// Network data receiver thread

// GREG BEGIN
//void* APR_THREAD_FUNC VoiceEngine::receiveThread(apr_thread_t* thread, void* param)
void *VoiceEngine::receiveThread(void* param)
// GREG END
{
    VoiceEngine* ve = (VoiceEngine*)param;

    while (ve->mRun)
    {
        char packetType;
        unsigned int packetSize;
        if (ve->recvPacketHeader(&packetType, &packetSize) <= 0)
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

void VoiceEngine::setEnabled(bool enabled)
{
    mEnabled = enabled;
    if (mSock != VE_INVALID_SOCKET)
        sendEnableVOIP(mEnabled);
}
