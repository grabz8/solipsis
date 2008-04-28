#include "CTBasicThread.h"
#include "CTScopedMutexLock.h"
#include "CTSystem.h"
#include "CTLog.h"

#include <assert.h>

namespace CommonTools {

#define LH LogHandler

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

    LH::logf(LH::VL_DEBUG, "%s>BasicThread::start()", mName.c_str());

	ScopedMutexLock lock(mMutex);
    if (mState != SInit)
        return false;

    mStopRequested = false;

    rc = pthread_create(&mThreadId, NULL, startRoutine, this);
    if (rc != 0)
    {
        LH::logf(LH::VL_DEBUG, "%s>BasicThread::start() pthread_create returned %d", mName.c_str(), rc);
        return false;
    }
    rc = pthread_detach(mThreadId);
    if (rc != 0)
    {
        LH::logf(LH::VL_DEBUG, "%s>BasicThread::start() pthread_detach returned %d", mName.c_str(), rc);
        return false;
    }

    mState = SRunning;

    return true;
}

//-------------------------------------------------------------------------------------
void BasicThread::stop(unsigned int stopTimeoutSec)
{
    LH::logf(LH::VL_DEBUG, "%s>BasicThread::stop() stop requested with stopTimeoutSec=%d", mName.c_str(), stopTimeoutSec);

    mStopRequested = true;
    mStopTimeoutSec = stopTimeoutSec;
}

//-------------------------------------------------------------------------------------
void BasicThread::finalize()
{
    int rc;
    unsigned long elapsedMs = 0;

    if (mState == SInit) return;

    LH::logf(LH::VL_DEBUG, "%s>BasicThread::finalize() waiting for termination ...", mName.c_str());

    mStopRequested = true;
    while ((mState == SRunning) && (elapsedMs < (unsigned long)mStopTimeoutSec*1000))
    {
        System::sleep(100);
        elapsedMs += 100;
    }
    // Kill thread ?
    if (mState == SRunning) {
        LH::logf(LH::VL_DEBUG, "%s>BasicThread::finalize() killing thread", mName.c_str());
        rc = pthread_cancel(mThreadId);
        if (rc != 0)
            LH::logf(LH::VL_DEBUG, "%s>BasicThread::finalize() pthread_cancel returned %d", mName.c_str(), rc);
    }

	{
		ScopedMutexLock lock(mMutex);
		mState = SInit;
	}

    LH::logf(LH::VL_DEBUG, "%s>BasicThread::finalize() thread terminated", mName.c_str());
}

//-------------------------------------------------------------------------------------
void* BasicThread::startRoutine(void* args)
{
    BasicThread* basicThread = (BasicThread*)args;
    assert(basicThread != 0);

    LH::logf(LH::VL_DEBUG, "%s>BasicThread::startRoutine() calling run()", basicThread->getName().c_str());

    // call the run() method
    basicThread->run();

    LH::logf(LH::VL_DEBUG, "%s>BasicThread::startRoutine() end of method run()", basicThread->getName().c_str());

    // end of thread, so thead is now stopped
	{
		ScopedMutexLock lock(basicThread->mMutex);
		basicThread->mState = SStopped;
	}

    // call the end method
    basicThread->end();

    return NULL;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools