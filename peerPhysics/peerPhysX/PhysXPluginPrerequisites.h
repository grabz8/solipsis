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
