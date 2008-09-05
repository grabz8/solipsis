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

#ifndef __CTStringHelpers_h__
#define __CTStringHelpers_h__

#include <string>
#include <vector>

namespace CommonTools {

/** This static class contains several helper methods above system.
 */
class StringHelpers
{
public:
    /// Tokenize function
    static void tokenize(const std::string& str, const std::string& delimiter, std::vector<std::string>& tokens);
    /// Retrieve host and port of 1 url
    static void getURLHostPort(const std::string& url, std::string& host, std::string& port);
};

} // namespace CommonTools

#endif // #ifndef __CTStringHelpers_h__