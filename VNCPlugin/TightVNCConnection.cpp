#include "TightVNCConnection.h"
#include "TightVNCTextureSystem.h"
//#include "../llcommon/llerror.h"

#include <vncviewer.h>

#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTextureManager.h>


PBITMAPINFO CreateBitmapInfo(HBITMAP hBmp);

static const float VNC_TEXTURE_UPDATE_DELAY = 0.5;
int TightVNCConnection::mTexIDCounter = 0;

// GREG BEGIN
// ==============================
// key map Solipsis::KeyCode to PC
// ==============================
static int sKeyMapKeyCode2PC[0xFF];
void initKeyMapKeyCode2PC()
{
    int i;

    /* Map the KC keysyms */
    for ( i=0; i<0xFF; ++i )
        sKeyMapKeyCode2PC[i] = 0;

    sKeyMapKeyCode2PC[Solipsis::KC_UNASSIGNED]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_ESCAPE]      = VK_ESCAPE;
    sKeyMapKeyCode2PC[Solipsis::KC_1]           = '1';
    sKeyMapKeyCode2PC[Solipsis::KC_2]           = '2';
    sKeyMapKeyCode2PC[Solipsis::KC_3]           = '3';
    sKeyMapKeyCode2PC[Solipsis::KC_4]           = '4';
    sKeyMapKeyCode2PC[Solipsis::KC_5]           = '5';
    sKeyMapKeyCode2PC[Solipsis::KC_6]           = '6';
    sKeyMapKeyCode2PC[Solipsis::KC_7]           = '7';
    sKeyMapKeyCode2PC[Solipsis::KC_8]           = '8';
    sKeyMapKeyCode2PC[Solipsis::KC_9]           = '9';
    sKeyMapKeyCode2PC[Solipsis::KC_0]           = '0';
    sKeyMapKeyCode2PC[Solipsis::KC_MINUS]       = VK_OEM_MINUS;
    sKeyMapKeyCode2PC[Solipsis::KC_EQUALS]      = VK_OEM_NEC_EQUAL;
    sKeyMapKeyCode2PC[Solipsis::KC_BACK]        = VK_BACK;
    sKeyMapKeyCode2PC[Solipsis::KC_TAB]         = VK_TAB;
    sKeyMapKeyCode2PC[Solipsis::KC_Q]           = 'Q';
    sKeyMapKeyCode2PC[Solipsis::KC_W]           = 'W';
    sKeyMapKeyCode2PC[Solipsis::KC_E]           = 'E';
    sKeyMapKeyCode2PC[Solipsis::KC_R]           = 'R';
    sKeyMapKeyCode2PC[Solipsis::KC_T]           = 'T';
    sKeyMapKeyCode2PC[Solipsis::KC_Y]           = 'Y';
    sKeyMapKeyCode2PC[Solipsis::KC_U]           = 'U';
    sKeyMapKeyCode2PC[Solipsis::KC_I]           = 'I';
    sKeyMapKeyCode2PC[Solipsis::KC_O]           = 'O';
    sKeyMapKeyCode2PC[Solipsis::KC_P]           = 'P';
    sKeyMapKeyCode2PC[Solipsis::KC_LBRACKET]    = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_RBRACKET]    = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_RETURN]      = VK_RETURN;
    sKeyMapKeyCode2PC[Solipsis::KC_LCONTROL]    = VK_LCONTROL;
    sKeyMapKeyCode2PC[Solipsis::KC_A]           = 'A';
    sKeyMapKeyCode2PC[Solipsis::KC_S]           = 'S';
    sKeyMapKeyCode2PC[Solipsis::KC_D]           = 'D';
    sKeyMapKeyCode2PC[Solipsis::KC_F]           = 'F';
    sKeyMapKeyCode2PC[Solipsis::KC_G]           = 'G';
    sKeyMapKeyCode2PC[Solipsis::KC_H]           = 'H';
    sKeyMapKeyCode2PC[Solipsis::KC_J]           = 'J';
    sKeyMapKeyCode2PC[Solipsis::KC_K]           = 'K';
    sKeyMapKeyCode2PC[Solipsis::KC_L]           = 'L';
    sKeyMapKeyCode2PC[Solipsis::KC_SEMICOLON]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_APOSTROPHE]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_GRAVE]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_LSHIFT]      = VK_LSHIFT;
    sKeyMapKeyCode2PC[Solipsis::KC_BACKSLASH]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_Z]           = 'Z';
    sKeyMapKeyCode2PC[Solipsis::KC_X]           = 'X';
    sKeyMapKeyCode2PC[Solipsis::KC_C]           = 'C';
    sKeyMapKeyCode2PC[Solipsis::KC_V]           = 'V';
    sKeyMapKeyCode2PC[Solipsis::KC_B]           = 'B';
    sKeyMapKeyCode2PC[Solipsis::KC_N]           = 'N';
    sKeyMapKeyCode2PC[Solipsis::KC_M]           = 'M';
    sKeyMapKeyCode2PC[Solipsis::KC_COMMA]       = VK_DECIMAL;
    sKeyMapKeyCode2PC[Solipsis::KC_PERIOD]      = VK_DIVIDE;
    sKeyMapKeyCode2PC[Solipsis::KC_SLASH]       = VK_SEPARATOR;
    sKeyMapKeyCode2PC[Solipsis::KC_RSHIFT]      = VK_RSHIFT;
    sKeyMapKeyCode2PC[Solipsis::KC_MULTIPLY]    = VK_MULTIPLY;
    sKeyMapKeyCode2PC[Solipsis::KC_LMENU]       = VK_LMENU;
    sKeyMapKeyCode2PC[Solipsis::KC_SPACE]       = VK_SPACE;
    sKeyMapKeyCode2PC[Solipsis::KC_CAPITAL]     = VK_CAPITAL;
    sKeyMapKeyCode2PC[Solipsis::KC_F1]          = VK_F1;
    sKeyMapKeyCode2PC[Solipsis::KC_F2]          = VK_F2;
    sKeyMapKeyCode2PC[Solipsis::KC_F3]          = VK_F3;
    sKeyMapKeyCode2PC[Solipsis::KC_F4]          = VK_F4;
    sKeyMapKeyCode2PC[Solipsis::KC_F5]          = VK_F5;
    sKeyMapKeyCode2PC[Solipsis::KC_F6]          = VK_F6;
    sKeyMapKeyCode2PC[Solipsis::KC_F7]          = VK_F7;
    sKeyMapKeyCode2PC[Solipsis::KC_F8]          = VK_F8;
    sKeyMapKeyCode2PC[Solipsis::KC_F9]          = VK_F9;
    sKeyMapKeyCode2PC[Solipsis::KC_F10]         = VK_F10;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMLOCK]     = VK_NUMLOCK;
    sKeyMapKeyCode2PC[Solipsis::KC_SCROLL]      = VK_SCROLL;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD7]     = VK_NUMPAD7;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD8]     = VK_NUMPAD8;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD9]     = VK_NUMPAD9;
    sKeyMapKeyCode2PC[Solipsis::KC_SUBTRACT]    = VK_SUBTRACT;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD4]     = VK_NUMPAD4;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD5]     = VK_NUMPAD5;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD6]     = VK_NUMPAD6;
    sKeyMapKeyCode2PC[Solipsis::KC_ADD]         = VK_ADD;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD1]     = VK_NUMPAD1;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD2]     = VK_NUMPAD2;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD3]     = VK_NUMPAD3;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPAD0]     = VK_NUMPAD0;
    sKeyMapKeyCode2PC[Solipsis::KC_DECIMAL]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_OEM_102]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_F11]         = VK_F11;
    sKeyMapKeyCode2PC[Solipsis::KC_F12]         = VK_F12;
    sKeyMapKeyCode2PC[Solipsis::KC_F13]         = VK_F13;
    sKeyMapKeyCode2PC[Solipsis::KC_F14]         = VK_F14;
    sKeyMapKeyCode2PC[Solipsis::KC_F15]         = VK_F15;
    sKeyMapKeyCode2PC[Solipsis::KC_KANA]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_ABNT_C1]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_CONVERT]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_NOCONVERT]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_YEN]         = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_ABNT_C2]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPADEQUALS]= 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_PREVTRACK]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_AT]          = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_COLON]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_UNDERLINE]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_KANJI]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_STOP]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_AX]          = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_UNLABELED]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_NEXTTRACK]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPADENTER] = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_RCONTROL]    = VK_RCONTROL;
    sKeyMapKeyCode2PC[Solipsis::KC_MUTE]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_CALCULATOR]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_PLAYPAUSE]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_MEDIASTOP]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_VOLUMEDOWN]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_VOLUMEUP]    = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBHOME]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_NUMPADCOMMA] = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_DIVIDE]      = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_SYSRQ]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_RMENU]       = VK_RMENU;
    sKeyMapKeyCode2PC[Solipsis::KC_PAUSE]       = VK_PAUSE;
    sKeyMapKeyCode2PC[Solipsis::KC_HOME]        = VK_HOME;
    sKeyMapKeyCode2PC[Solipsis::KC_UP]          = VK_UP;
    sKeyMapKeyCode2PC[Solipsis::KC_PGUP]        = VK_PRIOR;
    sKeyMapKeyCode2PC[Solipsis::KC_LEFT]        = VK_LEFT;
    sKeyMapKeyCode2PC[Solipsis::KC_RIGHT]       = VK_RIGHT;
    sKeyMapKeyCode2PC[Solipsis::KC_END]         = VK_END;
    sKeyMapKeyCode2PC[Solipsis::KC_DOWN]        = VK_DOWN;
    sKeyMapKeyCode2PC[Solipsis::KC_PGDOWN]      = VK_NEXT;
    sKeyMapKeyCode2PC[Solipsis::KC_INSERT]      = VK_INSERT;
    sKeyMapKeyCode2PC[Solipsis::KC_DELETE]      = VK_DELETE;
    sKeyMapKeyCode2PC[Solipsis::KC_LWIN]        = VK_LWIN;
    sKeyMapKeyCode2PC[Solipsis::KC_RWIN]        = VK_RWIN;
    sKeyMapKeyCode2PC[Solipsis::KC_APPS]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_POWER]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_SLEEP]       = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WAKE]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBSEARCH]   = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBFAVORITES]= 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBREFRESH]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBSTOP]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBFORWARD]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_WEBBACK]     = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_MYCOMPUTER]  = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_MAIL]        = 0x00;
    sKeyMapKeyCode2PC[Solipsis::KC_MEDIASELECT] = 0x00;
}
// GREG END

