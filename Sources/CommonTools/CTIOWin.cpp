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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
char IO::getPathSeparator()
{
    return '\\';
}

//-------------------------------------------------------------------------------------
bool IO::getFilenames(const std::string& pathname, FilenameVector& filenames)
{
    WIN32_FIND_DATA File;
    HANDLE hSearch;
    std::string filename;

    // Get the current directory
    char *currentDir;
    if ((currentDir = _getcwd(NULL, 0)) == NULL)
        return false;

    // Update and set the current directory
    _chdir(pathname.c_str());

    hSearch = FindFirstFile("*.*", &File);
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        _chdir(currentDir);
        return false;
    }

    BOOL loop = true;
    do
    {
        filename = File.cFileName;
        if ((filename != ".") && (filename != ".."))
            if (!isDirectoryExists(filename))
                filenames.push_back(filename);
        loop = FindNextFile(hSearch, &File);
    } while (loop);

    FindClose(hSearch);
    _chdir(currentDir);

    return true;
}

} // namespace CommonTools
