#include "..\TCPServerSocket.h"


namespace IP
{
/*

//------------Helper class--------------//

	class SafeSocket
	{
		SOCKET m_handle;
		bool m_bReleased;


	public:
		SafeSocket( SOCKET _handle)
			:m_handle( _handle ), m_bReleased( false)
		{
			if( m_handle == INVALID_SOCKET )
			{	    	
				THROW(TCPSocketCreationError,Tools::NETWORK_ERROR);
			}

		}
		//----------
		~SafeSocket()
		{
			if ( ! m_bReleased)
				::closesocket( m_handle);
		}
		//----------
		SOCKET release()
		{
			m_bReleased = true;
			return m_handle;
		}
		//----------
		operator SOCKET(){ return m_handle;}

		//----------


*/

TCPServerSocket::TCPServerSocket(IP::Port _port)
{
	IP::SafeSocket tmp = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = ::htonl( INADDR_ANY );
	service.sin_port = hton(_port);

	if ( ::bind( m_h_socket, (SOCKADDR*)&service, sizeof(service) ) == SOCKET_ERROR  )
	{
		THROW(TCPSocketBindError, Tools::NETWORK_ERROR);
	}

	if ( ::listen(  tmp,  SOMAXCONN) == SOCKET_ERROR )
	{
		THROW(TcpListenError, Tools::NETWORK_ERROR );
	}


	m_h_socket = tmp.release();
}


TCPServerSocket::~TCPServerSocket(void)
{
	::closesocket( m_h_socket);
}


SOCKET TCPServerSocket::accept()
{
	//sockaddr_in peer;
	//uint sizeOfsockaddr_in = sizeof( sockaddr_in);

	SOCKET sock = ::accept(	m_h_socket, NULL, NULL);

	if ( sock == INVALID_SOCKET )
	{
		THROW(TcpAcceptError, Tools::NETWORK_ERROR);
	}

	return sock;
	
}


}