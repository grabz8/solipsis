#ifndef __ANOTHER_BORING_SOCKET_CLASS__
#define __ANOTHER_BORING_SOCKET_CLASS__



#include "UDPPacket.h"

#include "P2PTools.h"


#ifndef _WIN32
typedef int SOCKET;
#endif


namespace IP
{

	DEFINE_EXCEPTION(UDPSocketSendError)
	DEFINE_EXCEPTION(UDPSocketRecvError)	
	DEFINE_EXCEPTION(UDPSocketOptionError)
	DEFINE_EXCEPTION(UDPSocketPackageTooBigError)
	DEFINE_EXCEPTION(UDPSocketBindError)

	class UDPSocket :public Tools::NO_COPY_ASSIGN
	{	
		
		SOCKET m_h_socket;
		int m_i_MaxKernelDatagramSize;
		unsigned short m_us_localBindPort;

					

	public:

		UDPSocket();
		~UDPSocket();

//GREG BEGIN
//		static void send( IP::Port _p, IP::Addr _a, const void* _pdata, ushort _size) ;
		static void send( IP::Port _p, IP::Addr _a, const void* _pdata, int _size) ;
//		uint recv( IP::Port& _port, IP::Addr& _addr, void* _data, ushort bufsize) ;
		uint recv( IP::Port& _port, IP::Addr& _addr, void* _data, int bufsize) ;
//GREG END
//GREG BEGIN
    	static void send( P2P::UDPPacket& _packet );
		P2P::UDPPacket recv( ) ;
//GREG END

		void bind(   unsigned short  _port);
		int getBindPort();	
		SOCKET getHandle();

	};



}

#endif