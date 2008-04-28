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






int VoiceServerSocket::socket()
{
  initWinSock();
  return (int) ::socket(AF_INET, SOCK_STREAM, 0);
}

void VoiceServerSocket::close(int fd)
{
  VoiceServerUtil::log(4, "VoiceServerSocket::close: fd %d.", fd);
#if defined(_WINDOWS)
  closesocket(fd);
#else
  ::close(fd);
#endif // _WINDOWS
}

bool VoiceServerSocket::setNonBlocking(int fd)
{
#if defined(_WINDOWS)
  unsigned long flag = 1;
  return (ioctlsocket((SOCKET)fd, FIONBIO, &flag) == 0);
#else
  return (fcntl(fd, F_SETFL, O_NONBLOCK) == 0);
#endif // _WINDOWS
}

bool VoiceServerSocket::setReuseAddr(int fd)
{
  // Allow this port to be re-bound immediately so server re-starts are not delayed
  int sflag = 1;
  return (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&sflag, sizeof(sflag)) == 0);
}

// Bind to a specified port
bool VoiceServerSocket::bind(int fd, int port)
{
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons((u_short) port);
  return (::bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == 0);
}

// Set socket in listen mode
bool VoiceServerSocket::listen(int fd, int backlog)
{
  return (::listen(fd, backlog) == 0);
}

int VoiceServerSocket::accept(int fd)
{
  struct sockaddr_in addr;
#if defined(_WINDOWS)
  int
#else
  socklen_t
#endif
    addrlen = sizeof(addr);

  return (int) ::accept(fd, (struct sockaddr*)&addr, &addrlen);
}
  
// Connect a socket to a server (from a client)
bool VoiceServerSocket::connect(int fd, std::string& host, int port)
{
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;

  struct hostent *hp = gethostbyname(host.c_str());
  if (hp == 0) return false;

  saddr.sin_family = hp->h_addrtype;
  memcpy(&saddr.sin_addr, hp->h_addr, hp->h_length);
  saddr.sin_port = htons((u_short) port);

  // For asynch operation, this will return EWOULDBLOCK (windows) or
  // EINPROGRESS (linux) and we just need to wait for the socket to be writable...
  int result = ::connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
  return result == 0 || nonFatalError();
}

// Read available text from the specified socket. Returns false on error.
int VoiceServerSocket::receive(int fd, char* buffer, int size)
{
#if defined(_WINDOWS)
  int n = ::recv(fd, buffer, size, 0);
#else
  int n = read(fd, buffer, size);
#endif
  VoiceServerUtil::log(5, "VoiceServerSocket::nbRead: read/recv returned %d.", n);
  return n;
}

// Write text to the specified socket. Returns false on error.
int VoiceServerSocket::send(int fd, const char* buffer, int size)
{
#if defined(_WINDOWS)
  int n = ::send(fd, buffer, size, 0);
#else
  int n = write(fd, buffer, size);
#endif
  VoiceServerUtil::log(5, "VoiceServerSocket::nbWrite: send/write returned %d.", n);
  return n;
}

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

} // namespace SolipsisVoiceServer
