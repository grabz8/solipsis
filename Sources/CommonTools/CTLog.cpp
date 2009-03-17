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

#include "CTLog.h"

#include <iostream>
#include <stdarg.h>
#include <time.h>

#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace CommonTools {

const char* LogHandler::ms_TrueStr = "true";
const char* LogHandler::ms_FalseStr = "false";

// Default log handler
static class DefaultLogHandler : public LogHandler
{
protected:
    /// Log file
    FILE* mLogFile;

public:
    DefaultLogHandler() :
        mLogFile(0)
    {
    }

    DefaultLogHandler::~DefaultLogHandler()
    {
        if (mLogFile != 0)
            fclose(mLogFile);
    }

    void setLogFilename(const std::string& filename)
    {
        ScopedMutexLock lock(mMutex);
        LogHandler::setLogFilename(filename);
        if (mLogFile != 0)
            fclose(mLogFile);
        mLogFile = fopen(mLogFilename.c_str(), "w");
    }

    void log(int level, const char* msg)
    { 
        if (level > mVerbosity) return;

        char timeBuf[16];
        _strtime_s(timeBuf, 16);
        char log[256];
        _snprintf(log, sizeof(log) - 2, "%s: %s\n", timeBuf, msg);
        log[sizeof(log) - 2] = '\n';
        log[sizeof(log) - 1] = '\0';

#if defined(USE_WINDOWS_DEBUG) || (defined(_WINDOWS) && defined(_DEBUG))
        OutputDebugString(log);
#endif
        std::cout << log; 

        if (mLogFile != 0)
        {
            fwrite(log, 1 , strlen(log), mLogFile);
            fflush(mLogFile);
        }
    }
} defaultLogHandler;

// Singleton
LogHandler* LogHandler::ms_LogHandler = &defaultLogHandler;

//-------------------------------------------------------------------------------------
LogHandler::LogHandler() :
    mMutex(PTHREAD_MUTEX_INITIALIZER),
    mVerbosity(VL_NONE),
    mLogFilename("")
{
}

//-------------------------------------------------------------------------------------
LogHandler::~LogHandler()
{
    if (ms_LogHandler == this)
        ms_LogHandler = &defaultLogHandler;
}

//-------------------------------------------------------------------------------------
std::string LogHandler::getLogFilename()
{
    ScopedMutexLock lock(mMutex);
    return mLogFilename;
}

//-------------------------------------------------------------------------------------
void LogHandler::setLogFilename(const std::string& filename)
{
    mLogFilename = filename;
}

//-------------------------------------------------------------------------------------
void LogHandler::logf(int level, const char* fmt, ...)
{
    char buf[256];
    {
        ScopedMutexLock lock(mMutex);
        va_list va;
        va_start(va, fmt);
        _vsnprintf(buf, sizeof(buf)-1, fmt, va);
        buf[sizeof(buf) - 1] = 0;
    }
    log(level, buf);
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
