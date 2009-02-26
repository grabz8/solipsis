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

#ifndef __VNCPlugin_h__
#define __VNCPlugin_h__

#define NOMINMAX

#include "OgrePlugin.h"
#include <OgreSharedPtr.h>

#include <omnithread.h>

class ClientConnection;
class TightVNCConnection;
class TightVNCTextureSystem;
class VNCviewerApp32;
// GREG BEGIN
#include <ExternalTextureSourceEx.h>
#include <Event.h>
// GREG END

namespace Solipsis
{
/** This class represents the VNC plugin.
*/
class VNCPlugin : public Ogre::Plugin
{
public:
    VNCPlugin();

    /// @copydoc Plugin::getName
    const Ogre::String& getName() const;

    /// @copydoc Plugin::install
    void install();

    /// @copydoc Plugin::initialise
    void initialise();

    /// @copydoc Plugin::shutdown
    void shutdown();

    /// @copydoc Plugin::uninstall
    void uninstall();

// GREG BEGIN
    /**
    *  Creates a new VNC connection asynchronously
    *  @param host VNC server host name
    *  @param port VNC server port
    *  @return Connection id
    */
//    int newConnection(const Ogre::String& host, int port);
    /**
    *  Creates a new VNC connection asynchronously
    *  @param host VNC server host name
    *  @param port VNC server port
    *  @param pwd VNC server password
    *  @param fps VNC update rate in frames per second
    *  @return Connection id
    */
    int newConnection(const Ogre::String& host, int port, const Ogre::String& pwd, int fps);
// GREG END

    /**
     *	@param id Connection id
     */
    void closeConnection(int id);

    /**
    *  Retrieves an existing VNC connection by host name and port
    *  @param host VNC server host name
    *  @param port VNC server port
    *  @return Connection id
    */
    int lookupConnection(const Ogre::String& host, int port);

    /**
    *  Returns the VNC texture for the given connection
    *  @param id VNC connection id
    *  @return VNC texture or a NULL TexturePtr if the texture is not yet available
    */
    Ogre::TexturePtr getTextureForConnection(const int id) const;

// GREG BEGIN
    void handleEvt(const int id, const Event& evt);
// GREG END

private:
    static unsigned long __stdcall vncThreadRun(void* params);

    VNCviewerApp32* mVNCApp;

    Ogre::String mName;
    TightVNCTextureSystem* mTextureSystem;

    typedef Ogre::SharedPtr<TightVNCConnection> ConnectionPtr;
    typedef std::map<int, ConnectionPtr> ConnectionById;
    ConnectionById mConnByID;

    int mConnIDCounter;
    mutable omni_mutex mConnMutex;

    bool mRunning;
    void* mVNCThreadHandle;
};

} // namespace Solipsis

#endif // #ifndef __VNCPlugin_h__