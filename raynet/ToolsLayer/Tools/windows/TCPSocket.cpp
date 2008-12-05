#include "..\TCPSocket.h"


#include "..\P2PTools.h"

#include <OSDependentTools.h>



namespace IP{

	









	template < class BufferType>
	 inline void getSockOpt( SOCKET _socket, int _level, int _optName, BufferType& _buffer )
		{		
			int sizeBuffer = sizeof(_buffer);
			if ( ::getsockopt( _socket, _level, _optName, (char*)&_buffer, &sizeBuffer ) != 0  )
			{				
				THROW(TCPSocketOptionError, Tools::NETWORK_ERROR);
			}
		}

	template < class BufferType>
	inline void setSockOpt( SOCKET _socket, int _level, int _optName, BufferType& _buffer )
	{		
			int sizeBuffer = sizeof(_buffer);
			if ( ::getsockopt( _socket, _level, _optName, (char*)&_buffer, &sizeBuffer ) != 0  )
			{				
				THROW(TCPSocketOptionError, Tools::NETWORK_ERROR);
			}
	}
















	TCPSocket::TCPSocket(	SOCKET _socket,  BlockingMode _eMode	)
		:
		m_h_socket( _socket),	
		m_bReleased( false)
	{	
		// socket defaults to non-blocking
		_setBlockingMode(_eMode);
	}

	//----------
	void TCPSocket::_setBlockingMode( BlockingMode _eMode)
	{
		const int BLOCKING = 0;
		const int NON_BLOCKING = 1;
		u_long  blockMode = BLOCKING;

		if ( _eMode == eAsync)
		{
			blockMode = NON_BLOCKING;
		}

		if ( ::ioctlsocket(m_h_socket, FIONBIO, &blockMode ) == SOCKET_ERROR )
		{
			THROW(IoctlSocketError, Tools::NETWORK_ERROR);
		}
	
	}

	//----------

	TCPSocket::TCPSocket(	unsigned short _port, 
							unsigned int _addr , 
							enum BlockingMode _eMode,
							unsigned int _timeout )
		: 
		m_h_socket( INVALID_SOCKET),		
		m_bReleased( false)
	
	{

		SafeSocket sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in servaddr;
		::memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = IP::hton(_port);
		servaddr.sin_addr.S_un.S_addr = _addr;
		
		
		_connect( sock,  servaddr, _eMode, _timeout  );
		

		m_h_socket = sock.release();
	}
	//----------
	TCPSocket::~TCPSocket(void)
	{
		if ( !m_bReleased)
		{
			::closesocket(m_h_socket );
		}
	}

	//----------
	uint TCPSocket::send( const void* _pBuffer, uint _MaxBufSize )
	{
	
		uint ret =	::send(  m_h_socket, (const char *)_pBuffer,  _MaxBufSize, 0);

	
		if ( ret == SOCKET_ERROR )
		{
			THROW(TCPSocketSendError, Tools::NETWORK_ERROR);		
		}
		return ret;
	}
	//----------
	uint TCPSocket::recv( void* _pBuffer, uint _MaxBufSize )
	{
		uint ret =	::recv(  m_h_socket, ( char *)_pBuffer,  _MaxBufSize, 0);
	
		if ( ret == SOCKET_ERROR )
		{
			THROW(TCPSocketRecvError, Tools::NETWORK_ERROR);		
		}
		return ret;
	}

	//----------
	SOCKET TCPSocket::release()
	{
		m_bReleased = true;
		return m_h_socket;
	}
	//----------
	void TCPSocket::_connect(	SOCKET _sock, 
								const sockaddr_in& _servaddr,
								enum BlockingMode _eDesiredMode,
								unsigned int _timeout )
	{

		// Whatever the Mode... start in non-blocking for connections	
		_setBlockingMode(eAsync );		

		//start connection
		if(  ::connect(_sock, (const sockaddr*) &_servaddr, sizeof(_servaddr) )  < 0 /*returns error*/  )
		{
			if (  ::WSAGetLastError() != WSAEWOULDBLOCK )
			{
				//simply failed
				THROW(TCPSocketConnectError, Tools::NETWORK_ERROR);
			}			
		}



		if ( _eDesiredMode == eSync)
		{ // must wait until connection has finished.. here goes nothing
			
			//wait for result
			fd_set rset, wset;
			FD_ZERO(&rset);
			FD_SET( _sock, &rset);
			wset = rset;
			timeval tval;
			tval.tv_sec = _timeout;
			tval.tv_usec = 0;

			unsigned int selectRet = ::select(0, &rset, &wset, NULL,&tval );
				
			if (   selectRet == SOCKET_ERROR) 
			{	//Nothing to read / write.. therefore timeout
				THROW(TCPSelectError, Tools::NETWORK_ERROR );
			}
			else if ( selectRet == 0)
			{
				THROW_WITH_DATA(TCPSelectError, "Timed out after:" << _timeout << " secs" );
			}
		
			// back to ....  blocking mode for later transfers
			_setBlockingMode(eSync);
		
		}	
	}
	//----------
	bool TCPSocket::isConnected()
	{		
		sockaddr_in whoCares;
		int whoCareAboutThisToo = sizeof(whoCares);
		return ::getpeername(m_h_socket, (sockaddr *)&whoCares, &whoCareAboutThisToo ) != SOCKET_ERROR;
	}

}