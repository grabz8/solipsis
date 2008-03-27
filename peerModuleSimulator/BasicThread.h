#ifndef __BasicThread_h__
#define __BasicThread_h__

#include <pthread.h>
#include <string>

namespace Solipsis {

/** This class manages a thread, implement the run method to specialize its treatment.
 */
class BasicThread
{
public:
    enum State {
        SInit,      // Thread is initialized
        SRunning,   // Thread is running
        SStopped    // Thread is stopped
    };

protected:
    std::string mName;
    pthread_t mThreadId;
    pthread_mutex_t mMutex;
    State mState;
    bool mStopRequested;
    unsigned int mStopTimeoutSec;

public:
    BasicThread(const std::string& name);
    ~BasicThread();

    // start the thread
    virtual bool start();

    // request the termination of the thread
    // call finalize() to wait thread is really stopped
    virtual void stop(unsigned int stopTimeoutSec = 5);

    // wait max. stopTimeoutSec thread stops before killing it
    virtual void finalize();

    // get and set
    std::string& getName() { return mName; }
    pthread_t getThreadId() { return mThreadId; }
    State getState() { return mState; }
    bool isStopRequested() { return mStopRequested; }

protected:
    // method to override to implement the work done by this thread,
    // method must return if isStopRequested() return true
    virtual void run() = 0; // pure virtual

private:
    // routine called when thread is starting
    void startRoutine();

    // static routine called when thread is starting, this routine will
    // forward the call to instance method startRoutine()
    static void *startRoutine(void* args);
};

} // namespace Solipsis

#endif // #ifndef __BasicThread_h__
