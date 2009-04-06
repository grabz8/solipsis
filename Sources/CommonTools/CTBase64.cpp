/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "CTBase64.h"

using namespace CommonTools;

//! initialization of static variables
unsigned char *CTBase64::m_osBase64 = (unsigned char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
unsigned char  CTBase64::m_u1Base64Index[128] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,   62, 0xff, 0xff, 0xff,   63,
    52,   53,   54,   55,   56,   57,   58,   59,   60,   61, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51, 0xff, 0xff, 0xff, 0xff, 0xff
};

/**
* @brief The base64Encode operation.
* 
* The base64Encode operation encodes a buffer to base64. The caller is
* responsible from destroying the returned buffer.
* 
* @param InBuffer the buffer to encode
* @param InBufferLength the length of the buffer to encode
* @param OutBuffer the encoded buffer
* @param OutBufferLength the length of the encoded buffer
*/
void CTBase64::base64Encode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength)
{
    // VARs
    unsigned int i   = 0;
    unsigned int j   = 0;
    unsigned int pad = 0;

    // assign output size and create encoded buffer
    u4OutBufferLength = (u4InBufferLength*4)/3;
    switch(u4InBufferLength % 3) // Fill the number of bytes needed to have a 4-multiple base64 character value
    {
    case 0 : u4OutBufferLength += 0; break;
    case 1 : u4OutBufferLength += 3; break;
    case 2 : u4OutBufferLength += 2; break;
    }
    // add one char in case of an \0 is needed
    u1OutBuffer = new unsigned char[u4OutBufferLength+1];

    // initialize local variables
    i = 0;
    j = 0;

    // loop encoding
    while (i < u4InBufferLength)
    {
        pad = 3 - (u4InBufferLength - i);
        if (pad == 2) 
        {
            u1OutBuffer[j  ] = m_osBase64[u1InBuffer[i]>>2];
            u1OutBuffer[j+1] = m_osBase64[(u1InBuffer[i] & 0x03) << 4];
            u1OutBuffer[j+2] = '=';
            u1OutBuffer[j+3] = '=';
        }
        else if (pad == 1)
        {
            u1OutBuffer[j  ] = m_osBase64[u1InBuffer[i]>>2];
            u1OutBuffer[j+1] = m_osBase64[((u1InBuffer[i] & 0x03) << 4) | ((u1InBuffer[i+1] & 0xf0) >> 4)];
            u1OutBuffer[j+2] = m_osBase64[(u1InBuffer[i+1] & 0x0f) << 2];
            u1OutBuffer[j+3] = '=';
        }
        else
        {
            u1OutBuffer[j  ] = m_osBase64[u1InBuffer[i]>>2];
            u1OutBuffer[j+1] = m_osBase64[((u1InBuffer[i] & 0x03) << 4) | ((u1InBuffer[i+1] & 0xf0) >> 4)];
            u1OutBuffer[j+2] = m_osBase64[((u1InBuffer[i+1] & 0x0f) << 2) | ((u1InBuffer[i+2] & 0xc0) >> 6)];
            u1OutBuffer[j+3] = m_osBase64[u1InBuffer[i+2] & 0x3f];
        }
        i += 3;
        j += 4;
    }
}

