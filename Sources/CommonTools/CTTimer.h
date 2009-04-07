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

#ifndef __CTTimer_h__
#define __CTTimer_h__

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace CommonTools {

/** This class implement timing features.
 */
class Timer
{
public:
    /// Time typedef
    typedef unsigned long Time;

private:
#ifdef WIN32
    clock_t mZeroClock;

    DWORD mStartTick;
    LONGLONG mLastTime;
    LARGE_INTEGER mStartTime;
    LARGE_INTEGER mFrequency;

    DWORD mTimerMask;
#else
    struct timeval start;
    clock_t zeroClock;
#endif

public:
    /// Constructor. MUST be called on same thread that calls getMilliseconds()
    Timer();
    /// Destructor
    ~Timer();

    /// Resets timer
    void reset();

    /// Returns milliseconds since initialisation or last reset
    Time getMilliseconds();

    /// Returns microseconds since initialisation or last reset
    Time getMicroseconds();

    /// Returns milliseconds since initialisation or last reset, only CPU time measured
    Time getMillisecondsCPU();

    /// Returns microseconds since initialisation or last reset, only CPU time measured
    Time getMicrosecondsCPU();
};

} // namespace CommonTools

#endif // #ifndef __CTTimer_h__