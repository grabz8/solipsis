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

#ifndef __NavigatorModule_h__
#define __NavigatorModule_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _NAVIGATORMODULE_EXPORT __declspec( dllexport )
#define _NAVIGATORMODULE_IMPORT

#if defined(NAVIGATORMODULE_SELF_BUILD)
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_EXPORT
#else
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_IMPORT
#endif

#  	define NAVIGATORMODULE_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _NAVIGATORMODULE_EXPORT __attribute__((dllexport))
#define _NAVIGATORMODULE_IMPORT __attribute__((dllimport))

#if defined(NAVIGATORMODULE_SELF_BUILD)
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_EXPORT
#  	define NAVIGATORMODULE_CW_EXPORT
#else
#  	define NAVIGATORMODULE_EXPORT
#  	define NAVIGATORMODULE_CW_EXPORT _NAVIGATORMODULE_IMPORT
#endif

#endif


#define NAVIGATORMODULEDIRECT_EXPORT _NAVIGATORMODULE_EXPORT

namespace Solipsis {

    #define CONTAINER_NAME_ENV "$CONTAINER_NAME"
    #define NAVI_SUPPORT_ENV "$NAVI_SUPPORT"

    // forward class declarations
    // this reduces cross-dependencies a lot

    class Event;
    class IApplication;
    class IInstance;
    class IWindow;
    class IPhysicsEngine;
    class IPhysicsScene;
    class IPhysicsBody;
    class IPhysicsCharacter;

    class OgreHelpers;
    class PhysicsEngineManager;

} // namespace Solipsis

#endif // #ifndef __NavigatorModule_h__
