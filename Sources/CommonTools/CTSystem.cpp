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

#include "CTSystem.h"

#include <time.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
std::string System::getDateTimeYYYYMMDDHHMMSS()
{
    time_t now;
    time(&now);
    struct tm *now_tm = localtime(&now);
    char timeBuf[16];
    _snprintf(timeBuf, sizeof(timeBuf) - 1, "%d%02d%02d%02d%02d%02d", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
    timeBuf[sizeof(timeBuf) - 1] = '\0';
    return timeBuf;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools