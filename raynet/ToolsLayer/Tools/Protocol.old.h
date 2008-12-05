#ifndef _PROTOCOL_MAIN_FILE_H_
#define _PROTOCOL_MAIN_FILE_H_

#include <vector>
#include "Thread.h"
#include "P2PTools.h"
//#include "ConfFile.h"

#include "LogFile.h"

namespace P2P{


	/*
		CLASS PROTOCOL
		--------------
		Class Protocol is a thread boundary class. This means that this class acts 
		as a mediator between different threads. Simply put, it serialises access.

	*/


	template < template <class> class AlgoStrategy, class Node >
	class Protocol :  public Tools::ProtocolViewer<Node>	
	{

	
		std::vector< Node > m_o_View;
		std::vector< Node > m_o_LastSent;
		mutable Mutex m_o_lock;
		AlgoStrategy<Node> m_o_algo;
		Node m_o_thisNode;
		int m_i_UpdateSize;





	public:
		Protocol( 	const std::vector< Node >& _View,
					int _updateSize,
					int _viewSize,				
					const Node& _thisNode );	

		
		void selectToSend(  Node& _addrTo, std::vector<Node>& );
		void selectToKeep(  const Node& _addrFrom, const std::vector<Node>& newStuff);

	
	

	

	

	DEFINE_EXCEPTION(NoRndNodeAvailable)

	};
/*


*/


	//implementation
	//-------------
	template < template <class> class AlgoStrategy, class Node >
	inline Protocol<AlgoStrategy, Node>::Protocol(	
		const std::vector< Node >& _View,
		int _updateSize,
		int _viewSize,				
		const Node& _thisNode
		)

	:m_o_View( _View ),
	m_o_algo(  _thisNode  ,_updateSize, _viewSize), 
	m_o_thisNode( _thisNode),
	m_i_UpdateSize(_updateSize)
	{		
		LOG_TABLE( "Current View", m_o_View );
	}

	
	//-------------

	template < template <class> class AlgoStrategy, class Node >
	inline std::vector< Node > Protocol<AlgoStrategy, Node>::getView() const
	{
		LOCK(m_o_lock)
		return m_o_View;
		

	}
	//-------------


	//-------------

	template < template <class> class AlgoStrategy, class Node >
	inline void Protocol<AlgoStrategy, Node>::selectToSend( Node& _addrTo,  std::vector<Node>& _sendables)
	{
		LOCK(m_o_lock)

		//Fix: Booting problem ..Solution C + 1
		const int ADD_MY_OWN_NODE = 1; 
		_sendables.reserve(  m_i_UpdateSize + ADD_MY_OWN_NODE);

		// chose sendables and save 
		m_o_algo.selectToSend(_addrTo, m_o_View, _sendables) ;		
		m_o_LastSent = _sendables;


		//Fix: Booting problem ..Solution C + 1
		// Nodes were NOT getting coordinates because selectToSend NEVER included
		// 'this' node.. AND.. only 'this' node KNOWs about 'this' node's coordinates
		//...therefore forcefully add it at end..
		_sendables.push_back( m_o_thisNode);
		LOG_TABLE( "Sendables", _sendables );

		
	}

	//-------------

	template < template <class> class AlgoStrategy, class Node >
	inline void Protocol<AlgoStrategy, Node>::selectToKeep( const Node& _addrFrom, const std::vector<Node>& recvdStuff)
	{
		LOCK(m_o_lock)
		m_o_algo.selectToKeep( _addrFrom, m_o_LastSent, recvdStuff, m_o_View );		
		
		LOG_TABLE( "Current View", m_o_View );	
	}



}

#endif