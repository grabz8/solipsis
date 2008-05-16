#include "VLCInstance.h"
#include "VLCTextureSource.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTextureManager.h>

namespace Solipsis {

//-------------------------------------------------------------------------------------
VLCInstance::VLCInstance(int id, VLCTextureSource* textureSource,
                         const Ogre::String& mrl, int width, int height, int fps) :
    mUpdateMutex(PTHREAD_MUTEX_INITIALIZER),
    mScreen(0),
    mMrl(mrl),
    mWidth(width),
    mHeight(height),
    mFps(fps),
    mTextureSource(textureSource),
    mID(id),
    mAlive(true),
    mSafeToDelete(true),
    mLibVLCInstance(0)
{
    Ogre::Root::getSingleton().addFrameListener(this);
    mUpdateTimer = 1.0/mFps;

    Ogre::TextureManager& tmgr = Ogre::TextureManager::getSingleton();
    Ogre::String textureName = "VLCTexture" + Ogre::StringConverter::toString(mID);
    mTexture = tmgr.createManual(textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    /*
     *  Initialise libVLC
     */
    char pclock[64], pcunlock[64], pcdata[64];
    char pwidth[32], pheight[32], ppitch[32];
    int item;
    char const *vlc_argv[] =
    {
        "--no-one-instance",
        "--no-stats",
        "--intf", "dummy",
//        "--loop",
        "--fast-mutex", "--win9x-cv-method=1",
        "--plugin-path=.\\VLCplugins",
        "--vout", "vmem",
        "--vmem-width", pwidth,
        "--vmem-height", pheight,
        "--vmem-pitch", ppitch,
        "--vmem-chroma", "RV16",
        "--vmem-lock", pclock,
        "--vmem-unlock", pcunlock,
        "--vmem-data", pcdata,
    };
    int vlc_argc = sizeof(vlc_argv)/sizeof(*vlc_argv);
    sprintf(pclock, "%lld", (long long int)(intptr_t)_libvlc_lock);
    sprintf(pcunlock, "%lld", (long long int)(intptr_t)_libvlc_unlock);
    sprintf(pcdata, "%lld", (long long int)(intptr_t)this);
    sprintf(pwidth, "%i", mWidth);
    sprintf(pheight, "%i", mHeight);
    sprintf(ppitch, "%i", mWidth*sizeof(unsigned short));
    mScreen = (unsigned char*)malloc(mWidth*mHeight*sizeof(unsigned short));
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_exception_init");
    libvlc_exception_init(&mLibVLCException);
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_new");
    mLibVLCInstance = libvlc_new(vlc_argc, (char**)vlc_argv, &mLibVLCException);
    _libvlc_exception(&mLibVLCException);
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_playlist_add");
    item = libvlc_playlist_add (mLibVLCInstance, mMrl.c_str(), NULL, &mLibVLCException); 
    _libvlc_exception(&mLibVLCException);
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_playlist_play");
    libvlc_playlist_play (mLibVLCInstance, item, 0, NULL, &mLibVLCException); 
    _libvlc_exception(&mLibVLCException);
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() END");
}

//-------------------------------------------------------------------------------------
VLCInstance::~VLCInstance()
{
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() waiting mSafeToDelete");
    while (!mSafeToDelete) {}

    Ogre::LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() libvlc_destroy");
    if (mLibVLCInstance != 0)
        libvlc_destroy(mLibVLCInstance);

    mTextureSource = 0;

    delete mScreen;
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() END");
}

//-------------------------------------------------------------------------------------
void VLCInstance::destroy()
{
    Ogre::LogManager::getSingleton().logMessage("VLCInstance::destroy()");
    mSafeToDelete = false;
    mAlive = false;
}

//-------------------------------------------------------------------------------------
bool VLCInstance::frameStarted(const Ogre::FrameEvent& e)
{
    pthread_mutex_lock(&mUpdateMutex);

    if (!mAlive)
    {
        pthread_mutex_unlock(&mUpdateMutex);
        return true;
    }

    mUpdateTimer -= e.timeSinceLastFrame;
    if (mUpdateTimer > 0)
    {
        pthread_mutex_unlock(&mUpdateMutex);
        return true;
    }

    mUpdateTimer = 1.0/mFps;

    // Update texture
    Ogre::HardwarePixelBufferSharedPtr pbuf = mTexture->getBuffer();
    pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = pbuf->getCurrentLock();
    long srcPixelSize = 2;
    long srcPitch = mWidth*srcPixelSize;
    unsigned char* pSrc = mScreen;
    size_t texPixelSize = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
    size_t texPitch = (pixelBox.rowPitch*texPixelSize);
    unsigned char* pDst = static_cast<unsigned char*>(pixelBox.data);
    for(int y = 0; y < mHeight; y++)
    {
        unsigned char* pSrcTmp = pSrc;
        unsigned char* pDstTmp = pDst;
        for(int x = 0; x < mWidth; x++)
        {
            unsigned short bgr565 = *((unsigned short*)pSrcTmp);
            pDstTmp[0] = ((bgr565 & 0xf800) >> 11) << 3;
            pDstTmp[1] = ((bgr565 & 0x07e0) >> 5) << 2;
            pDstTmp[2] = ((bgr565 & 0x001f) >> 0) << 3;
            pDstTmp[3] = 0;
            pSrcTmp += srcPixelSize;
            pDstTmp += texPixelSize;
        }
        pSrc += srcPitch;
        pDst += texPitch;
    }
    pbuf->unlock();

    pthread_mutex_unlock(&mUpdateMutex);

   return true;
}

//-------------------------------------------------------------------------------------
bool VLCInstance::frameEnded(const Ogre::FrameEvent& e)
{
    pthread_mutex_lock(&mUpdateMutex);

    if (!mAlive)
    {
        Ogre::Root::getSingletonPtr()->removeFrameListener(this);
        mSafeToDelete = true;
        pthread_mutex_unlock(&mUpdateMutex);
        mTextureSource->instanceDestroyed(mID);
       return true;
    }
    pthread_mutex_unlock(&mUpdateMutex);

    return true;
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_exception(libvlc_exception_t *ex)
{
    if(libvlc_exception_raised(ex))
    {
        Ogre::LogManager::getSingleton().logMessage("VLCInstance::exception() " + Ogre::String(libvlc_exception_get_message(ex)));
    }
    libvlc_exception_clear(ex);
}

//-------------------------------------------------------------------------------------
void * VLCInstance::_libvlc_lock(VLCInstance *ctx)
{
//    Ogre::LogManager::getSingleton().logMessage("VLCInstance::_libvlc_lock() ");
    pthread_mutex_lock(&ctx->mUpdateMutex);
    return ctx->mScreen;
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_unlock(VLCInstance *ctx)
{
//    Ogre::LogManager::getSingleton().logMessage("VLCInstance::_libvlc_unlock() ");
    pthread_mutex_unlock(&ctx->mUpdateMutex);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
