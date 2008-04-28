#ifndef __FModSpeexPluginPrerequisites_h__
#define __FModSpeexPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_FMODSPEEXPLUGIN_EXPORTS
#		define _FModSpeexPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _FModSpeexPluginExport
#       else
#    		define _FModSpeexPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _FModSpeexPluginExport  __attribute__ ((visibility("default")))
#else
#	define _FModSpeexPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class FModSpeexPlugin;
    class FModSpeexEngine;

} // namespace Solipsis

#endif // __FModSpeexPluginPrerequisites_h__
