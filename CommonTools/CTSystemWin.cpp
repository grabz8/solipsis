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

#include "CTSystem.h"

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#include <shellapi.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
void System::sleep(unsigned long durationMs)
{
    Sleep((DWORD)durationMs);
}

//-------------------------------------------------------------------------------------
void System::setMouseCursorVisibility(bool visible)
{
	ShowCursor((BOOL)visible);
}

//-------------------------------------------------------------------------------------
void System::showMessageBox(const std::string& text, const std::string& caption, bool okButton, bool okCancelButton, bool iconAsterisk, bool iconExclamation, bool iconHand)
{
    MessageBox(NULL, text.c_str(), caption.c_str(), (okButton?MB_OK:0) | (okCancelButton?MB_OKCANCEL:0) | (iconAsterisk?MB_ICONASTERISK:0) | (iconExclamation?MB_ICONEXCLAMATION:0) | (iconHand?MB_ICONHAND:0));
}

//-------------------------------------------------------------------------------------
void System::runExternalWebBrowser(const std::string& url)
{
    ShellExecute(0, "open", url.c_str(), 0, 0, SW_SHOWNORMAL);
}

//-------------------------------------------------------------------------------------
int System::getPID()
{
    return _getpid();
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools