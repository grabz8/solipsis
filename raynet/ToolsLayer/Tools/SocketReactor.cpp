#include "SocketReactor.h"
#include <algorithm>
#include <functional>
#include <assert.h>


namespace IP{



	// LOOPBACK implementation---
	LoopBackSocket::LoopBackSocket()
	{
		try
		{
			m_o_recvSocket.bind(LOOP_BACK_PORT);
		}
		catch( UDPSocketBindError& )
		{
			THROW_WITH_DATA(LoopBackSocketError, " Rare error: conflict on binding port " << LOOP_BACK_PORT << " used for loopback");
		}	
		//store this host addr
		m_addrMyhost = getHostAddr();

	}
	//---------
	void LoopBackSocket::raiseSignal()
	{
		char WAKE_UP = 0;
		UDPSocket::send( LOOP_BACK_PORT, m_addrMyhost, &WAKE_UP, sizeof( WAKE_UP) );
	}
	//---------
	void LoopBackSocket::dropSignal()
	{
		Port fromPort;
		Addr fromAddr;
		char WOKE_UP = 0;

		m_o_recvSocket.recv( fromPort, fromAddr, &WOKE_UP, sizeof( WOKE_UP) );
	}
	// --------------------------










	//------------------------------------------
	SocketReactor::SocketReactor(	SocketReactorHandler& socketHandler, 
		SocketReactorHandler& timeoutHandler, 
		uint _TimeOutIntervalSecs )
		:				
	m_o_socketHandler( socketHandler),
		m_o_timeoutHandler(timeoutHandler),
		m_i_TimeOutIntervalSecs( _TimeOutIntervalSecs  )		
	{			

	}
	//------------------------------------------
	SocketReactor::~SocketReactor()
	{
		// stop thread FIRST.. otherwise it will operate whilst Reactor is dying..
		// ... additionally.. raise signal to knock it out of waiting on sockets
		mustStop();
		m_o_fromLoopBack.raiseSignal();	
		stop();		
	}	
	//------------------------------------------
	void SocketReactor::run() // RUNS IN SEPARATE THREAD
	{
		while ( ! mustStop() ){

			//make a local copy to permit reentrance .. (but at least serialise access )	
			MonitoredSocketVec localMonitoredSocks;
			uint timeout = 0;
			{			
				LOCK( m_o_monitoredSocketsLock );
				localMonitoredSocks.assign( m_o_monitoredSockets.begin(), m_o_monitoredSockets.end() );
				timeout = m_i_TimeOutIntervalSecs;
			}
	
  



			//add loopback for interrupting select() 
			localMonitoredSocks.push_back( 
				MonitoredPair( m_o_fromLoopBack.getHandle(), eReadNotify)
				);			

			MonitoredSocketVec AnySignaledSockets;
			waitOn( timeout, localMonitoredSocks, AnySignaledSockets );

			if ( AnySignaledSockets.empty() )
			{	
				// Timeout.. so tell the world
				m_o_timeoutHandler.evTimeOut();	

			}
			else
			{
				// check which is signaled
				for ( MonitoredSocketVec::iterator it = AnySignaledSockets.begin(); 
					it != AnySignaledSockets.end(); it++)
				{					

					if ( it->second == eReadNotify)
					{
						if ( it->first == m_o_fromLoopBack.getHandle() )
						{//oopss.. we are supposed to stop.. been notified on loopBack
							m_o_fromLoopBack.dropSignal();
							break;
						}

						m_o_socketHandler.evSocketRead( it->first);
					}
					else
					{
						m_o_socketHandler.evSocketWrite(it->first);
					}


				}
			}

		}
	}
	//------------------------------------------
	void SocketReactor::add(    SOCKET _sockethandle,  NotifDirRegistration _eDir)
	{		
		{			
			LOCK( m_o_monitoredSocketsLock );
			m_o_monitoredSockets.insert(  MonitoredPair( _sockethandle, _eDir)   );		
		}
		m_o_fromLoopBack.raiseSignal();		
	}
	//------------------------------------------
	void SocketReactor::remove( SOCKET _sockethandle)
	{
		{			
			LOCK( m_o_monitoredSocketsLock );

			//delete whatevet the notification dir was
			m_o_monitoredSockets.erase( MonitoredPair(_sockethandle, eWriteNotify));
			m_o_monitoredSockets.erase( MonitoredPair(_sockethandle, eReadNotify));				

		}
		m_o_fromLoopBack.raiseSignal();		
	}

	//------------------------------------------


	void SocketReactor::waitOn( uint _timeout,  const MonitoredSocketVec& _toMonitor, MonitoredSocketVec& _signaled )
	{
		// copy all sockets into temp array
		fd_set fdCheckReadable, fdCheckWritable;


		{
			// guess what? Prepare arrays for select statement.. no kidding
			SOCKET* pReadSockets =  fdCheckReadable.fd_array;
			SOCKET* pWriteSockets = fdCheckWritable.fd_array;

			for( uint i = 0 ; i < _toMonitor.size(); i++ )
			{
				if ( _toMonitor[i].second == eWriteNotify  )
					*pWriteSockets++= _toMonitor[i].first;
				else
					*pReadSockets++= _toMonitor[i].first;		
			}
			fdCheckWritable.fd_count = pWriteSockets - fdCheckWritable.fd_array;		
			fdCheckReadable.fd_count = pReadSockets - fdCheckReadable.fd_array;
		}




		// get number of signaled sockets		
		timeval timeout;
		timeout.tv_sec = _timeout;
		timeout.tv_usec = 0;

		const int NumOfSignaled = ::select(
			_signaled.size(),
			&fdCheckReadable,
			&fdCheckWritable,
			0,
			&timeout
			);

		if ( NumOfSignaled == SOCKET_ERROR )
		{
			THROW(SelectFailed," Socket error returned");
		}

		// Store all readables and writeables..
		_signaled.reserve( _toMonitor.size() );

		SOCKET* pCurr = fdCheckReadable.fd_array;
		SOCKET* pEnd = &fdCheckReadable.fd_array[ fdCheckReadable.fd_count];
		while ( pCurr!= pEnd )
		{
			_signaled.push_back( MonitoredPair( *pCurr++, eReadNotify) );		
		}

		pCurr = fdCheckWritable.fd_array;
		pEnd = &fdCheckWritable.fd_array[ fdCheckWritable.fd_count];
		while ( pCurr!= pEnd )
		{
			_signaled.push_back( MonitoredPair( *pCurr++, eWriteNotify) );		
		}

	}

	//------------------------------------------


	void SocketReactor::setTimeOut(  uint _TimeOutIntervalSecs )
	{		
		LOCK( m_o_monitoredSocketsLock );		
		 m_i_TimeOutIntervalSecs = _TimeOutIntervalSecs;
		
	}

}



