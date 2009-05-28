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

#include "SWFInstance.h"
#include "SWFTextureSource.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTextureManager.h>

//#include "SolipsisEvent.h"
#include "Event.h"

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
SWFInstance::SWFInstance(int id, SWFTextureSource* textureSource,
                         FlashControl* flashControl,
                         const String& url, int width, int height, int fps, const String& swfParams) :
    mUpdateMutex(PTHREAD_MUTEX_INITIALIZER),
    mScreen(0),
    mUrl(url),
    mWidth(width),
    mHeight(height),
    mFps(fps),
    mSwfParams(swfParams),
    mTextureSource(textureSource),
    mID(id),
    mAlive(true),
    mSafeToDelete(true),
    mHikariInstance(flashControl)
{
    Root::getSingleton().addFrameListener(this);
    mUpdateTimer = 1.0/mFps;

    TextureManager& tmgr = TextureManager::getSingleton();
/*
    String textureName = "SWFTexture" + StringConverter::toString(mID);
    mTexture = tmgr.createManual(textureName,
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D,
        mWidth, mHeight, 0, PF_BYTE_BGRA, TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
*/

    ResourcePtr resource = MaterialManager::getSingletonPtr()->getByName( flashControl->getMaterialName() );
    MaterialPtr material = (MaterialPtr)resource;
    TextureUnitState* textureUnit = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    String textureName = textureUnit->getTextureName();
    mTexture = tmgr.getByName(textureName);



    /*
     *  Initialise Hikari
     */
/*
    char pclock[64], pcunlock[64], pcdata[64];
    char pwidth[32], pheight[32], ppitch[32];
    char const *swf_argv_default[] =
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
        "--vmem-lock", pclock,
        "--vmem-unlock", pcunlock,
        "--vmem-data", pcdata,
//        "--sout", "#transcode{vcodec=mp2v,vb=1024,scale=1,acodec=mpga,ab=192,channels=2}:duplicate{dst=display{vmem},dst=std{access=http,mux=ts,dst=127.0.0.1:8080}}",
    };
    // Set vmem plugin arguments lock/unlock functions + this as context pointer
    sprintf(pclock, "%lld", (long long int)(intptr_t)_libvlc_lock);
    sprintf(pcunlock, "%lld", (long long int)(intptr_t)_libvlc_unlock);
    sprintf(pcdata, "%lld", (long long int)(intptr_t)this);
    sprintf(pwidth, "%i", mWidth);
    sprintf(pheight, "%i", mHeight);
    sprintf(ppitch, "%i", mWidth*sizeof(unsigned short));
    // Add additional parameters
    std::list<String> args;
    for(int a=0; a<sizeof(swf_argv_default)/sizeof(*swf_argv_default); ++a)
        args.push_back(swf_argv_default[a]);
    for (String::size_type i = mSwfParams.find_first_not_of(" "); i != String::npos; i = mSwfParams.find_first_not_of(" ", i))
    {
        String::size_type j = mSwfParams.find_first_of(" ", i);
        if (j == String::npos) j = mSwfParams.length();
        args.push_back(mSwfParams.substr(i, j - i));
        i = j;
    }
    int swf_argc = (int)args.size();
    const char **vlc_argv = (const char**)malloc(swf_argc*sizeof(const char*));
    swf_argc = 0;
    for (std::list<String>::const_iterator it = args.begin(); it != args.end(); ++it)
        swf_argv[swf_argc++] = it->c_str();
*/

    // Allocating screen buffer
    //mScreen = (unsigned char*)malloc(mWidth*mHeight*sizeof(unsigned short));

    mStopped = false;
}

//-------------------------------------------------------------------------------------
SWFInstance::~SWFInstance()
{
    LogManager::getSingleton().logMessage("SWFInstance::~SWFInstance() waiting mSafeToDelete");
    while (!mSafeToDelete) {}

    LogManager::getSingleton().logMessage("SWFInstance::~SWFInstance() Hikari_destroy");
    
    //mHikariInstance->stop();
    mHikariInstance = NULL;
    mTextureSource = 0;

    delete mScreen;
    LogManager::getSingleton().logMessage("SWFInstance::~SWFInstance() END");
}

//-------------------------------------------------------------------------------------
String SWFInstance::handleEvt(const Event& evt)
{
    String result = "";

    if (mHikariInstance == 0) return result;

    unsigned short width, height;
    mHikariInstance->getExtents(width, height);
    int posX = ((int)(evt.getEvt().mMouse.mState.mXreal * width))%width;
    int posY = ((int)(evt.getEvt().mMouse.mState.mYreal * height))%height;

    // mouse moved
    if (evt.getEvt().mType == ETMouseMoved)
    {
        mHikariInstance->injectMouseMove(posX, posY);
    }

    // mouse wheel roll
    if (evt.getEvt().mMouse.mState.mZrel != 0)
    {
        mHikariInstance->injectMouseWheel(evt.getEvt().mMouse.mState.mZrel, posX, posY);
    }

    // mouse pressed
    if (evt.getEvt().mType == ETMousePressed)
    {
        // left button
        //if (evt.getEvt().mMouse.mState.mButtons == MBLeft)
        {
            mHikariInstance->injectMouseDown(posX, posY);
        }
        // right button         !!! RIGHT CLICK IS NO MORE ENABLE BECAUSE IT'S USED FOR THE CONTECT MENU
        //else
        //{}
    }

    // mouse released
    else if (evt.getEvt().mType == ETMouseReleased)
    {
        // left button
        //if (evt.getEvt().mMouse.mState.mButtons == MBLeft)
        {
            mHikariInstance->injectMouseUp(posX, posY);
        }
        // right button         !!! RIGHT CLICK IS NO MORE ENABLE BECAUSE IT'S USED FOR THE CONTECT MENU
        //else
        //{}
    }

    // key pressed
    if (evt.getEvt().mType == ETKeyPressed)
    {
        mHikariInstance->focus();
    }

    // key released
    else if (evt.getEvt().mType == ETKeyReleased)
    {
        mHikariInstance->defocus();
    }

    return result;
}

//-------------------------------------------------------------------------------------
String SWFInstance::handleEvt(const String& evt)
{
    pthread_mutex_lock(&mUpdateMutex);

    String result = "";

    if (mHikariInstance == 0) 
    {
        pthread_mutex_unlock(&mUpdateMutex);
       return result;
    }

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

    if (tokens.size() < 1)
    {
        pthread_mutex_unlock(&mUpdateMutex);
        return result;
    }

    if (tokens[0].compare("geturl") == 0)
        result == mUrl;
    else if (tokens[0].compare("seturl") == 0)
    {
        if ((tokens.size() < 2) || tokens[1].empty()) 
        {
            pthread_mutex_unlock(&mUpdateMutex);
            return result;
        }
        mUrl = tokens[1];
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_stop");
        mHikariInstance->stop();
        // ...
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_play");
        mHikariInstance->load( mUrl );
        mHikariInstance->play();
    }
    else if (tokens[0].compare("getmute") == 0)
    {
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_get_looped");
        result = "false";
    }
    else if (tokens[0].compare("playpause") == 0)
    {
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_play/flash_player_pause");
        //if (mStopped)
        //    mHikariInstance->play();
        //else
        //    mHikariInstance->stop();
        //mStopped = !mStopped;
        mHikariInstance->stop();
        mHikariInstance->load( mUrl );
        mHikariInstance->play();
        mStopped = false;
        mAlive = true;
    }
    else if (tokens[0].compare("stop") == 0)
    {
        pthread_mutex_unlock(&mUpdateMutex);
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_stop");
        //destroy(true);
        mHikariInstance->stop();
        mStopped = true;
        mAlive = false;
        pthread_mutex_lock(&mUpdateMutex);
    }
    else if (tokens[0].compare("prev") == 0)
    {
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_prev");
        mHikariInstance->rewind();
    }
    else if (tokens[0].compare("next") == 0)
    {
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_next");
        //mHikariInstance->gotoFrame(...);
    }
    else if (tokens[0].compare("mute") == 0)
    {
        LogManager::getSingleton().logMessage("SWFInstance::handleEvt() flash_player_loop_toggle");
        mHikariInstance->setLoop(result.compare("true") == 0);
    }

    pthread_mutex_unlock(&mUpdateMutex);

    return result;
}

//-------------------------------------------------------------------------------------
void SWFInstance::destroy(bool force)
{
    LogManager::getSingleton().logMessage("SWFInstance::destroy()" + String(force ? " forced" : ""));
    pthread_mutex_lock(&mUpdateMutex);
    mSafeToDelete = force;
    mAlive = false;
    mHikariInstance->stop();
    pthread_mutex_unlock(&mUpdateMutex);
}

//-------------------------------------------------------------------------------------
bool SWFInstance::frameStarted(const FrameEvent& e)
{
    pthread_mutex_lock(&mUpdateMutex);

    if (!mAlive)
    {
        mHikariInstance->stop();
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

    if(!mStopped)
        mHikariInstance->update();
/*
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
*/
    pthread_mutex_unlock(&mUpdateMutex);

   return true;
}

//-------------------------------------------------------------------------------------
bool SWFInstance::frameEnded(const FrameEvent& e)
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
/*
//-------------------------------------------------------------------------------------
void SWFInstance::_Hikari_exception(Hikari_exception_t *ex)
{
    if(Hikari_exception_raised(ex))
    {
        LogManager::getSingleton().logMessage("SWFInstance::exception() " + String(Hikari_exception_get_message(ex)));
    }
    Hikari_exception_clear(ex);
}
*/
} // namespace Solipsis
