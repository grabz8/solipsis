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

#ifndef __PhysXPluginPrerequisites_h__
#define __PhysXPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_PEERPHYSXPLUGIN_EXPORTS
#		define _PeerPhysXPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _PeerPhysXPluginExport
#       else
#    		define _PeerPhysXPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _PeerPhysXPluginExport  __attribute__ ((visibility("default")))
#else
#	define _PeerPhysXPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class PhysXPlugin;
    class PhysXEngine;
    class PhysXScene;
    class PhysXBody;
    class PhysXCharacter;
    class PhysXHelpers;

} // namespace Solipsis

#endif // __PhysXPluginPrerequisites_h__
