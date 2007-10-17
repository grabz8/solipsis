
#ifdef WIN32
	#include "windows.h"
	#include <String>
	//#include <string.h>
#else
	#warning "Code is not Linux compatible at the moment...."
#endif

#include <OgreNoMemoryMacros.h>

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
			HRESULT h = GetLastError();
			LPVOID lpMsgBuf;
			if (!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL ))
			{
				MessageBox(NULL,"Unknown Error","Error",MB_OK|MB_ICONEXCLAMATION);
				return NULL;
			}
			if (lpMsgBuf)
				MessageBox(NULL,(LPCSTR)lpMsgBuf,"Error",MB_OK|MB_ICONEXCLAMATION);
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
		return path;
#else
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* displayWindowForLoading(const char * pFilter , std::string pExtension)
	{
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
				MessageBox(NULL,"Unknown Error","Error",MB_OK|MB_ICONEXCLAMATION);
				return NULL;
			}
			if (lpMsgBuf)
				MessageBox(NULL,(LPCSTR)lpMsgBuf,"Error",MB_OK|MB_ICONEXCLAMATION);
			*/
			return NULL ;
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
		return path;
#else
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	char* browseImageToSave()
	{
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
				MessageBox(NULL,"Unknown Error","Error",MB_OK|MB_ICONEXCLAMATION);
				return NULL;
			}
			if (lpMsgBuf)
				MessageBox(NULL,(LPCSTR)lpMsgBuf,"Error",MB_OK|MB_ICONEXCLAMATION);
		}
	
		char* path = new char[strlen(buffer)];
		strcpy(path,buffer);
		return path;
#else
	return NULL;
#endif 
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------
	void displayMessageWindow(const char* title, const char* message)
	{
#ifdef WIN32
		MessageBox(NULL,message,title,MB_OK|MB_ICONEXCLAMATION);
#else
	return NULL;
#endif 
	}
}

