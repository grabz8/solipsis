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

#include "CTStatsManager.h"
#include "CTIO.h"

namespace CommonTools {

template<> StatsManager* Singleton<StatsManager>::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
StatsManager* StatsManager::getSingletonPtr(void)
{
    return ms_Singleton;
}
//-------------------------------------------------------------------------------------
StatsManager& StatsManager::getSingleton(void)
{  
    assert( ms_Singleton );  return ( *ms_Singleton );  
}

//-------------------------------------------------------------------------------------
StatsManager::StatsManager() :
    mPathname("stats"),
    mLastFlush(0),
    mNextFlush(0)
{
}

//-------------------------------------------------------------------------------------
StatsManager::~StatsManager()
{
    finalize();
}

//-------------------------------------------------------------------------------------
void StatsManager::initialize(const std::string& pathname, unsigned int flushPeriodSec)
{
    finalize();

    mPathname = pathname;
    mFlushPeriodSec = flushPeriodSec;
    time(&mLastFlush);
    mNextFlush = (mLastFlush - (mLastFlush%mFlushPeriodSec)) + mFlushPeriodSec;
}

//-------------------------------------------------------------------------------------
void StatsManager::finalize()
{
    time_t now;
    time(&now);
    flush(now, true);
    mLastFlush = 0;
}

//-------------------------------------------------------------------------------------
void StatsManager::addEvent(StatEventType type, StatEventId id, const std::string& desc)
{
    time_t now;

    time(&now);
    flush(now);

    StatEvent newEvent;
    newEvent.timestamp = now;
    newEvent.type = type;
    newEvent.id = id;
    newEvent.desc = desc;
    mEvents.push_back(newEvent);
}

//-------------------------------------------------------------------------------------
void StatsManager::flush(time_t now, bool force)
{
    if (mLastFlush == 0)
        return;
    if (!force && (now < mNextFlush))
        return;

    if (!IO::isDirectoryExists(mPathname.c_str()))
        if (!IO::createDirectory(mPathname.c_str()))
            return;
    char statsFilename[256];
    struct tm *stm = localtime(&mLastFlush);
    _snprintf(statsFilename, sizeof(statsFilename) - 1, "%s%c%d%02d%02d%02d%02d%02d.sta",
        mPathname.c_str(),
        IO::getPathSeparator(),
        stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
    FILE *statsFile = fopen(statsFilename, "w");
    if (statsFile == 0)
        return;
    char tmp[256];
    fprintf(statsFile, "%ld\n", mEvents.size());
    while (!mEvents.empty())
    {
        const StatEvent& evt = mEvents.front();
        stm = localtime(&evt.timestamp);
        _snprintf(tmp, sizeof(tmp) - 1, "%d%02d%02d%02d%02d%02d", stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
        fprintf(statsFile, "%s,%d,%d,%s\n", tmp, evt.type, evt.id, evt.desc.c_str());
        mEvents.pop_front();
    }
    fclose(statsFile);

    mLastFlush = mNextFlush;
    mNextFlush = (now - (now%mFlushPeriodSec)) + mFlushPeriodSec;
    mEvents.clear();
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
