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

#include "CTStringHelpers.h"

using namespace std;

namespace CommonTools {

//-------------------------------------------------------------------------------------
void StringHelpers::tokenize(const string& str, const string& delimiter, vector<string>& tokens)
{
    tokens.clear();
    string::size_type p0 = 0, p1 = string::npos;
    while (p0 != string::npos)
    {
        p1 = str.find_first_of(delimiter, p0);
        if (p1 != p0)
        {
            string token = str.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = str.find_first_not_of(delimiter, p1);
    }
}

//-------------------------------------------------------------------------------------
void StringHelpers::getURLHostPort(const string& url, string& host, string& port)
{
    host.clear();
    port.clear();
    string::size_type p = url.find_first_of("://");
    if (p != string::npos)
        host = url.substr(p + 3);
    else
        host = url;
    p = host.find_last_of(":");
    if (p != string::npos)
    {
        port = host.substr(p + 1);
        host = host.substr(0, p);
    }
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools