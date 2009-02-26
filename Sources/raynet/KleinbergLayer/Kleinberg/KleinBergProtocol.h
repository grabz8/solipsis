#ifndef __KLEINBERG_PROTOCOL___HEADER__
#define __KLEINBERG_PROTOCOL___HEADER__


#include <string>
#include <list>
#include <map>

#include "ConfFile.h"
#include "P2PTools.h"
#include "UDPSocket.h"
#include "UDPPacket.h"
#include "LogFile.h"




namespace P2P
{
	DEFINE_EXCEPTION(KleinBergAlgoException)
	DEFINE_EXCEPTION(NoKleinbergRndNodeAvailable)


	template <  class Node >
	class KleinbergProtocol
	{
	public: //basic typedefs
		typedef std::vector<Node> Nodes;
		typedef typename Node::Point  Point;



	private:	

		const unsigned int m_ui_ViewSize ;		
		const unsigned int m_ui_UpdateSize ;		
		Nodes m_o_View;
		Nodes m_o_LastSent;
	

		typedef int ProbCoeff;
		typedef std::pair< const Node* , ProbCoeff> NodeCoeff;
		typedef std::list< NodeCoeff> NodeCoeffs;
		void _createProbTable(	const Node& _thisNode,
								const Node& _peer,
								NodeCoeffs& _table,
								int& _iTotalCoeff);

		
	public:		


		KleinbergProtocol( const Nodes& _view,
							unsigned int _i_updateSize,
							unsigned int _i_ViewSize);

		
		

		bool         bHasNodes()const { return !m_o_View.empty();}
		const Nodes& getView() const {return m_o_View; }
		Nodes&       getView()        {return m_o_View; }

		Node getRndPeer() const;

		void setPosition( const Point& _point)	{	m_o_thisNode.setPoint(_point) ;}

		void selectToSend(  const Node& _thisNode,   const Node& _addrTo,    Nodes& );
		void selectToKeep(  const Node& _thisNode,   const Node& _addrFrom,	 const Nodes& newStuff);


	

	};

	

	//-------------------
	template < class Node>
	inline KleinbergProtocol<Node>::KleinbergProtocol( 				
			const Nodes& _view,
			unsigned int _i_updateSize,
			unsigned int _i_ViewSize		
		)
		:
		m_ui_ViewSize       ( _i_ViewSize  ),
		m_ui_UpdateSize ( _i_updateSize ),			
		m_o_View( _view)
		
		
	{		
		LOG_TABLE( "Current Kleinberg View", m_o_View );	
	}
	
	

	//-------------------
	template < class Node>
	inline Node KleinbergProtocol< Node>::getRndPeer() const 
	{				

		if ( m_o_View.empty() )
		{
			THROW(NoKleinbergRndNodeAvailable,"View is empty");
		}
		return m_o_View[ Tools::rndNumber( m_o_View.size() ) ];
	}
	//------------------
	template< class Node >
	inline void KleinbergProtocol<Node>::selectToKeep( const Node& _thisNode, const Node& _addrFrom, const Nodes&  _updates	)
	{
		// create a set of unique Nodes from view 
		typedef std::set< Node > tViewSet;
		tViewSet viewSet( m_o_View.begin(), m_o_View.end() );
		//like the View should NOT have duplicates
		assert( viewSet.size() == m_o_View.size() );


		// apply updates to view...IF not in view then add ELSE refresh
		typedef std::vector< Node > tVecNode;
		for ( tVecNode::const_iterator itUpdate = _updates.begin() ; itUpdate != _updates.end(); itUpdate++)
		{
			tViewSet::iterator itView =	viewSet.find( *itUpdate );
			if ( itView != viewSet.end() )
			{	// Update in View

				if ( itUpdate->fresher( *itView) )
				{
					*itView = *itUpdate;
				}				
			}
			else
			{	// Update NOT present in View
				viewSet.insert( *itUpdate);
			}		
		}


		// Now remove all the surplus Nodes ( using SentLast nodes )
		tVecNode::const_iterator itLastSent = m_o_LastSent.begin();
		while ( viewSet.size() > m_ui_ViewSize  &&  itLastSent != m_o_LastSent.end() )
		{		
			viewSet.erase( *itLastSent );
			itLastSent++;
		}
		

		// assign NEW view to the OLD view 
		m_o_View.clear();
		m_o_View.assign( viewSet.begin(), viewSet.end() );

		//age the nodes
		std::for_each( m_o_View.begin(), m_o_View.end(), std::mem_fun_ref(&Node::staler) );

	}

