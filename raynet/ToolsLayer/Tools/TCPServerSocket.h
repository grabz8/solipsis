#ifndef __TCPServerSocket_HEADER_H__
#define __TCPServerSocket_HEADER_H__

#include "P2PTools.h"
#include "TCPSocket.h"

namespace IP
{
	DEFINE_EXCEPTION(TcpListenError)
	DEFINE_EXCEPTION(TCPSocketBindError)
	DEFINE_EXCEPTION(TcpAcceptError)

	class TCPServerSocket
	{
		typedef unsigned short Port;
		SOCKET m_h_socket;

	public:
		TCPServerSocket(Port);
		~TCPServerSocket(void);
		SOCKET getHandle() { return m_h_socket;}
		SOCKET accept();
	};

}
#endif