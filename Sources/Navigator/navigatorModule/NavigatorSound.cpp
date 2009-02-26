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
#include <CTLog.h>
#include <CTIO.h>
#include <CTStringHelpers.h>

using namespace CommonTools;

// FourCC converter
#define FOURCC(a, b, c, d) (((unsigned int)a) | (((unsigned int)b) << 8) | (((unsigned int)c) << 16) | (((unsigned int)d) << 24))

namespace Solipsis {

class SoundBuffer
{
public:
    SoundBuffer(const std::string& name);
	~SoundBuffer();

	///	Set the frame size
    void setProperties(unsigned short sampleSize, unsigned short frameCount, unsigned int frameSize);
	///	Get the sample size
    unsigned short getSampleSize();
	///	Get the frame count
    unsigned short getFrameCount();
	///	Get the frame size
    unsigned int getFrameSize();
	///	Set the sound
    void setSound(FMOD::Sound* sound);
	///	Get the sound
    FMOD::Sound* getSound();
	///	Get the channel of the playing sound
    FMOD::Channel* getChannel();

    ///	Set the index of the last valid audio data in the sound (in PCM samples)
    void setLastPosition(unsigned int lastPosition);
    ///	Get the index of the last valid audio data in the sound (in PCM samples)
    unsigned int getLastPosition();

    /// Lock
    void lock();
    /// Unlock
    void unlock();