// ----------------------------------------------------------------------------

// GREG BEGIN
/*TightVNCConnection::TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                                       const std::string& host, int port)
    : mUpdateTimer(VNC_TEXTURE_UPDATE_DELAY)
    , mScreenDirty(false)*/
TightVNCConnection::TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                                       const std::string& host, int port, const std::string& pwd, int fps)
    : mScreenDirty(false)
// GREG END
    , mScreen(0)
    , mHost(host)
    , mPort(port)
// GREG BEGIN
    , mGrabScreenIfDirty(true)
    , wParam(0)
    , mPwd(pwd)
    , mFps(fps)
// GREG END
    , mConn(0)
    , mTextureSystem(textureSystem)
    , mID(id)
    , mAlive(true)
    , mSafeToDelete(true)
{
    Ogre::Root::getSingleton().addFrameListener(this);
// GREG BEGIN
    mUpdateTimer = 1.0/mFps;
    initKeyMapKeyCode2PC();
// GREG END
}

TightVNCConnection::~TightVNCConnection()
{
    while (!mSafeToDelete) {}

    if (mConn)
    {
        // NOTE: Connection can't be deleted here! see omni_thread::~omni_thread
        mConn->RemoveUpdateListener(this);
        mConn->KillThread();
//         delete mConn;
        mConn = 0;
    }

    mTextureSystem->connectionClosed(mID);
    mTextureSystem = 0;

// GREG BEGIN
    delete mScreen;
// GREG END
}

