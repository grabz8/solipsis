#ifndef _SOCKET_REACTOR_STUFF_
#define _SOCKET_REACTOR_STUFF_

#include "Thread.h"
#include <vector>
#include <map>
#include <set>

#include "P2PTools.h"
#include "UDPSocket.h"







namespace IP{
	

	// Callback interface (that client must implement)
	class SocketReactorHandler
	{
		protected:
		SocketReactorHandler(){}

		public:
		virtual ~SocketReactorHandler(){}
		
		virtual void evTimeOut() =0;
		virtual void evSocketRead(SOCKET) =0;
		virtual void evSocketWrite(SOCKET) =0;
	};


	//------------------------

	DEFINE_EXCEPTION(LoopBackSocketError)
	class LoopBackSocket
	{		
		UDPSocket m_o_recvSocket;
		static const int LOOP_BACK_PORT = 60000;
		IP::Addr m_addrMyhost;

	public:
		LoopBackSocket();
		
		void raiseSignal();
		
		void dropSignal();	

		SOCKET getHandle(){ return  m_o_recvSocket.getHandle() ;}
	};
	

	//------------------------

	
	class SocketReactor : public Tools::Thread
	{
		DEFINE_EXCEPTION(ReactorNotStopped)
		DEFINE_EXCEPTION(SelectFailed)

	public:
		
		enum NotifDirRegistration { eReadNotify, eWriteNotify };

		SocketReactor(	SocketReactorHandler& socketHandler, 
						SocketReactorHandler& timeoutHandler, 
						uint _TimeOutIntervalSecs = 0 );

		~SocketReactor(void);

		
		void setTimeOut(  uint _TimeOutIntervalSecs );	
		void add(  SOCKET _sockethandle, NotifDirRegistration );
		void remove(  SOCKET _sockethandle );
	

	private:
		//declaration order is important

		Tools::Mutex m_o_monitoredSocketsLock;
		SocketReactorHandler& m_o_socketHandler;
		SocketReactorHandler& m_o_timeoutHandler;

				
		// timeout interval
		volatile int m_i_TimeOutIntervalSecs;		

		// Monitored sockets		
		typedef std::pair< SOCKET, NotifDirRegistration> MonitoredPair;
		typedef std::vector < MonitoredPair > MonitoredSocketVec;		
		typedef std::set< MonitoredPair > MonitoredSocketSet;
		MonitoredSocketSet m_o_monitoredSockets;
		


		// from Thread class
		virtual void run();				
		void waitOn(  uint timeout, const MonitoredSocketVec& toMonitor, MonitoredSocketVec& signaled);

	
		LoopBackSocket m_o_fromLoopBack;		

	};


}
#endif