	///	Returns true if sound is played on its channel
    bool isChannelPlaying();
	///	Returns true if sound is playing
    bool isPlaying();
	///	Play the sound on system
    void play(FMOD::System* system);
	///	Stop the sound
    void stop();
	///	Get this sound name
    const char*  getName() {return mName.c_str();};

private:
    /// Name
    std::string mName;
    /// Sample size (in bytes)
    unsigned short mSampleSize;
    /// Frame count
    unsigned short mFrameCount;
    /// Frame size (in bytes)
    unsigned int mFrameSize;
    /// Audio data
    FMOD::Sound* mSound;
    /// Index of the last valid audio data in the sound (in PCM samples)
    unsigned int mLastPosition;
    /// Channel of the playing sound
    FMOD::Channel* mChannel;
    /// Playing flag (even if channel report not playing the sound, for example when
    /// the previously allocated channel was stolen by FMod for another sound)
    bool mIsPlaying;
    /// Mutex
    pthread_mutex_t mMutex;
};

//-------------------------------------------------------------------------------------
SoundBuffer::SoundBuffer(const std::string& name)
    : mName(name)
    , mSound(0)
    , mLastPosition(0)
    , mChannel(0)
    , mIsPlaying(false)
    , mMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
SoundBuffer::~SoundBuffer()
{
    setSound(0);
}

//-------------------------------------------------------------------------------------
void SoundBuffer::setProperties(unsigned short sampleSize, unsigned short frameCount, unsigned int frameSize)
{
    mSampleSize = sampleSize;
    mFrameCount = frameCount;
    mFrameSize = frameSize;
}

//-------------------------------------------------------------------------------------
unsigned int SoundBuffer::getFrameSize()
{
    return mFrameSize;
}

//-------------------------------------------------------------------------------------
unsigned short SoundBuffer::getSampleSize()
{
    return mSampleSize;
}

//-------------------------------------------------------------------------------------
unsigned short SoundBuffer::getFrameCount()
{
    return mFrameCount;
}

//-------------------------------------------------------------------------------------
void SoundBuffer::setSound(FMOD::Sound* sound)
{
    if (mChannel != 0)
        stop();
    if (mSound)
    {
        mSound->release();
        mSound = 0;
    }
    mSound = sound;
}

//-------------------------------------------------------------------------------------
FMOD::Sound* SoundBuffer::getSound()
{
    return mSound;
}

//-------------------------------------------------------------------------------------
FMOD::Channel* SoundBuffer::getChannel()
{
    return mChannel;
}

//-------------------------------------------------------------------------------------
void SoundBuffer::setLastPosition(unsigned int lastPosition)
{
    mLastPosition = lastPosition;
}

//-------------------------------------------------------------------------------------
unsigned int SoundBuffer::getLastPosition()
{
    return mLastPosition;
}

//-------------------------------------------------------------------------------------
void SoundBuffer::lock()
{
    pthread_mutex_lock(&mMutex);
}

//-------------------------------------------------------------------------------------
void SoundBuffer::unlock()
{
    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
void SoundBuffer::play(FMOD::System* system)
{
    if (mChannel != 0)
        stop();
    system->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
    mIsPlaying = true;
}

//-------------------------------------------------------------------------------------
void SoundBuffer::stop()
{
    mLastPosition = 0;
    if (mChannel)
    {
        mChannel->stop();
        mChannel = 0;
    }
    mIsPlaying = false;
}

//-------------------------------------------------------------------------------------
bool SoundBuffer::isChannelPlaying()
{
    /* The problem when testing the channel is due to FMod channels allocation strategy,
    it can steal 1 channel for another sound and then the previous sound loose its channel
    and stop playing ! */
    bool playing = false;
    if (mChannel != 0)
        mChannel->isPlaying(&playing);
    return playing;
}

//-------------------------------------------------------------------------------------
bool SoundBuffer::isPlaying()
{
    return mIsPlaying;
}

//-------------------------------------------------------------------------------------
NavigatorSound::NavigatorSound() :
    mSoundSystem(0),
    mUpdateRateMs(100),
    mSoundListenerCamera(0),
    mMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
NavigatorSound::~NavigatorSound()
{
    shutdown();
}

//-------------------------------------------------------------------------------------
bool NavigatorSound::initialize(unsigned int updateRateMs)
{
    FMOD_RESULT result;
    unsigned int version;

    mUpdateRateMs = updateRateMs;
    mLastUpdateTimeMs = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

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
    std::string cwd = IO::getCWD();
    settings.debugLogFilename = (char*)cwd.c_str();
    result = mSoundSystem->setAdvancedSettings(&settings);
    if (!fmodErrorCheck(result))
        return false;
    result = mSoundSystem->init(8, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
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
    voiceEngine->setLogger(&mVoiceEngineLogger);

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
void NavigatorSound::setSoundListenerCamera(Ogre::Camera* soundListenerCamera)
{
    mSoundListenerCamera = soundListenerCamera;
    mLastCameraPosition = mSoundListenerCamera->getDerivedPosition();
}

//-------------------------------------------------------------------------------------
void NavigatorSound::update()
{
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    if (now - mLastUpdateTimeMs < mUpdateRateMs) return;

    // Update the sound listener camera attributes
    if (mSoundListenerCamera != 0)
    {
        FMOD_VECTOR pos, vel, forward, up;
        convertVector3ToFModVector(mSoundListenerCamera->getDerivedPosition(), pos);
        convertVector3ToFModVector((mSoundListenerCamera->getDerivedPosition() - mLastCameraPosition)/(now  - mLastUpdateTimeMs), vel);
        convertVector3ToFModVector(mSoundListenerCamera->getDerivedDirection(), forward);
        convertVector3ToFModVector(mSoundListenerCamera->getDerivedUp(), up);
        mSoundSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
        mLastCameraPosition = mSoundListenerCamera->getDerivedPosition();
    }

    mLastUpdateTimeMs = now;

    // Update sound buffers
    pthread_mutex_lock(&mMutex);
    for (int soundId = 0; soundId < (int)mSoundBufferVector.size(); ++soundId)
    {
        if (mSoundBufferVector[soundId] == 0) continue;
        SoundBuffer *soundBuffer = mSoundBufferVector[soundId];
        if (soundBuffer == 0) continue;
        soundBuffer->lock();
        bool isPlaying = soundBuffer->isPlaying();
        if (isPlaying && !soundBuffer->isChannelPlaying())
        {
            // Sound is playing but it lost its channel
            soundBuffer->unlock();
            continue;
        }
        unsigned int frameSizePCM = soundBuffer->getFrameSize()/soundBuffer->getSampleSize();
        unsigned int lastPos = soundBuffer->getLastPosition();
        if (isPlaying)
        {
            unsigned int pos = 0;
            soundBuffer->getChannel()->getPosition(&pos, FMOD_TIMEUNIT_PCM);
            if ((pos > lastPos) && ((pos - lastPos) < frameSizePCM))
            {
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::update() soundId:%d Starving/not enough audio data ... stop (pos:%d, lastPos:%d)", soundId, pos, lastPos);
                // the player is starving (not enough audio data added in the sound buffer)
                soundBuffer->stop();
            }
        }
        if (!isPlaying && (lastPos >= frameSizePCM))
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::update() soundId:%d Enough audio data ... starting (lastPos:%d, frameSizePCM:%d)", soundId, lastPos, frameSizePCM);
            // Enough audio data in the sound buffer we can start playing
            soundBuffer->play(mSoundSystem);
        }
        soundBuffer->unlock();
    }
    // Update nodes (position+velocity) binded to sound buffers
    for (NodeSoundBufferMap::iterator it = mNodeSoundBufferMap.begin(); it != mNodeSoundBufferMap.end(); ++it)
    {
        Ogre::Node *node = it->first;
        SoundBuffer *soundBuffer = mSoundBufferVector[it->second];
        if (node == 0) continue;
        if ((soundBuffer == 0) || !soundBuffer->isPlaying()) continue;
        FMOD_VECTOR pos, vel;
        convertVector3ToFModVector(node->_getDerivedPosition(), pos);
        convertVector3ToFModVector(Ogre::Vector3::ZERO, vel);
        soundBuffer->getChannel()->set3DAttributes(&pos, &vel);
    }
    pthread_mutex_unlock(&mMutex);

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
int NavigatorSound::createSoundBuffer(const Ogre::String& name)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::createSoundBuffer(%s)", name.c_str());

    SoundBuffer *soundBuffer = new SoundBuffer(name);
    int soundId;
    pthread_mutex_lock(&mMutex);
    for (soundId = 0; soundId < (int)mSoundBufferVector.size(); ++soundId)
        if (mSoundBufferVector[soundId] == 0) break;
    if (soundId == mSoundBufferVector.size())
        mSoundBufferVector.push_back(soundBuffer);
    else
        mSoundBufferVector[soundId] = soundBuffer;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::createSoundBuffer(%s) soundId=%d", name.c_str(), soundId);
    pthread_mutex_unlock(&mMutex);
    return soundId;
}

//-------------------------------------------------------------------------------------
void NavigatorSound::destroySoundBuffer(int soundId)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::destroySoundBuffer(%d)", soundId);

    pthread_mutex_lock(&mMutex);
    delete mSoundBufferVector[soundId];
    mSoundBufferVector[soundId] = 0;
    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
void NavigatorSound::bindMaterialToSoundBuffer(const Ogre::String& material, int soundId)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::bindMaterialToSoundBuffer(%s, %d)", material.c_str(), soundId);

    if (mSoundBufferVector[soundId] == 0) return;
    mMtlSoundBufferMap[material] = soundId;
}

//-------------------------------------------------------------------------------------
void NavigatorSound::unbindMaterialToSoundBuffer(const Ogre::String& material)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::unbindMaterialToSoundBuffer(%s)", material.c_str());

    MtlSoundBufferMap::iterator it = mMtlSoundBufferMap.find(material);
    if (it == mMtlSoundBufferMap.end()) return;
    mMtlSoundBufferMap.erase(it);
}

//-------------------------------------------------------------------------------------
void NavigatorSound::openSoundBuffer(int soundId, const Ogre::String& soundParams, unsigned int *frequency, unsigned int *nbChannels, unsigned int *fourCCFormat, unsigned int *frameSize)
{
    if (mSoundBufferVector[soundId] == 0) return;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::openSoundBuffer(%d, %s, %d, %d, %d, %d) ==> %s",soundId, soundParams.c_str(), *frequency, *nbChannels, *fourCCFormat, *frameSize,mSoundBufferVector[soundId]->getName());

    *nbChannels = 1;

    // Create sound
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels = *nbChannels;
    exinfo.format = getFModSoundFormatFromFourCC(*fourCCFormat);
    *fourCCFormat = getFourCCFromFModSoundFormat(exinfo.format);
    exinfo.defaultfrequency = *frequency;
    #define FRAME_COUNT 4
    mSoundBufferVector[soundId]->setProperties(getSampleSizeFromFModSoundFormat(exinfo.format), FRAME_COUNT, *frameSize);
    exinfo.length = FRAME_COUNT*(*frameSize)*getSampleSizeFromFModSoundFormat(exinfo.format);

    FMOD::Sound* sound = 0;
    if (mSoundSystem->createSound(0, FMOD_3D | FMOD_OPENUSER | FMOD_LOOP_NORMAL, &exinfo, &sound) != FMOD_OK)
    {
        return;
    }

    // Apply additional sound parameters
    if (soundParams.find("3d") == 0)
    {
        std::vector<std::string> tokens;
        StringHelpers::tokenize(soundParams, " ", tokens);
        if (tokens.size() == 3)
        {
            float min = atof(tokens[1].c_str());
            float max = atof(tokens[2].c_str());
            sound->set3DMinMaxDistance(min, max);
        }
    }

    // Set the sound buffer
    mSoundBufferVector[soundId]->setSound(sound);
}

//-------------------------------------------------------------------------------------
void NavigatorSound::playSoundBuffer(int soundId, unsigned char *buffer, size_t bufferSize, unsigned int nbSamples)
{
    if (mSoundBufferVector[soundId] == 0) return;

    SoundBuffer *soundBuffer = mSoundBufferVector[soundId];
    unsigned short sampleSize = soundBuffer->getSampleSize();
    if ((unsigned short)(bufferSize/nbSamples) != sampleSize) return;
    soundBuffer->lock();
    FMOD::Sound* sound = soundBuffer->getSound();
    unsigned int len = 0;
    sound->getLength(&len, FMOD_TIMEUNIT_PCM);
    unsigned int lastPos = soundBuffer->getLastPosition();

    void *ptr1, *ptr2;
    unsigned int len1, len2;
    sound->lock(lastPos*sampleSize, (unsigned int)bufferSize, &ptr1, &ptr2, &len1, &len2);
    if (len1 + len2 == bufferSize)
    {
        if (len1 > 0)
        {
            memcpy(ptr1, buffer, len1);
            if (len2 > 0) memcpy(ptr2, &buffer[len1], len2);
        }
        else if (len2 > 0)
        {
            memcpy(ptr2, buffer, len2);
        }
        lastPos += (unsigned int)bufferSize/sampleSize;
        if (lastPos >= len)
            lastPos = lastPos - len;
    }
    else
    {
        ;// Invalid audio data
    }

    sound->unlock(ptr1, ptr2, len1, len2);
    soundBuffer->setLastPosition(lastPos);
    soundBuffer->unlock();
}

//-------------------------------------------------------------------------------------
void NavigatorSound::closeSoundBuffer(int soundId)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::closeSoundBuffer(%d)", soundId);

