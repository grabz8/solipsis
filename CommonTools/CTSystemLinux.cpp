#include "CTSystem.h"

#include <unistd.h>

namespace CommonTools {

//-------------------------------------------------------------------------------------
void sleep(unsigned long durationMs)
{
    struct timespec req={0},rem={0};

    time_t sec = (int)(durationMs/1000);
    durationMs = durationMs-(sec*1000);
    req.tv_sec = sec;
    req.tv_nsec = durationMs*1000000L;
    nanosleep(&req,&rem);
}

//-------------------------------------------------------------------------------------

}