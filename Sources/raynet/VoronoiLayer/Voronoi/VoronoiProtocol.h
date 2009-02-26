#ifndef __VORONOI_PROTOCOL_HEADER__
#define __VORONOI_PROTOCOL_HEADER__


#include <string>

#include "ConfFile.h"
#include "P2PTools.h"
#include "UDPSocket.h"
#include "UDPPacket.h"
#include "LogFile.h"


#include <math.h>


namespace P2P
{
	DEFINE_EXCEPTION(NoVoronoiRndNodeAvailable)

	template <  class Node>
	class VoronoiProtocol
	{
		typedef std::vector< Node > Nodes;
		typedef typename Node::Distance Distance;
		typedef typename Node::Point Point;

		typedef unsigned int Volume;
		static const unsigned int INFINITE_VOLUME = UINT_MAX;


		// Important: retain this order of declaration
		const unsigned int m_ui_ViewSize ;		
		const unsigned int m_ui_UpdateSize ;		
		Nodes m_o_View;		
	
	


		//rays and stuff
		static const int RAYS_PER_CIRCLE = 36;  // rays within a circle ... (360 degress * 360 degrees)						
		typedef unsigned short XYAngle;
		typedef unsigned short XZAngle;
		typedef std::pair< XYAngle, XZAngle> Angle3D;
		typedef std::vector< Angle3D > Angle3Ds;

		typedef std::map< Node,	Angle3Ds > Node2Rays;
		Node2Rays m_nodes2Rays;


		void _createBipartiteGraph(const Node& _thisNode);

		void _addUpdates(  const Nodes&	_updates );

		void _filterUnwantedPeers( const Node& _thisNode);

		unsigned int _volChangeRemovingNode(const Node& _thisNode, const Node& _removedNode);

		Distance  _distOfRay( XYAngle , XZAngle , const Node& _thisNode, const Node&	_peer)const;
	
	public:
	

		VoronoiProtocol( const Nodes& _view,
			unsigned int _i_updateSize,
			unsigned int _i_ViewSize
			);

		void setPosition( const Point& _point)	{	m_o_thisNode.setPoint(_point) ;}

		bool         bHasNodes()const { return !m_o_View.empty();}
		const Nodes& getView() const  {return m_o_View; }
		Nodes&       getView()        {return m_o_View; }

		Node getRndPeer() const;

		void selectToSend(	const Node& _thisNode,
							const Node&  _peer, 
							Nodes&   _updates  )const;

		void selectToKeep(  const Node& _thisNode,
							const Node& _addrFrom, 
							const std::vector<Node>& kleinbergView,
							const std::vector<Node>& newStuff);

	};






	//-------------------
	template <   class Node>
	inline VoronoiProtocol< Node>::VoronoiProtocol( 
		const Nodes& _view,
		unsigned int _i_updateSize,
		unsigned int _i_ViewSize)		
		:
		m_ui_ViewSize   (  _i_ViewSize  ),
		m_ui_UpdateSize ( _i_updateSize ),	
		m_o_View        ( _view)
	{		
		LOG_TABLE( "Current Voronoi View", m_o_View );
	}



	//-------------------

	template <   class Node>
	inline void VoronoiProtocol< Node>::selectToKeep(	const Node& _thisNode,
														const Node& _addrFrom, 
														const Nodes&  _kleinBergView,
														const Nodes&  _updates	)
	{
		// update and create graph
		_addUpdates( _updates );

		_createBipartiteGraph(_thisNode );

		_filterUnwantedPeers( _thisNode );
	}

		//-------------------
	template < class Node>
	inline Node VoronoiProtocol< Node>::getRndPeer() const 
	{				

		if ( m_o_View.empty() )
		{
			THROW(NoVoronoiRndNodeAvailable,"View is empty");
		}
		return m_o_View[ Tools::rndNumber( m_o_View.size() ) ];
	}
	//-------------------


