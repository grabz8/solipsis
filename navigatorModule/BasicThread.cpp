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
    int rc;
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
        rc = pthread_cancel(mThreadId);
        if (rc != 0)
            OGRE_LOG(mName + ">BasicThread::finalize() pthread_cancel returned " + StringConverter::toString(rc));
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
