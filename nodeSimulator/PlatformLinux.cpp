#include "PlatformLinux.h"

namespace Platform {

//-------------------------------------------------------------------------------------
void sleep(float durationSec)
{
    sleep((unsigned int)durationSec);
}

//-------------------------------------------------------------------------------------

} // namespace Platform