    if (mSoundBufferVector[soundId] == 0) return;

    SoundBuffer *soundBuffer = mSoundBufferVector[soundId];
    FMOD::Sound* sound = soundBuffer->getSound();
    if (soundBuffer->isPlaying())
        soundBuffer->stop();

    // Reset the sound buffer
    soundBuffer->setSound(0);
}

//-------------------------------------------------------------------------------------
void NavigatorSound::bindNodeToMaterial(Ogre::Node *node, const Ogre::String& material)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::bindNodeToMaterial(%s, %s)", node->getName().c_str(), material.c_str());

    MtlSoundBufferMap::iterator it = mMtlSoundBufferMap.find(material);
    if (it == mMtlSoundBufferMap.end()) return;

    // Temp Stephane -- On lance le SoundBuffer 
    mSoundBufferVector[it->second]->play(mSoundSystem);   
    mNodeSoundBufferMap[node] = it->second;
}

//-------------------------------------------------------------------------------------
void NavigatorSound::unbindNodeToMaterial(Ogre::Node *node)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorSound::unbindNodeToMaterial(%s)", node->getName().c_str());

    NodeSoundBufferMap::iterator it = mNodeSoundBufferMap.find(node);
    if (it == mNodeSoundBufferMap.end()) return;

    // Temp Stephane -- On stoppe également le SoundBuffer 
    mSoundBufferVector[it->second]->stop();   
    mNodeSoundBufferMap.erase(it);}

