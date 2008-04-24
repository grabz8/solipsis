#ifndef __VNCPluginPrerequisites_h__
#define __VNCPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_VNCPLUGIN_EXPORTS
#		define _VNCPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _VNCPluginExport
#       else
#    		define _VNCPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _VNCPluginExport  __attribute__ ((visibility("default")))
#else
#	define _VNCPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class VNCPlugin;
    class TightVNCTextureSystem;
    class TightVNCConnection;

} // namespace Solipsis

#endif // __VNCPluginPrerequisites_h__
