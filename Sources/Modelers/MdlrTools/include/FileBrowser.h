/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL & Patrice Desfonds

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
		FileBrowser.h
	\brief
		Solipsis File Browser
	\author
		ARTEFACTO - François FOURNEL & Patrice Desfonds
*/

#ifndef __FileBrowser_h__
#define __FileBrowser_h__

#include <String>

namespace Solipsis {

namespace FileBrowser
{

/*
 *
 *	\brief 
 *		Method which display a window in which the user can browse the files and folders on his disks
 *			and choose an place to save a file.
 *
 *	\param pFilter = filtre files with a specicfic extension (ex : pFilter = "Solipsis Files (*.saf)\0*.saf\0" )
 *	\param pExtension = extension of the file saving (ex : pExtension = "*.saf")
 *	\return The path of the file choosed by the user, can be an invalid path ! If the user click on Cancel,
 *				then NULL is returned.
 *
*/
	char* displayWindowForSaving(const char * pFilter, std::string pExtension );

/*
 *
 *	\brief 
 *		Method which display a window in which the user can browse the files and folders on his disks
 *			and choose a place to load a file.
 *
 *	\param pFilter = filtre files with a specicfic extension (ex : pFilter = "Solipsis Files (*.saf)\0*.saf\0" )
 *	\param pExtension = extension of the file saving (ex : pExtension = "*.saf")
 *	\return The path of the file choosed by the user, can be an invalid path ! If the user click on Cancel,
 *				then NULL is returned.
 *
*/
	char* displayWindowForLoading(const char * pFilter , std::string pExtension);
		
/*
 *
 *	\brief 
 *		Method which display a window in which the user can browse the files and folders on his disks
 *			and choose an place to save an image (jpg, bmp...).
 *	\return The path of the image choosed by the user, should return a valid path. If the user click 
 *				on Cancel, then NULL is returned.
*/
	char* browseImageToSave();

/*
 *
 *	\brief 
 *		Method which display a window with a message and title passed in parameter.
 *
 *	\param title = Title of the window.
 *	\param message = Message to display in the window.
 *
*/
	void displayMessageWindow(const char* title, const char* message);

}

} //namespace

#endif
