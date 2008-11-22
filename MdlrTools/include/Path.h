/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __Path_h__
#define __Path_h__

//regular mem handler
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>

using namespace Ogre;

namespace Solipsis {


/// brief This is the class representing a file or folder path. It is usefull because from a path with "/" it can return a path formated for the OS (with "/" for unix-linux, "\\" for windows). It can also exctract the extension of the path if there is one.
/// file Path.h
/// author François FOURNEL
/// date 2007.06.25


class Path
{
public:
	///brief Constructor
	///param path Path of the file or folder, the same format for all OS, with "/".
	Path(const String& path) : mUniversalPath(path), mFormatedPath(path)
	{
        // to be sure path is in universal format
		for(size_t indexChar = 0 ; indexChar < mUniversalPath.length() ; indexChar++)
		{
			if (mUniversalPath[indexChar] == '\\') mUniversalPath[indexChar] = '/';
		}
#ifdef WIN32
		for(size_t indexChar = 0 ; indexChar < mFormatedPath.length() ; indexChar++)
		{
			if (mFormatedPath[indexChar] == '/') mFormatedPath[indexChar] = '\\';
		}
#endif
	}

	///brief Method which return the universal path of the file or folder the same format for all OS, with "/".
	///return The universal path of the file or folder the same format for all OS, with "/".
	const String& getUniversalPath() const { return mUniversalPath; };
	const String& getUniversalPath(){ return mUniversalPath; };

	///brief Method which return the path of the file or folder, formated to the current OS ("/" for unix, "\\" for windows).
	///return The path of the file or folder, formated to the current OS ("/" for unix, "\\" for windows).
	const String& getFormatedPath() const { return mFormatedPath; };
	const String& getFormatedPath(){ return mFormatedPath; };

	///brief Method which return the extension of the last file of the path (if it as an extension).
	///return The extension of the last file of the path (if it as an extension).
	String getExtension()
	{
		size_t indexChar = mUniversalPath.length()-1;
		while((indexChar > 0) && (mUniversalPath[indexChar] != '/') && (mUniversalPath[indexChar] != '.'))
		{
			indexChar--;
		}

		if ((indexChar <= 0) || (mUniversalPath[indexChar] == '/')) return "";

		return mUniversalPath.substr(indexChar+1,mUniversalPath.length() - (indexChar+1));
	}

	///Method which return the name of the last file (or folder) of the path.
	///param withExtension If true the file's name will be returned without it's extension, for example if the path is "folder/file.ext" the function will return "file.ext" wheras if it is false the function will return "file".
	///return the name of the last file (or folder) of the path.
	String getLastFileName(bool withExtension=true)
	{
		String path = mUniversalPath;
		if (!withExtension)
		{
			size_t totalSizeExtension = getExtension().length()+1;
			path = mUniversalPath.substr(0,mUniversalPath.length() - totalSizeExtension); 
		}

		long indexChar = (long)path.length()-1;
		while((indexChar >= 0) && (path[indexChar] != '/'))
		{
			indexChar--;
		}

		return path.substr(indexChar+1,path.length() - (indexChar+1));
	}

	///Method which return the universal path of the folder where is placed the last file, in means the universal path less the last file (and without the last "/").
	///return The path of the folder where is placed the last file, in means the universal path less the last file.
	String getUniversalRootPath()
	{
		long indexChar = (long)mUniversalPath.length()-1;
		while((indexChar >= 0) && (mUniversalPath[indexChar] != '/'))
		{
			indexChar--;
		}

		return mUniversalPath.substr(0,indexChar);
	}

	///Method which return the formated path of the folder where is placed the last file, in means the universal path less the last file (and without the last "/").
	///return The path of the folder where is placed the last file, in means the universal path less the last file.
	String getFormatedRootPath()
	{
		long indexChar = (long)mFormatedPath.length()-1;
		while((indexChar >= 0) && (mFormatedPath[indexChar] != '\\'))
		{
			indexChar--;
		}

		return mFormatedPath.substr(0,indexChar);
	}

	Path operator/(const Path& path)
	{
		return Path(this->getUniversalPath() + "/" + path.getUniversalPath());
	}


private:
	String mUniversalPath;		///brief Path of the file or folder, the same for all OS, it means with "/".
	String mFormatedPath;		///brief Path of the file or folder, formated to the current OS ("/" for unix, "\\" for windows).
};

} //namespace

#endif
