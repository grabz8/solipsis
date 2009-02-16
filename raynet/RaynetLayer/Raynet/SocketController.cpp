#include "SocketController.h"






#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "RaynetMessage.h"
#include "SocketReactor.h"
#include "RingingTimer.h"

#include "Thread.h"
#include "LogFile.h"


//GREG BEGIN
// disable warning: 'this' : used in base member initializer list
// we won t access members on 'this' into base-class constructors
#pragma warning( disable : 4355 )
//GREG END


namespace P2P
{

	
		
	TCPSocketContext::TCPSocketContext( SOCKET _relSock, RaynetMessagePtr _pPack)
				:
				m_ui_CurrDataPos(0),
				m_ui_TotalDataSize( _pPack->getDataSize() ),
				m_bHeaderDone(false),
				m_o_sock( _relSock , IP::TCPSocket::eAsync ),
				m_p_packet( _pPack),
				m_eDirection( eSend)
			{}

			//-------------
			TCPSocketContext::TCPSocketContext( SOCKET _relSock)
				:
				m_ui_CurrDataPos(0),
				m_ui_TotalDataSize( 0 ),
				m_bHeaderDone(false),
				m_o_sock( _relSock , IP::TCPSocket::eAsync ),
				m_p_packet( ),
				m_eDirection( eRecv )
			{}


			//-------------
			void TCPSocketContext::_transferSend()
			{
				if ( !m_bHeaderDone )
				{
					uint sent =  m_o_sock.send(	m_p_packet->getHeaderBuffer() , 
												m_p_packet->getHeaderSize() );
					
					// goodness.. if I cannot send a header in its entirety!!
					if( sent < m_p_packet->getHeaderSize() )
					{
						THROW(TCPTransferError," Could not send header in its entirety!!");
					}
					m_bHeaderDone= true;
					
				}
				else
				{
					m_ui_CurrDataPos += m_o_sock.send(	m_p_packet->getDataBuffer() + m_ui_CurrDataPos, 
														m_p_packet->getDataSize() - m_ui_CurrDataPos );
				
				}			
			}
			//-------------
			void TCPSocketContext::_transferRecv()
			{
				if ( !m_bHeaderDone )
				{
					RaynetMessage::Header header;
					uint recv =  m_o_sock.recv(	header.getWritableBuffer() , 
												header.maxBufSize());

					header.forceSize(recv );
					
					// goodness.. if I cannot send a header in its entirety!!
					if( recv < header.maxBufSize() )
					{
						THROW(TCPTransferError," Could not recv header in its entirety!!");
					}

					m_p_packet = RaynetMessagePtr( new RaynetMessage( header) );
					m_bHeaderDone= true;

				}
				else
				{
					m_ui_CurrDataPos += m_o_sock.recv(	m_p_packet->getDataBuffer() + m_ui_CurrDataPos, 
														m_p_packet->getDataSize() - m_ui_CurrDataPos);					

				}

			}
			//-------------
			void TCPSocketContext::transfer()
			{
				if ( m_eDirection == eSend)
				{
					_transferSend();			
					
				}
				else
				{
					_transferRecv();					
				}			

			}
			//-------------
			bool TCPSocketContext::bIsDone()
			{
				bool bRet = false;
				if ( m_p_packet.get()!= 0 )
				{
					bRet = m_p_packet->getDataSize() == m_ui_CurrDataPos;
				}
				
				return bRet;
			}
		
		




















	/*
			MAIN CLASS : SOCKET CONTROLLER
	*/	
	



	SocketController::SocketController ( 
				SocketControllerHandler& _callback,
				uint _kleinbergGossip,
				uint _voronoiGossip,
				IP::Port _protocolPort,
				IP::Port _raynetPort, 
				uint _tcpTimeOut)
				:
		m_o_serverSocket( _raynetPort  ),
		m_o_protocolSocket(),
		m_o_socketReactor( *this, *this),
		m_o_controllerCallback( _callback ),
		m_ui_tcpTimeOut( _tcpTimeOut)
	{
		m_o_ProtocolTimer.add( eKLEINBERG, _kleinbergGossip );
		m_o_ProtocolTimer.add( eVORONOI, _voronoiGossip  );

		m_o_protocolSocket.bind( _protocolPort  );
		m_o_socketReactor.add(    m_o_protocolSocket.getHandle(), IP::SocketReactor::eReadNotify );
		m_o_socketReactor.add(    m_o_serverSocket.getHandle() , IP::SocketReactor::eReadNotify );				
		m_o_socketReactor.start();

	}


	//----------------------------

