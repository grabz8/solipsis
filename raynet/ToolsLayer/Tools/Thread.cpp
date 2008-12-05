#include "Thread.h"
//#include "CoreProtocol.h"
#include "P2PTools.h"

#include <iostream>
#include "ThreadImpl.h"

namespace Tools
{
	
	Thread::~Thread()		
	{
	}
	//------------------------------------------------
	Thread::Thread(void)
		: m_pImpl( new ThreadImpl( &Thread::callbackRun , this) )
	{
	}
	//--------------------------
	void Thread::start()
	{
		m_pImpl->start();
	}
	//--------------------------
	void Thread::stop()
	{
		m_pImpl->stop();
	}
	//--------------------------
	bool Thread::mustStop()
	{
		return m_pImpl->mustStop();
	}	
	//--------------------------
	bool Thread::isStopped()
	{
		return m_pImpl->isStopped();
	}
	//--------------------------
	void Thread::callbackRun( void* _pTHIS) // called by impl class
	{
		Thread* pt = (Thread*)(_pTHIS);
		static_cast<Thread*>(_pTHIS)->run();
	}
	//------------------------------------------------





	
	//------------------------------------------------	
	Mutex::Mutex() : m_pImpl( new MutexImpl() )
	{		
	}		
	//--------------------------
	Mutex::~Mutex()
	{		
	}	
	//--------------------------------
	Mutex::Mutex( const Mutex& _m):		
		m_pImpl( new MutexImpl() )
	{		
	}
	//--------------------------	
	void Mutex::lock() {
		m_pImpl->lock();
	}
	//--------------------------	
	void Mutex::unlock()  {
		m_pImpl->unlock();
	}
	//------------------------------------------------





	Event::~Event()
	{		
	}
	//--------------------------
	Event::Event() : m_pImpl( new EventImpl() )			
	{		
	}
	//--------------------------
	void Event::wait() 
	{
		m_pImpl->wait();			
	}
	//--------------------------
	void Event::set()
	{
		m_pImpl->set();						
	}
	//--------------------------
	void Event::reset()
	{
		m_pImpl->reset();						
	}
	//--------------------------
	bool Event::isSet()
	{
		return m_pImpl->isSet();
	}	
	//--------------------------
}