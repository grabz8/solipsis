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

#ifndef __Socket_h__
#define __Socket_h__

#include <string>

/**
	@brief	a socket class that is system independent
*/
class Socket
{
public:
	#ifdef WIN32
		typedef __w64 unsigned int Handle; // socket handle
	#else
		typedef int Handle; // socket handle
	#endif
	static const int mgINVALID_SOCKET_HANDLE = -1;
	/**
		@brief	constructor

		@note	the socket is not created after this constructor
	*/
    Socket( void );
    Socket( Handle socketHandle );
	virtual ~Socket();

	/**
		@brief	returns the socket handle
	*/
	Handle getHandle( void ) const { return mSocketHandle; }

	/**
		@brief	checks if the socket handle is valid
	*/
	bool isValid( void ) const { return mSocketHandle != mgINVALID_SOCKET_HANDLE; }

    /**
		@brief	Creates a stream (TCP) socket.
		
		Returns true on success, false otherwise.
	*/
    virtual bool create( void );

    /**
		@brief	Closes the socket.
	*/
    virtual void close( void );

    /**
		@brief	Sets a the socket to perform non-blocking IO.
		
		@return	false on failure.
	*/
    bool setNonBlocking( void );

    /**
		@brief	Read binary datas from the specified socket.
		
		@return	number of bytes read
	*/
    virtual int receive(char* buffer, int size);

    /**
		@brief	Write binary datas to the specified socket.

		@return The number of bytes written.
	*/
    virtual int send(const char* buffer, int size);

    //! @name methods that are appropriate for servers.
	//! @{
		//! Allow the port the specified socket is bound to to be re-bound immediately so 
		//! server re-starts are not delayed. Returns false on failure.
		bool setReuseAddr( void );

		//! Bind to a specified port
		bool bind(int port);

		//! Set socket in listen mode
		bool listen(int backlog);

		//! Accept a client connection request
		int accept( void );
	//! @}

    /**
		@brief	Connect a socket to a server (from a client)

		@return	true on success, false otherwise
	*/
    virtual bool connect(std::string& hostName, int port);

    /**
		@brief	Returns last errno
	*/
    static int getLastError(void);

    //! Returns message corresponding to last error
    static std::string getLastErrorMsg( void );

    //! Returns message corresponding to error
    static std::string getErrorMsg(int error);

	/**
		@brief	initializes the socket system
	*/
	static void initialize( void );

protected:
	Handle	mSocketHandle;
};

#endif	//	__Socket_h__
