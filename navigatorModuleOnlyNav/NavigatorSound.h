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

#ifndef __NavigatorSound_h__
#define __NavigatorSound_h__

#include <vector>
#include "fmod.hpp"
#include "fmod_errors.h"
#include <Ogre.h>
#include "ExternalTextureSourceEx.h"
#include <pthread.h>
#include <VoiceEngineManager.h>

namespace Solipsis {

class SoundBuffer;

/** This class manages sound into navigator (sounds, musics, voice chat).
 */
class NavigatorSound : public ExternalTextureSourceExSoundHandler
{
protected:
    /// Sound system
    FMOD::System *mSoundSystem;

    /// Update rate (in ms)
    unsigned long mUpdateRateMs;

    /// The sound listener camera
    Ogre::Camera *mSoundListenerCamera;
    /// Last camera position
    Ogre::Vector3 mLastCameraPosition;
    /// Last update time (in ms)
    unsigned long mLastUpdateTimeMs;

    typedef std::vector<SoundBuffer*> SoundBufferVector;
    /// Vector of sound buffers
    SoundBufferVector mSoundBufferVector;

    typedef std::map<Ogre::String, int> MtlSoundBufferMap;
    /// Map of <material, soundId>
    MtlSoundBufferMap mMtlSoundBufferMap;

    typedef std::map<Ogre::Node*, int> NodeSoundBufferMap;
    /// Map of <Node*, soundId>
    NodeSoundBufferMap mNodeSoundBufferMap;

    /// Mutex
    pthread_mutex_t mMutex;

public:
    NavigatorSound();
    ~NavigatorSound();

    /// initialize sound system
    bool initialize(unsigned int updateRateMs = 100);

    /// shutdown sound system
    void shutdown();

    /// update sound system
    void update();

    /// set the sound listener camera
    void setSoundListenerCamera(Ogre::Camera* soundListenerCamera);

    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::createSoundBuffer
    virtual int createSoundBuffer(const Ogre::String& name);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::destroySoundBuffer
    virtual void destroySoundBuffer(int soundId);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::bindMaterialToSoundBuffer
    virtual void bindMaterialToSoundBuffer(const Ogre::String& material, int soundId);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::unbindMaterialToSoundBuffer
    virtual void unbindMaterialToSoundBuffer(const Ogre::String& material);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::openSoundBuffer
    virtual void openSoundBuffer(int soundId, const Ogre::String& soundParams, unsigned int *frequency, unsigned int *nbChannels, unsigned int *fourCCFormat, unsigned int *frameSize);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::playSoundBuffer
    virtual void playSoundBuffer(int soundId, unsigned char *buffer, size_t bufferSize, unsigned int nbSamples);
    /// @copydoc Solipsis::ExternalTextureSourceExSoundHandler::closeSoundBuffer
    virtual void closeSoundBuffer(int soundId);

    /// bind 1 Ogre node to 1 material
    void bindNodeToMaterial(Ogre::Node *node, const Ogre::String& material);
    /// bind 1 Ogre node from 1 material
    void unbindNodeToMaterial(Ogre::Node *node);

protected:
    class VoiceEngineLogger : public IVoiceEngineLogger {
        /** See IVoiceEngineLogger. */
        virtual void logMessage(const std::string& message);
    };
    VoiceEngineLogger mVoiceEngineLogger;

private:
    /// convert Ogre Vector3 to FMod vector
    void convertVector3ToFModVector(const Ogre::Vector3& v, FMOD_VECTOR& fmodV);
    /// convert FourCC format into FMod sound format
    FMOD_SOUND_FORMAT getFModSoundFormatFromFourCC(unsigned int fourCC);
    /// convert FMod sound format into FourCC format
    unsigned int getFourCCFromFModSoundFormat(FMOD_SOUND_FORMAT format);
    /// get the size of 1 sample according to 1 FMod sound format
    unsigned short getSampleSizeFromFModSoundFormat(FMOD_SOUND_FORMAT format);
    /// check/log FMOD error
    bool fmodErrorCheck(FMOD_RESULT result);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorVoice_h__