#include "voiceuuid.h"
#include <string.h>


VoiceUUID::VoiceUUID()
{
}

VoiceUUID::VoiceUUID(const unsigned char id[16])
{
    memcpy(mID, id, 16);
}

VoiceUUID::VoiceUUID(const VoiceUUID& other)
{
    memcpy(mID, other.mID, 16);
}

void VoiceUUID::operator = (const VoiceUUID& other)
{
    memcpy(mID, other.mID, 16);
}

bool VoiceUUID::operator == (const VoiceUUID& other) const
{
    return (memcmp(mID, other.mID, 16) == 0);
}

bool VoiceUUID::operator < (const VoiceUUID& other) const
{
    return (memcmp(mID, other.mID, 16) < 0);
}

// GREG BEGIN
#include <time.h>
void VoiceUUID::generateID()
{
    memset(mID, 0, sizeof(mID));
    time((time_t*)((char*)(&mID) + sizeof(mID) - sizeof(time_t)));
}
// GREG END