/**
* @brief The base64Decode operation.
* 
* The base64Decode operation decodes a buffer from base64.
* 
* @param InBuffer the buffer to encode
* @param InBufferLength the length of the buffer to encode
* @param OutBuffer the encoded buffer
* @param OutBufferLength the length of the encoded buffer
*/
void CTBase64::base64Decode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength)
{
    // VARs
    unsigned int i   = 0;
    unsigned int j   = 0;
    unsigned int pad = 0;
    unsigned char  c[4];

    // allocate output buffer
    u4OutBufferLength = (u4InBufferLength*3)/4;
    // add one char in case of an \0 is needed
    u1OutBuffer = new unsigned char[u4OutBufferLength+1];

    while ((i + 3) < u4InBufferLength)
    {
        pad  = 0;
        c[0] = base64DecodeUChar(u1InBuffer[i  ]);pad += (c[0] == 0xff);
        c[1] = base64DecodeUChar(u1InBuffer[i+1]); pad += (c[1] == 0xff);
        c[2] = base64DecodeUChar(u1InBuffer[i+2]); pad += (c[2] == 0xff);
        c[3] = base64DecodeUChar(u1InBuffer[i+3]); pad += (c[3] == 0xff);
        if (pad == 2)
        {
            u1OutBuffer[j++] = (unsigned char) ((c[0] << 2) | ((c[1] & 0x30) >> 4));
            u1OutBuffer[j]   = (unsigned char) ((c[1] & 0x0f) << 4);
        }
        else if (pad == 1)
        {
            u1OutBuffer[j++] = (unsigned char) ((c[0] << 2) | ((c[1] & 0x30) >> 4));
            u1OutBuffer[j++] = (unsigned char) (((c[1] & 0x0f) << 4) | ((c[2] & 0x3c) >> 2));
            u1OutBuffer[j]   = (unsigned char) ((c[2] & 0x03) << 6);
        }
        else
        {
            u1OutBuffer[j++] = (unsigned char) ((c[0] << 2) | ((c[1] & 0x30) >> 4));
            u1OutBuffer[j++] = (unsigned char) (((c[1] & 0x0f) << 4) | ((c[2] & 0x3c) >> 2));
            u1OutBuffer[j++] = (unsigned char) (((c[2] & 0x03) << 6) | (c[3] & 0x3f));
        }
        i += 4;
    }
    u4OutBufferLength = j;
    u1OutBuffer[j] = '\0';
}

/**
* @brief The asciiHexaDecode operation
* 
* The asciiHexaDecode operation decodes a buffer of ascii characters that represent
* a biary hexa value ex : "FF" becomes 0xFF
* 
* @param InBuffer the buffer to encode
* @param InBufferLength the length of the buffer to encode
* @param OutBuffer the encoded buffer
* @param OutBufferLength the length of the encoded buffer
*/
void CTBase64::asciiHexaDecode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength)
{
    // VARs
    unsigned int i   = 0;
    unsigned int j   = 0;
    unsigned int pad = 0;

    // allocate output buffer
    u4OutBufferLength = (u4InBufferLength/2) + (u4InBufferLength % 2);
    u1OutBuffer = new unsigned char[u4OutBufferLength];

    while (i < u4InBufferLength)
    {
        j = i/2;
        if (pad == 0)
        {
            u1OutBuffer[i/2] = (unsigned char) ((asciiHexaDecodeUChar(u1InBuffer[i]) << 4) & 0xF0);
            pad = 1;
        }
        else if (pad == 1)
        {
            u1OutBuffer[i/2] = (unsigned char) (u1OutBuffer[i/2] | asciiHexaDecodeUChar(u1InBuffer[i]));
            pad = 0;
        }
        // iterate
        i = i + 1;
    }
}

