#include <SocketReactor.h>
#include <UDPSocket.h>
#include "UDPPacket.h"
#include "CppTest.h"

//----------------REACTOR------------
/*  
class SocketReactor : public P2P::Thread
	{
		DEFINE_EXCEPTION(ReactorNotStopped)
		DEFINE_EXCEPTION(SelectFailed)

	public:
		
		SocketReactor( int _TimeOutIntervalSecs);
		~SocketReactor(void);

		// Handler funcs : throw exception if called when thread running
		void addRecvHandler(    SOCKET _sockethandle,  SocketHandler& _handler);
		void addTimeOutHandler(  SocketHandler& _handler);
		void clearHandlers( );
*/


const unsigned short LOCAL_PORT = 2000;
const unsigned int   LOCAL_ADDR = ::inet_addr("127.0.0.1");

//-----------------------------------------------------





/*
	PURPOSE: Ensure the reactor responds to read events
	METHOD: -func creates a recv socket
			-func creates a test handler ( that signals the shared event object)
			-func runs a socket reactor that sends messages to test handler
			LOOP
				-func sends a package to recv socket ... ( and waits on event object)
					- socket reactor dispatches event to test handler
					- test handler recvs packet and signals event object 
				-func wakes up, reads packet and ensures its the same as the one sent
*/



int gRead = 0;

class TestHandler1 : public IP::SocketReactorHandler
{
	
	IP::UDPSocket&  m_o_ReadSocket; 	
	Tools::Event&      m_o_CallbackRead;
	
public:

	
	P2P::UDPPacket m_o_packetRecvd;

	TestHandler1( 				 
				IP::UDPSocket&  _o_ReadSocket,
				Tools::Event&      _o_CallbackRead			
				):
		m_o_ReadSocket( _o_ReadSocket),
		m_o_CallbackRead( _o_CallbackRead ),				
		m_o_packetRecvd(0,0,0,P2P::UDPPacket::e_KLEINBERG_RESPONSE)
	{
		
	}

	virtual void evTimeOut() 
	{
		//m_o_CallbackTimeOut.set();		
	}
	virtual void evSocketRead(SOCKET) 
	{
		gRead ++;
		m_o_packetRecvd = m_o_ReadSocket.recv();
		m_o_CallbackRead.set();
	
	}
	virtual void evSocketWrite(SOCKET) {};
};






static void test1()
{
	gRead = 0;
	
	
	//read socket
	IP::UDPSocket ReadSocket;
	ReadSocket.bind(LOCAL_PORT);
		
//GREG BEGIN
//	// set up reactor
//	const int TIMEOUT = 1;	
//	P2P::SocketReactor reactor( TIMEOUT);				
//GREG END
		
	//create test handler
	Tools::Event      o_CallbackRead;
	TestHandler1 testhandler( ReadSocket , o_CallbackRead) ;
//GREG BEGIN
//	reactor.addHandler( ReadSocket.getHandle(), testhandler);

	// set up reactor
	const int TIMEOUT = 1;	
	IP::SocketReactor reactor( testhandler, testhandler, TIMEOUT);				
	reactor.add( ReadSocket.getHandle(), IP::SocketReactor::eReadNotify);
//GREG END

	reactor.start();

	const int RUN = 1000;
	for ( int ii = 0; ii < RUN ; ii++)
	{
		
		//check handler before packet
		ASSURE( ! o_CallbackRead.isSet() );	



		//send message, waking reactor
		P2P::UDPPacket packetToReac( LOCAL_PORT, LOCAL_ADDR, LOCAL_PORT, P2P::UDPPacket::e_KLEINBERG_MESSAGE );
		std::vector<int> myInts;
		myInts.push_back(0xEEEEEEEE);
		myInts.push_back(0xFFFFFFFF);
		packetToReac.setObjects( myInts);
		IP::UDPSocket::send( packetToReac);
		o_CallbackRead.wait();

		//check handler after packet
		ASSURE( testhandler.m_o_packetRecvd.sameData( packetToReac) );
		
	
	}

	ASSURE( RUN == gRead  )

}











//-----------------------------------------------------

/*
	PURPOSE: Ensure the reactor DOES NOT raise "fantom" recv events (when nothing is to be recvd)
	METHOD: -func creates a recv socket
			-func creates a test handler ( that signals the shared event object)
			- func waits on event object
			LOOP 
				- reactor times out and decrements a counter
				- if counter == 0 .. then signal event object
				- if reactor recvs a recv request, then ASSERT ( FALSE)
			- func wakes up and exits

*/

class TestHandler2 : public IP::SocketReactorHandler
{
	Tools::Event& m_ev_Timeout;
	int m_i_time_out;
public:
	TestHandler2( Tools::Event& _ev)
		:m_ev_Timeout(_ev),
		m_i_time_out( 5 )
	{
	}
	virtual void evTimeOut() 
	{
		if ( m_i_time_out-- == 0)
			m_ev_Timeout.set();
	}
	virtual void evSocketRead(SOCKET) 
	{
		ASSURE( false)
	}
	virtual void evSocketWrite(SOCKET) {};
};



