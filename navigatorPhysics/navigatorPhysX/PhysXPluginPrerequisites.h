#ifndef __PhysXPluginPrerequisites_h__
#define __PhysXPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_NAVIGATORPHYSXPLUGIN_EXPORTS
#		define _NavigatorPhysXPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _NavigatorPhysXPluginExport
#       else
#    		define _NavigatorPhysXPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _NavigatorPhysXPluginExport  __attribute__ ((visibility("default")))
#else
#	define _NavigatorPhysXPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class PhysXPlugin;
    class PhysXEngine;
    class PhysXScene;
    class PhysXBody;
    class PhysXCharacter;
    class PhysXHelpers;

} // end namespace

#endif // __PhysXPluginPrerequisites_h__
