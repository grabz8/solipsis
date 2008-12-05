#ifndef _RAYNET_FACADE_MAIN_HEADER_H_
#define _RAYNET_FACADE_MAIN_HEADER_H_

#include "KleinbergProtocol.h"
#include "VoronoiProtocol.h"

#include "TCPSocket.h"

#include "SocketController.h"
#include "RaynetMessage.h"




namespace P2P
{

	// HANDLER CLASS : client of raynet implements this
	class RaynetHandler
	{
	protected:
		RaynetHandler(){};
	public:
		// Undefined thread context : may or may not be callers context
		virtual void evRaynetMessage( RaynetMessagePtr _INpackage )=0;	

		// Guaranteed to be always called in callers thread's context
		virtual void evRaynetMessage( const RaynetMessage& _INpackage )=0;	
	};







	// RAYNET FACADE CLASS  : main facade class

	template <class Node>
	class RaynetFacade :  public SocketControllerHandler,	private Tools::NO_COPY_ASSIGN
	{

		Tools::Mutex m_o_protocolLock;

		ConfFile<Node> m_o_confFile;
		Node m_o_thisNode;

		RaynetHandler& m_o_RaynetHandler;		
	


		typedef std::vector<Node> Nodes;
		typedef typename Node::Point Point;


		//kleinberg
		KleinbergProtocol<Node> m_o_KleinbergProtocol;	

		//voronoi
		VoronoiProtocol<Node> m_o_voronoiProtocol;		

		

		SocketController m_o_sockController;

		void _recvProtocolMessage() ;
		void _send( const RaynetMessage& _package, const Node& _peer);
		void _getNearestPeer( const Nodes& _PeerVec,  Node& _NearestPeer, const Point& _targetPoint );
		void _sendNodes( unsigned short _port, int _addr, Nodes& _nodes, UDPPacket::Type _type );
		void _recvRaynetMessage();			
		Node _getNearestPeer( const Point& _point);


	protected:
		//from AsyncSocketHandler.. ( Reactor thread callbacks)
		virtual void protocolUpdate(const UDPPacket&);
		virtual void timeOutUpdate( ProtocolType);
		virtual void routeUpdate( RaynetMessagePtr);


	public:

		RaynetFacade( RaynetHandler& ,const ConfFile<Node>& );
		//~RaynetFacade(void);		


		// Contract:
		// return :  (true)message was sent... (false) message not sent and returned to this node
		// ( non-blocking )
		void asyncRoute( RaynetMessagePtr _pPackage);

		// Contract:
		// return :  (true)message was sent... (false) message not sent and returned to this node
		// ( blocking until achieved)
		void syncRoute( const RaynetMessage& _package);
		

		void setPosition( const Point& _point)
		{
			LOCK(m_o_protocolLock);
			m_o_thisNode.setPoint(_point) ;
			
		}

	};






	//----------------implementation--------------------------



	template <class Node>
	inline RaynetFacade<Node>::RaynetFacade( RaynetHandler& _callbackInterface , const ConfFile<Node>& _confFile)
		:		 
		m_o_confFile( _confFile),
		m_o_thisNode( _confFile.protocolPort(), _confFile.raynetPort(), IP::getHostAddr(), _confFile.position() ),
		m_o_RaynetHandler( _callbackInterface),					
		m_o_KleinbergProtocol( 	_confFile.initView(), _confFile.updateSize() , _confFile.viewSize()	),	
		m_o_voronoiProtocol	( 	_confFile.initView(),  _confFile.updateSize(), _confFile.viewSize() ),
		m_o_sockController( *this,
							_confFile.kleinbergGossipInterval(),
							_confFile.voronoiGossipInterval(),
							_confFile.protocolPort(),
							_confFile.raynetPort(),
							_confFile.tcpTimeOut()
							) 

	{	

	}
	
	//----------------------------------------
	template <class Node>
	inline void RaynetFacade<Node>::syncRoute( const RaynetMessage&  _package)
	{
		Node o_NearestPeer  = _getNearestPeer( _package.getPoint()  );		

		if ( o_NearestPeer != m_o_thisNode )
		{
			//need to send it then
			m_o_sockController.route(	_package, 
										o_NearestPeer.getRaynetPort(), 
										o_NearestPeer.getAddr() );			
		}
		else
		{
			m_o_RaynetHandler.evRaynetMessage( _package );
		}
	}
	//----------------------------------------
	template <class Node>
	inline void RaynetFacade<Node>::asyncRoute( RaynetMessagePtr _pPackage)
	{
		// Design Note:
		//  Duplication of node values between voronoi and kleinberg is not a 
		//  problem. Performance-wise, it is more efficient to not use a std::set<node>
		//  which would remove duplications but generate loads of heap allocations and
		//  require a sort algorithm.
		//   Summary: Leave duplications 


		// default to THIS NODE as target Node		
		Node o_NearestPeer  = _getNearestPeer( _pPackage->getPoint()  );		

		if ( o_NearestPeer != m_o_thisNode )
		{
			//need to send it then
			m_o_sockController.route(	_pPackage,
										o_NearestPeer.getRaynetPort(), 
										o_NearestPeer.getAddr() );			
		}
		else
		{
			m_o_RaynetHandler.evRaynetMessage( _pPackage );
		}



	}

	//----------------------------------------
	
