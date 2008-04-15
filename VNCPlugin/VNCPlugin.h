#ifndef __VNCPlugin_h__
#define __VNCPlugin_h__

#include "OgrePlugin.h"
#include <OgreSharedPtr.h>

#include <omnithread.h>

class ClientConnection;
class TightVNCConnection;
class TightVNCTextureSystem;
class VNCviewerApp32;

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
    *  @return Connection id
    */
    int newConnection(const Ogre::String& host, int port, const Ogre::String& pwd);
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