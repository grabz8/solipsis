#ifndef __TokamakPluginPrerequisites_h__
#define __TokamakPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_NAVIGATORTOKAMAKPLUGIN_EXPORTS
#		define _NavigatorTokamakPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _NavigatorTokamakPluginExport
#       else
#    		define _NavigatorTokamakPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _NavigatorTokamakPluginExport  __attribute__ ((visibility("default")))
#else
#	define _NavigatorTokamakPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class TokamakPlugin;
    class TokamakEngine;
    class TokamakScene;
    class TokamakBody;
    class TokamakCharacter;

} // end namespace

#endif // __TokamakPluginPrerequisites_h__
