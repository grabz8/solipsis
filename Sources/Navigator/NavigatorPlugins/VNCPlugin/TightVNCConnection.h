/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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
#include <ExternalTextureSourceEx.h>
#include <Event.h>
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
                       const std::string& host, int port, const std::string& pwd, int fps);
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
    void handleEvt(const Solipsis::Event& evt);
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
    int mFps;
// GREG END

    TightVNCTextureSystem* mTextureSystem;

    int mID;

    bool mAlive;
    volatile bool mSafeToDelete;

};  //  class TightVNCConnection

#endif  //  _TIGHTVNCUPDATER_H_
