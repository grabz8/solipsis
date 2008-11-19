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
#include "CTSystem.h"

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
    virtual void initialize(const std::string& applicationName, const std::string& minidumpPathname, const std::string& minidumpFilename, const std::string& message);

protected:
    /// Top level filter of unhandled exceptions
    static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
};

CrashReporterWin ms_CrashReporterWin;

CrashReporter* CrashReporter::ms_Singleton = &ms_CrashReporterWin;
std::string CrashReporter::ms_ApplicationName;
std::string CrashReporter::ms_MinidumpPathname;
std::string CrashReporter::ms_MinidumpFilename;
std::string CrashReporter::ms_Message;

//-------------------------------------------------------------------------------------
void CrashReporterWin::initialize(const std::string& applicationName, const std::string& minidumpPathname, const std::string& minidumpFilename, const std::string& message)
{
    CrashReporter::initialize(applicationName, minidumpPathname, minidumpFilename, message);
    ::SetUnhandledExceptionFilter(TopLevelFilter);
}

//-------------------------------------------------------------------------------------
LONG CrashReporterWin::TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    LONG retval = EXCEPTION_CONTINUE_SEARCH;
    HWND hParent = NULL;						// find a better value for your app

    // be sure the mouse is visible
    System::setMouseCursorVisibility(true);

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old 
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    char szCWDPath[_MAX_PATH];
    char szDbgHelpPath[_MAX_PATH];
    szCWDPath[0] = '\0';
    szDbgHelpPath[0] = '\0';

    // try to load dbghelp.dll from current path
    if (GetModuleFileName(NULL, szCWDPath, _MAX_PATH))
    {
        _tcscpy(szDbgHelpPath, szCWDPath);
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

    // work out a good place for the dump file
    char szCrashReportPath[_MAX_PATH];
    szCrashReportPath[0] = '\0';
    if (!ms_MinidumpPathname.empty())
        _tcscpy(szCrashReportPath, ms_MinidumpPathname.c_str());
    else
    {
        if (!GetTempPath(_MAX_PATH, szCrashReportPath))
            _tcscpy(szCrashReportPath, szCWDPath);
    }
    if (szCrashReportPath[_tcslen(szCrashReportPath) - 1] != '\\')
        _tcscat(szCrashReportPath, "\\");
    BOOL displayFileBrowser = FALSE;
    LPCTSTR szResult = NULL;
    if (hDll)
    {
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        if (pDump)
        {
            char szDumpPath[_MAX_PATH];
            char szScratch[_MAX_PATH];
            szDumpPath[0] = '\0';
            szScratch[0] = '\0';

            _tcscpy(szDumpPath, szCrashReportPath);

            // ask the user if they want to save a dump file
            if (::MessageBox(NULL, "Sorry, a crash was detected,\nwould you like to help us by sending report files ?", ms_ApplicationName.c_str(), MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
            {
                // create the minidump file
                _tcscat(szDumpPath, ms_MinidumpFilename.c_str());
                _tcscat(szDumpPath, ".dmp");
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
                        sprintf(szScratch, "Dump file saved:\n%s", szDumpPath);
                        if (!ms_Message.empty())
                        {
                            _tcscat(szScratch, "\n");
                            _tcscat(szScratch, ms_Message.c_str());
                        }
                        szResult = szScratch;
                        displayFileBrowser = TRUE;
                        retval = EXCEPTION_EXECUTE_HANDLER;
                    }
                    else
                    {
                        sprintf(szScratch, "Failed to save dump file:\n%s\n(error %d)", szDumpPath, GetLastError());
                        szResult = szScratch;
                    }
                    ::CloseHandle(hFile);
                }
                else
                {
                    sprintf(szScratch, "Failed to create dump file:\n%s\n(error %d)", szDumpPath, GetLastError());
                    szResult = szScratch;
                }
            }
            else
                retval = EXCEPTION_EXECUTE_HANDLER;
        }
        else
            szResult = "DBGHELP.DLL too old !";
    }
    else
        szResult = "DBGHELP.DLL not found !";

    if (szResult)
    {
        ::MessageBox(NULL, szResult, ms_ApplicationName.c_str(), MB_ICONINFORMATION | MB_OK);
        if (displayFileBrowser)
            System::runExternalFileBrowser(std::string(szCrashReportPath));
    }

    return retval;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools