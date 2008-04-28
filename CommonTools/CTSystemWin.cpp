#include "CTSystem.h"

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
void System::sleep(unsigned long durationMs)
{
    Sleep((DWORD)durationMs);
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools