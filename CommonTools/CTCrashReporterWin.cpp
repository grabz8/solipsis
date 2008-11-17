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

#include "CTCrashReporter.h"

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>

// minidump write function based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                         CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

namespace CommonTools {

//-------------------------------------------------------------------------------------

/** This class implements the Windows crash reporter.
 */
class CrashReporterWin : public CrashReporter
{
public:
    /// Initialize
    virtual void initialize(const std::string& applicationName, const std::string& minidumpFilename);

protected:
    /// Top level filter of unhandled exceptions
    static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
};

CrashReporterWin ms_CrashReporterWin;

CrashReporter* CrashReporter::ms_Singleton = &ms_CrashReporterWin;
std::string CrashReporter::ms_ApplicationName;
std::string CrashReporter::ms_MinidumpFilename;

//-------------------------------------------------------------------------------------
void CrashReporterWin::initialize(const std::string& applicationName, const std::string& minidumpFilename)
{
    CrashReporter::initialize(applicationName, minidumpFilename);
    ::SetUnhandledExceptionFilter(TopLevelFilter);
}

//-------------------------------------------------------------------------------------
LONG CrashReporterWin::TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    LONG retval = EXCEPTION_CONTINUE_SEARCH;
    HWND hParent = NULL;						// find a better value for your app

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old 
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    char szDbgHelpPath[_MAX_PATH];

    // try to load dbghelp.dll from current path
    if (GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH))
    {
        char *pSlash = _tcsrchr(szDbgHelpPath, '\\');
        if (pSlash)
        {
            _tcscpy(pSlash+1, "DBGHELP.DLL");
            hDll = ::LoadLibrary(szDbgHelpPath);
        }
    }

    // load any version we can if not found into current path
    if (hDll == NULL)
        hDll = ::LoadLibrary("DBGHELP.DLL");

    LPCTSTR szResult = NULL;
    if (hDll)
    {
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        if (pDump)
        {
            char szDumpPath[_MAX_PATH];
            char szScratch [_MAX_PATH];

            // work out a good place for the dump file
            if (!GetTempPath(_MAX_PATH, szDumpPath))
                _tcscpy(szDumpPath, "c:\\");
            _tcscat(szDumpPath, ms_MinidumpFilename.c_str());
            _tcscat(szDumpPath, ".dmp");

            // ask the user if they want to save a dump file
            if (::MessageBox(NULL, "Something bad happened in your program, would you like to save a diagnostic file?", ms_ApplicationName.c_str(), MB_YESNO) == IDYES)
            {
                // create the file
                HANDLE hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
                    ExInfo.ThreadId = ::GetCurrentThreadId();
                    ExInfo.ExceptionPointers = pExceptionInfo;
                    ExInfo.ClientPointers = NULL;

                    // write the dump
                    BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
                    if (bOK)
                    {
                        sprintf(szScratch, "Saved dump file to '%s'", szDumpPath);
                        szResult = szScratch;
                        retval = EXCEPTION_EXECUTE_HANDLER;
                    }
                    else
                    {
                        sprintf(szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError());
                        szResult = szScratch;
                    }
                    ::CloseHandle(hFile);
                }
                else
                {
                    sprintf(szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError());
                    szResult = szScratch;
                }
            }
        }
        else
            szResult = "DBGHELP.DLL too old";
    }
    else
        szResult = "DBGHELP.DLL not found";

    if (szResult)
        ::MessageBox(NULL, szResult, ms_ApplicationName.c_str(), MB_OK);

    return retval;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools