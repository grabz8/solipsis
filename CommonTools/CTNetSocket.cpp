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

#include "CTNetSocket.h"

#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#endif

namespace CommonTools {

//-------------------------------------------------------------------------------------
bool NetSocket::getMyIP(IPAddressVector& IPAddresses)
{
    IPAddresses.clear();

    char name[80];
    if (gethostname(name, sizeof(name)) == -1)
        return false;

    struct hostent *phe = gethostbyname(name);
    if (phe == 0)
        return false;

    for (int i = 0; phe->h_addr_list[i] != 0 && i < 10; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        IPAddresses.push_back(inet_ntoa(addr));
    }

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
