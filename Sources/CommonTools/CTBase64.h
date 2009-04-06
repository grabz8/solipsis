/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)

Author Christophe Floutier

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

#ifndef __CTBase64_h__
#define __CTBase64_h__



#include <string>

namespace CommonTools {
/**
* @brief The CTBase64 class.
* 
* The CTBase64 class is a class for basis decoding operations like
* base to base decoding.
*/
class CTBase64
{
public : 
    /**
    * @brief The base64Encode operation.
    * 
    * The base64Encode operation encodes a buffer to base64. The caller is
    * responsible from destroying the returned buffer.
    * 
    * @param u1InBuffer The buffer to encode.
    * @param u4InBufferLength The length of the buffer to encode.
    * @param u1OutBuffer The encoded buffer.
    * @param u4OutBufferLength The length of the encoded buffer.
    */
    static void base64Encode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength);

    /**
    * @brief The base64Decode operation.
    * 
    * The base64Decode operation decodes a buffer from base64.
    * 
    * @param u1InBuffer The buffer to decode.
    * @param u4InBufferLength The length of the buffer to decode.
    * @param u1OutBuffer The decoded buffer.
    * @param u4OutBufferLength The length of the decoded buffer.
    */
    static void base64Decode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength);

    /**
    * @brief The base16Encode operation.
    * 
    * The base16Encode operation encodes a buffer to base16. The caller is
    * responsible from destroying the returned buffer.
    * 
    * @param u1InBuffer The buffer to encode.
    * @param u4InBufferLength The length of the buffer to encode.
    * @param u1OutBuffer The encoded buffer.
    * @param u4OutBufferLength The length of the encoded buffer.
    */
    static void base16Encode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength);


    /**
    * @brief The base64Decodeunsigned char operation.
    * 
    * The base64Decodeunsigned char operation decodes a unsigned char from base64.
    * 
    * @param u1unsigned char The uint to decode.
    * @return The decoded unsigned char.
    */
    static inline unsigned char base64DecodeUChar(unsigned char uChar);

    /**
    * @brief The asciiHexaDecode operation
    * 
    * The asciiHexaDecode operation decodes a buffer of ascii characters that represent
    * a biary hexa value ex : "FF" becomes 0xFF
    * 
    * @param u1InBuffer The buffer to encode.
    * @param u4InBufferLength The length of the buffer to encode.
    * @param u1OutBuffer The encoded buffer.
    * @param u4OutBufferLength The length of the encoded buffer.
    */
    static void asciiHexaDecode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength);

    /**
    * @brief The asciiHexaDecodeunsigned char operation.
    * 
    * The asciiHexaDecodeunsigned char operation decodes a ascii encoded unsigned char. i.e. '0'
    * becomes 0x30.
    * 
    * !! WARNING PORTABILITY DEPENDS ON CHARACTER TABLES !!
    *
    * @param u1unsigned char The uint to decode.
    * @return The decoded unsigned char.
    */
    static unsigned char asciiHexaDecodeUChar(unsigned char uChar);


    // do personnal encryption using xor and base64
    static std::string encrypt(const std::string & myString);
    static std::string decrypt(const std::string & myString);


private:
    //! The base64 member.
    static unsigned char *m_osBase64;

    //! The array of base64 members.
    static unsigned char  m_u1Base64Index[128];
};

};

#endif
