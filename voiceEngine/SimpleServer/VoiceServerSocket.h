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

#ifndef __VoiceServerSocket_h__
#define __VoiceServerSocket_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#include <Socket.h>

namespace SolipsisVoiceServer
{

  //! A platform-independent socket API.
	class VoiceServerSocket : public Socket
	{
	public:
		VoiceServerSocket( Socket::Handle socketHandle );

		typedef Socket super;	// superclass (parent class)
		//! @name Socket overriding
		//!	@{
			virtual void close( void );
			virtual	int receive(char* buffer, int size);
			virtual int send(const char* buffer, int size);
		//!	@}
#if 0
    //! Returns last errno
    static int getError();

    //! Returns message corresponding to last error
    static std::string getErrorMsg();

    //! Returns message corresponding to error
    static std::string getErrorMsg(int error);
#endif // 0
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerSocket_h__