	void SocketController::route( const RaynetMessage& _package, IP::Port _peerPort, IP::Addr _peerAddr )
	{
		// Since peers can be so unreliable... connection time is shortened so as not to block for minutes
		IP::TCPSocket o_socket( _peerPort, _peerAddr ,  IP::TCPSocket::eSync,  m_ui_tcpTimeOut );

		o_socket.send( _package.getHeaderBuffer(), _package.getHeaderSize() );
		o_socket.send( _package.getDataBuffer(), _package.getDataSize() );
	}
	//----------------------------
	void SocketController::route( RaynetMessagePtr _pPacket, IP::Port _peerPort, IP::Addr _peerAddr)
	{
		//create sock .. no connection timeout means async socket connection
		IP::TCPSocket tempSock( _peerPort,	_peerAddr, IP::TCPSocket::eAsync, m_ui_tcpTimeOut	);

		{
			LOCK(m_o_lock);

			//store context for later... 					

			m_o_activeSockMap[ tempSock.getHandle() ]=	new TCPSocketContext( tempSock.getHandle() ,_pPacket)  ;
			//tempSock.getHandle() ]=    ;		

			//tell reactor about socket

			m_o_socketReactor.add( tempSock.getHandle() ,  IP::SocketReactor::eWriteNotify ); 
			// do this internally ...m_o_socketReactor.refresh();
		}	
		tempSock.release();

	}
	//----------------------------
	void SocketController::send( const UDPPacket& _packet)
	{		
		IP::UDPSocket::send( 							
						_packet.getRemotePort(),
						_packet.getRemoteAddr(),
						_packet.getBuffer(),
						_packet.size() );
	}
	//----------------------------

	void SocketController::evTimeOut() 
	{		

		if( m_o_ProtocolTimer.isRinging(eKLEINBERG) )
		{			
			m_o_controllerCallback.timeOutUpdate(eKLEINBERG);

		}
		if( m_o_ProtocolTimer.isRinging(eVORONOI) )  //must be TCP socket
		{
			m_o_controllerCallback.timeOutUpdate(eVORONOI);
		}


		m_o_socketReactor.setTimeOut(  m_o_ProtocolTimer.timeLeft()  );
	}

	//----------------------------
	// Thread context: Socket Reactor
	void SocketController::evSocketRead(IP::SOCKET _sock)
	{
		{
			LOCK(m_o_lock);
			

			if ( _sock  == m_o_serverSocket.getHandle() )
			{
				SOCKET sock = m_o_serverSocket.accept() ;

				m_o_socketReactor.add( sock ,  IP::SocketReactor::eReadNotify ); 					

				m_o_activeSockMap[ sock ]=  new TCPSocketContext( sock ) ;		


			}
			else if ( _sock == m_o_protocolSocket.getHandle() )
			{
//GREG BEGIN
//				char buff[UDPPacket::MAX_PACKET_SIZE];					
				char buff[IP::MAX_UDP_PACKET_SIZE];					
//GREG END

				IP::Port remotePort=0;
				IP::Addr remoteAddr=0;					

				uint read = m_o_protocolSocket.recv( 
											remotePort,
											remoteAddr,
											buff,											
//GREG BEGIN
//											UDPPacket::MAX_PACKET_SIZE			
                                            IP::MAX_UDP_PACKET_SIZE
//GREG END
											);

				m_o_controllerCallback.protocolUpdate( 						
						UDPPacket(  m_o_protocolSocket.getBindPort() , //host byte order
									remoteAddr,  //network byte order					
									buff, 
									read) 						
				);
			
			}
			else
			{
				_handleClientSocket(_sock);				
			}
			

		}	


		m_o_socketReactor.setTimeOut(  m_o_ProtocolTimer.timeLeft()  );

	}
	//----------------------------
	void SocketController::evSocketWrite(IP::SOCKET _sock) 
	{
		// Precondition :These messages should ONLY come from client sockets as 
		//               we do not check to see if server sockets can write....
		
		_handleClientSocket( _sock);	
		m_o_socketReactor.setTimeOut(  m_o_ProtocolTimer.timeLeft()  );
	}
	//----------------------------
	void SocketController::_handleClientSocket( SOCKET _sock)
	{	
		{
			LOCK(m_o_lock);

			Sock2Context::iterator itClientContext = m_o_activeSockMap.find( _sock) ;

			if ( itClientContext != m_o_activeSockMap.end() )
			{
				// Client socket is known

				// Warning.. do not
				TCPSocketContextPtr pSockContext  = itClientContext->second;

				if ( pSockContext->m_o_sock.isConnected() )
				{ //closed

					try
					{
						pSockContext->transfer();	
						if ( pSockContext->bIsDone() )
						{
							// Nominal case : fine, all has been transfered
							if ( pSockContext->m_eDirection == TCPSocketContext::eRecv )
							{
								m_o_controllerCallback.routeUpdate( pSockContext->m_p_packet );
							}

							
							m_o_activeSockMap.erase( pSockContext->getHandle() );
							m_o_socketReactor.remove( pSockContext->getHandle() );
						}
					}
					catch ( std::exception& e)
					{
						LOG(" Socket error: dropping transfer..reason:" << e.what() )

						
						m_o_activeSockMap.erase( pSockContext->getHandle() );
						m_o_socketReactor.remove( pSockContext->getHandle() );
					}
					
				}
				else
				{   // Yikes.. disconnected

					m_o_activeSockMap.erase(  pSockContext->getHandle() );
					m_o_socketReactor.remove( pSockContext->getHandle() );

					if ( pSockContext->m_eDirection == TCPSocketContext::eSend)
						LOG(" Warning: failed to finish sending TCP data.. connection was dropped")
					else
						LOG(" Warning: failed to finish receiving TCP data.. connection was dropped")
						
				}
			}
		}
	}

}

