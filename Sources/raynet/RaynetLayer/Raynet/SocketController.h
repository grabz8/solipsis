#ifndef _A_SOCKET_CONTROLLER_H_
#define _A_SOCKET_CONTROLLER_H_



#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "RaynetMessage.h"
#include "SocketReactor.h"
#include "RingingTimer.h"

#include "Thread.h"

#include "BasicTypedefs.h"

#include "refptr.h"



namespace P2P
{


	// CALLBACK CLASS
	class SocketControllerHandler
	{

		//void _readClientSocket( SOCKET _sock  );				

	protected: 
		SocketControllerHandler(){}
		virtual ~SocketControllerHandler(){}

	public:
		virtual void protocolUpdate( const UDPPacket&) = 0;
		virtual void timeOutUpdate( ProtocolType) = 0;
		virtual void routeUpdate( RaynetMessagePtr ) = 0;

	};








		// TCP ASYNC CONTEXT  CLASS

		DEFINE_EXCEPTION(TCPTransferError)

		struct TCPSocketContext : private Tools::NO_COPY_ASSIGN
		{
			enum TransferDir { eSend, eRecv};
			uint m_ui_CurrDataPos;
			uint m_ui_TotalDataSize;
			bool m_bHeaderDone;
			IP::TCPSocket m_o_sock;
			RaynetMessagePtr m_p_packet;
			TransferDir m_eDirection;

			
			TCPSocketContext( SOCKET _relSock, RaynetMessagePtr _pPack);
			
			TCPSocketContext( SOCKET _relSock);
						
			SOCKET getHandle(){   return m_o_sock.getHandle();    }

			void _transferSend();
			
			void _transferRecv();			
			
			void transfer();			
			
			bool bIsDone();
			
		};
		typedef Tools::RefPtr<TCPSocketContext> TCPSocketContextPtr;











	/*
			MAIN CLASS : SOCKET CONTROLLER
	*/	
		class SocketController : public IP::SocketReactorHandler, Tools::NO_COPY_ASSIGN
	{

		


		Tools::Timer<2/*entries*/> m_o_ProtocolTimer;


		Tools::Mutex m_o_lock;

		//sockets		
		IP::TCPServerSocket m_o_serverSocket;
		IP::UDPSocket       m_o_protocolSocket;
		IP::SocketReactor   m_o_socketReactor;

		SocketControllerHandler& m_o_controllerCallback;

			


		typedef std::map<IP::SOCKET, TCPSocketContextPtr> Sock2Context;
		Sock2Context m_o_activeSockMap;


		void _handleClientSocket( IP::SOCKET);

		uint m_ui_tcpTimeOut;

	protected:

		//reactor callback methods
		virtual void evTimeOut();
		virtual void evSocketRead(IP::SOCKET);
		virtual void evSocketWrite(IP::SOCKET);

	public:

		SocketController (	SocketControllerHandler& callback,
							uint _kleinbergGossip,
							uint _voronoiGossip,
							IP::Port _protocolPort,
							IP::Port _raynetPort, 
							uint _tcpTimeOut
							);

		
		void route( RaynetMessagePtr , IP::Port, IP::Addr);
		void route( const RaynetMessage&  , IP::Port, IP::Addr );

		void send( const UDPPacket& );


	};



	
}

#endif