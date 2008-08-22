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

#include "RakNetSiteNode.h"
#include "RakNetAvatarNode.h"
#include "RakNetEntity.h"
#include "RakNetServer.h"
#include <StringTable.h>
#include <StringCompressor.h>

using namespace RakNet;

namespace Solipsis {

//-------------------------------------------------------------------------------------
RakNetSiteNode::RakNetSiteNode() :
    RakNetNode("site"),
    mEntity(0)
{
    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::RakNetSiteNode()");
}

//-------------------------------------------------------------------------------------
RakNetSiteNode::~RakNetSiteNode()
{
    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::~RakNetSiteNode()");
}

//-------------------------------------------------------------------------------------
bool RakNetSiteNode::SerializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::SerializeConstruction()");

    StringTable::Instance()->EncodeString("SiteNode", 128, bitStream);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetSiteNode::Serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::Serialize()");

    bitStream->Write(mSystemAddress);
    stringCompressor->EncodeString(mNodeId.c_str(), 16, bitStream);
    if (RakNetConnection::getSingleton()->mServer)
    {
        if (mEntity)
            bitStream->Write(mEntity->GetNetworkID());
        else
            bitStream->Write(UNASSIGNED_NETWORK_ID);
    }

/*    char entityPtrStr[16];
    sprintf(entityPtrStr, "0x%08x", mEntity);
    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::Serialize() mNodeId:" + mNodeId + ", mEntity:" + std::string(entityPtrStr) + "(" + (mEntity ? std::string(mEntity->GetNetworkID().systemAddress.ToString()) : ""));
*/
    return true;
}

//-------------------------------------------------------------------------------------
void RakNetSiteNode::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetSiteNode::Deserialize()");

	bitStream->Read(mSystemAddress);
    char output[16];
    stringCompressor->DecodeString(output, 16, bitStream);
    mNodeId = output;

    if (!RakNetConnection::getSingleton()->mServer)
    {
        NetworkID entityNetworkId;
        bitStream->Read(entityNetworkId);
        mEntity = (RakNetEntity*)RakNetConnection::getSingleton()->mNetworkIdManager.GET_OBJECT_FROM_ID(entityNetworkId);
    }

    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetSiteNode::Deserialize() mSystemAddress:%s, mNodeId:%s, mEntity:0x%08x", mSystemAddress.ToString(), mNodeId.c_str(), (unsigned int)mEntity);
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