	template <   class Node>
	inline void VoronoiProtocol< Node>::selectToSend(	const Node& _thisNode, 
														const Node&  _peer, 
														Nodes&   _updates  )const
	{
		_updates.clear();		
		_updates.reserve( m_ui_UpdateSize  );

		//only copy C - 1 amount of nodes .. ( using rnd access operation.. only on vectors )
		const unsigned int shorter = std::min<unsigned int>(m_o_View.size(),m_ui_UpdateSize - 1);
		_updates.assign( m_o_View.begin(), 	m_o_View.begin() + shorter ); 
		_updates.push_back( _thisNode );		

	}

	//---------------------
	template <   class Node>
	inline void VoronoiProtocol< Node>::_addUpdates(  const Nodes&	_updates )
	{		
		// DESIGN NOTE: O-N squared
		// Though a set/map would be "theoretically" faster..
		// ..in practice this is most unlikely due to heaped memory allocations and page faults
		// ... Unless the size of C is in the hundreds.. I suspect a vector to be faster in O N squared


		// Simple N2 update algo

		for ( Nodes::const_iterator itUpdate = _updates.begin() ; itUpdate != _updates.end(); itUpdate++ )
		{
			Nodes::iterator itView = m_o_View.begin();

			for (  ; itView!= m_o_View.end(); itView++ )
			{
				if ( itView->samePeer(*itUpdate)   )
				{
					if ( itUpdate->fresher( *itView) )
					{
						*itView = *itUpdate;
					}					
					break;
				}				
			}
			if( itView == m_o_View.end())
			{
				//adds
				m_o_View.push_back( *itUpdate);
			}

		}

	}

	//----------------------

	template < class Node>
	inline void VoronoiProtocol< Node>::_createBipartiteGraph(  const Node& _thisNode)
	{
		// DESIGN NOTE: Rays
		//			Rays are not stored but rather calculated..
		//          The bipartite graph stores Node->rays only

		m_nodes2Rays.clear();		


		typedef unsigned int RayAngle;

		// forall Rays in 3D Sphere
		for( RayAngle NthXYray = 0; NthXYray < RAYS_PER_CIRCLE; NthXYray ++)
		{
			for ( RayAngle NthXZray = 0; NthXZray < RAYS_PER_CIRCLE; NthXZray ++)
			{
				XYAngle xyAngle = (NthXYray * 360) / RAYS_PER_CIRCLE;
				XZAngle xzAngle = (NthXZray * 360) / RAYS_PER_CIRCLE;

				Distance closestDist = Node::Point::MAX_DISTANCE;
				Node* pClosestPeer = 0;

				// forall Nodes in view ..see if Node is closer to ray
				for ( Nodes::iterator itPeer =m_o_View.begin(); itPeer != m_o_View.end(); itPeer++)
				{
					Distance rayDist = _distOfRay( xyAngle, xzAngle, _thisNode, *itPeer );

					if ( rayDist < closestDist  )
					{
						closestDist = rayDist;
						pClosestPeer = &(*itPeer); 
					}				
				}

				if ( pClosestPeer )
				{
					// add to total vol, the vol of ray sphere
				//	m_nodes2Rays[ *pClosestPeer ].m_i_NodeVolume += pow ( closestDist , 3);
					// link rayAngle to Node
					m_nodes2Rays[ *pClosestPeer ].push_back( Angle3D(xyAngle, xzAngle)  );
				}
			}

		}

	}

	//----------------------

