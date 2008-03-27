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
