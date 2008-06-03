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

public:
    /// Returns a pointer to the currently installed log handler
    static LogHandler* getLogHandler() { return ms_LogHandler; }

    /// Set the log handler
    static void setLogHandler(LogHandler* logHandler) { if (logHandler != 0) ms_LogHandler = logHandler; }

    /// Returns the current verbosity level
    static VerbosityLevel getVerbosityLevel() { return ms_Verbosity; }

    /// Set the verbosity level
    static void setVerbosityLevel(VerbosityLevel verbosityLevel) { ms_Verbosity = verbosityLevel; }

    /// Log method to define
    virtual void log(int level, const char* msg) = 0;

    /// Helper log method
    static void logf(int level, const char* fmt, ...);

protected:
    /// Log handler
    static LogHandler* ms_LogHandler;

    /// Verbosity level
    static VerbosityLevel ms_Verbosity;
};

} // namespace CommonTools

#endif // #ifndef __CTLog_h__
