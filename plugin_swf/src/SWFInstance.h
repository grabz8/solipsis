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

#ifndef __SWFInstance_h__
#define __SWFInstance_h__

#include <pthread.h>

#include <hikari.h>

#include <OgreFrameListener.h>
#include <OgreTexture.h>

#include <ExternalTextureSourceEx.h>
#include <Event.h>

using namespace Hikari;

namespace Solipsis {

class SWFTextureSource;

/**
 *  A class that updates and creates SWF textures for one SWF instance.
 *  All textures will be updated in Ogre main thread.
 */
class SWFInstance : public Ogre::FrameListener
{
public:
    /** Constructor
    @param[in] id The unique identifier
    @param[in] mrl The media resource link
    @param[in] width The video width
    @param[in] height The video height
    @param[in] fps The update rate in frames per second
    @param[in] swfParams The additional SWF parameters
    */
    SWFInstance(int id, SWFTextureSource* textureSource,
                FlashControl* flashControl,
                const Ogre::String& url, int width, int height, int fps, const Ogre::String& swfParams);
    /// Destructor
    ~SWFInstance();

    /// Get MRL
    const Ogre::String& getUrl() const { return mUrl; }

    /// Get Texture
    Ogre::TexturePtr getTexture() const { return mTexture; }

    /// Handle 1 event
    Ogre::String handleEvt(const Ogre::String& evt);
    Ogre::String handleEvt(const Event& evt);

    /// Destroy
    void destroy(bool force = false);

private:
    /// @copydoc Ogre::FrameListener::frameStarted
    bool frameStarted(const Ogre::FrameEvent& e);
    /// @copydoc Ogre::FrameListener::frameEnded
    bool frameEnded(const Ogre::FrameEvent& e);
/*
    /// libvlc exception helper method
    void _Hikari_exception(Hikari_exception_t *ex);
*/
    // Static callbacks used by libvlc
    static void * _Hikari_lock(SWFInstance *ctx);
    static void _Hikari_unlock(SWFInstance *ctx);

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
    Ogre::String mUrl;
    /// Video width
    int mWidth;
    /// Video height
    int mHeight;
    /// Video frames per second
    int mFps;
    /// Additional SWF parameters
    Ogre::String mSwfParams;
/*
    typedef struct { int item; Ogre::String mrl; } PlayListEntry;
    typedef std::vector<PlayListEntry> PlayList;
    /// Playlist
    PlayList mPlayList;
*/
    /// Current playlist item
    int mCurrentPlayListItem;
    /// Play stopped
    bool mStopped;

    /// SWF texture source
    SWFTextureSource* mTextureSource;

    /// Identifier
    int mID;

    /// True if texture is alive
    bool mAlive;
    /// True if texture can be deleted (no more listening frames)
    volatile bool mSafeToDelete;
/*
    /// libvlc exception structure
    Hikari_exception_t mHikariException;
*/
    /// hikari instance
    FlashControl* mHikariInstance;
};  //  class SWFInstance

} // namespace Solipsis

#endif // #ifndef __SWFInstance_h__
