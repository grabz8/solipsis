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

#ifndef __CTIO_h__
#define __CTIO_h__

#include <string>
#include <vector>

namespace CommonTools {

/** This static class contains several helper methods for Input/Output.
 */
class IO
{
public:
    typedef std::vector<std::string> FilenameVector;

public:
    /// Get the current working directory
    static std::string getCWD();
    /// Set the current working directory
    static void setCWD(const std::string& pathname);
    /// Returns true if a file exists
    static bool isFileExists(const std::string& filename);
    /// Returns true if a directory exists
    static bool isDirectoryExists(const std::string& pathname);
    /// Returns true if a directory was created
    static bool createDirectory(const std::string& pathname);
    /// Returns the size of a file
    static long getFileSize(const std::string& filename);
    /// Returns the path separator
    static char getPathSeparator();
    /// Returns the filename without path
    static std::string getFileName(const std::string& pathname);
    /// Copy a file
    static bool copyFile(const std::string& srcFilename, const std::string& dstFilename);
    /// Delete a file
    static bool deleteFile(const std::string& filename);
    /// Rename a file
    static bool renameFile(const std::string& srcFilename, const std::string& dstFilename);
    /// Write a file content
    static bool writeFileContent(const std::string& filename, char *data, unsigned dataLength);
    /// Get filenames contained in a directory
    static bool getFilenames(const std::string& pathname, FilenameVector& filenames);
    /// Retrieve a relative path by descending from the current working directory
    static std::string retrieveRelativePathByDescendingCWD(const std::string& pathname);
    
    /// check if the directory exists
    static bool FolderExist(std::string strPath);
    
    /// recursively remove a directory
    static bool RemoveDir(const std::string & path);

private:
    /// check if the path look like a directory. 
    static bool isDirectory(const char path[]);
};

} // namespace CommonTools

#endif // #ifndef __CTIO_h__
