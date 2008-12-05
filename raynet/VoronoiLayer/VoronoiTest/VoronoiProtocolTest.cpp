
//#include <IpNode.h>
#include <VoronoiProtocol.h>


#include "CppTest.h"
#include "Point3D.h"






////------------------------TEST NODE --------------------------------------------

const int MY_NODE_ALSO_SENT =1;

using P2P::Point3D;

struct MyNode
{
	unsigned short m_us_port;
	int m_i_addr;
		
	Point3D m_o_position;
	typedef Point3D Point;
	typedef Point3D::Distance Distance;


	MyNode(  const Point3D& _pos) :m_us_port(0), m_i_addr(0), m_o_position(_pos){}

	MyNode(unsigned short _us_port, int _i_addr, const Point3D& _pos) 
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
		_out <<"addr:" << _n.m_i_addr <<", port:" << _n.m_us_port << ", pos:" << _n.m_o_position;
		return _out;
	}

	Distance getDistance( const MyNode& _peer ) const 
	{ 
		return _peer.getPoint().getDistance( m_o_position);
	};


	void logWrite(  std::ostream& _out )const
	{
		// don't bother writing to log file for this test
	}

	const Point3D& getPoint() const{ return m_o_position;}
};





/////-----------------------TestSelectToSend---------------------------------------
/*
	Purpose: To prove that the updates:
			- exclude the peer nodes that recv's nodes
			- have the correct quantity
			- that they include ThisNode every time
*/

void TestSelectToSend()
{
	typedef std::vector<MyNode> Nodes;

	// setup objects etc

	const int _UPDATE_SIZE = 2;
	const int _VIEW_SIZE = 4;	

	MyNode THIS_NODE  ( 1, 1, Point3D(10,10,10) );
	MyNode NEARER    ( 2, 2, Point3D(9,9,9) );
	MyNode QUITE_NEAR ( 3, 3, Point3D(8,8,8) );
	MyNode PEER       ( 4, 4, Point3D(7,8,7) );
	MyNode FARER        ( 5, 5, Point3D(15,15,15) );
	MyNode V_FAR      ( 6, 6, Point3D(1000,1000,1000) );
	
	
	Nodes view;	
	view.push_back(  	NEARER		);
	view.push_back( QUITE_NEAR); 
	view.push_back(  FARER  ); 
	view.push_back(  PEER    ); 


	P2P::VoronoiProtocol<MyNode> o_protocol(	view, _UPDATE_SIZE,	_VIEW_SIZE );
								

	int ToRun = 1000;
	int i_nearer = 0;
	int i_quite_near = 0;
	int i_farer = 0;

	while ( ToRun > 0 )
	{
		Nodes updates;
		o_protocol.selectToSend( THIS_NODE, PEER, updates );

		//right size		
		ASSURE(  updates.size() == _UPDATE_SIZE );

		// Do not send peer itself
		ASSURE( std::find( updates.begin(), updates.end(), PEER) == updates.end()    );

		//includes this
		ASSURE( std::find( updates.begin(), updates.end(), THIS_NODE) != updates.end()    );
		
		if ( std::find( updates.begin(), updates.end(), NEARER ) != updates.end() )
			i_nearer++;
		if ( std::find( updates.begin(), updates.end(), QUITE_NEAR ) != updates.end() )
			i_quite_near++;
		if ( std::find( updates.begin(), updates.end(),  FARER) != updates.end() )
			i_farer++;		

		ToRun--;
	}

	// for sure, far ones must be chosen MORE than near ones for updates

	
	ASSURE(	i_nearer +  i_quite_near + i_farer == ToRun );
	ASSURE(	i_nearer < ToRun /2 );
	ASSURE(	i_quite_near < ToRun /2 );
	ASSURE(	i_farer  < ToRun /2 );


}


/////-----------------------TestSelectToSend---------------------------------------



