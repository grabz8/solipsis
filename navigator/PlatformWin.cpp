#include "Platform.h"

namespace Platform {

void sleep(unsigned long durationMs)
{
    Sleep((DWORD)durationMs);
}

//-------------------------------------------------------------------------------------

}