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

#ifndef VOICECODEC_H
#define VOICECODEC_H

#include "voiceformat.h"
#include <XmlDatasBasicTypes.h>

/**
 *  Voice codec interface
 */
class VoiceCodec
{
public:
    /**
     *  Encodes audio from an input data buffer to a given output buffer
     *  @param input Input data buffer
     *  @parma output A pointer to output data buffer
     *  @param outSize Maximum number of bytes that can be written to output buffer
     */
    virtual unsigned short encode(const char* input, size_t inSize, char* output, size_t outSize) = 0;

    /**
     *  Decodes audio from an input data buffer to a given output buffer
     *  @param voiceId Decoder identifier
     *  @param input Input data buffer
     *  @parma output A pointer to output data buffer
     *  @param outSize Maximum number of bytes that can be written to output buffer
     */
    virtual unsigned short decode(const Solipsis::EntityUID& id, const char* input, size_t inSize, char* output, size_t outSize) = 0;

    /**
     *	Allocates a new decoder that can be used to decode audio
     *  @param id Decoder identifier
     */
    virtual void allocDecoder(const Solipsis::EntityUID& id) = 0;

    /**
     *  Looks up if the codec has a decoder with the given id
     *	@return true if the decoder was found or false otherwise
     */
    virtual bool hasDecoder(const Solipsis::EntityUID& id) = 0;

    /**
     *	Frees a decoder
     *  @param Decoder identifier
     */
    virtual void releaseDecoder(const Solipsis::EntityUID& id) = 0;

    /**
     *  @return The format in which the audio will be after it's encoded
     */
    virtual int getEncodeFormat() const = 0;

    /**
     *  @return The format the audio should be in before encoding
     */
    virtual VoiceFormat getDecodeFormat() const = 0;

    /**
     *  @return Desired sample rate in Hz
     */
    virtual int getSampleRate() const = 0;

    /**
     *  @return True if the size of an encoded frame is same for all
     *          frames or false if the encoded frame size varies
     *          depending on the input
     */
    virtual bool isEncodedSizeConstant() const = 0;

    /**
     *  @return Frame size in PCM samples
     */
    virtual unsigned short getFrameSizePCM() const = 0;


};  //  class VoiceCodec

#endif  //  VOICECODEC_H
