#include <winsock2.h>
#include "VNCPlugin.h"

#include "TightVNCConnection.h"
#include "TightVNCTextureSystem.h"

#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

#include <Exception.h>
#include <HotKeys.h>
#include <vncviewer.h>
#include <VNCviewerApp32.h>
#include <VNCHelp.h>

using namespace Solipsis;

const Ogre::String sPluginName = "VNC Texture Source";

//-------------------------------------------------------------------------------------
VNCPlugin::VNCPlugin() :
    mTextureSystem(0),
    mConnIDCounter(0)
{
}

//-------------------------------------------------------------------------------------
const Ogre::String& VNCPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void VNCPlugin::install()
{
}

//-------------------------------------------------------------------------------------
void VNCPlugin::initialise()
{
    mRunning = true;
    mVNCThreadHandle = CreateThread(0, 0, &VNCPlugin::vncThreadRun, (void*)this, 0, 0);

    mTextureSystem = new TightVNCTextureSystem(this);
}

//-------------------------------------------------------------------------------------
void VNCPlugin::shutdown()
{
    mRunning = false;
// GREG BEGIN
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VNCPlugin::shutdown() WaitForSingleObject(mVNCThreadHandle)");
// GREG END
    WaitForSingleObject(mVNCThreadHandle, 5000);
// GREG BEGIN
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VNCPlugin::shutdown() deleting texture system");
// GREG END
    mConnByID.clear();
    delete mTextureSystem;
    mTextureSystem = 0;
// GREG BEGIN
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VNCPlugin::shutdown() end");
// GREG END
}

//-------------------------------------------------------------------------------------
void VNCPlugin::uninstall()
{
}

//-------------------------------------------------------------------------------------

int VNCPlugin::lookupConnection(const Ogre::String& host, int port)
{
    Ogre::LogManager::getSingleton().logMessage("VNCPlugin::lookupConnection");
//     omni_mutex_lock l(m_connMutex);

    for (ConnectionById::const_iterator i = mConnByID.begin(); i != mConnByID.end(); ++i)
    {
        if (i->second->getHost() == host && i->second->getPort() == port)
            return i->first;
    }

    return -1;
}

// GREG BEGIN
//int VNCPlugin::newConnection(const Ogre::String& host, const int port)
int VNCPlugin::newConnection(const Ogre::String& host, const int port, const Ogre::String& pwd, int fps)
// GREG END
{
    omni_mutex_lock l(mConnMutex);

// GREG BEGIN
/*    Ogre::LogManager::getSingleton().logMessage("VNCPlugin - Creating new connection for '" +
        host + ":" + Ogre::StringConverter::toString(port) + "'");*/
    Ogre::LogManager::getSingleton().logMessage("VNCPlugin - Creating new connection for '" +
        host + ":" + Ogre::StringConverter::toString(port) + ";" + pwd + "'");
// GREG END

    // Set connection host and port and map it by id
    // The connection will be established asynchronously in VNC main thread
    // so it won't block application threads
    int id = mConnIDCounter++;
// GREG BEGIN
//    ConnectionPtr conn(new TightVNCConnection(id, mTextureSystem, host, port));
    ConnectionPtr conn(new TightVNCConnection(id, mTextureSystem, host, port, pwd, fps));
// GREG END
    mConnByID.insert(std::make_pair(id, conn));

    return id;
}

void VNCPlugin::closeConnection(int id)
{
    omni_mutex_lock l(mConnMutex);
    Ogre::LogManager::getSingleton().logMessage("VNCPlugin::closeConnection");
    ConnectionById::iterator i = mConnByID.find(id);
    if (i != mConnByID.end())
    {
        i->second->destroy();
    }
}

Ogre::TexturePtr VNCPlugin::getTextureForConnection(const int id) const
{
    //omni_mutex_lock l(m_connMutex);
    //Ogre::LogManager::getSingleton().logMessage("VNCPlugin - Retrieving texture for connection " +
    //    Ogre::StringConverter::toString(id));

    ConnectionById::const_iterator i = mConnByID.find(id);
    if (i != mConnByID.end())
        return i->second->getTexture();

    // Not found
    return Ogre::TexturePtr();
}

// GREG BEGIN
void VNCPlugin::handleEvt(const int id, const Event& evt)
{
    ConnectionById::const_iterator i = mConnByID.find(id);
    if (i == mConnByID.end())
        return;
    ConnectionPtr conn = i->second;
    conn->handleEvt(evt);
}
// GREG END

// ----------------------------------------------------------------------------
// VNC Thread

unsigned long __stdcall VNCPlugin::vncThreadRun(void* params)
{
    VNCPlugin* plugin = (VNCPlugin*)params;

    // Create VNC app

    plugin->mVNCApp = new VNCviewerApp32((HINSTANCE)GetVNCModule(), "");

    // Enter VNC main loop

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (plugin->mRunning)
    {
        try
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) &&
                !GetVNCHotkeys().TranslateAccel(&msg) &&
                !GetVNCHelp().TranslateMsg(&msg) &&
                !plugin->mVNCApp->ProcessDialogMessage(&msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }

            // Go through connection info's and create new connections for uninitiated connections
            // This must be done in a separate thread so Ogre main thread won't stall
            {
                omni_mutex_lock lock(plugin->mConnMutex);
                ConnectionById::iterator i = plugin->mConnByID.begin();
                while (i != plugin->mConnByID.end())
                {
                    ConnectionPtr& conn = i->second;
                    if (!conn->isConnected())
                    {
                        if (!conn->connect(plugin->mVNCApp))
                        {
                            Ogre::LogManager::getSingleton().logMessage("Removing connection with id '" +
                                Ogre::StringConverter::toString(i->first) + "' (unable to establish connection)");
                            conn->destroy();
//                            LLOgreMediaEngine::getInstance()->unauthorizeMediaPlayback(conn->getURL());
                            i = plugin->mConnByID.erase(i);
                            continue;
                        }
                    }
                    else if (!conn->isAlive())
                    {
                        Ogre::LogManager::getSingleton().logMessage("Removing dead connection with id '" +
                            Ogre::StringConverter::toString(i->first) + "'");
//                        LLOgreMediaEngine::getInstance()->unauthorizeMediaPlayback(conn->getURL());
                        i = plugin->mConnByID.erase(i);
                        continue;
                    }
                    ++i;
                }
            }

            Sleep(50);
        }
        catch (...)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Unhandled exception in VNC main thread");
        }
    }

    WSACleanup();

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Shutting down VNC main thread...");

    delete plugin->mVNCApp;

// GREG BEGIN
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VNC main thread end");
// GREG END

    return 0;
}
