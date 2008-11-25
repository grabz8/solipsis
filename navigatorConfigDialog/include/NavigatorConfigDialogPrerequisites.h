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

#ifndef __NavigatorConfigDialogPrerequisites_h__
#define __NavigatorConfigDialogPrerequisites_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _NAVCONFDLG_EXPORT __declspec( dllexport )
#define _NAVCONFDLG_IMPORT

#if defined(NAVCONFDLG_SELF_BUILD)
#  	define NAVCONFDLG_EXPORT _NAVCONFDLG_EXPORT
#else
#  	define NAVCONFDLG_EXPORT _NAVCONFDLG_IMPORT
#endif

#  	define NAVCONFDLG_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _NAVCONFDLG_EXPORT __attribute__((dllexport))
#define _NAVCONFDLG_IMPORT __attribute__((dllimport))

#if defined(NAVCONFDLG_SELF_BUILD)
#  	define NAVCONFDLG_EXPORT _NAVCONFDLG_EXPORT
#  	define NAVCONFDLG_CW_EXPORT
#else
#  	define NAVCONFDLG_EXPORT
#  	define NAVCONFDLG_CW_EXPORT _NAVCONFDLG_IMPORT
#endif

#endif


#define NAVCONFDLGDIRECT_EXPORT _NAVCONFDLG_EXPORT

namespace Solipsis {

    // forward class declarations
    // this reduces cross-dependencies a lot

    class NavigatorConfigDialog;

} // namespace Solipsis

#endif // #ifndef __NavigatorConfigDialogPrerequisites_h__
