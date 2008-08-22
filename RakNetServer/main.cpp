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

using namespace Solipsis;

class ConsoleThread : public CommonTools::BasicThread
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
        printf("ConsoleThread::run()\n");

        while (!mStopRequested)
        {
            // Keyboard
            char c[256];
            fscanf(stdin, "%s", c);
            if ((*c == 'q') || (*c == 'Q'))
            {
                printf("Quitting.\n");
                stop();
            }
        }
    }

    virtual void end()
    {
        mRakNetServer->quit();
    }
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
int main(int argc, char** argv)
#else
int main(int argc, char *argv[])
#endif
{
    RakNetServer rakNetServer(argc, argv);

    ConsoleThread consoleThread(&rakNetServer);
    consoleThread.start();

    rakNetServer.initialize();
    rakNetServer.run();
    rakNetServer.finalize();

    return 0;
}

#ifdef __cplusplus
}
#endif
