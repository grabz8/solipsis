#ifndef __TCPSocket_I_hate_macros_type_of_HEADER_H__
#define __TCPSocket_I_hate_macros_type_of_HEADER_H__


#include "P2PTools.h"

#include <memory.h>


namespace IP{
	
	DEFINE_EXCEPTION(TCPSocketSendError)
	DEFINE_EXCEPTION(TCPSocketRecvError)
	DEFINE_EXCEPTION(TCPSocketCreationError)
	DEFINE_EXCEPTION(TCPSocketConnectError)
	DEFINE_EXCEPTION(TCPSocketOptionError)
	DEFINE_EXCEPTION(IoctlSocketError)
	DEFINE_EXCEPTION(TCPSelectError)



	class TCPSocket :private Tools::NO_COPY_ASSIGN
	{
		

	public:
	
		enum BlockingMode{ eSync, eAsync};

		TCPSocket(	unsigned short _port, 
					unsigned int _addr, 
					enum BlockingMode _eMode,
					unsigned int _timeoutSecs
				);	


		TCPSocket(	SOCKET , BlockingMode 	);	

		//Design Note: this empty destructor has to be defined in .cpp
		~TCPSocket();

		uint recv( void* pBuffer, uint MaxBufSize );
	
		uint send( const void* pBuffer, uint MaxBufSize );
				
	
		SOCKET getHandle();
		SOCKET release();

		bool isConnected();

	private:
		SOCKET m_h_socket;			
		bool m_bReleased;
		void _connect( SOCKET ,	const sockaddr_in& ,enum BlockingMode ,	unsigned int _timeout );
		void _setBlockingMode( BlockingMode);
		
	};



	inline SOCKET TCPSocket::getHandle(){ return  m_h_socket;} 










}



#endif