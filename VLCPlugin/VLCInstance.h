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
    @param[in] vlcParams The additional VLC parameters
    */
    VLCInstance(int id, VLCTextureSource* textureSource,
                const Ogre::String& mrl, int width, int height, int fps, const Ogre::String& vlcParams);
    /// Destructor
    ~VLCInstance();

    /// Get MRL
    const Ogre::String& getMrl() const { return mMrl; }

    /// Get Texture
    Ogre::TexturePtr getTexture() const { return mTexture; }

    /// Destroy
    void destroy();

private:
    /// @copydoc Ogre::FrameListener::frameStarted
    bool frameStarted(const Ogre::FrameEvent& e);
    /// @copydoc Ogre::FrameListener::frameEnded
    bool frameEnded(const Ogre::FrameEvent& e);

    /// libvlc exception helper method
    void _libvlc_exception(libvlc_exception_t *ex);

    // Static callbacks used by libvlc
    static void * _libvlc_lock(VLCInstance *ctx);
    static void _libvlc_unlock(VLCInstance *ctx);

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
    /// Additional VLC parameters
    Ogre::String mVlcParams;

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
};  //  class VLCInstance

} // namespace Solipsis

#endif // #ifndef __VLCInstance_h__