	//from SocketHandler
	template <class Node>
	inline void RaynetFacade<Node>::timeOutUpdate( ProtocolType _protocolType)
	{

		Nodes sendableNodes;
		Node rnd;
		UDPPacket::Type eMessType = UDPPacket::e_VORONOI_MESSAGE;

		{// IMPORTANT NOTE: Do NOT remove these braces... they ensure mutex is released!!!!!!!!!!!

			LOCK(m_o_protocolLock);			

			if( _protocolType == eKLEINBERG )
			{			
				rnd = m_o_KleinbergProtocol.getRndPeer();															
				m_o_KleinbergProtocol.selectToSend(  m_o_thisNode, rnd , sendableNodes )  ;
				eMessType = UDPPacket::e_KLEINBERG_MESSAGE;			
			}

			if( _protocolType == eVORONOI )  //must be TCP socket
			{
				rnd = m_o_voronoiProtocol.getRndPeer();			
				m_o_voronoiProtocol.selectToSend(  m_o_thisNode, rnd , sendableNodes )  ;							
			}
		}


		// send packet
		UDPPacket messPacket(
							rnd.getProtocolPort(),
							rnd.getAddr(),
							m_o_confFile.protocolPort(),
							eMessType);

		messPacket.setObjects(  sendableNodes);
		m_o_sockController.send( messPacket );	

	


	}
	//----------------------------------------

		// called in context of SocketReactor thread
	template <class Node>
	inline void RaynetFacade<Node>::protocolUpdate( const UDPPacket& _packet)
	{
				

		// Get Received Nodes and PEER (that sent nodes)
		Nodes recvdNodes;		
		recvdNodes.reserve( m_o_confFile.updateSize() );
		_packet.getObjects( recvdNodes );

		const Node& PEER = recvdNodes.back(); // peer is always last sent node


		// handle packet
		switch( _packet.getType() )
		{
		case UDPPacket::e_KLEINBERG_MESSAGE:  
			{
				LOG( "\t recvd KLEINBERG message.. first send peer my nodes" );

				// send nodes						
				Nodes sendableNodes;				
				sendableNodes.reserve( m_o_confFile.updateSize() );

				m_o_KleinbergProtocol.selectToSend(  m_o_thisNode, PEER , sendableNodes )  ;

				UDPPacket respPacket(
									_packet.getRemotePort(),
									_packet.getRemoteAddr(),
									m_o_confFile.protocolPort(),
									UDPPacket::e_KLEINBERG_RESPONSE);

				respPacket.setObjects(  sendableNodes);
				m_o_sockController.send( respPacket );



				//recv nodes
				LOG( "\t accept peer's kleinberg nodes" );
				m_o_KleinbergProtocol.selectToKeep( m_o_thisNode, PEER, recvdNodes );
			}
			break;
			//----
		case  UDPPacket::e_KLEINBERG_RESPONSE:
			{
				LOG( "\t recvd KLEINBERG Response" );
				m_o_KleinbergProtocol.selectToKeep( m_o_thisNode, PEER, recvdNodes );
			}
			break;
			//----
		case UDPPacket::e_VORONOI_MESSAGE:
			{
				LOG( "\t recvd VORONOI message .. first send peer my nodes" );

				// send nodes						
				Nodes sendableNodes;				
				sendableNodes.reserve( m_o_confFile.updateSize() );

				m_o_KleinbergProtocol.selectToSend(  m_o_thisNode, PEER , sendableNodes )  ;

				UDPPacket respPacket(	
									_packet.getRemotePort(),
									_packet.getRemoteAddr(),
									m_o_confFile.protocolPort(),
									UDPPacket::e_KLEINBERG_RESPONSE);

				respPacket.setObjects(  sendableNodes);
				m_o_sockController.send( respPacket );


				// recv nodes
				LOG( "\t accept peer's voronoi nodes" );
				m_o_voronoiProtocol.selectToKeep( m_o_thisNode, PEER, m_o_KleinbergProtocol.getView(), recvdNodes );
			}
			break;
			//----
		case UDPPacket::e_VORONOI_RESPONSE:
			{
				LOG( "\t recvd VORONOI Response" );
				m_o_voronoiProtocol.selectToKeep( m_o_thisNode, PEER, m_o_KleinbergProtocol.getView(), recvdNodes );				
				break;
			};		

		}
	}


	//-----------------------------------------
	template <class Node>
	inline void RaynetFacade<Node>::routeUpdate( RaynetMessagePtr _pPack)
	{
	
		Node nearestPeer = _getNearestPeer( _pPack->getPoint()  );

		if ( nearestPeer == m_o_thisNode )
		{
			m_o_RaynetHandler.evRaynetMessage( _pPack );			
		}
	
	}

		//-----------------------------------------
	template <class Node>
	inline Node RaynetFacade<Node>::_getNearestPeer( const Point& _targetPos)
	{
		LOCK(m_o_protocolLock);

		Node o_NearestPeer = m_o_thisNode;
		Nodes knodes = m_o_KleinbergProtocol.getView();
		_getNearestPeer( knodes, o_NearestPeer, _targetPos );

		Nodes vnodes = m_o_voronoiProtocol.getView();
		_getNearestPeer( vnodes, o_NearestPeer, _targetPos );

		return o_NearestPeer;
	}
	//----------------------------------------
	template <class Node>
	inline void RaynetFacade<Node>::_getNearestPeer(	const Nodes& _PeerVec, 
		Node& _NearestPeer,
		const Point& _targetPoint )
	{
		Node::Distance shortestDist = _NearestPeer.getPoint().getDistance( _targetPoint ) ;

		//try to find a better peer than the current node
		for ( Nodes::const_iterator it = _PeerVec.begin(); it != _PeerVec.end(); it++)
		{	
			const Node::Distance peerDist = it->getPoint().getDistance( _targetPoint   );

			if (  shortestDist > peerDist    )
			{
				shortestDist = peerDist;
				_NearestPeer = *it;
			}
		}
	}



}



#endif
