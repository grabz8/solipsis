/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