bool TightVNCConnection::connect(VNCviewerApp* app)
{
// GREG BEGIN
//    mConn = app->NewConnection((TCHAR*)mHost.c_str(), mPort);
    mConn = app->NewConnection((TCHAR*)mHost.c_str(), mPort, mPwd.empty() ? NULL : (TCHAR*)mPwd.c_str());
// GREG END
    if (mConn)
    {
        mConn->AddUpdateListener(this);
        return true;
    }

    return false;
}

bool TightVNCConnection::isConnected() const
{
    return (mConn != 0);
}

bool TightVNCConnection::isAlive() const
{
    if (mAlive && mConn)
        return mConn->IsRunning();
    else
        return false;
}


void TightVNCConnection::destroy()
{
    mSafeToDelete = false;
    mAlive = false;
}

void TightVNCConnection::textureReceived()
{
    Ogre::String textureName = "TightVNCTexture" + Ogre::StringConverter::toString(mTexIDCounter++);

    Ogre::TextureManager& tmgr = Ogre::TextureManager::getSingleton();
    // GREG BEGIN
/*    mTexture = tmgr.createManual(textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);*/
    mTexture = tmgr.createManual(textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    // GREG END

    mTextureSystem->connectionCreated(mID, textureName);
}

void TightVNCConnection::textureResized()
{
    Ogre::LogManager::getSingleton().logMessage("TightVNCConnection - Reloading VNC texture (resized)");

    Ogre::String name = mTexture->getName();
    Ogre::TextureManager& tmgr = Ogre::TextureManager::getSingleton();
//     tmgr.unload(m_pInfo->m_texture->getHandle());
    tmgr.remove(mTexture->getHandle());

    // GREG BEGIN
/*    mTexture = tmgr.createManual(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);*/
    mTexture = tmgr.createManual(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    // GREG END
}

std::string TightVNCConnection::getURL() const
{
    return "vnc://" + getHost() + ":" + Ogre::StringConverter::toString(getPort());
}


// ----------------------------------------------------------------------------
// VNC ScreenUpdateListener callback

// GREG BEGIN
/*
void TightVNCConnection::screenUpdated(HDC dc, HBITMAP bitmap)
{
    omni_mutex_lock lock(mUpdateMutex);

    PBITMAPINFO info = CreateBitmapInfo(bitmap);
    if (!info)
        return;

    delete mScreen;
    mScreen = new BYTE[info->bmiHeader.biSizeImage];
    if (!GetDIBits(dc, bitmap, 0, info->bmiHeader.biHeight, mScreen, info, DIB_RGB_COLORS))
    {
        delete mScreen;
        mScreen = 0;
        return;
    }

    mWidth = info->bmiHeader.biWidth;
    mHeight = info->bmiHeader.biHeight;

    mScreenDirty = true;
}
*/
void TightVNCConnection::screenUpdated(HDC dc, HBITMAP bitmap)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mGrabScreenIfDirty)
        return;

    PBITMAPINFO info = CreateBitmapInfo(bitmap);
    if (!info)
        return;

    if ((mScreen == 0) || (info->bmiHeader.biWidth != mWidth) || (info->bmiHeader.biHeight != mHeight))
    {
        mWidth = info->bmiHeader.biWidth;
        mHeight = info->bmiHeader.biHeight;
        delete mScreen;
        mScreen = new BYTE[info->bmiHeader.biSizeImage];
    }
    info->bmiHeader.biHeight = -info->bmiHeader.biHeight;
    if (!GetDIBits(dc, bitmap, 0, -info->bmiHeader.biHeight, mScreen, info, DIB_RGB_COLORS))
    {
        delete mScreen;
        mScreen = 0;
        return;
    }

    mScreenDirty = true;
    mGrabScreenIfDirty = false;
}
// GREG END

