#include "BasicThread.h"
#include "Ogre.h"
#include "OgreHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
BasicThread::BasicThread(const std::string& name) :
    mName(name),
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

    OGRE_LOG(mName + ">BasicThread::start()");

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
        OGRE_LOG(mName + ">BasicThread::start() pthread_create returned " + StringConverter::toString(rc));
        pthread_mutex_unlock(&mMutex);
        return false;
    }
    rc = pthread_detach(mThreadId);
    if (rc != 0)
    {
        OGRE_LOG(mName + ">BasicThread::start() pthread_detach returned " + StringConverter::toString(rc));
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
    OGRE_LOG(mName + ">BasicThread::stop() stop requested with stopTimeoutSec=" + StringConverter::toString(stopTimeoutSec));

    mStopRequested = true;
    mStopTimeoutSec = stopTimeoutSec;
}

//-------------------------------------------------------------------------------------
void BasicThread::finalize()
{
    unsigned long elapsedMs = 0;

    if (mState == SInit) return;

    OGRE_LOG(mName + ">BasicThread::finalize() waiting for termination ...");

    mStopRequested = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)mStopTimeoutSec*1000))
    {
        Platform::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
        OGRE_LOG(mName + ">BasicThread::finalize() killing thread");
    }

    pthread_mutex_lock(&mMutex);
    mState = SInit;
    pthread_mutex_unlock(&mMutex);

    OGRE_LOG(mName + ">BasicThread::finalize() thread terminated");
}

//-------------------------------------------------------------------------------------
void BasicThread::startRoutine()
{
    OGRE_LOG(mName + ">BasicThread::startRoutine() calling run()");

    // call the run() method
    run();

    OGRE_LOG(mName + ">BasicThread::startRoutine() end of method run()");

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

    OGRE_LOG(basicThread->getName() + ">BasicThread::startRoutine() calling BasicThread::startRoutine()");

    basicThread->startRoutine();

    OGRE_LOG(basicThread->getName() + ">BasicThread::startRoutine() end");

    return NULL;
}

//-------------------------------------------------------------------------------------
