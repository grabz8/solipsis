
//#include <IpNode.h>
#include <KleinBergProtocol.h>


#include "CppTest.h"






////------------------------TEST NODE --------------------------------------------

const int MY_NODE_ALSO_SENT =1;


struct MyNode
{
	unsigned short m_us_port;
	int m_i_addr;
	typedef int Distance;

	struct Point{
		int m_val;
		Point( int _val):m_val(_val){}
		operator int(){ return m_val;}
		Distance getDistance( const Point& _other)const
		{
			return m_val - _other.m_val;
		}
	};
	
	Point m_o_position;

	


	MyNode( int _pos) :m_us_port(0), m_i_addr(0), m_o_position(_pos){}

	MyNode(unsigned short _us_port, int _i_addr, int _pos) 
		:m_us_port(_us_port ),
		m_i_addr( _i_addr),
		m_o_position(_pos)
	{}

	

	friend bool operator == ( const MyNode& _l, const MyNode& _r)
	{
		return _l.m_us_port == _r.m_us_port &&
			_l.m_i_addr == _r.m_i_addr;
	}
	friend bool operator != ( const MyNode& _l, const MyNode& _r)
	{
		return ! ( _l ==_r);
	}
    bool samePeer( const MyNode& _other) const
	{
		return _other.m_i_addr == m_i_addr && _other.m_us_port ==  m_us_port;
	}
	bool fresher( const MyNode& _l)const
	{
		return true;
	}
	void staler()const
	{
	}

	friend bool operator < ( const MyNode& _lhs, const MyNode& _rhs)
	{
		if (_rhs.m_i_addr == _lhs.m_i_addr )
			return _rhs.m_us_port >  _lhs.m_us_port;
		else 
			return _rhs.m_i_addr >  _lhs.m_i_addr;
	}

	friend std::ostream& operator << ( std::ostream& _out, const MyNode& _n)
	{
		_out <<"addr:" << _n.m_i_addr <<", port:" << _n.m_us_port << ", pos:" << _n.m_o_position.m_val;
		return _out;
	}
	void logWrite(  std::ostream& _out )const
	{
		// don't bother writing to log file for this test
	}

	const Point& getPoint() const{ return m_o_position;}
};





/////-----------------------TestSelectToSend---------------------------------------


void TestSelectToSend()
{
	typedef std::vector<MyNode> Nodes;

	// setup objects etc

	const int _UPDATE_SIZE = 2;
	const int _VIEW_SIZE = 4;
	const int _HERE=10;
	const int _NEAR=8;
	const int _NOT_FAR=6;
	const int _FAR=2;

	MyNode ThisNode(  0, 0, _HERE );
	MyNode Peer( 3, 4, 10);

	Nodes view;	
	view.push_back( MyNode( 1, 2, _NEAR)     );
	view.push_back( Peer); 
	view.push_back( MyNode( 5, 6, _NOT_FAR)   ); 
	view.push_back( MyNode( 7, 8, _FAR)     ); 


	P2P::KleinbergProtocol<MyNode> o_protocol(	view, _UPDATE_SIZE,	_VIEW_SIZE );
								

/*
 	const std::vector< Node >& _View,
					int _updateSize,
					int _viewSize,				
					const Node& _node );	
*/


	int ToRun = 1000;

	int i_near = 0;
	int i_peer = 0; 
	int	i_not_far = 0;
	int i_far = 0;


	while ( ToRun > 0 )
	{
		Nodes updates;
		o_protocol.selectToSend(  ThisNode, Peer, updates );

		//right size
		
		ASSURE(  updates.size() == _UPDATE_SIZE + MY_NODE_ALSO_SENT);

		// Do not send peer itself
		ASSURE( std::find( updates.begin(), updates.end(), Peer) == updates.end()    );

		if ( std::find( updates.begin(), updates.end(), view[0] ) != updates.end() )
			i_near++;
		if ( std::find( updates.begin(), updates.end(), view[1] ) != updates.end() )
			i_peer++;
		if ( std::find( updates.begin(), updates.end(), view[2] ) != updates.end() )
			i_not_far++;
		if ( std::find( updates.begin(), updates.end(), view[3] ) != updates.end() )
			i_far++;

		ToRun--;
	}

	// for sure, far ones must be chosen MORE than near ones for updates
	ASSURE ( i_peer ==0 )
	ASSURE ( i_far > i_not_far )	
	ASSURE ( i_not_far > i_near)


}


/////-----------------------TestSelectToSend---------------------------------------



void testhandlesFewUpdates()
{
	typedef std::vector<MyNode> Nodes;

	// setup objects etc

	const int _UPDATE_SIZE = 2;
	const int _VIEW_SIZE = 4;
	

	MyNode ThisNode(  0, 0, 1 );
	MyNode Peer( 3, 4, 10);
	MyNode SomeNode( 3, 5, 10);

	/*
		Protocol( 	const std::vector< Node >& _View,
					int _updateSize,
					int _viewSize,				
					const Node& _node );	
	*/

	// Peer and another Host : only one update
	{
		Nodes smallView;	
		smallView.push_back( SomeNode     );
		smallView.push_back( Peer);
		P2P::KleinbergProtocol<MyNode> o_protocol(  smallView,
											_UPDATE_SIZE,
											_VIEW_SIZE);	

			
		
		Nodes updates;
		o_protocol.selectToSend( ThisNode, Peer, updates );
		ASSURE ( std::find( updates.begin(), updates.end(), SomeNode  ) != updates.end() );
		ASSURE(  updates.size() == 1 + MY_NODE_ALSO_SENT );
	}

	// Peer and Host : no updates
	{
		Nodes smallView;			
		smallView.push_back( Peer);
		P2P::KleinbergProtocol<MyNode> o_protocol(   smallView, _UPDATE_SIZE, _VIEW_SIZE  );
		
		Nodes updates;
		o_protocol.selectToSend( ThisNode, Peer, updates );		
		ASSURE(  updates.size() == MY_NODE_ALSO_SENT  );
	}
	

	//right size
	

}


