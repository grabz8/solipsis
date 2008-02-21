#ifndef __XmlDatasPrerequisites_h__
#define __XmlDatasPrerequisites_h__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// define all the import/export stuff
#ifndef CYGWIN_BUILD

#define _XMLDATAS_EXPORT __declspec( dllexport )
#define _XMLDATAS_IMPORT

#if defined(XMLDATAS_SELF_BUILD)
#  	define XMLDATAS_EXPORT _XMLDATAS_EXPORT
#else
#  	define XMLDATAS_EXPORT _XMLDATAS_IMPORT
#endif

#  	define XMLDATAS_CW_EXPORT

#else

// with cygwin, you cannot import a class or function 'definition'

#define _XMLDATAS_EXPORT __attribute__((dllexport))
#define _XMLDATAS_IMPORT __attribute__((dllimport))

#if defined(XMLDATAS_SELF_BUILD)
#  	define XMLDATAS_EXPORT _XMLDATAS_EXPORT
#  	define XMLDATAS_CW_EXPORT
#else
#  	define XMLDATAS_EXPORT
#  	define XMLDATAS_CW_EXPORT _XMLDATAS_IMPORT
#endif

#endif


#define XMLDATASDIRECT_EXPORT _XMLDATAS_EXPORT

namespace Solipsis {

} // namespace Solipsis

#endif // #ifndef __XmlDatasPrerequisites_h__
