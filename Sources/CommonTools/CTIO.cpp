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
#define MKDIR _mkdir
// we 've got our own implementation
#include "direntWindows/dirent.h"
#else
#include <unistd.h>
#include <sys/stat.h>
#define GETCWD getcwd
#define CHDIR chdir
#define MKDIR _mkdir
#include <dirent.h>
#endif

#include <fstream>

#include <direct.h>
#include <errno.h>
#include <string>

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
bool IO::createDirectory(const std::string& pathname)
{
    // Try creating it directly
    if ((MKDIR(pathname.c_str()) == 0) || (errno == EEXIST))
        return true;

    // Create intermediate directories
    std::string::size_type iSeparator = pathname.find_first_of(getPathSeparator(), 0);
    std::string currentPath;
    while ((iSeparator = pathname.find_first_of(getPathSeparator(), iSeparator + 1)) != std::string::npos)
    {
        currentPath = pathname.substr(0, iSeparator);
        if (isDirectoryExists(currentPath))
            continue;
        if (MKDIR(currentPath.c_str()) != 0)
            return false;
    }

    return (MKDIR(pathname.c_str()) == 0);
}

//-------------------------------------------------------------------------------------
long IO::getFileSize(const std::string& filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp == 0) return -1;
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
	fclose(fp);
    return length;
}

//-------------------------------------------------------------------------------------
std::string IO::getFileName(const std::string& pathname)
{
    std::string::size_type sep = pathname.find_last_of(getPathSeparator());
    if (sep == std::string::npos)
        return pathname;
    if (sep == pathname.length() - 1)
        return std::string();
    return pathname.substr(sep + 1);
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
bool IO::writeFileContent(const std::string& filename, char *data, unsigned dataLength)
{
    if (filename.length() == 0) return false;
    if ((data == 0) || (dataLength == 0)) return false;

    FILE *fp = fopen(filename.c_str(), "wb");
    if (fp == 0) return false;
    fwrite(data, 1, dataLength, fp);
    fclose(fp);

    return true;
}

//-------------------------------------------------------------------------------------
std::string IO::retrieveRelativePathByDescendingCWD(const std::string& pathname)
{
    std::string fullPath;
    char *cwd = GETCWD(NULL, 0);
    std::string currentPath = cwd;
    std::string relativePath = pathname;
    while (currentPath.find_last_of(getPathSeparator()) != std::string::npos)
    {
        fullPath = currentPath + getPathSeparator() + pathname;
        if (isDirectoryExists(fullPath))
            return relativePath;
        currentPath = currentPath.substr(0, currentPath.find_last_of(getPathSeparator()));
        relativePath = std::string("..") + getPathSeparator() + relativePath;
    }
    return "";
}

//-------------------------------------------------------------------------------------
/**
* A recursive function to remove a directory and it's contents
* Author: Danny Battison
* Contact: gabehabe@gmail.com
*/
bool IO::RemoveDir(const std::string & path1)
{
    std::string path = path1;
    if (path[path.length()-1] != '\\') path += "\\";
    // first off, we need to create a pointer to a directory
    DIR *pdir = NULL; // remember, it's good practice to initialise a pointer to NULL!
    pdir = opendir (path.c_str());
    struct dirent *pent = NULL;
    if (pdir == NULL) { // if pdir wasn't initialised correctly
        return false; // return false to say "we couldn't do it"
    } // end if
    char file[256];

    int counter = 1; // use this to skip the first TWO which cause an infinite loop (and eventually, stack overflow)
    while (pent = readdir (pdir)) { // while there is still something in the directory to list
        if (counter > 2) {
            for (int i = 0; i < 256; i++) file[i] = '\0';
            strcat(file, path.c_str());
            if (pent == NULL) { // if pent has not been initialised correctly
                return false; // we couldn't do it
            } // otherwise, it was initialised correctly, so let's delete the file~
            strcat(file, pent->d_name); // concatenate the strings to get the complete path
            if (isDirectory(file) == true) {
                RemoveDirectory(file);
            } else { // it's a file, we can use remove
                remove(file);
            }
        } counter++;
    }

    // finally, let's clean up
    closedir (pdir); // close the directory
    if (!_rmdir(path.c_str())) return false; // delete the directory
    return true;
}

//-------------------------------------------------------------------------------------
bool IO::isDirectory(const char path[]) 
{
    size_t i = strlen(path) - 1;
    if (path[strlen(path)] == '.') 
    {
        return true;
    } // exception for directories
    // such as \. and \..
    for(i; i >= 0; i--) 
    {
        if (path[i] == '.') 
            return false; // if we first encounter a . then it's a file
        else if (path[i] == '\\' || path[i] == '/') 
            return true; // if we first encounter a \ it's a dir
    }
    return false;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