/////-----------------------TestSelectToKeep---------------------------------------


/*
template < class AlgoStrategy, class Node >
class Protocol
{


Protocol( const AlgoStrategy&, const std::vector< Node >& _initialView);	
const Node& getRndNode()const;
void selectToSend(  Node& _addrTo, std::vector<Node>& );
void selectToKeep(  const Node& _addrFrom, const std::vector<Node>& newStuff);
std::vector< Node > getView() const;

*/

void TestSelectToKeep()
{
	using namespace P2P;
	typedef std::vector<MyNode> Nodes;

	// Prepare objects for test	

	const int HERE = 10;

	MyNode Peer(1000,1000, 0 );
	MyNode ThisNode(  1, 1, HERE );

	MyNode TouchingNode( 2, 2, 11);
	MyNode NearNode( 3, 3, 12);
	MyNode NotSoNearNode( 4, 4, 15);
	MyNode QuiteFarNode( 5, 5, 20);
	MyNode FarNode( 6, 6,30);
	MyNode VeryFarNode(7, 7, 40);

	//for ( int TimesToRun = 0; TimesToRun < 100; TimesToRun++)
	//{

		//create view
		const int _UPDATE_SIZE = 2;
		const int _VIEW_SIZE = 4;
		Nodes view;	
		view.push_back( Peer);
		view.push_back( TouchingNode ); 
		view.push_back( NearNode ); 
		view.push_back( NotSoNearNode ); 
		std::sort( view.begin(), view.end() );

		// place view in protocol		
		
		P2P::KleinbergProtocol<MyNode> o_protocol(  view,  _UPDATE_SIZE, _VIEW_SIZE  );

		ASSURE( o_protocol.getView() == view );	


		// get last sent data..
		Nodes lastSent;
		o_protocol.selectToSend( ThisNode, Peer, lastSent );	

		// C+1 : A fix applied to selectToSend that includes ITSELF in sent list.. need to be removed therefore
		 //      so that it WILL NOT be added to view and break the logic
		lastSent.erase( 
						std::remove( lastSent.begin(), lastSent.end(), ThisNode ),
						lastSent.end()
						);




		//Test 1: Assure that applying LASTSENT as UPDATE changes nothing
		// Reason: Because all the updates will be duplicates and thus ALWAYS ignored
		o_protocol.selectToKeep( ThisNode, Peer, lastSent);	

		Nodes ViewAfterRecvLastSent = o_protocol.getView();

		


		std::sort( ViewAfterRecvLastSent.begin(), ViewAfterRecvLastSent.end() );

		ASSURE( ViewAfterRecvLastSent == view)




			//Test 2: Ensure SelectToKeep retains new UPDATES
			// Reason: Previous view will send out TWO nodes (excluding PEER). Probably NotSoNear and NearNode.
			//         This being the case, in receiving an UPDATE( FarNode and VeryFarNode), it will replace
			//         NotSoNear and Near nodes with the Far ones ( seems bizarre but algo permits this).
			//         WHATEVER the nodes sent, Far nodes WILL be taken on board ..EVERYTIME
			//         ( as NOT on LastSent list ).


			//apply update
			Nodes update;	
		update.push_back( FarNode    );
		update.push_back( VeryFarNode ); 		
		o_protocol.selectToKeep( ThisNode, Peer, update);	

		Nodes ViewAfterUpdate =  o_protocol.getView();
		ASSURE ( std::find(ViewAfterUpdate.begin(), ViewAfterUpdate.end(), FarNode  ) != ViewAfterUpdate.end() )
		ASSURE ( std::find(ViewAfterUpdate.begin(), ViewAfterUpdate.end(), VeryFarNode  ) != ViewAfterUpdate.end() )


			// Test 3: Ensure that selectToSend MORE THAN OFTEN rejects the FarNode and VeryFar node
			//
		Nodes SendsFarNodes;
		o_protocol.selectToSend( ThisNode, Peer, SendsFarNodes );	
		ASSURE ( std::find(SendsFarNodes.begin(), SendsFarNodes.end(), FarNode  ) != SendsFarNodes.end() )
		ASSURE ( std::find(SendsFarNodes.begin(), SendsFarNodes.end(), VeryFarNode  ) != SendsFarNodes.end() )

			// Test 4 : Ensure, that after not wanting Far nodes, it takes on board BETTER nodes upon update
		Nodes BetterUpdate;	
		BetterUpdate.push_back( TouchingNode    );
		BetterUpdate.push_back( NearNode ); 
		o_protocol.selectToKeep(  ThisNode,Peer, BetterUpdate);	

		Nodes BetterView = o_protocol.getView();
		ASSURE ( std::find(BetterView.begin(), BetterView.end(), TouchingNode) != BetterView.end() )
		ASSURE ( std::find(BetterView.begin(), BetterView.end(), NearNode ) != BetterView.end() )
	//}

}















ADD_TEST(TestSelectToSend,Kleinberg, "Test select to send works")
ADD_TEST(TestSelectToKeep,Kleinberg, "Test select to keep works")
ADD_TEST(testhandlesFewUpdates,Kleinberg, "Test odd amount of updates")




/*
void Test


ADD_TEST(TestDataBlock,TOOLS,"Test datablocks")**/