	//------------------
		template< class Node >
		inline void KleinbergProtocol<Node>::_createProbTable(	
										const Node& _thisNode,
										const Node& _peer,	
										NodeCoeffs& _table,		
										int& _iTotalCoeff		)
		{
			// - (first avoid considering the peer node... must be absent from table)
			// - work out float coeffs for each node, multiple and store in NodeCoeffs
			// - add all NodeCoeff integers to the total
			// ( this is v.mildly inprecise but acceptable and fairly efficient)
			// .. that's all folks.. easy as pie ( a little inprecise but this is acceptable)

			//get Node float coefficents			
			for ( Nodes::const_iterator it= m_o_View.begin() ; it!= m_o_View.end() ; it++)
			{
				if ( ! it->samePeer(_peer)  ) // do not add peer to sendables
				{
					const Node::Distance DIST = it->getPoint().getDistance( _thisNode.getPoint() );	
					const int iNodeCoef = (int) DIST * DIST; // further is better (we want those as update)
					_table.push_back(  NodeCoeff( &(*it) , iNodeCoef ) );
					_iTotalCoeff += iNodeCoef;					
				}		
			}
		}
		//----------------------
		
	template< class Node >
	void  KleinbergProtocol<Node>::selectToSend( const Node& _thisNode, const Node&    _peer,	Nodes&   _updates  ) 
	{
		/*
		Algorithm
		---------
		- remove Peer
		-  get NodeCoeff table ( the more distant a node, the HIGHER its coeff )
		- while (updates not right size)
		- using a rnd offset number from 0..coeff Total , find the node corresponding to the offset
		-..then REMOVE the node .. reduce the cop
		
		*/
		//bool bHasUpdates = false;

		//reset update to hold copy of view
		_updates.clear();		
	

		// create prob table
		NodeCoeffs NodeProbTable;
		int iTotalCoeff = 0;
		_createProbTable( _thisNode, _peer,   NodeProbTable, iTotalCoeff );

		if ( ! NodeProbTable.empty() )
		{
			const int OriginalTableSize = NodeProbTable.size();
			
			// load updates with all nodes in prob table 
			const int LEAVE_PLACE_FOR_SELF = 1; //..... ..Solution C - 1
			while (   ! ( NodeProbTable.empty() || _updates.size()== m_ui_UpdateSize - LEAVE_PLACE_FOR_SELF )    )
			{
				// new Rnd number, goto start of NodeTable... search again
				assert( iTotalCoeff > 0 );//... this fails when distance(node1,thisNode) == 0
				int iRndOffset = Tools::rndNumber( iTotalCoeff );
				NodeCoeffs::iterator itNodeProb = NodeProbTable.begin();
				bool bChosen = false;

				while( !bChosen  && itNodeProb != NodeProbTable.end() )
				{
					if (  iRndOffset < itNodeProb->second )
					{
						//bHasUpdates = true;
						bChosen = true;
						_updates.push_back( *itNodeProb->first );
						iTotalCoeff -= itNodeProb->second ;
						NodeProbTable.erase( itNodeProb );					
					}
					else
					{
						iRndOffset  -= itNodeProb->second ;
						itNodeProb++;
					}

				}
				if ( !bChosen)
				{
					THROW(KleinBergAlgoException, "Could not chose Node!");
				}			

			}
		

			//age the nodes			
			std::for_each( m_o_View.begin(), m_o_View.end(), std::mem_fun_ref(&Node::staler)   );
		}

		m_o_LastSent = _updates;


		//Fix: Booting problem ..Solution C - 1
		// Nodes were NOT getting coordinates because selectToSend NEVER included
		// 'this' node.. AND.. only 'this' node KNOWs about 'this' node's coordinates
		//...therefore forcefully add it at end..
		_updates.push_back( _thisNode);
		LOG_TABLE( "Kleinberg sendables", _updates );



		//return bHasUpdates;
	}

}

#endif







