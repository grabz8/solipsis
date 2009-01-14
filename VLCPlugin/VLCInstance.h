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

#ifndef __VLCInstance_h__
#define __VLCInstance_h__

#include <pthread.h>

#include <vlc/libvlc.h>

#include <OgreFrameListener.h>
#include <OgreTexture.h>

namespace Solipsis {

class VLCTextureSource;

/**
 *  A class that updates and creates VLC textures for one VLC instance.
 *  All textures will be updated in Ogre main thread.
 */
class VLCInstance : public Ogre::FrameListener
{
public:
    /** Constructor
    @param[in] id The unique identifier
    @param[in] mrl The media resource link
    @param[in] width The video width
    @param[in] height The video height
    @param[in] fps The update rate in frames per second
    @param[in] soundParams The additional sound parameters
    @param[in] vlcParams The additional VLC parameters
    */
    VLCInstance(int id, VLCTextureSource* textureSource,
                const Ogre::String& mrl, int width, int height, int fps, const Ogre::String& soundParams, const Ogre::String& vlcParams);
    /// Destructor
    ~VLCInstance();

    /// Get MRL
    const Ogre::String& getMrl() const { return mMrl; }
    /// Get additional sound parameters
    const Ogre::String& getSoundParams() const { return mSoundParams; }
    /// Get sound identifier
    int getSoundId() { return mSoundId; }

    /// Get Texture
    Ogre::TexturePtr getTexture() const { return mTexture; }

    /// Get TextureSource
    VLCTextureSource* getTextureSource() { return mTextureSource; }

    /// Handle 1 event
    Ogre::String handleEvt(const Ogre::String& evt);

    /// Destroy
    void destroy(bool force = false);

private:
    /// @copydoc Ogre::FrameListener::frameStarted
    bool frameStarted(const Ogre::FrameEvent& e);
    /// @copydoc Ogre::FrameListener::frameEnded
    bool frameEnded(const Ogre::FrameEvent& e);

    /// libvlc exception helper method
    static void _libvlc_exception(libvlc_exception_t *ex);

    // Static callbacks used by libvlc
    static void * _libvlc_lock(VLCInstance *ctx);
    static void _libvlc_unlock(VLCInstance *ctx);
    static void _libvlc_opensb(VLCInstance *ctx, unsigned int *frequency, unsigned int *nbChannels, unsigned int *fourCCFormat, unsigned int *frameSize);
    static void _libvlc_playsb(VLCInstance *ctx, unsigned char *buffer, size_t bufferSize, unsigned int nbSamples);
    static void _libvlc_closesb(VLCInstance *ctx);

private:
    /// Update mutex
    pthread_mutex_t mUpdateMutex;
    /// Update timer
    Ogre::Real mUpdateTimer;

    /// Texture
    Ogre::TexturePtr mTexture;
    /// Video buffer
    unsigned char* mScreen;

    /// Media resource link
    Ogre::String mMrl;
    /// Video width
    int mWidth;
    /// Video height
    int mHeight;
    /// Video frames per second
    int mFps;
    /// Additional sound parameters
    Ogre::String mSoundParams;
    /// Additional VLC parameters
    Ogre::String mVlcParams;
    /// Sound identifier
    int mSoundId;

    typedef struct { int item; Ogre::String mrl; } PlayListEntry;
    typedef std::vector<PlayListEntry> PlayList;
    /// Playlist
    PlayList mPlayList;
    /// Current playlist item
    int mCurrentPlayListItem;
    /// Play stopped
    bool mStopped;

    /// VLC texture source
    VLCTextureSource* mTextureSource;

    /// Identifier
    int mID;

    /// True if texture is alive
    bool mAlive;
    /// True if texture can be deleted (no more listening frames)
    volatile bool mSafeToDelete;

    /// libvlc instance
    libvlc_instance_t *mLibVLCInstance;

    /// Static mutex
    static pthread_mutex_t ms_Mutex;

    /// libvlc exception structure
    static libvlc_exception_t ms_LibVLCException;
    static bool ms_LibVLCExceptionInstalled;
};  //  class VLCInstance

} // namespace Solipsis

#endif // #ifndef __VLCInstance_h__
