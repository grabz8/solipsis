#ifndef __Platform_h__
#define __Platform_h__

#ifdef WIN32
#include "PlatformWin.h"
#else
#include "PlatformLinux.h"
#endif

namespace Platform {

// Sleeping function
void sleep(unsigned long durationMs);

} // namespace Platform

#endif // #ifndef __Platform_h__