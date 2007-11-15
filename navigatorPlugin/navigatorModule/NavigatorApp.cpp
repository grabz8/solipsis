#include "NavigatorApp.h"
#include "Navigator.h"
#include "Platform.h"

using namespace NavigatorModule;

// this is a simple C stub so that dll can be loaded dynamically
// and the static method createNavigator can be called
extern "C" {
	NAVIGATORMODULEDIRECT_EXPORT INavigatorApp* createNavigatorApp(const char* appPath)
	{
		return INavigatorApp::createNavigatorApp(appPath);
	}
}

NavigatorApp* NavigatorApp::ms_Singleton = 0;

INavigatorApp* INavigatorApp::createNavigatorApp(const char* appPath)
{
    if (NavigatorApp::ms_Singleton == 0)
    {
        NavigatorApp::ms_Singleton = new NavigatorApp();
        assert(NavigatorApp::ms_Singleton);
        NavigatorApp::ms_Singleton->_initialize(appPath);
    }

    assert(NavigatorApp::ms_Singleton != 0);

    return NavigatorApp::ms_Singleton;
}	

bool NavigatorApp::destroy()
{
    _finalize();

    // If there are still alive instances, cannot delete app
    if (mThreads.size() > 0)
        return false;

    delete this;

    return true;
}

IInstance* NavigatorApp::createInstance()
{
    // create the main thread
    Instance* instance = new Navigator();
    MainThread* mainThread = new MainThread(instance);
    mainThread->start();

    mThreads.push_back(mainThread);

    return mainThread->getInstance();
}

bool NavigatorApp::destroyInstance(IInstance* instance)
{
    // find instance
    MainThread* mainThread = NULL;

    for ( unsigned int i = 0; i < mThreads.size(); ++i ) {
        mainThread = mThreads[i];
        if ((mainThread != NULL) && (mainThread->getInstance() == instance))
        {
            mThreads.erase(mThreads.begin() + i, mThreads.begin() + i + 1);
            break;
        }
    }

    mainThread->stop();
    delete mainThread->getInstance();
    delete mainThread;

    return true;
}

NavigatorApp::NavigatorApp()
{
    assert(NavigatorApp::ms_Singleton == 0);
}

NavigatorApp::~NavigatorApp()
{
    assert(NavigatorApp::ms_Singleton == this);
    NavigatorApp::ms_Singleton = 0;
}

void NavigatorApp::_initialize(const char* appPath)
{
    assert(mThreads.size() == 0);
    mThreads.clear();
}

void NavigatorApp::_finalize()
{
    assert(mThreads.size() == 0);
}

NavigatorApp::MainThread::MainThread(Instance* instance) :
    mInstance(instance),
    mState(SInit),
    mStop(false)
{
}

NavigatorApp::MainThread::~MainThread()
{
    // if not already stopped ...
    stop(5);
}

void NavigatorApp::MainThread::start()
{
    int rc;

    rc = pthread_create(&mThread, NULL, start_routine, this);
    if (rc != 0)
    {
    }

    rc = pthread_detach(mThread);
    if (rc != 0)
    {
    }

    mState = SRunning;
    mStop = false;
}

void NavigatorApp::MainThread::stop(unsigned int timeoutSec)
{
    unsigned long elapsedMs = 0;

    // Stop the instance
    if (mState == SRunning)
        mInstance->requestTerminate();

    mStop = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)timeoutSec*1000))
    {
        Platform::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
    }
}

void NavigatorApp::MainThread::run()
{
    // run the render loop
    mInstance->run();
    mState = SStopped;
}

void* NavigatorApp::MainThread::start_routine(void* args)
{
    NavigatorApp::MainThread* mainThread = (NavigatorApp::MainThread*)args;
    if (mainThread != 0)
        mainThread->run();

    return NULL;
}

