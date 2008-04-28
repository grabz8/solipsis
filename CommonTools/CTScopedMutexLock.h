#ifndef __CTScopedMutexLock_h__
#define __CTScopedMutexLock_h__

#include <pthread.h>

namespace CommonTools {

/** This class locks a mutex on construction and release it on destruction.
 */
class ScopedMutexLock
{
public:
	/// Constructor, specify the mutex to lock/unlock
    ScopedMutexLock(pthread_mutex_t& mutex) : mMutex(mutex) { pthread_mutex_lock(&mMutex); }
	/// Destructor, mutex is unlocked
    ~ScopedMutexLock() { pthread_mutex_unlock(&mMutex); }

private:
    // dummy copy constructor and operator= to prevent copying
    ScopedMutexLock(const ScopedMutexLock&);
    ScopedMutexLock& operator=(const ScopedMutexLock&);

protected:
	pthread_mutex_t& mMutex;
};

} // namespace CommonTools

#endif // #ifndef __CTScopedMutexLock_h__
