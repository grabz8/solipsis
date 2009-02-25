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

#ifndef __VoiceServerSource_h__
#define __VoiceServerSource_h__

#include <Socket.h>

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

namespace SolipsisVoiceServer {

  //! An RPC source represents a file descriptor to monitor
  class VoiceServerSource {
  public:
    //! Constructor
    //!  @param fd The socket file descriptor to monitor.
    //!  @param deleteOnClose If true, the object deletes itself when close is called.
    VoiceServerSource(int fd = -1, bool deleteOnClose = false);

    //! Destructor
    virtual ~VoiceServerSource();

    //! Return the file descriptor being monitored.
    const Socket & getSocket() const { return mSocket; }
    Socket & getSocket() { return mSocket; }
    //! Specify the file descriptor to monitor.
//    void setfd(int fd) { _fd = fd; }

    //! Close the owned fd. If deleteOnClose was specified at construction, the object is deleted.
    virtual void close();

    //! Return true to continue monitoring this source
    virtual unsigned handleEvent(unsigned eventType) = 0;

  protected:

    // Socket. This should really be a SOCKET (an alias for unsigned int*) on windows...
	Socket	mSocket;

  private:
    // In the server, a new source (VoiceServerConnection) is created
    // for each connected client. When each connection is closed, the
    // corresponding source object is deleted.
    bool _deleteOnClose;
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerSource_h__