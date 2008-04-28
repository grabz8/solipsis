#ifndef __VoiceServerUtil_h__
#define __VoiceServerUtil_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#if defined(_MSC_VER)
# define snprintf	    _snprintf
# define vsnprintf    _vsnprintf
# define strcasecmp	  _stricmp
# define strncasecmp	_strnicmp
#elif defined(__BORLANDC__)
# define strcasecmp stricmp
# define strncasecmp strnicmp
#endif

namespace SolipsisVoiceServer {

  //! Utilities for XML parsing, encoding, and decoding and message handlers.
  class VoiceServerUtil {
  public:
    //! Dump messages somewhere
    static void log(int level, const char* fmt, ...);

    //! Dump error messages somewhere
    static void error(const char* fmt, ...);

    //! Sleep
    static void sleep(unsigned long durationMs);
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerUtil_h__