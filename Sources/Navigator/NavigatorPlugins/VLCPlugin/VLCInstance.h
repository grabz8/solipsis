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

// define argsPVLC to use p_vlc struct args ptr OR don t define argsPVLC to use the instance_params_handler
// even with vlc0.9.6, arguments passed to new libvlc instance and read by plugins (vmem, amem, ...)
// are always GLOBAL so 2nd call to libvlc_new(argc, argv) overwrites old args of 1st call
// + when stopping/playing 1 video plugins are freed/recreated so args are read again
// 1 workaround consists in either using p_vlc->args saved pointers to get them from plugins
// or using 1 iph arg pointing to 1 static callback able to give correct args according to p_vlc pointer (libvlc instance *)
// Another problem appears when creating 2 VLCinstances in the same time, 1st new/play is ok but 2nd libvlc_new is crashing due to
// call to 1st plugin Create() by 1st thread not yet performed when calling 2nd libvlc_new
// post about this on VLC devs forums : http://forum.videolan.org/viewtopic.php?f=32&t=55117&p=181676#p181676
#define argsPVLC

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

#ifndef argsPVLC
    /// Instance parameters handler
    void instanceParamsHandler(char *name, char *value);
#endif

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
#ifndef argsPVLC
    static void _libvlc_instparamshdlr(void *vlcObject, char *name, char *value);
#endif
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

    /// libvlc exception structure
    libvlc_exception_t mLibVLCException;

    /// libvlc instance
    libvlc_instance_t *mLibVLCInstance;

#ifndef argsPVLC
    /// Static mutex
    static pthread_mutex_t ms_Mutex;

    /// Map of VLC object ptr and VLC instance
    typedef std::map<void*, VLCInstance*> VLCObjInstMap;
    static VLCObjInstMap ms_VLCObjInstMap;
#else
    /// VLC instance arguments list
    std::list<Ogre::String> args;
    /// VLC instance arguments count
    int vlc_argc;
    /// VLC instance arguments ptr array
    const char **vlc_argv;
#endif
};  //  class VLCInstance

} // namespace Solipsis

#endif // #ifndef __VLCInstance_h__
