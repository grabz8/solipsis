#include <UDPPacket.h>
#include <IpNode.h>

#include <iostream>

#include "CppTest.h"





/*  ---------PACKET---------------
	
		UDPPacket(  unsigned short _port,
					unsigned int _addr,
					const  char* _data, 
					int _size) :  			

		UDPPacket(unsigned short _port,
					unsigned int _addr,
					Type _type) :  

		const  char* getBuffer()const { return m_arr_packet;}
		
		int getRemoteAddr()const { return m_i_RemoteAddr;}
		
		unsigned short getRemotePort() const          { return m_us_port;}

		int size() const{ return m_arr_packet.size();}

		Type getType() const ;
		
		void setObjects( const container& _objects);
		
		void getObjects(  container& _objects ) const;




*/


/*
	Purpose: Test packets correctly de-serialise and serialise
	Method: - load a packet
			- write packet to buffer
			- use another packet to read buffer
			- ensure that values written are values read (simple)
*/



static void PacketTypes()
{

	P2P::UDPPacket Written(1, 2, 3, P2P::UDPPacket::e_KLEINBERG_MESSAGE);

	int intArr[]={ 1,2,3,4,5,6,7,8};					
	std::vector<int> vecInts( intArr, intArr + sizeof( intArr) / sizeof(int) );
	Written.setObjects( vecInts);

	P2P::UDPPacket Read(4, 4, 4, P2P::UDPPacket::e_KLEINBERG_RESPONSE);
	ASSURE( Read != Written);

	//assignment
	Read = Written;
	ASSURE( Read == Written);


		//Copying Works
	P2P::UDPPacket  Copyfrom( Written );	
	ASSURE( Written == Copyfrom)
	
}





/*



*/
static void testPackets(){



	using namespace P2P;
	// Compare ports etc
	const short REMOTE_PORT = 3124;
	const short CALLBACK_PORT = 700;
	const int REMOTE_ADDR = 0x12345678;
	const UDPPacket::Type PKTYPE = UDPPacket::e_KLEINBERG_RESPONSE;
	
	// create from and from2 packet
	P2P::UDPPacket  from(  REMOTE_PORT,	REMOTE_ADDR, CALLBACK_PORT,  PKTYPE	) ;	
	{
		P2P::UDPPacket  from2 (  REMOTE_PORT,	REMOTE_ADDR, CALLBACK_PORT, PKTYPE	) ;	
	
		ASSURE(  from.getRemoteAddr()  == from2 .getRemoteAddr()  );
		ASSURE(  from.getRemotePort()  == from2 .getRemotePort()  );
		ASSURE(  from.getType()  == from2 .getType() &&  from2 .getType() == PKTYPE);
	}
	
	


	// write complex data to from packet
	P2P::IPNode stuffArr[]={ 
				P2P::IPNode(0xFFFF,0xEE1E,0xEEEE , P2P::IPNode::Point(1.0f,2.5f,0.008f)),
				P2P::IPNode( 0xEE2E , 0xFF5F, 0xBBB)  
	};
	std::vector< P2P::IPNode > vecFrom( stuffArr, &stuffArr[2]);
	from.setObjects( vecFrom);

	//get from.buffer
	const char * BUFFER = from.getBuffer();
	int SIZE = from.size();	

	//use from.buffer to make a "to" packet 
	P2P::UDPPacket  to2   (  CALLBACK_PORT, REMOTE_ADDR, BUFFER, SIZE	) ;		
	std::vector< P2P::IPNode> vecTo  ;
	
	// check objects are the same
	to2.getObjects(  vecTo  ); 
	ASSURE( to2.size() == from.size() );
	ASSURE(  vecTo.size() == vecFrom.size() ); 

	for ( unsigned int p=0; p < vecFrom.size(); p++ )
	{		
		ASSURE( vecTo[p] == vecFrom[p] ); 
	}
	
	ASSURE(   from.getType()  == to2.getType() ) ;
	ASSURE(  from.sameData( to2) );
	


}



///////////////////////////////////////////////////////////////////

/*
DataBlock( const  char* _data, int size) 


		
		void setAt( unsigned int _pos, const T& _t) 		
		T getAt( unsigned int _pos) const  	
		void writeAllAt (  unsigned int _pos, const container& _objects)		
		void readAllAt (  unsigned int _pos, container& _objects, int _quOfObjects) const
		
		operator const  char*() const{ return m_buf; }

		int size() const{  return m_i_size; }
*/
void TestDataBlock()
{
	Tools::DataBlock<5000> block1, block2;
	const short FIRST = 0x1F00;
	const int   SECOND= 0xFFFF ;
	const short THIRD = 0x01;
	const unsigned short FOURTH = 0xFFFF;
	const unsigned int FIFTH = 0xEEEEFFFF ;

	block1.setAt(0,FIRST );
	block1.setAt(2,SECOND);
	block1.setAt(6,THIRD);
	block1.setAt(8,FOURTH);
	block1.setAt(10,FIFTH);






	ASSURE( block1.getAt<short>(0 ) ==FIRST);
	ASSURE( block1.getAt<int>(2 ) == SECOND );
	ASSURE( block1.getAt<short>(6 ) == THIRD );
	ASSURE( block1.getAt<unsigned short>(8 ) == FOURTH);
	ASSURE( block1.getAt<unsigned int>(10 ) == FIFTH );


	int arr[]= { 1,2,3,4,5,0xFFFF,-44,999999,0};
	std::vector< int > myVec1 ( &arr[0], &arr[  sizeof(arr)/sizeof(int) ] );

	block2.writeAllAt( 1, myVec1);


	
	std::vector< int > myVec2;
	myVec2.reserve( myVec1.size() );
	block2.readAllAt( 1, myVec2, myVec1.size());

	
	ASSURE( myVec2.size()  == myVec1.size() );

	for ( unsigned int i = 0 ; i < myVec1.size(); i++)
	{
		ASSURE( myVec2[i]  == myVec1[i] );
	}
}










ADD_TEST(PacketTypes,Packet, "Test packet copying,assigning")
ADD_TEST(testPackets,Packet, "Test P2P Packets")
ADD_TEST(TestDataBlock,TOOLS,"Test datablocks")