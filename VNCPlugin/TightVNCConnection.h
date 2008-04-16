#ifndef _TIGHTVNCUPDATER_H_
#define _TIGHTVNCUPDATER_H_

#include <VNCScreenUpdateListener.h>

#include <omnithread.h>
#include <OgreFrameListener.h>
#include <OgreTexture.h>

#include <string>

class ClientConnection;
class TightVNCTextureSystem;
class VNCviewerApp;
// GREG BEGIN
#include "OgreExternalTextureSourceEx.h"
// GREG END

/**
 *  A class that updates and creates VNC textures for one VNC connection.
 *  All textures will be updated in Ogre main thread.
 */
class TightVNCConnection
    : public Ogre::FrameListener
    , public VNCScreenUpdateListener
{
public:
// GREG BEGIN
/*    TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                       const std::string& host, int port);*/
    TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                       const std::string& host, int port, const std::string& pwd);
// GREG END
    ~TightVNCConnection();

    const Ogre::String& getHost() const { return mHost; }
    int getPort() const { return mPort; }

    std::string getURL() const;

    Ogre::TexturePtr getTexture() const { return mTexture; }

    bool connect(VNCviewerApp* app);
    bool isConnected() const;

    bool isAlive() const;
    void destroy();

// GREG BEGIN
    void mouseEvt(int x, int y, Ogre::ExternalTextureSourceEx::eMouseKbdEvent mouseKbdEvent);
// GREG END

private:
    void textureResized();
    void textureReceived();
    void release();

    // ScreenUpdateListener callback
    void screenUpdated(HDC dc, HBITMAP bitmap);

    // FrameListener callbacks
    bool frameStarted(const Ogre::FrameEvent& e);
    bool frameEnded(const Ogre::FrameEvent& e);

private:
    static int mTexIDCounter;

    ClientConnection* mConn;

    omni_mutex mUpdateMutex;
    Ogre::Real mUpdateTimer;

    bool mScreenDirty;
// GREG BEGIN
    bool mGrabScreenIfDirty;
    WPARAM wParam;
// GREG END

    Ogre::TexturePtr mTexture;
    unsigned char* mScreen;
    long mWidth, mHeight;

    std::string mHost;
    int mPort;
// GREG BEGIN
    std::string mPwd;
// GREG END

    TightVNCTextureSystem* mTextureSystem;

    int mID;

    bool mAlive;
    volatile bool mSafeToDelete;

};  //  class TightVNCConnection

#endif  //  _TIGHTVNCUPDATER_H_