// ----------------------------------------------------------------------------
// Ogre FrameListener callbacks

bool TightVNCConnection::frameStarted(const Ogre::FrameEvent& e)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mAlive)
        return true;

// GREG BEGIN
/*    // Allocate new texture if data is available and texture hasn't been created
    if (mScreen && mTexture.isNull())
        textureReceived();

    // If no VNC data has been received yet
    if (!mScreen || mTexture.isNull())
        return true;

    // Check for resize
    if ((mWidth != mTexture->getWidth()) || (mHeight != mTexture->getHeight()))
        textureResized();
*/
// GREG END

    mUpdateTimer -= e.timeSinceLastFrame;
    if (mUpdateTimer > 0)
        return true;

// GREG BEGIN
//    mUpdateTimer = VNC_TEXTURE_UPDATE_DELAY;
    mUpdateTimer = 1.0/mFps;
    mGrabScreenIfDirty = true;
// GREG END

    // Update texture

    if (mScreenDirty)
    {
// GREG BEGIN
        // Allocate new texture if data is available and texture hasn't been created
        if (mScreen && mTexture.isNull())
            textureReceived();

        // If no VNC data has been received yet
        if (!mScreen || mTexture.isNull())
            return true;

        // Check for resize
        if ((mWidth != mTexture->getWidth()) || (mHeight != mTexture->getHeight()))
            textureResized();
// GREG END
        Ogre::HardwarePixelBufferSharedPtr pbuf = mTexture->getBuffer();
        pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixelBox = pbuf->getCurrentLock();
// GREG BEGIN
//        memcpy(pixelBox.data, mScreen, pixelBox.getConsecutiveSize());
        long srcPixelSize = 4;
        // if height is negative -> top-down DIB
        // if height is positive -> bottom-up DIB
//        long srcPitch = (mHeight < 0) ? mWidth*srcPixelSize : -mWidth*srcPixelSize;
//        unsigned char* pSrc = (mHeight < 0) ? mScreen : (mScreen + (mHeight - 1)*mWidth*srcPixelSize);
        long srcPitch = mWidth*srcPixelSize;
        unsigned char* pSrc = mScreen;
	    size_t texPixelSize = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	    size_t texPitch = (pixelBox.rowPitch*texPixelSize);
	    unsigned char* pDst = static_cast<unsigned char*>(pixelBox.data);
        for(int y = 0; y < mHeight; y++)
        {
            memcpy(pDst, pSrc, mWidth*srcPixelSize);
/*            unsigned char* pSrcTmp = pSrc;
            unsigned char* pDstTmp = pDst;
            for(int x = 0; x < mWidth; x++)
            {
                pDstTmp[0] = pSrcTmp[0];
                pDstTmp[1] = pSrcTmp[1];
                pDstTmp[2] = pSrcTmp[2];
                pDstTmp[3] = 0;
                pSrcTmp += srcPixelSize;
                pDstTmp += texPixelSize;
            }*/
            pSrc += srcPitch;
            pDst += texPitch;
        }
// GREG END
        pbuf->unlock();
        mScreenDirty = false;
    }

   return true;
}