//-------------------------------------------------------------------------------------
void NavigatorSound::VoiceEngineLogger::logMessage(const std::string& message)
{
    LOGHANDLER_LOG(LogHandler::VL_DEBUG, message.c_str());
}

//-------------------------------------------------------------------------------------
void NavigatorSound::convertVector3ToFModVector(const Ogre::Vector3& v, FMOD_VECTOR& fmodV)
{
    fmodV.x = v.x;
    fmodV.y = v.y;
    fmodV.z = v.z;
}

//-------------------------------------------------------------------------------------
FMOD_SOUND_FORMAT NavigatorSound::getFModSoundFormatFromFourCC(unsigned int fourCC)
{
    FMOD_SOUND_FORMAT format;

    switch (fourCC)
    {
    case FOURCC('u','8',' ',' '):
    case FOURCC('s','8',' ',' '):
        format = FMOD_SOUND_FORMAT_PCM8;
        break;
    case FOURCC('u','1','6','l'):
    case FOURCC('u','1','6','b'):
    case FOURCC('s','1','6','l'):
    case FOURCC('s','1','6','b'):
        format = FMOD_SOUND_FORMAT_PCM16;
        break;
    default:
        format = FMOD_SOUND_FORMAT_PCM8;
        break;
    }

    return format;
}

//-------------------------------------------------------------------------------------
unsigned int NavigatorSound::getFourCCFromFModSoundFormat(FMOD_SOUND_FORMAT format)
{
    unsigned int fourCCformat;

    switch (format)
    {
    case FMOD_SOUND_FORMAT_PCM8:
        fourCCformat = FOURCC('s','8',' ',' ');
        break;
    case FMOD_SOUND_FORMAT_PCM16:
        fourCCformat = FOURCC('s','1','6','l');
        break;
    }

    return fourCCformat;
}

//-------------------------------------------------------------------------------------
unsigned short NavigatorSound::getSampleSizeFromFModSoundFormat(FMOD_SOUND_FORMAT format)
{
    switch (format)
    {
        case FMOD_SOUND_FORMAT_PCM8: return sizeof(char);
        case FMOD_SOUND_FORMAT_PCM16: return sizeof(short);
        case FMOD_SOUND_FORMAT_PCM32: return sizeof(int);
    }

    return 0;
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