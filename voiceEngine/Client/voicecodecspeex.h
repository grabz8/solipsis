/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef VOICECODECSPEEX_H
#define VOICECODECSPEEX_H

#include "voicecodec.h"
#include "voiceuuid.h"
#include <speex/speex.h>
#include <map>

/**
 *  Speex audio codec for voice data
 */
class VoiceCodecSpeex : public VoiceCodec
{
public:

    VoiceCodecSpeex(int sampleRate);
    ~VoiceCodecSpeex();

    unsigned short encode(const char* input, size_t inSize, char* output, size_t outSize);
    unsigned short decode(const VoiceUUID& id, const char* input, size_t inSize, char* output, size_t outSize);

    VoiceFormat getDecodeFormat() const;
    int getEncodeFormat() const;

    bool isEncodedSizeConstant() const;
    unsigned short getFrameSizePCM() const;

    int getSampleRate() const;

    void allocDecoder(const VoiceUUID& id);
    bool hasDecoder(const VoiceUUID& id);
    void releaseDecoder(const VoiceUUID& id);

private:
    SpeexBits mEncBits, mDecBits;
    void* mEncState;
    int mSampleRate;

    typedef std::map<VoiceUUID, void*> DecoderMap;
    DecoderMap mDecoders;

};  //  class VoiceCodecSpeex


#endif  //  VOICECODECSPEEX_H
