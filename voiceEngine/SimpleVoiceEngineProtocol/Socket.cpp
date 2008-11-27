/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Guillaume Raffy (Proservia)

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

#include "Socket.h"
#if defined(_MSC_VER)
	#define snprintf	    _snprintf
#endif

#ifndef MAKEDEPEND

	#if defined(WIN32)
		#include <stdio.h>

		#include <winsock2.h>
		//# pragma lib(WS2_32.lib)

		#define EINPROGRESS	WSAEINPROGRESS
		#define EWOULDBLOCK	WSAEWOULDBLOCK
		#define ETIMEDOUT	    WSAETIMEDOUT
	#else
		extern "C"
		{
			#include <unistd.h>
			#include <stdio.h>
			#include <sys/types.h>
			#include <sys/socket.h>
			#include <netinet/in.h>
			#include <netdb.h>
			#include <errno.h>
			#include <fcntl.h>
		}
	#endif  // _WINDOWS

#endif // MAKEDEPEND

/**
	@brief	These errors are not considered fatal for an IO operation; the operation will be re-tried.
*/
static inline bool nonFatalError()
{
	int err = Socket::getLastError();
	return (err == EINPROGRESS || /*err == EAGAIN ||*/ err == EWOULDBLOCK /*|| err == EINTR*/);
}


Socket::Socket( void )
	:mSocketHandle( mgINVALID_SOCKET_HANDLE )
{
}

Socket::Socket( Handle socketHandle )
	:mSocketHandle( socketHandle )
{
}


Socket::~Socket()
{
}

bool Socket::create( void )
{
	#ifdef WIN32
		initialize();
	#endif
	mSocketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
	return (mSocketHandle != mgINVALID_SOCKET_HANDLE);
}

void Socket::close()
{
	if(mSocketHandle == mgINVALID_SOCKET_HANDLE)
		return; // nothing to do

	#if defined(WIN32)
		closesocket(mSocketHandle);
	#else
		::close(mSocketHandle);
	#endif // WIN32
	mSocketHandle = mgINVALID_SOCKET_HANDLE;
}

bool Socket::setNonBlocking( void )
{
	#if defined(WIN32)
		unsigned long flag = 1;
		return (ioctlsocket(mSocketHandle, FIONBIO, &flag) == 0);
	#else
		return (fcntl(mSocketHandle, F_SETFL, O_NONBLOCK) == 0);
	#endif // WIN32
}

int Socket::receive(char* buffer, int size)
{
	#if defined(WIN32)
		int n = ::recv(mSocketHandle, buffer, size, 0);
	#else
		int n = read(mSocketHandle, buffer, size);
	#endif
	return n;
}

int Socket::send(const char* buffer, int size)
{
	#if defined(WIN32)
		int n = ::send(mSocketHandle, buffer, size, 0);
	#else
		int n = write(mSocketHandle, buffer, size);
	#endif
	return n;
}

bool Socket::setReuseAddr()
{
	// Allow this port to be re-bound immediately so server re-starts are not delayed
	int sflag = 1;
	return (setsockopt(mSocketHandle, SOL_SOCKET, SO_REUSEADDR, (const char *)&sflag, sizeof(sflag)) == 0);
}

// Bind to a specified port
bool Socket::bind(int port)
{
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons((u_short) port);
	return (::bind(mSocketHandle, (struct sockaddr *)&saddr, sizeof(saddr)) == 0);
}

// Set socket in listen mode
bool Socket::listen(int backlog)
{
	return (::listen(mSocketHandle, backlog) == 0);
}

int Socket::accept(void)
{
	struct sockaddr_in addr;
	#if defined(WIN32)
		int
	#else
		socklen_t
	#endif
	addrlen = sizeof(addr);

	return (int) ::accept(mSocketHandle, (struct sockaddr*)&addr, &addrlen);
}

bool Socket::connect(std::string& hostName, int port)
{
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;

	struct hostent *hp = gethostbyname(hostName.c_str());
	if (hp == 0) return false;

	saddr.sin_family = hp->h_addrtype;
	memcpy(&saddr.sin_addr, hp->h_addr, hp->h_length);
	saddr.sin_port = htons((u_short) port);

	// For asynch operation, this will return EWOULDBLOCK (windows) or
	// EINPROGRESS (linux) and we just need to wait for the socket to be writable...
	int result = ::connect(mSocketHandle, (struct sockaddr *)&saddr, sizeof(saddr));
	return result == 0 || nonFatalError();
}

int Socket::getLastError( void )
{
	#if defined(WIN32)
		return WSAGetLastError();
	#else
		return errno;
	#endif
}

std::string Socket::getLastErrorMsg( void )
{
	return getErrorMsg(getLastError());
}

std::string Socket::getErrorMsg(int error)
{
	char err[60];
	snprintf(err,sizeof(err),"error %d", error);
	return std::string(err);
}

void Socket::initialize( void )
{
	#if defined(WIN32)
		static bool winSocketsAreAlreadyInitialized = false;
		if (! winSocketsAreAlreadyInitialized)
		{
			WORD wVersionRequested = MAKEWORD( 2, 0 );
			WSADATA wsaData;
			WSAStartup(wVersionRequested, &wsaData);
			winSocketsAreAlreadyInitialized = true;
		}
	#endif
}