void testhandlesFewUpdates()
{
typedef std::vector<MyNode> Nodes;

	// setup objects etc

	const int _UPDATE_SIZE = 2;
	const int _VIEW_SIZE = 4;	

	MyNode THIS_NODE  ( 1, 1, Point3D(1,1,1) );
	MyNode PEER       ( 4, 4, Point3D(4,3,3) );	
	MyNode FARER        ( 5, 5, Point3D(6,6,6) );

	
	/*
		Protocol( 	const std::vector< Node >& _View,
					int _updateSize,
					int _viewSize,				
					const Node& _node );	
	*/

	// Peer and another Host : only one update
	{
		Nodes smallView;	
		smallView.push_back( FARER     );
		smallView.push_back( PEER);
		P2P::VoronoiProtocol<MyNode> o_protocol(	smallView,
													_UPDATE_SIZE,
													_VIEW_SIZE);	

			
		
		Nodes updates;
		o_protocol.selectToSend( THIS_NODE, PEER, updates );
		ASSURE ( std::find( updates.begin(), updates.end(), FARER  ) != updates.end() );
		ASSURE ( std::find( updates.begin(), updates.end(), THIS_NODE  ) != updates.end() );
		ASSURE(  updates.size() == 2 );
	}

	// Peer and Host : no updates but myself
	{
		Nodes smallView;			
		smallView.push_back( PEER);
		P2P::VoronoiProtocol<MyNode> o_protocol(   smallView, _UPDATE_SIZE, _VIEW_SIZE  );
		
		Nodes updates;
		o_protocol.selectToSend( THIS_NODE, PEER, updates );		
		ASSURE ( std::find( updates.begin(), updates.end(), THIS_NODE  ) != updates.end() );
		ASSURE(  updates.size() == 1 );
	}
	

	//right size
	

}


















/////-----------------------TestSelectToKeep---------------------------------------













static void TestSelectToKeep3D()
{
	/*		
		PURPOSE :  Ensure that the 3D aspect is working
		METHOD: -Create first and second layer of peers
				-Update with second layer nodes
				- ensure that all third layer is absent and first is retained
	*/

	typedef std::vector<MyNode> Nodes;

	// setup objects etc

	const int _UPDATE_SIZE = 4;
	const int _VIEW_SIZE = 6;	

	MyNode THIS_NODE         ( 1, 1, Point3D(10,10,10) );
	MyNode NOT_IN_VIEW_PEER  ( 6, 6, Point3D(3,3,3) );	


	Nodes firstLayer;
	firstLayer.push_back(  MyNode(1,1, Point3D(11,10,10) )    );
	firstLayer.push_back(  MyNode(1,2, Point3D(10,11,10)  )   );
	firstLayer.push_back(  MyNode(1,3, Point3D(10,10,11)  )   );

	Nodes secondLayer;
	secondLayer.push_back(  MyNode(2,1, Point3D(14,10,10)   )  );
	secondLayer.push_back(  MyNode(2,2, Point3D(10,14,10)   )  );
	secondLayer.push_back(  MyNode(2,3, Point3D(10,10,14)  )   );


	Nodes thirdLayer;
	thirdLayer.push_back(  MyNode(3,1, Point3D(16,10,10)  )   );
	thirdLayer.push_back(  MyNode(3,2, Point3D(10,16,10)  )   );
	thirdLayer.push_back(  MyNode(3,3, Point3D(10,10,16)  )   );


	Nodes view;	
	view.reserve(_VIEW_SIZE);
	view.insert( view.end(), firstLayer.begin(), firstLayer.end() );	
	view.insert( view.end(), thirdLayer.begin(), thirdLayer.end() );

	{
		P2P::VoronoiProtocol<MyNode> o_protocol(	view, _UPDATE_SIZE,	_VIEW_SIZE );

		Nodes updates;
		updates.insert( updates.end(), secondLayer.begin(), secondLayer.end() );
	
		Nodes emptyKleinberg;
		o_protocol.selectToKeep(  THIS_NODE, NOT_IN_VIEW_PEER , emptyKleinberg, updates);	

		const Nodes& View = o_protocol.getView();

		//all third layer removed	
		Nodes::iterator it ;
		for ( it= thirdLayer.begin(); it != thirdLayer.end() ; it++)
		{
			ASSURE( std::find( View.begin(), View.end(), *it ) == View.end() );
		}

		//all first layer retained
		for ( it = firstLayer.begin(); it != firstLayer.end() ; it++)
		{
			ASSURE( std::find( View.begin(), View.end(), *it ) != View.end() );
		}

		//all second layer retained
		for ( it = secondLayer.begin(); it != secondLayer.end() ; it++)
		{
			ASSURE( std::find( View.begin(), View.end(), *it ) != View.end() );
		}
	}

	

}









ADD_TEST(TestSelectToSend,Voronoi, "Test simple selectToSend works")
ADD_TEST(TestSelectToKeep3D,Voronoi, "Test 3D selectToKeep works")
ADD_TEST(testhandlesFewUpdates,Voronoi, "Test update cornercases")




/*
void Test


ADD_TEST(TestDataBlock,TOOLS,"Test datablocks")**/