/**
* @brief The asciiHexaDecodeUChar operation.
* 
* The asciiHexaDecodeUChar operation decodes a ascii encoded unsigned char. i.e. '0'
* becomes 0x30.
*
* !! WARNING PORTABILITY DEPENDS ON CHARACTER TABLES !!
* 
* @param Inunsigned char the uint to decode
* @return the decoded unsigned char
*/
unsigned char CTBase64::asciiHexaDecodeUChar(unsigned char uChar)
{
    // get the ascii value range : 0..9, a..f or A..F
    if ((0x30 <= uChar) && (uChar <= 0x39))
    {
        return (unsigned char) (uChar - 0x30);
    }
    else if ((0x41 <= uChar) && (uChar <= 0x46))
    {
        return (unsigned char) (uChar - 0x41 + 0x0A);
    }
    else if ((0x61 <= uChar) && (uChar <= 0x66))
    {
        return (unsigned char) (uChar - 0x61 + 0x0A);
    }
    return 0;
}


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
void CTBase64::base16Encode(unsigned char* u1InBuffer, unsigned int u4InBufferLength, unsigned char*& u1OutBuffer, unsigned int& u4OutBufferLength)
{
    // VARs
    unsigned int i   = 0;
    unsigned int j   = 0;

    // assign output size and create encoded buffer
    u4OutBufferLength = u4InBufferLength*2;
    u1OutBuffer = new unsigned char[u4OutBufferLength+1];

    // initialize local variables
    i = 0;
    j = 0;

    // loop encoding
    while (i < u4InBufferLength)
    {
        u1OutBuffer[j] = m_osBase64[u1InBuffer[i]>>2];

        sprintf( (char * ) &u1OutBuffer[j], "%02x", u1InBuffer[i] );

        i ++;
        j += 2;
    }

    u4OutBufferLength = j;
    u1OutBuffer[j] = '\0';
}

/**
* @brief The base64Decodeunsigned char operation.
* 
* The base64Decodeunsigned char operation decodes a unsigned char from base64.
* 
* @param u1unsigned char the uint to decode
* @return the decoded unsigned char
*/
unsigned char CTBase64::base64DecodeUChar(unsigned char uChar)
{
    // return the computed value
    return (unsigned char) (((uChar > 127) ? 0xff : m_u1Base64Index[(uChar)]));
}


void crypt_XOR(unsigned char * u1Buffer, unsigned int u4BufferSize) 
{
    unsigned char* dataToCrypt = u1Buffer;
    unsigned int dataLength = u4BufferSize;

    // We create the key used for the Vernam algorithm from the length of the data
    unsigned char pXORKey[4];
    unsigned char Byte0 = (unsigned char)(dataLength & 0x000000FF);
    unsigned char Byte1 = (unsigned char)((dataLength >> 8) & 0x000000FF);
    unsigned char Byte2 = (unsigned char)((dataLength >> 16) & 0x000000FF);
    pXORKey[0] = (unsigned char) (Byte0 ^ Byte1 ^ Byte2);
    pXORKey[1] = (unsigned char) (Byte0 ^ Byte2);
    pXORKey[2] = (unsigned char) (Byte2 ^ Byte1);
    pXORKey[3] = (unsigned char) (Byte1 ^ Byte0);

    // Encryption
    for (unsigned int i = 0 ; i < dataLength ; i++)	
    {
        dataToCrypt[i] ^= pXORKey[i%4];
    }
}

// do personnal encryption using xor and base64
std::string CTBase64::encrypt(const std::string & myString)
{
    unsigned int size = (unsigned int) myString.size();
    unsigned char * u1Buffer  = new unsigned char[size];
    memcpy(u1Buffer, myString.c_str(), size);

    crypt_XOR(u1Buffer, size);

    unsigned char * resultChar;
    unsigned int resultSize;
    base64Encode(u1Buffer, size, resultChar, resultSize);
    delete [] u1Buffer;
    resultChar[resultSize] = '\0';
    

    std::string result;
    result.assign ((const char *) resultChar);
    delete [] resultChar;
    return result;  
}

std::string CTBase64::decrypt(const std::string & myString)
{
    unsigned int size = (unsigned int)  myString.size();
    unsigned char * u1Buffer  = new unsigned char[size];
    memcpy(u1Buffer, myString.c_str(), size);

    unsigned char * resultChar;
    unsigned int resultSize;
    base64Decode(u1Buffer, size, resultChar, resultSize);
    delete [] u1Buffer;

    crypt_XOR(resultChar, resultSize);
    resultChar[resultSize] = '\0';

    std::string result;
    result.assign ((const char *) resultChar);
    delete []resultChar;
    return result;  
}

