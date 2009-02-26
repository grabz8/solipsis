#ifndef OS_INDEPENDENT_FUNCTIONS_AND_FREE_DIET_COKE
#define OS_INDEPENDENT_FUNCTIONS_AND_FREE_DIET_COKE


#include <Winsock2.h>
#include <windows.h>

namespace Tools
{
	//------------EXCEPTIONS STUFF-----------------//
	inline std::string systemError()
	{
		std::string errorMsg;
		char* s; 
		if(::FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM,   
			NULL,  
			GetLastError(),
			0,
			(LPTSTR)&s,
			0,
			NULL)
			) 
		{
			char* p = strchr(s, '\r');
			if(p != NULL)
			{
				*p = 0;
			}
			errorMsg = s;
			::LocalFree(s);
		}
		return errorMsg;
	}

	inline std::string networkError()
	{
		std::stringstream strm; 
		strm  <<"WSA error = " << ::WSAGetLastError();
		return strm.str();
	}

	//------------PATHNAME STUFF-----------------//


	const char SEP_SLASH='\\';

	inline std::string getModuleDir()
	{
		

		char buf[_MAX_PATH];		
		::GetModuleFileNameA( ::GetModuleHandle(0),buf, sizeof(buf) / sizeof (TCHAR) );
		* strrchr( buf,SEP_SLASH) = 0;
		return std::string ( buf );
	}
	


	//---------------USEFUL FUNCTIONS----------

	inline void Sleep(int milli)
	{
		::Sleep(milli);
	}
}





#endif