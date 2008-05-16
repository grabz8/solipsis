#ifndef __VLCPluginPrerequisites_h__
#define __VLCPluginPrerequisites_h__

#include "OgreException.h"

namespace Solipsis {

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRE_VLCPLUGIN_EXPORTS
#		define _VLCPluginExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _VLCPluginExport
#       else
#    		define _VLCPluginExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#   define _VLCPluginExport  __attribute__ ((visibility("default")))
#else
#	define _VLCPluginExport
#endif	// OGRE_WIN32

    // forward class declarations
    // this reduces cross-dependencies a lot
    class VLCPlugin;
    class VLCTextureSource;

} // namespace Solipsis

#endif // __VLCPluginPrerequisites_h__
