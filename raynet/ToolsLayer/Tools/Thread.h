#ifndef __P2P_THREAD_H__
#define __P2P_THREAD_H__


#include "P2PTools.h"


namespace Tools{

	




	//---------------------synchronization classes----------------------//

	

	/*-----------------MUTEX------------------*/
	// NOTE :  TEMPORARILY SUPPORT COPY CONSTRUCTION

	class MutexImpl;
	typedef std::auto_ptr< MutexImpl> MutexImplPtr;

	class Mutex : private Tools::NO_COPY_ASSIGN{
		
		MutexImplPtr m_pImpl;

		Mutex& operator =(  const Mutex&);

	public:
		
		Mutex();		
		~Mutex();
		Mutex( const Mutex&);		
		
		void lock() ;		
		void unlock() ;		
	};
	
	struct ScopeLock : private Tools::NO_COPY_ASSIGN{
		 Mutex& m_m;
		ScopeLock(  Mutex& _m) : m_m(_m){  m_m.lock(); }
		~ScopeLock(){ m_m.unlock(); }
	};	
#define LOCK(LOCK_OBJ) Tools::ScopeLock LOCK_OBJ##temp(LOCK_OBJ);


	/*------------------EVENT-------------------------*/

	class EventImpl;
	typedef std::auto_ptr<EventImpl> EventImplPtr;

	class Event : private Tools::NO_COPY_ASSIGN
	{
		EventImplPtr m_pImpl;
	
	public:
		Event ();	
		~Event ();
		void wait();
		void set();
		void reset();
		bool isSet();
	};


	/*------------------THREAD-------------------------*/
	class ThreadImpl;
	typedef std::auto_ptr< ThreadImpl > ThreadImplPtr;

	class Thread :  private Tools::NO_COPY_ASSIGN
	{
		ThreadImplPtr m_pImpl;

		// called by the ThreadImpl object
		static void callbackRun(void * _THIS); 

	protected:
		// Thread Constructor: thread obj commences in stopped mode..needs starting
		
		Thread();		
		virtual void  run() = 0;		


	public:
		
		
		virtual ~Thread();		
		void start();
		void stop();
		bool mustStop();
		bool isStopped();
	};

}

#endif