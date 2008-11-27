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

#include "fmod.hpp"
#include "fmod_errors.h"

#include "FModSpeexVoipHandler.h"
#include <DummyPhonetizerPlugin.h>
#include <PhonetizerManager.h>

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int main(int argc, char* argv[])
{
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 30000
#define DEFAULT_VERBOSITY 5
    char host[256];
    int port;
    int verbosity;
    FMOD::System *system = 0;
    unsigned int version;
    FMOD_RESULT result;
    FModSpeexVoipHandler *voiceEngine = 0;
    Solipsis::EntityUID myAvatarId = "dummyAvatar";

    printf("Voice client test application\n");
    printf("=============================\n");
    printf("\n");

    strcpy_s(host, DEFAULT_HOST);
    port = DEFAULT_PORT;
    verbosity = DEFAULT_VERBOSITY;
    for (int iarg=1; iarg < argc; iarg++)
    {
        if ((_stricmp(argv[iarg], "/h") == 0) || (_stricmp(argv[iarg], "/H") == 0) || (_stricmp(argv[iarg], "?") == 0))
        {
            printf("Usage: TestClient -h <host> -p <port> -v <verbosity>\n");
            printf(" -p <host:str>      : host (default: localhost)\n");
            printf(" -p <port:int>      : port (default: 30000)\n");
            printf(" -v <verbosity:int> : verbosity level (0..2) (default: 5)\n");
            printf("\n");
            return -1;
        }
        if ((_stricmp(argv[iarg], "-h") == 0) && (argc > iarg+1))
        {
            iarg++;
            strcpy(host, argv[iarg]);
            continue;
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

    printf("Initializing FMOD system ...\n");
    printf("\n");

    // Create a System object and initialize.
    result = FMOD::System_Create(&system);
    ERRCHECK(result);
    result = system->getVersion(&version);
    ERRCHECK(result);
    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }
    result = system->init(1, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    printf("Initializing Phonetizer ...\n");
    printf("\n");

	new Solipsis::PhonetizerManager();
	Solipsis::DummyPhonetizerPlugin* pDummyPhonetizerPlugin = new Solipsis::DummyPhonetizerPlugin();
	pDummyPhonetizerPlugin->install();
	//Solipsis::PhonetizerManager::getSingleton().addPhonetizer(pDummyPhonetizer);
	Solipsis::PhonetizerManager::getSingleton().selectPhonetizer( "Dummy Phonetizer" );

    printf("Initializing Voice Engine ...\n");
    printf("\n");

    voiceEngine = new FModSpeexVoipHandler(system, NULL);
    if (voiceEngine == 0)
    {
        printf("Error!  Could not create the voice engine\n");
        return 0;
    }

    printf("Trying to connect to Voice server (%s:%d) ...\n", host, port);
    printf("\n");
    if (!voiceEngine->connect(host, port, myAvatarId))
    {
        printf("Error!  Could not connect to the Voice server (%s:%d) ...\n", host, port);
        return 0;
    }

    // Main loop until ESCAPE pressed
    printf("Entering main loop ...\n");
    printf(" press SPACE to start/stop speaking\n");
    printf(" press ESCAPE to quit\n");
    int key = 0;
    do
    {
        // check keyboard
        if (_kbhit())
        {
            key = _getch();
            if (key == ' ')
                if (voiceEngine->isRecording())
                    voiceEngine->stopRecording();
                else
                    voiceEngine->startRecording();
        }

        // update voice engine
        voiceEngine->update();

        // update FMOD
        result = system->update();
        ERRCHECK(result);

        // sleeping
        Sleep(10);
    } while (key != 27);

    printf("\n");
    printf("Disconnecting ...\n");

    voiceEngine->disconnect();

    printf("\n");
    printf("Shutting down.\n");

    // Shut down

    delete voiceEngine;
	pDummyPhonetizerPlugin->uninstall();
	delete pDummyPhonetizerPlugin;

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    return 0;
}
