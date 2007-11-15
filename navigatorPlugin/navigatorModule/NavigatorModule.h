#ifndef __NAVMODNAVIGATORMODULE_H__
#define __NAVMODNAVIGATORMODULE_H__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _NAVIGATORMODULE_EXPORT __declspec( dllexport )
#define _NAVIGATORMODULE_IMPORT

#if defined(NAVIGATORMODULE_SELF_BUILD)
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_EXPORT
#else
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_IMPORT
#endif

#  	define NAVIGATORMODULE_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _NAVIGATORMODULE_EXPORT __attribute__((dllexport))
#define _NAVIGATORMODULE_IMPORT __attribute__((dllimport))

#if defined(NAVIGATORMODULE_SELF_BUILD)
#  	define NAVIGATORMODULE_EXPORT _NAVIGATORMODULE_EXPORT
#  	define NAVIGATORMODULE_CW_EXPORT
#else
#  	define NAVIGATORMODULE_EXPORT
#  	define NAVIGATORMODULE_CW_EXPORT _NAVIGATORMODULE_IMPORT
#endif

#endif


#define NAVIGATORMODULEDIRECT_EXPORT _NAVIGATORMODULE_EXPORT

namespace NavigatorModule {

    // forward class declarations
    // this reduces cross-dependencies a lot
    class INavigatorApp;
    class IInstance;
    class IWindow;

    class NavigatorApp;
    class Event;
    class Instance;

} // end namespace

#endif // __NAVMODNAVIGATORMODULE_H__
