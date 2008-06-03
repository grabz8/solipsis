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

namespace SolipsisVoiceServer {

  //! A platform-independent socket API.
  class VoiceServerSocket {
  public:

    //! Creates a stream (TCP) socket. Returns -1 on failure.
    static int socket();

    //! Closes a socket.
    static void close(int socket);

    //! Sets a stream (TCP) socket to perform non-blocking IO. Returns false on failure.
    static bool setNonBlocking(int socket);

    //! Read binary datas from the specified socket. Returns read size.
    static int receive(int socket, char* buffer, int size);

    //! Write binary datas to the specified socket. Returns written size.
    static int send(int socket, const char* buffer, int size);


    // The next four methods are appropriate for servers.

    //! Allow the port the specified socket is bound to to be re-bound immediately so 
    //! server re-starts are not delayed. Returns false on failure.
    static bool setReuseAddr(int socket);

    //! Bind to a specified port
    static bool bind(int socket, int port);

    //! Set socket in listen mode
    static bool listen(int socket, int backlog);

    //! Accept a client connection request
    static int accept(int socket);

    //! Connect a socket to a server (from a client)
    static bool connect(int socket, std::string& host, int port);

    //! Returns last errno
    static int getError();

    //! Returns message corresponding to last error
    static std::string getErrorMsg();

    //! Returns message corresponding to error
    static std::string getErrorMsg(int error);
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerSocket_h__