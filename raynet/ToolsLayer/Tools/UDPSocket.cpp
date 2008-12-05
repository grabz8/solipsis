#include "UDPSocket.h"
#include "P2PTools.h"
#include "UDPPacket.h"


#include <assert.h>

namespace IP
{

	//----------SOCKET RUNTIME CONTROLLER-------- 
	// Windows require that winsock is started and stopped
	// statically.. meaning that the runtime lasts all the executables
	// time in memory.. and unloads afterwards
	DEFINE_EXCEPTION(SocketSubsystemError)

	void SocketInit()
	{

		WSADATA wsaData;  
		if (::WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
			THROW( SocketSubsystemError, Tools::SYSTEM_ERROR);		

	}

	void SocketCleanUp()
	{
		::WSACleanup();
	}

	struct SocketSystemRunner
	{
		SocketSystemRunner()
		{
			SocketInit();
		}
		~SocketSystemRunner()
		{
			SocketCleanUp();
		}
	};
	SocketSystemRunner getSockSubUp;
	//----------------------------------------------------






	template < class BufferType>
	void getSockOpt( SOCKET _socket, int _level, int _optName, BufferType& _buffer )
	{		
			int sizeBuffer = sizeof(_buffer);
			if ( ::getsockopt( _socket, _level, _optName, (char*)&_buffer, &sizeBuffer ) != 0  )
			{
				int p = ::WSAGetLastError();
				THROW(UDPSocketOptionError, Tools::SYSTEM_ERROR);
			}
	}






	UDPSocket::UDPSocket():
		m_h_socket( INVALID_SOCKET ),
		m_us_localBindPort(0)		
	{
		// create socket	
		SafeSocket temp  = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP  );

		//ensure that the package buffer is sufficient
		int buff = 0;
		getSockOpt( temp,  SOL_SOCKET, SO_MAX_MSG_SIZE, buff );

		m_i_MaxKernelDatagramSize = buff;	

		m_h_socket = temp.release();
	
	}
	//---------------------

	UDPSocket::~UDPSocket()
	{
		::closesocket( m_h_socket );
	}
	//---------------------
	void UDPSocket::send( IP::Port _p, IP::Addr _a, const void* _pdata, ushort _size) 
	{
		UDPSocket o_socket;
		

		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address of
		// the receiver (in this example case "123.456.789.1")
		// and the specified port number.
		sockaddr_in RecvAddr;
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port =   hton( _p );
		RecvAddr.sin_addr.s_addr = _a;


		int ret = ::sendto(	o_socket.m_h_socket, 
			(const char*)_pdata, 
			_size, 
			0, 
			(SOCKADDR *) &RecvAddr, 
			sizeof(RecvAddr));

		if ( ret == SOCKET_ERROR )
		{
			THROW(UDPSocketSendError, Tools::NETWORK_ERROR);
		}
	}
	//---------------------

	uint  UDPSocket::recv(IP::Port& _port, IP::Addr& _addr, void* _buf, ushort _bufsize) 
	{
		//clearly we cannot recv packages if NOT bound
		assert( m_us_localBindPort != 0);		
		assert( MAX_UDP_PACKET_SIZE > _bufsize);

		sockaddr_in RemoteAddr;
		int remAddrSize = sizeof( sockaddr_in);
		uint ret  =::recvfrom(	m_h_socket, 
			(char*)_buf, 
			_bufsize , 
			0/*flags*/, 
			(sockaddr*) &RemoteAddr, 
			&remAddrSize
			);

		if ( sizeof( sockaddr_in) != remAddrSize )
		{
			THROW(UDPSocketRecvError, "Wrong size of RemoteAddr structure" ) ;
		}

		if ( ret == SOCKET_ERROR  )
		{		
			THROW(UDPSocketRecvError, Tools::NETWORK_ERROR ) ;
		}

		_addr = RemoteAddr.sin_addr.S_un.S_addr ;
		_port = RemoteAddr.sin_port ;


		return ret;
	}
	//---------------------
	void UDPSocket::bind(   unsigned short  _port)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = ::htonl( INADDR_ANY );
		service.sin_port = hton(_port);

		if ( ::bind( m_h_socket, (SOCKADDR*)&service, sizeof(service) ) == SOCKET_ERROR  )
		{
			THROW(UDPSocketBindError, Tools::NETWORK_ERROR);
		}
		m_us_localBindPort = _port;
	}
	//---------------------
	int UDPSocket::getBindPort()
	{
		return m_us_localBindPort;
	}
	//---------------------
	SOCKET UDPSocket::getHandle()
	{
		return m_h_socket;
	}
	//---------------------


}




