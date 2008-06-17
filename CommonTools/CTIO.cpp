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

#include "CTIO.h"

#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sys\stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <fstream>

namespace CommonTools {

//-------------------------------------------------------------------------------------
bool IO::isFileExists(std::string& filename)
{
    struct stat _stat;
    if (stat(filename.c_str(), &_stat) != 0) return false;
    return ((_stat.st_mode & S_IFMT) != 0);
}

//-------------------------------------------------------------------------------------
bool IO::copyFile(std::string& srcFilename, std::string& dstFilename)
{
    std::ifstream in(srcFilename.c_str(), std::ios::in | std::ios::binary);
    std::ofstream out(dstFilename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!in)
        return false;
    if (!out)
        return false;

    std::streamsize c = 0;
    char tmpBuf[2048];
    while (!in.eof())
    {
        // Read until 2048 chars in the source file
        in.read(tmpBuf, 2048);
        // Number of chars we read
        c = in.gcount();
        // Write read characters to dest file
        out.write(tmpBuf, c);
    }
    // Close the two files
    in.close();
    out.close();

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
