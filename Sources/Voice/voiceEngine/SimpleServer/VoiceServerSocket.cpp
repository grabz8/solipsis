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

#include "VoiceServerSocket.h"
#include "VoiceServerUtil.h"

#ifndef MAKEDEPEND

#if defined(_WINDOWS)
# include <stdio.h>

# include <winsock2.h>
//# pragma lib(WS2_32.lib)

# define EINPROGRESS	WSAEINPROGRESS
# define EWOULDBLOCK	WSAEWOULDBLOCK
# define ETIMEDOUT	    WSAETIMEDOUT

#else
extern "C" {
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <errno.h>
# include <fcntl.h>
}
#endif  // _WINDOWS

#endif // MAKEDEPEND

namespace SolipsisVoiceServer {

#if 0
#if defined(_WINDOWS)
  
static void initWinSock()
{
  static bool wsInit = false;
  if (! wsInit)
  {
    WORD wVersionRequested = MAKEWORD( 2, 0 );
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    wsInit = true;
  }
}

#else

#define initWinSock()

#endif // _WINDOWS

// These errors are not considered fatal for an IO operation; the operation will be re-tried.

static inline bool
nonFatalError()

{

  int err = VoiceServerSocket::getError();

  return (err == EINPROGRESS || /*err == EAGAIN ||*/ err == EWOULDBLOCK /*|| err == EINTR*/);

}
#endif //0

VoiceServerSocket::VoiceServerSocket( Socket::Handle socketHandle )
	: super( socketHandle )
{
}

void VoiceServerSocket::close( void )
{
  VoiceServerUtil::log(4, "VoiceServerSocket::close: fd %d.", mSocketHandle);
  super::close();
}

int VoiceServerSocket::receive(char* buffer, int size)
{
	int numBytesReceived = super::receive( buffer, size );
	VoiceServerUtil::log(5, "VoiceServerSocket::nbRead: read/recv returned %d.", numBytesReceived);
	return numBytesReceived;
}

int VoiceServerSocket::send(const char* buffer, int size)
{
	int numBytesSent = super::send( buffer, size );
	VoiceServerUtil::log(5, "VoiceServerSocket::nbWrite: send/write returned %d.", numBytesSent);
	return numBytesSent;
}
  
#if 0
// Returns last errno
int VoiceServerSocket::getError()
{
#if defined(_WINDOWS)
  return WSAGetLastError();
#else
  return errno;
#endif
}

// Returns message corresponding to last errno
std::string VoiceServerSocket::getErrorMsg()
{
  return getErrorMsg(getError());
}

// Returns message corresponding to errno... well, it should anyway
std::string VoiceServerSocket::getErrorMsg(int error)
{
  char err[60];
  snprintf(err,sizeof(err),"error %d", error);
  return std::string(err);
}
#endif 0
} // namespace SolipsisVoiceServer
