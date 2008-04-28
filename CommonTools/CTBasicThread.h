#ifndef __CTBasicThread_h__
#define __CTBasicThread_h__

#include <string>

#include <pthread.h>

namespace CommonTools {

/** This class manages a thread, implement the run method to specialize its treatment.
 */
class BasicThread
{
public:
    /// states
    enum State {
        SInit,      // Thread is initialized
        SRunning,   // Thread is running
        SStopped    // Thread is stopped
    };

protected:
    /// name
    std::string mName;
    /// thread identifier
    pthread_t mThreadId;
    /// mutex to synchronize state change
    pthread_mutex_t mMutex;
    /// current state
    State mState;
    /// true if stop is requested
    bool mStopRequested;
    /// timeout to stop the thread until it is killed
    unsigned int mStopTimeoutSec;

public:
    /// constructor, specify the name
    BasicThread(const std::string& name);
    /// Destructor
    ~BasicThread();

    /// start the thread
    virtual bool start();

    /** request the termination of the thread,
        call finalize() to wait thread is really stopped
    */
    virtual void stop(unsigned int stopTimeoutSec = 5);

    /// wait max. stopTimeoutSec thread stops before killing it
    virtual void finalize();

    /// get the name
    std::string& getName() { return mName; }

    /// set the name
    void setName(const std::string& name) { mName = name; }

    /// get thread identifier
    pthread_t getThreadId() { return mThreadId; }

    /// get current state
    State getState() { return mState; }

    /// returns true if stop is requested
    bool isStopRequested() { return mStopRequested; }

protected:
    /** implement the work done by this thread until isStopRequested() returns true.
    @remarks An implementation must be supplied for this method.
    */
    virtual void run() = 0;

    /** handle the end of thread run.
    @remarks An implementation must be supplied for this method.
    */
    virtual void end() {}

private:
    /** static routine called when thread is starting, this routine will
        forward the call to instance method startRoutine()
    */
    static void *startRoutine(void* args);
};

} // namespace CommonTools

#endif // #ifndef __CTBasicThread_h__
