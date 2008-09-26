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

#ifdef WIN32
	#include "windows.h"
	#include <String>
	//#include <string.h>
	#include <Winbase.h>
#else
	#warning "Code is not Linux compatible at the moment...."
#endif

#include <OgreNoMemoryMacros.h>

#include <CTSystem.h>

namespace Solipsis {

//#define sFilterSAF "Solipsis Avatar Files (*.saf)\0*.saf\0";
#define sFilterIMG "Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0";

namespace FileBrowser
{
	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* getExtension(const char* path)
	{
		if (path == NULL) return NULL;

		int i=-1;
		while(path[++i] != '\0');
		while(path[--i] != '.');
		char* extension = new char[strlen(path) - i];
		int j=0;
		while(path[++i] != '\0') extension[j++] = path[i];

		extension[j] = '\0';

		return extension;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* displayWindowForSaving(const char * pFilter , std::string pExtension)
	{
        CommonTools::System::setMouseCursorVisibility(true);
#ifdef WIN32


		// Filename buffer
		char buffer[65535];
		buffer[0]=0;

        // Create a dialog
		OPENFILENAME openFile;
		memset (&openFile, 0, sizeof (OPENFILENAME));
		openFile.lStructSize = sizeof (OPENFILENAME);
		openFile.hwndOwner = NULL;
		openFile.lpstrFilter = pFilter;
		openFile.nFilterIndex = 0;
		openFile.lpstrFile = buffer;
		openFile.nMaxFile = 65535;
		openFile.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_EXPLORER;
		openFile.lpstrDefExt = pExtension.c_str();
		

		if (!GetSaveFileName(&openFile))
		{
			/*
			HRESULT h = GetLastError();
			LPVOID lpMsgBuf;
			if (!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL ))
			{
                CommonTools::System::showMessageBox("Unknown Error", "Error", true, false, false, true, false);
				return NULL;
			}
			if (lpMsgBuf)
                CommonTools::System::showMessageBox(lpMsgBuf, "Error", true, false, false, true, false);
			*/
            CommonTools::System::setMouseCursorVisibility(false);
			return NULL ;
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
        CommonTools::System::setMouseCursorVisibility(false);
		return path;
#else
    CommonTools::System::setMouseCursorVisibility(false);
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* displayWindowForLoading(const char * pFilter , std::string pExtension)
	{
        CommonTools::System::setMouseCursorVisibility(true);
#ifdef WIN32

		// Filename buffer
		char buffer[65535];
		buffer[0]=0;

		OPENFILENAME openFile;
		memset (&openFile, 0, sizeof (OPENFILENAME));
		openFile.lStructSize = sizeof (OPENFILENAME);
		openFile.hwndOwner = NULL;
		openFile.lpstrFilter = pFilter;
		openFile.nFilterIndex = 0;
		openFile.lpstrFile = buffer;
		openFile.nMaxFile = 65535;
		openFile.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_EXPLORER;
		openFile.lpstrDefExt = pExtension.c_str();
		

		if (!GetOpenFileName(&openFile))
		{
			/*
			HRESULT h = GetLastError();
			LPVOID lpMsgBuf;
			if (!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL ))
			{
                CommonTools::System::showMessageBox("Unknown Error", "Error", true, false, false, true, false);
				return NULL;
			}
			if (lpMsgBuf)
                CommonTools::System::showMessageBox(lpMsgBuf, "Error", true, false, false, true, false);
			*/
            CommonTools::System::setMouseCursorVisibility(false);
			return NULL ;
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
        CommonTools::System::setMouseCursorVisibility(false);
		return path;
#else
    CommonTools::System::setMouseCursorVisibility(false);
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* browseImageToSave()
	{
        CommonTools::System::setMouseCursorVisibility(true);
#ifdef WIN32
		// Filename buffer
		char buffer[65535];
		buffer[0]=0;

		OPENFILENAME openFile;
		memset (&openFile, 0, sizeof (OPENFILENAME));
		openFile.lStructSize = sizeof (OPENFILENAME);
		openFile.hwndOwner = NULL;
		openFile.lpstrFilter = sFilterIMG;
		openFile.nFilterIndex = 0;
		openFile.lpstrFile = buffer;
		openFile.nMaxFile = 65535;
		openFile.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_EXPLORER;
		openFile.lpstrDefExt = "*.png;*.jpg;*.bmp";
		

		if (!GetSaveFileName(&openFile))
		{
			HRESULT h = GetLastError();
			LPVOID lpMsgBuf;
			if (!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL ))
			{
                CommonTools::System::showMessageBox("Unknown Error", "Error", true, false, false, true, false);
                CommonTools::System::setMouseCursorVisibility(false);
				return NULL;
			}
			if (lpMsgBuf)
                CommonTools::System::showMessageBox((char*)lpMsgBuf, "Error", true, false, false, true, false);
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
        CommonTools::System::setMouseCursorVisibility(false);
		return path;
#else
    CommonTools::System::setMouseCursorVisibility(false);
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	void displayMessageWindow(const char* title, const char* message)
	{
#ifdef WIN32
        CommonTools::System::setMouseCursorVisibility(true);
        CommonTools::System::showMessageBox(message, title, true, false, false, true, false);
        CommonTools::System::setMouseCursorVisibility(false);
#else
	return NULL;
#endif 
	}
}

} // namespace