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
