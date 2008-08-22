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

#ifndef __PeerModule_h__
#define __PeerModule_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _PEERMODULE_EXPORT __declspec( dllexport )
#define _PEERMODULE_IMPORT

#if defined(PEERMODULE_SELF_BUILD)
#  	define PEERMODULE_EXPORT _PEERMODULE_EXPORT
#else
#  	define PEERMODULE_EXPORT _PEERMODULE_IMPORT
#endif

#  	define PEERMODULE_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _PEERMODULE_EXPORT __attribute__((dllexport))
#define _PEERMODULE_IMPORT __attribute__((dllimport))

#if defined(PEERMODULE_SELF_BUILD)
#  	define PEERMODULE_EXPORT _PEERMODULE_EXPORT
#  	define PEERMODULE_CW_EXPORT
#else
#  	define PEERMODULE_EXPORT
#  	define PEERMODULE_CW_EXPORT _PEERMODULE_IMPORT
#endif

#endif


#define PEERMODULEDIRECT_EXPORT _PEERMODULE_EXPORT

namespace Solipsis {

    // forward class declarations
    // this reduces cross-dependencies a lot

    class IPeer;
    class IPhysicsEngine;
    class IPhysicsScene;
    class IPhysicsBody;
    class IPhysicsCharacter;

    class OgreHelpers;
    class PhysicsEngineManager;

} // namespace Solipsis

#endif // #ifndef __PeerModule_h__
