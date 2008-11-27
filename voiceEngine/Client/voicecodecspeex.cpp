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

#include "voicecodecspeex.h"

VoiceCodecSpeex::VoiceCodecSpeex(int sampleRate)
    : mEncState(0)
    , mSampleRate(sampleRate)
{
    // Initialize speex
    speex_bits_init(&mEncBits);
    speex_bits_init(&mDecBits);

    mEncState = speex_encoder_init(&speex_nb_mode);
    speex_encoder_ctl(mEncState, SPEEX_SET_SAMPLING_RATE, &mSampleRate);

//     int quality = 3;
//     speex_encoder_ctl(mEncState, SPEEX_SET_QUALITY, &quality);
}

VoiceCodecSpeex::~VoiceCodecSpeex()
{
    speex_bits_destroy(&mEncBits);
    speex_bits_destroy(&mDecBits);

    speex_encoder_destroy(mEncState);
    mEncState = 0;

    for (DecoderMap::iterator i = mDecoders.begin(); i != mDecoders.end(); ++i)
    {
        speex_decoder_destroy(i->second);
    }
}

unsigned short VoiceCodecSpeex::encode(const char* input, size_t inSize, char* output, size_t outSize)
{
    speex_bits_reset(&mEncBits);
    if (speex_encode_int(mEncState, (short*)input, &mEncBits) == 0)
        return 0;

    return (unsigned short)speex_bits_write(&mEncBits, output, (int)outSize);
}

unsigned short VoiceCodecSpeex::decode(const Solipsis::EntityUID& id, const char* input, size_t inSize, char* output, size_t outSize)
{
    DecoderMap::const_iterator i = mDecoders.find(id);
    if (i == mDecoders.end())
        return 0;

    speex_bits_read_from(&mDecBits, (char*)input, (int)inSize); //  XXX Why isn't read buffer const?
    speex_decode_int(i->second, &mDecBits, (short*)output);

    return (unsigned short)outSize;
}

VoiceFormat VoiceCodecSpeex::getDecodeFormat() const
{
    return VF_PCM16;
}

int VoiceCodecSpeex::getEncodeFormat() const
{
    return VF_SPEEX;
}

bool VoiceCodecSpeex::isEncodedSizeConstant() const
{
    return true;
}

unsigned short VoiceCodecSpeex::getFrameSizePCM() const
{
    int frameSize;
    speex_encoder_ctl(mEncState, SPEEX_GET_FRAME_SIZE, &frameSize);
    return (unsigned short)frameSize;
}

int VoiceCodecSpeex::getSampleRate() const
{
    return mSampleRate;
}

void VoiceCodecSpeex::allocDecoder(const Solipsis::EntityUID& id)
{
    void* decState = speex_decoder_init(&speex_nb_mode);
    speex_decoder_ctl(decState, SPEEX_SET_SAMPLING_RATE, &mSampleRate);
    mDecoders.insert(std::make_pair(id, decState));
}

bool VoiceCodecSpeex::hasDecoder(const Solipsis::EntityUID& id)
{
    return (mDecoders.find(id) != mDecoders.end());
}

void VoiceCodecSpeex::releaseDecoder(const Solipsis::EntityUID& id)
{
    DecoderMap::iterator i = mDecoders.find(id);
    if (i != mDecoders.end())
    {
        speex_decoder_destroy(i->second);
        mDecoders.erase(i);
    }
}
