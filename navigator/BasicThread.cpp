#include "BasicThread.h"
#include "Ogre.h"
#include "Platform.h"

BasicThread::BasicThread() :
    mMutex(PTHREAD_MUTEX_INITIALIZER),
    mState(SInit),
    mStopRequested(false),
    mStopTimeoutSec(5)
{
}

//-------------------------------------------------------------------------------------
BasicThread::~BasicThread()
{
    finalize();
}

//-------------------------------------------------------------------------------------
bool BasicThread::start()
{
    int rc;

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::start()");

    pthread_mutex_lock(&mMutex);
    if (mState != SInit)
    {
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    mStopRequested = false;

    rc = pthread_create(&mThreadId, NULL, startRoutine, this);
    if (rc != 0)
    {
        Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::start() pthread_create returned " + Ogre::StringConverter::toString(rc));
        pthread_mutex_unlock(&mMutex);
        return false;
    }
    rc = pthread_detach(mThreadId);
    if (rc != 0)
    {
        Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::start() pthread_detach returned " + Ogre::StringConverter::toString(rc));
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    mState = SRunning;
    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------
void BasicThread::stop(unsigned int stopTimeoutSec)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::stop() stop requested with stopTimeoutSec=" + Ogre::StringConverter::toString(stopTimeoutSec));

    mStopRequested = true;
    mStopTimeoutSec = stopTimeoutSec;
}

//-------------------------------------------------------------------------------------
void BasicThread::finalize()
{
    unsigned long elapsedMs = 0;

    if (mState == SInit) return;

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::finalize() waiting for termination ...");

    mStopRequested = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)mStopTimeoutSec*1000))
    {
        Platform::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
        Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::finalize() killing thread");
    }

    pthread_mutex_lock(&mMutex);
    mState = SInit;
    pthread_mutex_unlock(&mMutex);

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::finalize() thread terminated");
}

//-------------------------------------------------------------------------------------
void BasicThread::startRoutine()
{
    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::startRoutine() calling run()");

    // call the run() method
    run();

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::startRoutine() end of method run()");

    // end of thread, so thead is now stopped
    pthread_mutex_lock(&mMutex);
    mState = SStopped;
    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
void* BasicThread::startRoutine(void* args)
{
    BasicThread* basicThread = (BasicThread*)args;
    assert(basicThread != 0);

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::startRoutine() calling BasicThread::startRoutine()");

    basicThread->startRoutine();

    Ogre::LogManager::getSingletonPtr()->logMessage("BasicThread::startRoutine() end");

    return NULL;
}