bool TightVNCConnection::frameEnded(const Ogre::FrameEvent& e)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mAlive)
    {
        Ogre::Root::getSingletonPtr()->removeFrameListener(this);
        mSafeToDelete = true;
        return true;
    }

    return true;
}

// GREG BEGIN
void TightVNCConnection::handleEvt(const Solipsis::Event& evt)
{
    using namespace Solipsis;
    if (!mConn) return;

    const Evt& e = evt.getEvt();
    if ((e.mType == ETMousePressed) ||
        (e.mType == ETMouseReleased) ||
        (e.mType == ETMouseMoved))
    {
        int width = (int)mTexture->getSrcWidth();
        int height = (int)mTexture->getSrcHeight();
        int x = ((int)(e.mMouse.mState.mXreal*width))%width;
        int y = ((int)(e.mMouse.mState.mYreal*height))%height;
        UINT iMsg;
        int typeIdx = 0;
        switch (e.mType)
        {
        case ETMouseMoved: typeIdx = 0; break;
        case ETMousePressed: typeIdx = 1; break;
        case ETMouseReleased: typeIdx = 2; break;
        }
        int btnIdx = 0;
        switch (e.mMouse.mState.mButtons)
        {
        case MBLeft: btnIdx = 1; break;
        case MBRight: btnIdx = 2; break;
        case MBMiddle: btnIdx = 3; break;
        }
        static UINT eMouseKbdEventMapping[3][4] = {
            {WM_MOUSEMOVE, WM_MOUSEMOVE, WM_MOUSEMOVE, WM_MOUSEMOVE},
            {WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN}, 
            {WM_MOUSEMOVE, WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP}
        };
        iMsg = eMouseKbdEventMapping[typeIdx][btnIdx];
        static UINT eMouseKbdEventMappingBtnStateOr[3][4] = {
            {0 ,0 ,0 ,0},
            {0, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON},
            {0 ,0 ,0 ,0}
        };
        static UINT eMouseKbdEventMappingBtnStateAnd[3][4] = {
            {0 ,0 ,0 ,0},
            {0 ,0 ,0 ,0},
            {0, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON}
        };
        wParam |= eMouseKbdEventMappingBtnStateOr[typeIdx][btnIdx];
        wParam &= ~eMouseKbdEventMappingBtnStateAnd[typeIdx][btnIdx];

        mConn->mouseEvt(x, y, iMsg, wParam);
    }
    else if ((e.mType == ETKeyPressed) || (e.mType == ETKeyReleased))
    {
        DWORD lParam = 0;
        if (e.mType == ETKeyPressed) lParam |= 0x80000000l; // down
        lParam |= 0x1000000; // extended
        int pcvirtkey = sKeyMapKeyCode2PC[e.mKeyboard.mKey];
        mConn->kbdEvt(pcvirtkey, lParam);
    }
}
// GREG END

// ---------------------------------------------------------------------------
// Taken from MSDN

PBITMAPINFO CreateBitmapInfo(HBITMAP hBmp)
{
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        return 0;

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
}
