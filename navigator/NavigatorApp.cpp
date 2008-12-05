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

#include <sstream>
#include <CTCrashReporter.h>
#include <CTIO.h>
#include <NavigatorModule.h>
#include <IApplication.h>
#include <IInstance.h>
#include <IWindow.h>
#ifdef NULLCLIENTSERVER
#include <PeerModule.h>
#include <IPeer.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace Solipsis;

#ifdef NULLCLIENTSERVER
/** This class implements the IPeerRenderSystemLock interface to avoid concurrent
    render system calls between Peer thread and Navigator thread.
*/
class PeerOgreRenderSystemLock : public IPeerRenderSystemLock
{
public:
    /// Constructor
    PeerOgreRenderSystemLock(IApplication* application) :
      mApplication(application)
      {}
    /// See IPeerRenderSystemLock::lock()
    virtual void lock() { mApplication->lock(); }
    /// See IPeerRenderSystemLock::unlock()
    virtual void unlock() { mApplication->unlock(); }
private:
    /// Application
    IApplication* mApplication;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "English");
    std::string title = "Solipsis - StandAlone Navigator";
    CommonTools::CrashReporter::getSingletonPtr()->initialize(title, CommonTools::IO::getCWD(), "SolipsisNavigator_minidump", "Latest log file saved:\n" + CommonTools::IO::getCWD() + "\\Ogre_XXXX_YYMMDDhhmm.log\n\nPlease send us files to solipsis.bugs@gmail.com");
    try
    {
#ifdef NULLCLIENTSERVER
        // Create the Peer application
#ifdef WIN32
        IPeer* peer = IPeer::createPeer("", __argc, __argv);
#else
        int argc = 1;
        static char* argv[] = {"Navigator.exe"};
        IPeer* peer = IPeer::createPeer("", argc, argv);
#endif 
        if (peer == 0)
            throw std::exception("Unable to create the peer !");
#endif

        // Create application
        IApplication* application = IApplication::createApplication("", true, title.c_str());
        if (application == 0)
            throw std::exception("Unable to create the navigator application !");

#ifdef NULLCLIENTSERVER
        // Initialize the Peer application
        PeerOgreRenderSystemLock peerOgreRenderSystemLock(application);
        bool initialized = peer->initialize(&peerOgreRenderSystemLock);
        if (!initialized)
            throw std::exception("Unable to initialize the peer !");
#endif

        std::string envVar;
        envVar = CONTAINER_NAME_ENV"=standalone";
        _putenv(envVar.c_str());
        envVar = NAVI_SUPPORT_ENV"=yes";
        _putenv(envVar.c_str());

        // Create instance
        IInstance* instance = application->createInstance();
        assert(instance != 0);

        // Create the internal automatic window
        instance->setWindow(0);

        // Run instance
        instance->run();

        // Destroy instance
        application->destroyInstance(instance);

#ifdef NULLCLIENTSERVER
        // Destroy the Peer application
        peer->destroy();
#endif

        // Destroy application
        application->destroy();
    }
    catch (std::exception& e)
    {
        std::string msg = "Unable to run the Navigator ...\n" + std::string(e.what());
#ifdef WIN32
        MessageBox(0, msg.c_str(), "An exception has occurred !", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occurred !\n" << msg.c_str() << std::endl;
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
