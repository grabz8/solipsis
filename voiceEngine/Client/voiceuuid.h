#ifndef VOICEUUID_H
#define VOICEUUID_H

// GREG BEGIN
#include <stdio.h>
#include <string>
// GREG END

class VoiceUUID
{
public:
    VoiceUUID();
    VoiceUUID(const unsigned char id[16]);
	VoiceUUID(const VoiceUUID& other);

    void operator = (const VoiceUUID& other);

    bool operator == (const VoiceUUID& other) const;
    bool operator < (const VoiceUUID& other) const;

    unsigned char mID[16];

// GREG BEGIN
    void generateID();
    void setID(const char* buf, int size) { memset(mID, 0, sizeof(mID)); memcpy(mID + sizeof(mID) - size, buf, size); }
    std::string getID() { char mIDString[32+1]; for(int i=0;i<16;i++) sprintf(mIDString+i*2, "%02x", mID[i]); return mIDString; }
// GREG END

};	//	class VoiceUUID

#endif	//	VOICEUUID_H
