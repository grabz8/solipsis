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

#ifndef __CTStatsManager_h__
#define __CTStatsManager_h__

#include <time.h>
#include <list>
#include "CTSingleton.h"

namespace CommonTools {

/** This class gathers statistics.
 */
class StatsManager : public Singleton<StatsManager>
{
public:
    /// Type of statistic event
    enum StatEventType
    {
        SET_RELATIVE = 0,
        SET_ABSOLUTE
    };
    /// Statistics identifiers
    enum StatEventId
    {
        SEI_SERVER_FIRST = 100,
        SEI_SERVER_START = SEI_SERVER_FIRST,
        SEI_SERVER_END,
        SEI_SERVER_CLIENT_CONNECTION,
        SEI_SERVER_CLIENT_DISCONNECTION,
        SEI_SERVER_CLIENT_LOST,
        SEI_SERVER_CLIENT_REQFILETRANSFER,
        SEI_SERVER_LAST = 199,
        SEI_WSERVER_FIRST = 200,
        SEI_WSERVER_START = SEI_WSERVER_FIRST,
        SEI_WSERVER_END,
        SEI_WSERVER_REQ,
        SEI_WSERVER_RESP_ERROR,
        SEI_WSERVER_NAV_INCOMPATBILITY,
        SEI_WSERVER_AUTH_SUCCESS,
        SEI_WSERVER_AUTH_FAILURE,
        SEI_WSERVER_NEW_USER,
        SEI_WSERVER_NB_USERS,
        SEI_WSERVER_LAST = 299
    };

protected:
    /// Pathname of statistics files storage
    std::string mPathname;
    /// Flush period in seconds
    unsigned int mFlushPeriodSec;

    /// StatEvent structure
    typedef struct {
        time_t timestamp;
        StatEventType type;
        StatEventId id;
        std::string desc;
    } StatEvent;
    /// List of events for current period
    std::list<StatEvent> mEvents;

    /// Timestamp of last flush
    time_t mLastFlush;
    /// Timestamp of next flush
    time_t mNextFlush;

public:
    /** Constructor. */
    StatsManager();
    /** Destructor. */
    virtual ~StatsManager();

    /** Initialize.
    @param pathname Location where statistics file are stored
    */
    void initialize(const std::string& pathname = "stats", unsigned int flushPeriodSec = 60*60);
    /** Finalize. */
    void finalize();

    /** Add now 1 new statistic event.
    @param type The event type
    @param id The event identifier
    @param desc The event description
    */
    void addEvent(StatEventType type, StatEventId id, const std::string& desc);

    /** Override standard Singleton retrieval.
    @remarks
    Why do we do this? Well, it's because the Singleton
    implementation is in a .h file, which means it gets compiled
    into anybody who includes it. This is needed for the
    Singleton template to work, but we actually only want it
    compiled into the implementation of the class based on the
    Singleton, not all of them. If we don't change this, we get
    link errors when trying to use the Singleton-based class from
    an outside dll.
    @par
    This method just delegates to the template version anyway,
    but the implementation stays in this single compilation unit,
    preventing link errors.
    */
    static StatsManager& getSingleton(void);
    /** Override standard Singleton retrieval.
    @remarks
    Why do we do this? Well, it's because the Singleton
    implementation is in a .h file, which means it gets compiled
    into anybody who includes it. This is needed for the
    Singleton template to work, but we actually only want it
    compiled into the implementation of the class based on the
    Singleton, not all of them. If we don't change this, we get
    link errors when trying to use the Singleton-based class from
    an outside dll.
    @par
    This method just delegates to the template version anyway,
    but the implementation stays in this single compilation unit,
    preventing link errors.
    */
    static StatsManager* getSingletonPtr(void);

protected:
    void flush(time_t now, bool force = false);
};

} // namespace CommonTools

#endif // #ifndef __CTStatsManager_h__