static void test2()
{
	gRead = 0;
	
	
	//read socket
	IP::UDPSocket ReadSocket;
	ReadSocket.bind(LOCAL_PORT);
	
	Tools::Event      m_o_evTimeOut;

	// set up reactor
	const int TIMEOUT = 1;	
	
//GREG BEGIN
//    TestHandler2 testhandler( m_o_evTimeOut );
//	IP::SocketReactor reactor( TIMEOUT);	
//	reactor.addHandler( ReadSocket.getHandle(), testhandler);
//	reactor.addTimeOutHandler( testhandler);
	TestHandler2 testhandler( m_o_evTimeOut );
	IP::SocketReactor reactor( testhandler, testhandler, TIMEOUT);				
	reactor.add( ReadSocket.getHandle(), IP::SocketReactor::eReadNotify);
//GREG END
	reactor.start();

	// wait for the reactor to timeout the handler
	m_o_evTimeOut.wait();
}

/*

class TestHandler : public P2P::SocketHandler
{
	
	
	P2P::SocketReactor& m_o_reactor;
	
public:

	P2P::UDPPacket m_o_packRecvd;
	P2P::Event m_o_CallbackRead;
	P2P::Event m_o_CallbackTimeOut;

	int m_i_Read ;
	int m_i_TimedOut ;

	TestHandler(  P2P::SocketReactor& _reactor ):
		m_o_reactor( _reactor),
		m_o_packRecvd(0,0,P2P::UDPPacket::e_RESPONSE),
		m_i_Read ( 0),
		m_i_TimedOut ( 0)
	{	
		
	}

	virtual void evTimeOut() 
	{
		m_o_CallbackTimeOut.set();		
		m_i_TimedOut++;
	}
	virtual void evSocketRead(SOCKET*) 
	{
		m_o_CallbackRead.set();
		m_i_Read ++;
		m_o_packRecvd = m_o_ReadSocket.recv();
	}

	~TestHandler( )
	{
		m_o_reactor.stop();
	}

};


static void test1()
{
	int gg= 1000;
	while ( --gg){
	

	{
		//setup socket
		P2P::UDPSocket  o_ReadSocket; 
		o_ReadSocket.bind(LOCAL_PORT);


		// set up reactor
		const int TIMEOUT = 1;
		P2P::SocketReactor reactor( TIMEOUT);		
		
		//setup test handler
		ASSURE( reactor.isStopped() )
		TestHandler testhandler( reactor);
		reactor.addRecvHandler( o_ReadSocket, testhandler );
		ASSURE( !reactor.isStopped() )
			
		ASSURE( !testhandler.m_o_CallbackRead.isSet() )

		
		//send message, waking reactor
		P2P::UDPPacket packetToReac( LOCAL_PORT, LOCAL_ADDR, P2P::UDPPacket::e_MESSAGE );
		P2P::UDPSocket().send( packetToReac);
		

		//wait
		const int Nread = testhandler.m_i_Read;
		m_o_CallbackRead.wait();
		ASSURE( testhandler.m_i_Read = Nread + 1)
		ASSURE( testhandler.m_i_Read = Nread + 1)
		ASSURE( m_o_CallbackRead.m_o_packRecvd = packetToReac );

		//ASSURE( !CallbackRead.isSet() ) //immediately.. the event is NOT signaled after a wait

		//clear and start again
		//CallbackRead.reset();
		ASSURE( !CallbackRead.isSet() )

	}// reactor is dead...
	}
}*/





class Pants : public IP::SocketReactorHandler
{	
public:
	Tools::Event& m_e;
	Pants ( Tools::Event& _e) :  m_e( _e){};	
	~Pants( ){}

	virtual void evTimeOut() 	{
		m_e.set();	
	}
	virtual void evSocketRead(SOCKET)  {}
	virtual void evSocketWrite(SOCKET) {};
};



void startStop()
{
	Tools::Event      o_evTimeOut;
// set up reactor
	const int TIMEOUT = 1;	
//GREG BEGIN
//	IP::SocketReactor reactor( TIMEOUT);				
		
	//create test handler
	Pants eatMyShorts(o_evTimeOut);	
	IP::UDPSocket  o_ReadSocket; 
	o_ReadSocket.bind(LOCAL_PORT);

//	reactor.addHandler( o_ReadSocket.getHandle(), eatMyShorts);
//	reactor.addTimeOutHandler(  eatMyShorts);
	IP::SocketReactor reactor( eatMyShorts, eatMyShorts, TIMEOUT);				
	reactor.add( o_ReadSocket.getHandle(), IP::SocketReactor::eReadNotify);
//GREG END

	for ( int i = 0; i < 10 ; i++)
	{
		o_evTimeOut.reset();
		reactor.start();	
		o_evTimeOut.wait();
		reactor.stop();		
	}

}


ADD_TEST(test1,Reactor,"Simple write and read")
ADD_TEST(test2,Reactor,"No packages, no recv events")
ADD_TEST(startStop,Reactor,"start/stop many times")
