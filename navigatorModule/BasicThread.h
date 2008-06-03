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
