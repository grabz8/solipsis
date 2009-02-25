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

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "VoiceServer.h"

#include "CTBasicThread.h"

using namespace SolipsisVoiceServer;

class VoiceServerMainThread : public CommonTools::BasicThread, VoiceServerErrorHandler, public VoiceServerLogHandler {
public:

	//! Constructor
    VoiceServerMainThread(int port, int verbosity);

    /// See SolipsisVoiceServer::VoiceServerErrorHandler
    virtual void error(const char* msg) { printf("VoiceServerMainThread::error() %s\n", msg); }
    /// See SolipsisVoiceServer::VoiceServerLogHandler
    virtual void log(int level, const char* msg) { printf("VoiceServerMainThread::log(%d) %s\n", level, msg); }

private:
    /** See BasicThread. */
    void run();

protected:
    // The port to bind/listen
    int mPort;

    // The verbosity level
    int mVerbosity;

    // The voice server
    VoiceServer mVoiceServer;
};

VoiceServerMainThread::VoiceServerMainThread(int port, int verbosity) :
    BasicThread("TestServer"),
    mPort(port),
    mVerbosity(verbosity),
    mVoiceServer()
{
    printf("VoiceServerMainThread::VoiceServerMainThread()\n");

    VoiceServerErrorHandler::setErrorHandler(this);
    VoiceServerLogHandler::setLogHandler(this);
    setVerbosity(mVerbosity);
}

void VoiceServerMainThread::run()
{
    printf("VoiceServerMainThread::run() running ...\n");

    printf("Binding and listening (port:%d) ...\n", mPort);
    printf("\n");
    if (!mVoiceServer.bindAndListen(mPort))
    {
        printf("Error!  Could not bind/listen (port:%d) ...\n", mPort);
        return;
    }

    while (!mStopRequested)
        mVoiceServer.work(1.0);

    printf("VoiceServerMainThread::run() end\n");
}

int main(int argc, char* argv[])
{
    VoiceServerMainThread* voiceServerMainThread = 0;
    #define DEFAULT_PORT 30000
    #define DEFAULT_VERBOSITY 5
    int port;
    int verbosity;

    printf("Voice server test application\n");
    printf("=============================\n");
    printf("\n");

    port = DEFAULT_PORT;
    verbosity = DEFAULT_VERBOSITY;
    for (int iarg=1; iarg < argc; iarg++)
    {
        if ((_stricmp(argv[iarg], "/h") == 0) || (_stricmp(argv[iarg], "/H") == 0) || (_stricmp(argv[iarg], "-h") == 0) || (_stricmp(argv[iarg], "-H") == 0) || (_stricmp(argv[iarg], "?") == 0))
        {
            printf("Usage: TestServer -p <port> -v <verbosity>\n");
            printf(" -p <port:int>      : port (default: 30000)\n");
            printf(" -v <verbosity:int> : verbosity level (0..5) (default: 5)\n");
            printf("\n");
            return -1;
        }
        if ((_stricmp(argv[iarg], "-p") == 0) && (argc > iarg+1))
        {
            iarg++;
            port = atoi(argv[iarg]);
            continue;
        }
        if ((_stricmp(argv[iarg], "-v") == 0) && (argc > iarg+1))
        {
            iarg++;
            verbosity = atoi(argv[iarg]);
            continue;
        }
    }

    printf("Initializing Voice Server ...\n");
    printf("\n");

    voiceServerMainThread = new VoiceServerMainThread(port, verbosity);
    if (voiceServerMainThread == 0)
    {
        printf("Error!  Could not create the voice server\n");
        return 0;
    }
    voiceServerMainThread->start();

    // Main loop until ESCAPE pressed
    printf("Entering main loop ...\n");
    printf(" press ESCAPE to quit\n");
    int key = 0;
    do
    {
        // check keyboard
        if (_kbhit())
        {
            key = _getch();
        }

        // sleeping
        Sleep(10);
    } while (key != 27);

    printf("\n");
    printf("Shutting down.\n");

    // Shut down
    voiceServerMainThread->stop();
    voiceServerMainThread->finalize();
    delete voiceServerMainThread;

    return 0;
}
