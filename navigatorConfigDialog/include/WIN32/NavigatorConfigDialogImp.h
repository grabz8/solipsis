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

#ifndef __NavigatorConfigDialogImp_h__
#define __NavigatorConfigDialogImp_h__

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//Get around Windows hackery
#ifdef max
#  undef max
#endif
#ifdef min
#  undef min
#endif

namespace Solipsis {

/** Defines the behaviour of the Solipsis renderer configuration dialog.
*/
class NAVCONFDLG_EXPORT NavigatorConfigDialog
{
public:
    NavigatorConfigDialog();
	~NavigatorConfigDialog();
    
	/** Displays the dialog.
	@remarks
		This method displays the dialog and from then on the dialog
		interacts with the user independently. The dialog will be
		calling the relevant OGRE rendering systems to query them for
		options and to set the options the user selects. The method
		returns when the user closes the dialog.
	@returns
		If the user accepted the dialog, <b>true</b> is returned.
	@par
		If the user cancelled the dialog (indicating the application
		should probably terminate), <b>false</b> is returned.
	@see
		RenderSystem
	*/
    bool display();

protected:
    /** Callback to process window events */
#if OGRE_ARCHITECTURE_64 == OGRE_ARCH_TYPE
    static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
#else
    static BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
#endif
    Ogre::RenderSystem* mSelectedRenderSystem;
    HINSTANCE mHInstance; // HInstance of application, for dialog
};

} // namespace Solipsis

#endif // #ifndef __NavigatorConfigDialogImp_h__
