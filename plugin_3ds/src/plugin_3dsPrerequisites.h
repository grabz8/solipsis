#ifndef __Plugin_3dsPrerequisites_h__
#define __Plugin_3dsPrerequisites_h__

#include <OgreException.h>


#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef PLUGIN_3DS_EXPORTS
#		define _Plugin_3dsExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _Plugin_3dsExport
#       else
#    		define _Plugin_3dsExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _Plugin_3dsExport  __attribute__ ((visibility("default")))
#else
#	define _Plugin_3dsExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class Plugin_3ds;
    

#endif // __Plugin_3dsPrerequisites_h__
