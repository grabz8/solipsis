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

namespace CommonTools {

//-------------------------------------------------------------------------------------
Timer::Timer()
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
    zeroClock = clock();
    gettimeofday(&start, NULL);
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMilliseconds()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec - start.tv_sec)*1000 + (now.tv_usec - start.tv_usec)/1000;
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMicroseconds()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec - start.tv_sec)*1000000 + (now.tv_usec - start.tv_usec);
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMillisecondsCPU()
{
    clock_t newClock = clock();
    return (Timer::Time)((float)(newClock - zeroClock)/((float)CLOCKS_PER_SEC/1000.0f)) ;
}

//-------------------------------------------------------------------------------------
Timer::Time Timer::getMicrosecondsCPU()
{
    clock_t newClock = clock();
    return (Timer::Time)((float)(newClock - zeroClock)/((float)CLOCKS_PER_SEC/1000000.0f)) ;
}

//-------------------------------------------------------------------------------------

}
