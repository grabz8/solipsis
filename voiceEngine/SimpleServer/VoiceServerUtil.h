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