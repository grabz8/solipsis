#include <UDPPacket.h>
#include <UDPSocket.h>
#include <vector>
#include <iterator>
#include <iomanip>

#include "CppTest.h"
#include "IPNode.h"



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


/*            ------SOCKET-----------
UDPSocket();
	~UDPSocket();

		void send( const UDPPacket&) ;
		UDPPacket  recv() ;

		void bind(   unsigned short  _port);

		template < class BufferType>
		void getSockOpt(  int _level, int _optName, BufferType& _buffer )
		{		
			_getSockOpt( m_h_socket, _level,  _optName,  _buffer );
		}

		operator SOCKET() { return m_h_socket;}
*/


/*
	Test 1: Simply get a socket to talk to another...no sweat
*/
void testCanTalk()
{
	const unsigned short TO_PORT = 2000;
	const unsigned short FROM_PORT = 3000;
	const unsigned int   TO_ADDR = ::inet_addr("127.0.0.1");

	//prepare Listen socket
	IP::UDPSocket  ReadSocket; 	
	ReadSocket.bind(TO_PORT);

	//prepare package	
	int intArr[]={ 1,2,3,4,5,6,7,8};					
	std::vector<int> vecInts( intArr, intArr + sizeof( intArr) / sizeof(int) );
	
	P2P::UDPPacket Written( TO_PORT, TO_ADDR, FROM_PORT, P2P::UDPPacket::e_KLEINBERG_MESSAGE );
	Written.setObjects( vecInts);
	

	IP::UDPSocket::send( 	Written );
	P2P::UDPPacket Read = ReadSocket.recv();

	
	ASSURE( Read == Read );
	ASSURE( Read.sameData( Written) );			
	ASSURE( Written.getCallBackPort() == Read.getRemotePort()  );
	ASSURE( TO_ADDR == Read.getRemoteAddr()  );

		

}

//---------------------------------------------------------------------------------------

void testCanTalkTooMuch()
{
	const unsigned short LOCAL_PORT = 2000;
	const unsigned int   LOCAL_ADDR = ::inet_addr("127.0.0.1");

	//prepare Listen socket
	IP::UDPSocket  ReadSocket; 	
	ReadSocket.bind(LOCAL_PORT);

	//prepare package	
	typedef std::vector<int> tVecInts;
	tVecInts vecInts;
	std::generate_n( std::back_insert_iterator< tVecInts>(vecInts) ,  250,  rand );
	
	P2P::UDPPacket Written( LOCAL_PORT, LOCAL_ADDR, 0, P2P::UDPPacket::e_KLEINBERG_MESSAGE );
	Written.setObjects( vecInts);
	
	// Reading and writing non-stop
	const int TIMES_TO_RUN = 100;
	for ( int i = 0; i < TIMES_TO_RUN; i++ )
	{
		IP::UDPSocket::send( 	Written );
		P2P::UDPPacket Read = ReadSocket.recv();
		ASSURE( Read.sameData( Written) );			
	}



	// Burst reading and writing .. seems to only support this amount reliably
	const int TIMES_TO_BURST =6;
	for ( int i = 0; i < TIMES_TO_BURST; i++ )
	{
		IP::UDPSocket::send( 	Written );
	}

	for ( int i = 0; i < TIMES_TO_BURST; i++ )
	{
		P2P::UDPPacket Read = ReadSocket.recv();
		ASSURE( Read.sameData( Written) );			
	}



	
}
//-------------------------------------------------


/*
	Test 1: Simply get a socket to talk to another...no sweat
*/
static void testCanSendNodes()
{
	using P2P::IPNode;

	const unsigned short TO_PORT = 2000;
	const unsigned short FROM_PORT = 3000;
	const unsigned int   TO_ADDR = ::inet_addr("127.0.0.1");

	//prepare Listen socket
	IP::UDPSocket  ReadSocket; 	
	ReadSocket.bind(TO_PORT);

	//prepare package	
						
	std::vector<IPNode> Nodes;
	Nodes.push_back( IPNode(   0, 1, 2, IPNode::Point( 1.0f, 2.0f, 3.0f ) ) );
	Nodes.push_back( IPNode(   3, 4, 5, IPNode::Point( 100.0f, 200.0f, 3.0001f ) ) );

	
	P2P::UDPPacket Written( TO_PORT, TO_ADDR, FROM_PORT, P2P::UDPPacket::e_KLEINBERG_MESSAGE );
	Written.setObjects( Nodes);
	

	IP::UDPSocket::send( 	Written );
	P2P::UDPPacket Read = ReadSocket.recv();

	
	ASSURE( Read == Read );
	ASSURE( Read.sameData( Written) );			
	ASSURE( Written.getCallBackPort() == Read.getRemotePort()  );
	ASSURE( TO_ADDR == Read.getRemoteAddr()  );

	std::vector<IPNode> ReadNodes;
	Read.getObjects( ReadNodes );

	ASSURE( ReadNodes == Nodes );

		

}


static void networkByteReorder()
{
	using namespace IP;

	{
	unsigned int from=0x12345678;
	unsigned int network = hton( from);
	unsigned int to=ntoh(network);
	ASSURE( from == to);
	}
	

	{
	unsigned short from=0x1234;
	unsigned short network = hton( from);
	unsigned short to=ntoh(network);
	ASSURE( from == to);
	}

	{
	unsigned char from=0x34;
	unsigned char network = hton( from);
	unsigned char to=ntoh(network);
	ASSURE( from == to);
	}


/*  We don't particuliarly want this to compile.. only deal with unsigned values
	{
	short from=0x1234;
	short network = hton( from);
	short to=ntoh(network);
	ASSURE( from == to);
	}
*/
}

static void streamFloats()
{
	//normally this is a banal operation.. but due to rounding off problems
	// there can be problems
	std::stringstream strm;

	float f1= 151.264f;
	float f2= 14.856f;

	strm << f1 << ',' <<f2;

	float f3,f4;

	strm >> f3 ;
	strm.ignore(1);
	strm>> f4;
	ASSURE( f1 == f3);
	ASSURE( f2 == f4);
	
}
























ADD_TEST(testCanTalk,Socket, "Simple write and read")
ADD_TEST(testCanTalkTooMuch,Socket, "Loads written and read")
ADD_TEST(testCanSendNodes,Socket,"can serialise IPNodes correctly")
ADD_TEST(networkByteReorder,Socket,"can network re-order bytes correctly")
ADD_TEST(streamFloats,Socket,"merely stream floats")