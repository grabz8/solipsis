/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Source from OGRE (Object-oriented Graphics Rendering Engine)

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

#include "CTTimer.h"
#include <xutility>
#include <stdlib.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
Timer::Timer() :
    mTimerMask(0)
{
    reset();
}

//-------------------------------------------------------------------------------------
Timer::~Timer()
{
}

//-------------------------------------------------------------------------------------
void Timer::reset()
{
    // Get the current process core mask
    DWORD procMask;
    DWORD sysMask;
    #if _MSC_VER >= 1400 && defined (_M_X64)
    GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&procMask, (PDWORD_PTR)&sysMask);
    #else
    GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);
    #endif

    // If procMask is 0, consider there is only one core available
    // (using 0 as procMask will cause an infinite loop below)
    if (procMask == 0)
        procMask = 1;

    // Find the lowest core that this process uses
    if( mTimerMask == 0 )
    {
        mTimerMask = 1;
        while( ( mTimerMask & procMask ) == 0 )
            mTimerMask <<= 1;
    }

    HANDLE thread = GetCurrentThread();

    // Set affinity to the first core
    DWORD oldMask = (DWORD)SetThreadAffinityMask(thread, mTimerMask);

    // Get the constant frequency
    QueryPerformanceFrequency(&mFrequency);

    // Query the timer
    QueryPerformanceCounter(&mStartTime);
    mStartTick = GetTickCount();

    // Reset affinity
    SetThreadAffinityMask(thread, oldMask);

    mLastTime = 0;
    mZeroClock = clock();
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMilliseconds()
{
    LARGE_INTEGER curTime;

    HANDLE thread = GetCurrentThread();

    // Set affinity to the first core
    DWORD oldMask = (DWORD)SetThreadAffinityMask(thread, mTimerMask);

    // Query the timer
    QueryPerformanceCounter(&curTime);

    // Reset affinity
    SetThreadAffinityMask(thread, oldMask);

    LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

    // scale by 1000 for milliseconds
    Timer::Time newTicks = (Timer::Time) (1000*newTime/mFrequency.QuadPart);

    // detect and compensate for performance counter leaps
    // (surprisingly common, see Microsoft KB: Q274323)
    Timer::Time check = GetTickCount() - mStartTick;
    signed long msecOff = (signed long)(newTicks - check);
    if (msecOff < -100 || msecOff > 100)
    {
        // We must keep the timer running forward :)
        LONGLONG adjust = (std::min)(msecOff*mFrequency.QuadPart/1000, newTime - mLastTime);
        mStartTime.QuadPart += adjust;
        newTime -= adjust;

        // Re-calculate milliseconds
        newTicks = (Timer::Time)(1000*newTime/mFrequency.QuadPart);
    }

    // Record last time for adjust
    mLastTime = newTime;

    return newTicks;
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMicroseconds()
{
    LARGE_INTEGER curTime;

    HANDLE thread = GetCurrentThread();

    // Set affinity to the first core
    DWORD oldMask = (DWORD)SetThreadAffinityMask(thread, mTimerMask);

    // Query the timer
    QueryPerformanceCounter(&curTime);

    // Reset affinity
    SetThreadAffinityMask(thread, oldMask);

    LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

    // get milliseconds to check against GetTickCount
    Timer::Time newTicks = (Timer::Time)(1000*newTime/mFrequency.QuadPart);

    // detect and compensate for performance counter leaps
    // (surprisingly common, see Microsoft KB: Q274323)
    Timer::Time check = GetTickCount() - mStartTick;
    signed long msecOff = (signed long)(newTicks - check);
    if (msecOff < -100 || msecOff > 100)
    {
        // We must keep the timer running forward :)
        LONGLONG adjust = (std::min)(msecOff*mFrequency.QuadPart/1000, newTime - mLastTime);
        mStartTime.QuadPart += adjust;
        newTime -= adjust;
    }

    // Record last time for adjust
    mLastTime = newTime;

    // scale by 1000000 for microseconds
    Timer::Time newMicro = (Timer::Time)(1000000*newTime/mFrequency.QuadPart);

    return newMicro;
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMillisecondsCPU()
{
    clock_t newClock = clock();
    return (Timer::Time)((float)(newClock - mZeroClock)/((float)CLOCKS_PER_SEC/1000.0f));
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMicrosecondsCPU()
{
    clock_t newClock = clock();
    return (Timer::Time)((float)(newClock - mZeroClock)/((float)CLOCKS_PER_SEC/1000000.0f));
}

//-------------------------------------------------------------------------------------

}
