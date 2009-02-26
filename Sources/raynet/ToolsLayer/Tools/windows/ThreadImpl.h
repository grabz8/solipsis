#ifndef __P2P_ThreadImpl_H__
#define __P2P_ThreadImpl_H__


#include "P2PTools.h"

namespace Tools{


	//---------------------MutexImpl-------------------------


	// NOTE :  TEMPORARILY SUPPORT COPY CONSTRUCTION
	class MutexImpl {		

		CRITICAL_SECTION m_criticalsection;	

		MutexImpl& operator =(  const MutexImpl&);

	public:

		MutexImpl()
		{
			::InitializeCriticalSection(  &m_criticalsection  );
		}
		~MutexImpl()
		{
			::DeleteCriticalSection(&m_criticalsection);
		}
		MutexImpl( const MutexImpl&)
		{
			::InitializeCriticalSection(  &m_criticalsection  );
		};		

		void lock() volatile
		{
			::EnterCriticalSection( const_cast<CRITICAL_SECTION*>(&m_criticalsection) );
		}
		void unlock() volatile
		{
			::LeaveCriticalSection( const_cast<CRITICAL_SECTION*>(&m_criticalsection) );
		}
	};


	//----------------------EventImpl------------------------



	DEFINE_EXCEPTION(EventImplCreationFailure)
	DEFINE_EXCEPTION(EventImplUsageFailure)

	class EventImpl : private Tools::NO_COPY_ASSIGN
	{
		HANDLE m_h_event;
	public:
		EventImpl()
			:m_h_event(NULL)
		{
			m_h_event = ::CreateEvent(
				NULL, //security attributes
				FALSE,//  __in      BOOL bManualReset,
				FALSE,//  __in      BOOL bInitialState,
				NULL//__in_opt  LPCTSTR lpName
				);	
			if ( m_h_event == NULL)
			{
				THROW(EventImplCreationFailure, Tools::SYSTEM_ERROR);
			}
		}
		//----
		~EventImpl ()
		{
			::CloseHandle( m_h_event  );
		}
		//----
		void wait()
		{
			::WaitForSingleObject(m_h_event,INFINITE);
		}
		//----
		void set()
		{
			if ( ! ::SetEvent(m_h_event ) )
				THROW(EventImplUsageFailure, Tools::SYSTEM_ERROR);
		}
		//----
		void reset()
		{
			if ( ! ::ResetEvent( m_h_event ) )
				THROW(EventImplUsageFailure, Tools::SYSTEM_ERROR);
		}
		//----
		bool isSet()
		{
			return ::WaitForSingleObject(m_h_event,0) == WAIT_OBJECT_0;
		}
	};




	/*------------------ThreadImpl-------------------------*/

	//DWORD  WINAPI ThreadProc( void* pTHIS),

	DEFINE_EXCEPTION(ThreadFailedException)

	class ThreadImpl : private Tools::NO_COPY_ASSIGN
	{	
		typedef void* CallbackParam;
		typedef void (*CallbackFunc)( CallbackParam );
		CallbackFunc m_p_callbackFunc;
		CallbackParam m_p_param;

		volatile bool m_b_RequestedToStop;
		volatile HANDLE m_h_handle;
		DWORD m_i_threadID;

		friend DWORD  WINAPI ThreadProc( void* );



		public:


		ThreadImpl(CallbackFunc _callBackFun, CallbackParam _p_param)
			: 
		m_p_callbackFunc( _callBackFun),
		m_p_param(_p_param),
			m_b_RequestedToStop( false),
			m_h_handle ( NULL ),
			m_i_threadID(0)
		{
		}
		//------
		 ~ThreadImpl(void)
		{
			stop();
		}
		//------
		void start()
		{
			if ( m_h_handle == NULL)
			{
				HANDLE temp =  ::CreateThread(
					NULL,
					0,
					&ThreadProc,
					this,
					0,
					&m_i_threadID
					);

				if ( !temp)
				{
					THROW( ThreadFailedException, Tools::SYSTEM_ERROR);
				}
				
				m_h_handle  = temp;
			}
		}
		//------
		void stop()
		{
			if ( m_h_handle != NULL)
			{	
				m_b_RequestedToStop = true;
				::WaitForSingleObject( m_h_handle, INFINITE);	
				::CloseHandle( m_h_handle);
				m_h_handle = NULL;
				m_b_RequestedToStop = false;
			}
		}
		//------
		bool mustStop()
		{
			return m_b_RequestedToStop;
		}
		//------
		bool isStopped()
		{
			return m_h_handle == NULL;
		}


	};

	// OS thread function callback
	DWORD  WINAPI ThreadProc( void* _pTHIS)
	{ 
		int ret = 0;
		try
		{
			ThreadImpl* pTHIS =static_cast<ThreadImpl*>(_pTHIS) ; 
			pTHIS->m_p_callbackFunc( pTHIS->m_p_param );
			ret = 1;	
		}
		catch( std::exception& _e  )
		{
			std::cout << "\n" << _e.what() << std::endl;
		}
		catch(...)
		{			
		}
		return ret;
	}



}

#endif