/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - Stéphane CHAPLAIN

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

/**
	\file 
		SolipsisErrorHandler.h
	\brief
		Solipsis erros handling
	\author
		ARTEFACTO - Stéphane CHAPLAIN
*/

#ifndef _SOLIPSISERRORS_H 
#define _SOLIPSISERRORS_H 

#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <unistd.h>
#endif
#include <direct.h>

#include <Ogre.h>

using namespace Ogre;

namespace Solipsis {

// Increment if there are major upgrades 
#define SOLMODVERSION "1.0.1" 

// Standard error codes
/// Error opening a file 
#define SOL_ERROR_CANNOTOPENFILE 1
/// Error writing to a file
#define SOL_ERROR_CANNOTWRITETOFILE 1


typedef std::vector<unsigned int> uintvector;
typedef std::vector<Real> realvector;

/**
 * Substitute a substring in another one 
 *
 *	\brief
 *
 *		Find all occurences of a string in another one and replace them by a second string
 *
 *	\param src = The source string where we have to do the sustitutions
 *	\param toFind = The substring to replace 
 *	\param subst = the substitution string
 *	\return a string copy of the source where the substitutions are done
 */
Ogre::String replaceSubstr(const Ogre::String &src, const char* toFind, const char* subst);

/**
 * Convert any string to a type T  
 *
 *
 *	\param Str = The source string to convert
 *	\param toFind = The destination object to fill
 *	\return true on success
 */
template<typename T> 
bool from_string( const char* Str, T & Dest )
{
    // créer un flux à partir de la chaîne donnée
    std::istringstream iss( Str );
    // tenter la conversion vers Dest
    return iss >> Dest != 0;
}

/**
 * Delete a file 
 *
 *	\brief
 *
 *		Remove a file from disk
 *
 *	\param filePath = The path to the file to delete
 *	\return true if the file is successfully deleted
 */
bool SOLdeleteFile(const char *filePath);

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
bool SOLcopyFile(const char *srcPath,const char *destPath);


/**
 * Test is the path is a directory
 *
 *	\brief
 *
 *		Tells if a path is a directory path
 *
 *	\param pathName = The path to test
 *	\return true if the path is a directory 
 */
bool SOLisDirectory(const char* pathName);

/**
 * List all files in a directory
 *
 *	\brief
 *
 *		Fills a string vector with all fileNames
 *
 *	\param path = The directory to scan
 *	\param toFill = the string vector to fill 
 *	\return true if no error occured
 */
bool SOLlistDirectoryFiles(const char* path,std::vector<std::string> *toFill);

} //namespace

#endif