


#include <Thread.h>

#include "CppTest.h"


class MyTest1 : public P2P::Thread
{
public:
	~MyTest1()
	{
		stop();
	} 

	virtual void run()
	{
		while ( ! mustStop() )
		{
			Tools::Sleep(1000);
		}
	}
};



void threadTest1()
{
	for ( int i = 0; i < 1000; i++)
	{
		MyTest1 obj;
		obj.start();
		obj.stop();
	}
}



volatile bool gRUNNING = false;

class NoSleep : public P2P::Thread
{
public:
	~NoSleep()
	{
		stop();
	} 

	virtual void run()
	{
		gRUNNING = true;
		while ( ! mustStop() )
		{
			
			Tools::Sleep(0);
			
		}
		gRUNNING = false;
	}
};


void manytimes()
{
	for ( int i = 0; i < 10000; i++)
	{
		// run once checking start stop
		NoSleep helloEveryone;
		helloEveryone.start();			
		ASSURE(! helloEveryone.isStopped() );
		ASSURE(! helloEveryone.mustStop() );
		
		while(! gRUNNING){ Tools::Sleep(0);}

		helloEveryone.stop();
		ASSURE( !helloEveryone.mustStop() );//already stopped.. NOT must stop again!
		ASSURE( helloEveryone.isStopped() );

		ASSURE(! gRUNNING)

		helloEveryone.start();	
		ASSURE(! helloEveryone.isStopped() );
		ASSURE(! helloEveryone.mustStop() );
		
		while(! gRUNNING){ Tools::Sleep(0);}

		helloEveryone.stop();
		ASSURE( !helloEveryone.mustStop() );//already stopped.. NOT must stop again!
		ASSURE( helloEveryone.isStopped() );

		ASSURE(! gRUNNING)

	}
	
}



ADD_TEST(threadTest1,thread, "start/stop many times")
ADD_TEST(manytimes,thread, "start/stop many times check running")
