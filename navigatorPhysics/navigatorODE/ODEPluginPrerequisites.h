#ifndef __ODEPluginPrerequisites_h__
#define __ODEPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_NAVIGATORODEPLUGIN_EXPORTS
#		define _NavigatorODEPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _NavigatorODEPluginExport
#       else
#    		define _NavigatorODEPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _NavigatorODEPluginExport  __attribute__ ((visibility("default")))
#else
#	define _NavigatorODEPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class ODEPlugin;
    class ODEEngine;
    class ODEScene;
    class ODEBody;
    class ODECharacter;

} // end namespace

#endif // __ODEPluginPrerequisites_h__
