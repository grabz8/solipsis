#ifndef __Platform_h__
#define __Platform_h__

namespace Platform {

#ifdef WIN32
#include "PlatformWin.h"
#else
#include "PlatformLinux.h"
#endif

// Sleeping function
void sleep(unsigned long durationMs);

}

#endif // #ifndef __Platform_h__