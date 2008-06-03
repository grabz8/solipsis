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
#include "CTScopedMutexLock.h"

#include <iostream>
#include <stdarg.h>

#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace CommonTools {

// Default log handler
static class DefaultLogHandler : public LogHandler
{
public:
    void log(int level, const char* msg)
    { 
#ifdef USE_WINDOWS_DEBUG
        if (level <= ms_Verbosity) { OutputDebugString(msg); OutputDebugString("\n"); }
#else
        if (level <= ms_Verbosity) std::cout << msg << std::endl; 
#endif  
    }
} defaultLogHandler;

// Message log singleton
LogHandler* LogHandler::ms_LogHandler = &defaultLogHandler;

// Default verbosity level
LogHandler::VerbosityLevel LogHandler::ms_Verbosity = VL_NONE;

// Variable args mutex
pthread_mutex_t ms_LogMutex(PTHREAD_MUTEX_INITIALIZER);

//-------------------------------------------------------------------------------------
void LogHandler::logf(int level, const char* fmt, ...)
{
    if (level <= LogHandler::getVerbosityLevel())
    {
        char buf[1024];
        {
            ScopedMutexLock lock(ms_LogMutex);
            va_list va;
            va_start(va, fmt);
            vsnprintf(buf, sizeof(buf)-1, fmt, va);
            buf[sizeof(buf) - 1] = 0;
        }
        LogHandler::getLogHandler()->log(level, buf);
    }
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