	template < class Node>
	inline typename Node::Distance  VoronoiProtocol<Node>::_distOfRay(	XYAngle _XYrayAngle, 
																		XZAngle  _XZrayAngle, 
																		const Node& _thisNode, 
																		const Node&	_peer)const
	{
		Distance ret = Node::Point::MAX_DISTANCE;

		// Details: 
		//	RayAngles: These start by North and rotate clockwise.. RAYS_PER_CIRCLE rays
		//	360 ° = 2PI radians ... all values in radians


		const Point& peerPoint = _peer.getPoint();
		const Point& thisPoint =  _thisNode.getPoint();

		// get right angle in radians
		const double PI = 3.14159265;
		const double RightAngle = PI / 2.0f;


		// work out the angles of peer from Y axis and Z axis ... ANGLES IN RADIANS
		const double PeerAngleXY = 	atan(   /*opp*/(peerPoint.getYpos() - thisPoint.getYpos() ) 
			/
			/*adj*/(peerPoint.getXpos() - thisPoint.getXpos() )   );


		const double PeerAngleXZ = 	atan(   /*opp*/(peerPoint.getZpos() - thisPoint.getZpos() ) 
			/
			/*adj*/(peerPoint.getXpos() - thisPoint.getXpos() )   );



		// ensure that a ray can even cross through a plane drawn by the peer
		// ... 90° or more means that this is impossible
		const double XYAngularDifference =  std::max<double>(PeerAngleXY ,_XYrayAngle) 
											- std::min<double>(PeerAngleXY ,_XYrayAngle);
		const double XZAngularDifference =  std::max<double>(PeerAngleXZ , _XZrayAngle) 
											- std::min<double>(PeerAngleXZ , _XZrayAngle) ;


		if (  XYAngularDifference < RightAngle	&&	XZAngularDifference < RightAngle		)
		{			
			// Cos' the distance for the Y angle then the Z angle
			ret =(Distance) ( _thisNode.getDistance(_peer)  /  (   cos(PeerAngleXY)*cos(PeerAngleXZ)  ) );		

		}

		return ret;
	}
	//----------------------
	template <  class Node>
	inline void VoronoiProtocol< Node>::_filterUnwantedPeers( const Node& _thisNode)
	{
		// sort Nodes in order of impact each has on volume
		typedef std::pair< unsigned int, Node* >  NodeDistance;

		std::vector< NodeDistance > NodeDistList;
		NodeDistList.reserve( m_o_View.size() );

		Nodes UnSortedView;
		UnSortedView.assign( m_o_View.begin(), m_o_View.end() );

		//sort
		for ( Nodes::iterator itPeer = UnSortedView.begin(); itPeer != UnSortedView.end() ; itPeer++)
		{				
			NodeDistList.push_back(  NodeDistance( _volChangeRemovingNode(_thisNode, *itPeer), &(*itPeer)  )    );		
		}
		std::sort( NodeDistList.begin(), NodeDistList.end() );


		//decide on view length
		const unsigned int limitViewSize = std::min<unsigned int>( UnSortedView.size(), m_ui_ViewSize );
		m_o_View.clear();
	
		//copy right amount of nodes to view
		for ( unsigned int pos = 0; pos < limitViewSize; pos++)
		{
			m_o_View.push_back( *NodeDistList[pos].second );
		}
		


	}
	//----------------------	
	template <   class Node>
	inline unsigned int VoronoiProtocol< Node>::_volChangeRemovingNode(const Node& _thisNode, const Node& _removedNode)
	{

		Volume newVolume = 0;

		// get ray details
		Angle3Ds& raysOnPeer  = m_nodes2Rays[_removedNode];		
		

		// for all rays on peer.. relocate rays and addition the volumes
		for( Angle3Ds::iterator it3DRayAngle = raysOnPeer.begin();
				it3DRayAngle != raysOnPeer.end() && newVolume != INFINITE_VOLUME; 
				it3DRayAngle++)
		{			
			Distance closestDist = Node::Point::MAX_DISTANCE;				

				// find closest peer for ray
				for ( Nodes::iterator itPeer =m_o_View.begin(); itPeer != m_o_View.end(); itPeer++)
				{
					// ignore the Peer we want to avoid
					if( *itPeer != _removedNode)
					{
						Distance rayDist = _distOfRay( it3DRayAngle->first, it3DRayAngle->second, _thisNode, *itPeer );

						if ( rayDist < closestDist  )
						{
							closestDist = rayDist;							 
						}
					}
				}

				//modify vol in terms of ray length
				if ( closestDist == Node::Point::MAX_DISTANCE)
				{
					newVolume = INFINITE_VOLUME;
				}
				else
				{	///cubed volume
					newVolume += closestDist * closestDist * closestDist;
				}


		}

		return newVolume ;

	}



}

#endif