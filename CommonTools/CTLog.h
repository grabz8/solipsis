#ifndef __CTLog_h__
#define __CTLog_h__

namespace CommonTools {

/** This static class contains several helper methods above system.
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
    static void setLogHandler(LogHandler* logHandler) { ms_LogHandler = logHandler; }

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
