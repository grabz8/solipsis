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

#include "VLCInstance.h"
#include "VLCTextureSource.h"
#include "ExternalTextureSourceEx.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTextureManager.h>

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
VLCInstance::VLCInstance(int id, VLCTextureSource* textureSource,
                         const String& mrl, int width, int height, int fps, const String& soundParams, const String& vlcParams) :
    mUpdateMutex(PTHREAD_MUTEX_INITIALIZER),
    mScreen(0),
    mMrl(mrl),
    mWidth(width),
    mHeight(height),
    mFps(fps),
    mSoundParams(soundParams),
    mVlcParams(vlcParams),
    mSoundId(-1),
    mTextureSource(textureSource),
    mID(id),
    mAlive(true),
    mSafeToDelete(true),
    mLibVLCInstance(0)
{
    Root::getSingleton().addFrameListener(this);
    mUpdateTimer = 1.0/mFps;

    TextureManager& tmgr = TextureManager::getSingleton();
    String textureName = "VLCTexture" + StringConverter::toString(mID);
    mTexture = tmgr.createManual(textureName,
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D,
        mWidth, mHeight, 0, PF_BYTE_BGRA, TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    // Add 1 sound buffer into the sound handler
    ExternalTextureSourceExSoundHandler *soundHandler = mTextureSource->getSoundHandler();
    if (soundHandler != 0)
        mSoundId = soundHandler->createSoundBuffer(mMrl);

    /*
     *  Initialise libVLC
     */
    char plock[64], punlock[64], pdata[64];
    char pwidth[32], pheight[32], ppitch[32];
    char const *vlc_argv_default[] =
    {
        "--no-one-instance",
        "--no-stats",
        "--intf", "dummy",
        "--fast-mutex", "--win9x-cv-method=1",
        "--plugin-path=.\\VLCplugins",
        "--vout", "vmem",
        "--vmem-width", pwidth,
        "--vmem-height", pheight,
        "--vmem-pitch", ppitch,
        "--vmem-chroma", "RV16",
        "--vmem-lock", plock,
        "--vmem-unlock", punlock,
        "--vmem-data", pdata,
//        "--sout", "#transcode{vcodec=mp2v,vb=1024,scale=1,acodec=mpga,ab=192,channels=2}:duplicate{dst=display{vmem},dst=std{access=http,mux=ts,dst=127.0.0.1:8080}}",
    };
    char popensb[64], pplaysb[64], pclosesb[64];
    char const *vlc_argv_amem[] =
    {
        "--aout", "amem",
        "--amem-opensb", popensb,
        "--amem-playsb", pplaysb,
        "--amem-closesb", pclosesb,
        "--amem-data", pdata,
    };
    // Set vmem plugin arguments lock/unlock functions + this as context pointer
    sprintf(plock, "%lld", (long long int)(intptr_t)_libvlc_lock);
    sprintf(punlock, "%lld", (long long int)(intptr_t)_libvlc_unlock);
    sprintf(pdata, "%lld", (long long int)(intptr_t)this);
    sprintf(pwidth, "%i", mWidth);
    sprintf(pheight, "%i", mHeight);
    sprintf(ppitch, "%i", mWidth*sizeof(unsigned short));
    sprintf(popensb, "%lld", (long long int)(intptr_t)_libvlc_opensb);
    sprintf(pplaysb, "%lld", (long long int)(intptr_t)_libvlc_playsb);
    sprintf(pclosesb, "%lld", (long long int)(intptr_t)_libvlc_closesb);
    // Set standard parameters in list
    std::list<String> args;
    for(int a=0; a<sizeof(vlc_argv_default)/sizeof(*vlc_argv_default); ++a)
        args.push_back(vlc_argv_default[a]);
    // Add additional sound parameters
    if (mSoundParams.find("3d") == 0)
    {
        for(int a=0; a<sizeof(vlc_argv_amem)/sizeof(*vlc_argv_amem); ++a)
            args.push_back(vlc_argv_amem[a]);
    }
    // Add additional parameters
    for (String::size_type i = mVlcParams.find_first_not_of(" "); i != String::npos; i = mVlcParams.find_first_not_of(" ", i))
    {
        String::size_type j = mVlcParams.find_first_of(" ", i);
        if (j == String::npos) j = mVlcParams.length();
        args.push_back(mVlcParams.substr(i, j - i));
        i = j;
    }
    int vlc_argc = (int)args.size();
    const char **vlc_argv = (const char**)malloc(vlc_argc*sizeof(const char*));
    vlc_argc = 0;
    for (std::list<String>::const_iterator it = args.begin(); it != args.end(); ++it)
        vlc_argv[vlc_argc++] = it->c_str();
    // Allocating screen buffer
    mScreen = (unsigned char*)malloc(mWidth*mHeight*sizeof(unsigned short));
    memset(mScreen, 0, mWidth*mHeight*sizeof(unsigned short));
    // libvlc Init/New instance/Add playlist/play
    LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_exception_init");
    libvlc_exception_init(&mLibVLCException);
    LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_new");
    mLibVLCInstance = libvlc_new(vlc_argc, (char**)vlc_argv, &mLibVLCException);
    _libvlc_exception(&mLibVLCException);
    LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_playlist_add");
    int item = libvlc_playlist_add(mLibVLCInstance, mMrl.c_str(), NULL, &mLibVLCException); 
    _libvlc_exception(&mLibVLCException);
    PlayListEntry playListEntry = {item, mMrl};
    mPlayList.push_back(playListEntry);
    mCurrentPlayListItem = 0;
    LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() libvlc_playlist_play");
    libvlc_playlist_play(mLibVLCInstance, mPlayList[mCurrentPlayListItem].item, 0, NULL, &mLibVLCException); 
    _libvlc_exception(&mLibVLCException);
    mStopped = false;
    LogManager::getSingleton().logMessage("VLCInstance::VLCInstance() END");
    free(vlc_argv);
}

//-------------------------------------------------------------------------------------
VLCInstance::~VLCInstance()
{
    LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() waiting mSafeToDelete");
    while (!mSafeToDelete) {}

    LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() libvlc_destroy");
    if (mLibVLCInstance != 0)
        libvlc_destroy(mLibVLCInstance);

    // Destroy the sound buffer into the sound handler
    ExternalTextureSourceExSoundHandler *soundHandler = mTextureSource->getSoundHandler();
    if (soundHandler != 0)
        soundHandler->destroySoundBuffer(mSoundId);

    mTextureSource = 0;

    delete mScreen;

    LogManager::getSingleton().logMessage("VLCInstance::~VLCInstance() END");
}

//-------------------------------------------------------------------------------------
String VLCInstance::handleEvt(const String& evt)
{
    String result = "";

    if (mLibVLCInstance == 0) return result;

    std::vector<String> tokens;
    std::string delimiter = "?";
    size_t p0 = 0, p1 = String::npos;
    while (p0 != String::npos)
    {
        p1 = evt.find_first_of(delimiter, p0);
        if (p1 != p0)
        {
            String token = evt.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = evt.find_first_not_of(delimiter, p1);
    }

    if (tokens.size() < 1) return result;
    if (tokens[0].compare("getmrl") == 0)
        return mMrl;
    else if (tokens[0].compare("setmrl") == 0)
    {
        if ((tokens.size() < 2) || tokens[1].empty()) return result;
        mMrl = tokens[1];
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_stop");
        libvlc_playlist_stop(mLibVLCInstance, &mLibVLCException);
        _libvlc_exception(&mLibVLCException);
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_add");
        int item = libvlc_playlist_add(mLibVLCInstance, mMrl.c_str(), NULL, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
        PlayListEntry playListEntry = {item, mMrl};
        mPlayList.push_back(playListEntry);
        mCurrentPlayListItem = (int)mPlayList.size() - 1;
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_play");
        libvlc_playlist_play(mLibVLCInstance, mPlayList[mCurrentPlayListItem].item, 0, NULL, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
    }
    else if (tokens[0].compare("getmute") == 0)
    {
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_audio_get_mute");
        result = (libvlc_audio_get_mute(mLibVLCInstance, &mLibVLCException) ? "true" : "false");
        _libvlc_exception(&mLibVLCException);
    }
    else if (tokens[0].compare("playpause") == 0)
    {
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_play/libvlc_playlist_pause");
        if (mStopped)
            libvlc_playlist_play(mLibVLCInstance, mPlayList[mCurrentPlayListItem].item, 0, NULL, &mLibVLCException); 
        else
            libvlc_playlist_pause(mLibVLCInstance, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
        mStopped = false;
    }
    else if (tokens[0].compare("stop") == 0)
    {
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_stop");
        libvlc_playlist_stop(mLibVLCInstance, &mLibVLCException);
        _libvlc_exception(&mLibVLCException);
        mStopped = true;
    }
    else if ((tokens[0].compare("prev") == 0) && (mCurrentPlayListItem > 0))
    {
        mCurrentPlayListItem--;
        mMrl = mPlayList[mCurrentPlayListItem].mrl;
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_play");
        libvlc_playlist_play(mLibVLCInstance, mPlayList[mCurrentPlayListItem].item, 0, NULL, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
    }
    else if ((tokens[0].compare("next") == 0) && (mCurrentPlayListItem < (int)mPlayList.size() - 1))
    {
        mCurrentPlayListItem++;
        mMrl = mPlayList[mCurrentPlayListItem].mrl;
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_playlist_play");
        libvlc_playlist_play(mLibVLCInstance, mPlayList[mCurrentPlayListItem].item, 0, NULL, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
    }
    else if (tokens[0].compare("mute") == 0)
    {
        LogManager::getSingleton().logMessage("VLCInstance::handleEvt() libvlc_audio_toggle_mute");
        libvlc_audio_toggle_mute(mLibVLCInstance, &mLibVLCException); 
        _libvlc_exception(&mLibVLCException);
    }

    return result;
}

//-------------------------------------------------------------------------------------
void VLCInstance::destroy(bool force)
{
    LogManager::getSingleton().logMessage("VLCInstance::destroy()" + String(force ? " forced" : ""));
    pthread_mutex_lock(&mUpdateMutex);
    mSafeToDelete = force;
    mAlive = false;
    pthread_mutex_unlock(&mUpdateMutex);
}

//-------------------------------------------------------------------------------------
bool VLCInstance::frameStarted(const FrameEvent& e)
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
    HardwarePixelBufferSharedPtr pbuf = mTexture->getBuffer();
    pbuf->lock(HardwareBuffer::HBL_DISCARD);
    const PixelBox& pixelBox = pbuf->getCurrentLock();
    long srcPixelSize = 2;
    long srcPitch = mWidth*srcPixelSize;
    unsigned char* pSrc = mScreen;
    size_t texPixelSize = PixelUtil::getNumElemBytes(pixelBox.format);
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
bool VLCInstance::frameEnded(const FrameEvent& e)
{
    pthread_mutex_lock(&mUpdateMutex);

    if (!mAlive)
    {
        Root::getSingletonPtr()->removeFrameListener(this);
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
        LogManager::getSingleton().logMessage("VLCInstance::exception() " + String(libvlc_exception_get_message(ex)));
    }
    libvlc_exception_clear(ex);
}

//-------------------------------------------------------------------------------------
void * VLCInstance::_libvlc_lock(VLCInstance *ctx)
{
//    LogManager::getSingleton().logMessage("VLCInstance::_libvlc_lock() ");
    pthread_mutex_lock(&ctx->mUpdateMutex);
    return ctx->mScreen;
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_unlock(VLCInstance *ctx)
{
//    LogManager::getSingleton().logMessage("VLCInstance::_libvlc_unlock() ");
    pthread_mutex_unlock(&ctx->mUpdateMutex);
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_opensb(VLCInstance *ctx, unsigned int *frequency, unsigned int *nbChannels, unsigned int *fourCCFormat, unsigned int *frameSize)
{
//    char tmp[256];
//    _snprintf(tmp, sizeof(tmp), "mSoundId:%d, frequency:%d, nbChannels:%d, fourCCFormat:%d, frameSize:%d", ctx->getSoundId(), *frequency, *nbChannels, *fourCCFormat, *frameSize);
//    LogManager::getSingleton().logMessage("VLCInstance::_libvlc_opensb() " + String(tmp));
    ExternalTextureSourceExSoundHandler *soundHandler = ctx->getTextureSource()->getSoundHandler();
    if (soundHandler != 0)
        soundHandler->openSoundBuffer(ctx->getSoundId(), ctx->getSoundParams(), frequency, nbChannels, fourCCFormat, frameSize);
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_playsb(VLCInstance *ctx, unsigned char *buffer, size_t bufferSize, unsigned int nbSamples)
{
//    char tmp[256];
//    _snprintf(tmp, sizeof(tmp), "mSoundId:%d, buffer:0x%08x, bufferSize:%ld, nbSamples:%d", ctx->getSoundId(), buffer, bufferSize, nbSamples);
//    LogManager::getSingleton().logMessage("VLCInstance::_libvlc_playsb() " + String(tmp));
    ExternalTextureSourceExSoundHandler *soundHandler = ctx->getTextureSource()->getSoundHandler();
    if (soundHandler != 0)
        soundHandler->playSoundBuffer(ctx->getSoundId(), buffer, bufferSize, nbSamples);
}

//-------------------------------------------------------------------------------------
void VLCInstance::_libvlc_closesb(VLCInstance *ctx)
{
//    char tmp[256];
//    _snprintf(tmp, sizeof(tmp), "mSoundId:%d", ctx->getSoundId());
//    LogManager::getSingleton().logMessage("VLCInstance::_libvlc_closesb() " + String(tmp));
    ExternalTextureSourceExSoundHandler *soundHandler = ctx->getTextureSource()->getSoundHandler();
    if (soundHandler != 0)
        soundHandler->closeSoundBuffer(ctx->getSoundId());
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
