/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

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


#include "SolipsisErrorHandler.h"
#include <sys/stat.h>
#include <CTLog.h>

using namespace CommonTools;

namespace Solipsis {

Ogre::String replaceSubstr(const Ogre::String &src, const char* toFind, const char* subst)
{
	Ogre::String return_value = src; // backup original string
	// Find first occurence of toFind string
	size_t ind = return_value.find(toFind);
	do 
	{
		if (ind == return_value.npos) 
			break; // No more to find
		// Substitute substr
		return_value.replace(ind,strlen(toFind),subst);
		// Find next occurence of substr
		ind = return_value.find(toFind, ind);

	} while (ind != return_value.npos);

	return return_value;
}

bool SOLdeleteFile(const char *filePath)
{
#ifdef WIN32
	if (!DeleteFile(filePath))
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLdeleteFile() Unable to delete file - Please verify sharing access and user rights :", filePath);
#else
	// if the file exists and there's no write access
	if (access (filePath, F_OK) == 0)
	{
		// try to set the read/write access
		if (chmod (filePath, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH) == -1)
		{
            LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLdeleteFile() Can't set RW access to file :", filePath);
			return false;
		}
		int err = unlink (filePath);
		if (err == -1)
		{
            LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLdeleteFile() Unable to delete file - Please verify sharing access and user rights :", filePath);
			return false;
		}
	}
	else
	{
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLdeleteFile() Can't access to file :", filePath);
	}
#endif 
	return true;
}

/**
 * Copy a file 
 *
 *	\brief
 *
 *		Copy a file from a path to another
 *
 *	\param srcPath = The path to the source file 
 *	\param destPath = The path to the destination file
 *	\return true if the file is successfully copied
 */
bool SOLcopyFile(const char *srcPath,const char *destPath)
{
	std::ifstream in(srcPath, std::ios::in | std::ios::binary);
	std::ofstream out(destPath, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!in)
	{
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLcopyFile() Unable to open source file :", srcPath);
		return false;
	}

	if (!out)
	{
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "SOLcopyFile() Unable to open dest file :", destPath);
		return false;
	}
	
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

bool SOLisDirectory(const char* pathName)
{
	struct stat my_stat;
	if (stat(pathName, &my_stat) != 0) return false;
	return ((my_stat.st_mode & S_IFDIR) != 0);
}

bool SOLlistDirectoryFiles(const char* path,std::vector<std::string> *toFill)
{
#ifdef WIN32
	WIN32_FIND_DATA File;
	HANDLE hSearch;
	BOOL re;
	std::string nomFic;

	// Get the current directory
	char* currentDir;
	if( (currentDir = _getcwd( NULL, 0 )) == NULL )
		return FALSE;

	// Update and set the current directory
/*	std::string newPath (currentDir);
	newPath += "\\";
	newPath += path;
	_chdir(newPath.c_str());*/
	_chdir(path);

	hSearch=FindFirstFile("*.*", &File);
	if(hSearch ==  INVALID_HANDLE_VALUE)
	{
		_chdir(currentDir);
		return FALSE;
	}

	re=TRUE;
	do
	{
		nomFic = File.cFileName;
		if ((nomFic != ".") && (nomFic != ".."))
		{
			if (!SOLisDirectory(nomFic.c_str()))
			{ // It is not a directory name
				toFill->push_back(nomFic);
			}
		}
		/* Traitement */
		re = FindNextFile(hSearch, &File);
	} while(re);

	FindClose(hSearch);
	_chdir(currentDir);
#else
	struct dirent *lecture;
	DIR *rep;
	rep = opendir("path");
	while ((lecture = readdir(rep)))
	{
		toFill->push_back(lecture->d_name);
	}
	closedir(rep);
#endif
	return true;
}

} //namespace