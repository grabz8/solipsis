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

namespace Solipsis {

using namespace Ogre;

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
 * Write a message in the SOLIPSIS log file
 *
 *	\brief
 *
 *		Decorates and add a message in the SOLIPSIS log file
 *			- A message is composed of 2 parts (ex for inserting fileNames...)	
 *			- Add date and time at the beginning of the message 
 *			- Add the type of the message 
 *
 *	\param type = The type of the message : 0 for info, 1 for warning, 2 for error 
 *	\param message  = The first part of the message to insert
 *	\param message2 = The sond part of the message to insert
 */
void insertInLog(int type,const char *message,const char* message2);

/**
 * Write an error message in the SOLIPSIS log file
 *
 *	\param message  = The first part of the message to insert
 *	\param message2 = The sond part of the message to insert
 */
void SOLIPSISERROR(const char *message,const char* message2 = NULL);


/**
 * Write an warning message in the SOLIPSIS log file
 *
 *	\param message  = The first part of the message to insert
 *	\param message2 = The sond part of the message to insert
 */
void SOLIPSISWARNING(const char *message,const char* message2 = NULL);

/**
 * Write an warning message in the SOLIPSIS log file
 *
 *	\param message  = The first part of the message to insert
 *	\param message2 = The sond part of the message to insert
 */
void SOLIPSISINFO(const char *message,const char* message2 = NULL);


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