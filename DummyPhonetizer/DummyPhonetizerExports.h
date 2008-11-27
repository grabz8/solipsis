/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Guillaume Raffy

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

#ifndef __DummyPhonetizerExports_h__
#define __DummyPhonetizerExports_h__

#include "OgreException.h"

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
	#ifdef DUMMYPHONETIZER_EXPORTS
		#define DUMMYPHONETIZER_EXPORT __declspec(dllexport)
	#else
		#if defined( __MINGW32__ )
			#define DUMMYPHONETIZER_EXPORT
		#else
			#define DUMMYPHONETIZER_EXPORT __declspec(dllimport)
		#endif
	#endif
#elif defined ( OGRE_GCC_VISIBILITY )
	#define DUMMYPHONETIZER_EXPORT  __attribute__ ((visibility("default")))
#else
	#define DUMMYPHONETIZER_EXPORT
#endif	// OGRE_WIN32

#endif // __DummyPhonetizerExports_h__
