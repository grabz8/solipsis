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

#ifndef __CTLog_h__
#define __CTLog_h__

#include <string>
#include "CTScopedMutexLock.h"

namespace CommonTools {

/** This static class contains several helper methods for logging.
 */
class LogHandler
{
public:
    /// Verbosity levels
    enum VerbosityLevel
    {
        VL_NONE = 0,
        VL_CRITICAL,
        VL_ERROR,
        VL_WARNING,
        VL_INFO,
        VL_DEBUG,
        VL_COUNT
    };
    /// True string
    static const char* ms_TrueStr;
    /// False string
    static const char* ms_FalseStr;

protected:
    /// Verbosity levels strings
    static const char* ms_VerbosityLevelsStr[];

protected:
    /// Current log handler
    static LogHandler* ms_LogHandler;

    /// Mutex
    pthread_mutex_t mMutex;

    /// Verbosity level
    VerbosityLevel mVerbosity;

    /// Log filename
    std::string mLogFilename;

public:
    /// Constructor
    LogHandler();
    /// Destructor
    ~LogHandler();

    /// Returns a pointer to the currently installed log handler
    static LogHandler* getLogHandler() { return ms_LogHandler; }
    /// Set the log handler
    static void setLogHandler(LogHandler* logHandler) { if (logHandler != 0) ms_LogHandler = logHandler; }

    /// Returns the current verbosity level
    VerbosityLevel getVerbosityLevel() { return mVerbosity; }
    /// Set the verbosity level
    void setVerbosityLevel(VerbosityLevel verbosityLevel) { mVerbosity = verbosityLevel; }
    const char* getVerbosityLevelStr(VerbosityLevel verbosityLevel);

    /// Returns the current log filename
    std::string getLogFilename();
    /// Set the log filename
    virtual void setLogFilename(const std::string& filename);

    /// Log method to define
    virtual void log(VerbosityLevel level, const char* msg) = 0;
    /// Log method to define
    virtual void log(VerbosityLevel level, const std::string& msg);
    /// Log method with variable arguments
    void logf(VerbosityLevel level, const char* fmt, ...);
};

#define LOGHANDLER_LOGBOOL(boolean) (boolean ? CommonTools::LogHandler::ms_TrueStr : CommonTools::LogHandler::ms_FalseStr)

#define LOGHANDLER_LOG(level, msg) CommonTools::LogHandler::getLogHandler()->log(level, msg)
#define LOGHANDLER_LOGF(level, fmt, ...) CommonTools::LogHandler::getLogHandler()->logf(level, fmt, __VA_ARGS__)

} // namespace CommonTools

#endif // #ifndef __CTLog_h__
