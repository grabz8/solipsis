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

#include "RakNetServer.h"
#include <CTBasicThread.h>
#include <CTLog.h>
#include <CTSystem.h>
#include <CTStringHelpers.h>

using namespace CommonTools;
using namespace Solipsis;

class ConsoleThread : public BasicThread
{
protected:
    RakNetServer *mRakNetServer;

public:
    /// constructor, specify the name
    ConsoleThread(RakNetServer* rakNetServer) :
      BasicThread("ConsoleThread"),
      mRakNetServer(rakNetServer) {}

protected:
    virtual void run()
    {
        LOGHANDLER_LOGF(LogHandler::VL_INFO, "ConsoleThread::run()");

        while (!mStopRequested)
        {
            // Keyboard
            char c[256];
            fscanf(stdin, "%s", c);
            if ((*c == 'q') || (*c == 'Q'))
            {
                LOGHANDLER_LOGF(LogHandler::VL_INFO, "Quitting ...");
                stop();
            }
        }
    }

    virtual void end()
    {
        mRakNetServer->quit();
    }
};

/// RakNet server
RakNetServer *rakNetServer = 0;

/// Console thread
ConsoleThread *consoleThread = 0;

#ifdef WIN32
/// Console handler to catch close event from the console window
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    switch(CEvent)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        if (rakNetServer != 0)
            rakNetServer->quit();
        // Waiting server is shutting down
        System::sleep(1000);
        break;
    }
    return TRUE;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
int main(int argc, char** argv)
#else
int main(int argc, char *argv[])
#endif
{
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "English");

//    LogHandler::getLogHandler()->setLogFilename("RakNetServer_" + StringHelpers::toString(System::getPID()) + "_"  + System::getDateTimeYYYYMMDDHHMMSS() + ".log");
    LogHandler::getLogHandler()->setLogFilename("RakNetServer.log");
    LogHandler::getLogHandler()->setVerbosityLevel(LogHandler::VL_DEBUG);
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Starting RakNet server");

#ifdef WIN32
    // Install the console handler
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
#endif

    rakNetServer = new RakNetServer(argc, argv);

    consoleThread = new ConsoleThread(rakNetServer);
    consoleThread->start();

    rakNetServer->initialize();
    rakNetServer->run();
    delete rakNetServer;
    rakNetServer = 0;

    consoleThread->stop(0);
    delete consoleThread;
    consoleThread = 0;

    return 0;
}

#ifdef __cplusplus
}
#endif
