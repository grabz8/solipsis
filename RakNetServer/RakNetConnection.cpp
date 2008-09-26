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

#include "RakNetConnection.h"
#include <CacheManager.h>
#include <StringCompressor.h>

using namespace RakNet;

namespace Solipsis {

RakNetConnection *RakNetConnection::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeString(BitStream *bitStream, const std::string &str)
{
    stringCompressor->EncodeString(str.c_str(), 256, bitStream);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeVector3(BitStream *bitStream, const Ogre::Vector3 &v)
{
    bitStream->Write(v.x);
    bitStream->Write(v.y);
    bitStream->Write(v.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeQuaternion(BitStream *bitStream, const Ogre::Quaternion &q)
{
    bitStream->Write(q.w);
    bitStream->Write(q.x);
    bitStream->Write(q.y);
    bitStream->Write(q.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeString(BitStream *bitStream, std::string &str)
{
    char output[256];
    stringCompressor->DecodeString(output, 256, bitStream);
    str = output;
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeVector3(BitStream *bitStream, Ogre::Vector3 &v)
{
    bitStream->Read(v.x);
    bitStream->Read(v.y);
    bitStream->Read(v.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeQuaternion(BitStream *bitStream, Ogre::Quaternion &q)
{
    bitStream->Read(q.w);
    bitStream->Read(q.x);
    bitStream->Read(q.y);
    bitStream->Read(q.z);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
