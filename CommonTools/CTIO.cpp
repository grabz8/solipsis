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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sys\stat.h>
#include <direct.h>
#define GETCWD _getcwd
#define CHDIR _chdir
#else
#include <unistd.h>
#include <sys/stat.h>
#define GETCWD getcwd
#define CHDIR chdir
#endif

#include <fstream>

namespace CommonTools {

//-------------------------------------------------------------------------------------
std::string IO::getCWD()
{
    char *buffer;
    if ((buffer = GETCWD(NULL, 0)) == NULL)
        return "";

    std::string cwd = buffer;
    free(buffer);
    return cwd;
}

//-------------------------------------------------------------------------------------
void IO::setCWD(const std::string& pathname)
{
    CHDIR(pathname.c_str());
}

//-------------------------------------------------------------------------------------
bool IO::isFileExists(const std::string& filename)
{
    struct stat _stat;
    if (stat(filename.c_str(), &_stat) != 0) return false;
    return ((_stat.st_mode & S_IFMT) != 0);
}

//-------------------------------------------------------------------------------------
bool IO::isDirectoryExists(const std::string& pathname)
{
    struct stat _stat;
    if (stat(pathname.c_str(), &_stat) != 0) return false;
    return ((_stat.st_mode & S_IFDIR) != 0);
}

//-------------------------------------------------------------------------------------
bool IO::copyFile(const std::string& srcFilename, const std::string& dstFilename)
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
bool IO::deleteFile(const std::string& filename)
{
    return (_unlink(filename.c_str()) == 0);
}

//-------------------------------------------------------------------------------------
bool IO::renameFile(const std::string& srcFilename, const std::string& dstFilename)
{
    if (isFileExists(dstFilename))
        deleteFile(dstFilename);
    return (rename(srcFilename.c_str(), dstFilename.c_str()) == 0);
}

//-------------------------------------------------------------------------------------
std::string IO::retrieveRelativePathByDescendingCWD(const std::string& pathname)
{
    std::string fullPath;
    char *cwd = _getcwd(NULL, 0);
    std::string currentPath = cwd;
    std::string relativePath = pathname;
    while (currentPath.find_last_of("\\") != std::string::npos)
    {
        fullPath = currentPath + "\\" + pathname;
        if (isDirectoryExists(fullPath))
            return relativePath;
        currentPath = currentPath.substr(0, currentPath.find_last_of("\\"));
        relativePath = "..\\" + relativePath;
    }
    return "";
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
