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

#include "RakNetAvatarNode.h"
#include "RakNetEntity.h"
#include "RakNetServer.h"
#include <StringTable.h>
#include <StringCompressor.h>

using namespace RakNet;

namespace Solipsis {

RakNetAvatarNode::AvatarNodeList RakNetAvatarNode::avatarNodes;

//-------------------------------------------------------------------------------------
RakNetAvatarNode::RakNetAvatarNode() :
    RakNetNode("avatar"),
    mEntity(0),
    mName(""),
    mIsLocal(false),
    mFileListTransferSetID(65535)
{
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::RakNetAvatarNode()");

    avatarNodes.push_back(this);
}

//-------------------------------------------------------------------------------------
RakNetAvatarNode::~RakNetAvatarNode()
{
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::~RakNetAvatarNode()");

    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if (*it == this)
        {
            avatarNodes.erase(it);
            break;
        }

    if (mEntity != 0)
    {
        if (RakNetConnection::getSingleton()->mServer)
            mEntity->BroadcastDestruction();
        delete mEntity;
        mEntity = 0;
    }
}

//-------------------------------------------------------------------------------------
RakNetAvatarNode* RakNetAvatarNode::findByAddress(SystemAddress& systemAddress)
{
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::findByAddress() systemAddress:" + std::string(systemAddress.ToString()));

    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if ((*it)->getSystemAddress() == systemAddress)
        {
            RakNetAvatarNode *avatarNode = (RakNetAvatarNode*)*it;
            return avatarNode;
        }

    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::findByAddress() systemAddress:" + std::string(systemAddress.ToString()) + " not found !");
    return 0;
}

//-------------------------------------------------------------------------------------
void RakNetAvatarNode::deleteByAddress(SystemAddress& systemAddress)
{
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::deleteByAddress() systemAddress:" + std::string(systemAddress.ToString()));

    std::string avatarNodesListStr;
    for (AvatarNodeList::iterator itz = avatarNodes.begin(); itz != avatarNodes.end(); ++itz)
        avatarNodesListStr += " " + (*itz)->mNodeId;
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::deleteByAddress() avatarNodesListStr:" + avatarNodesListStr);
    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if ((*it)->getSystemAddress() == systemAddress)
        {
            RakNetAvatarNode *avatarNode = (RakNetAvatarNode*)*it;
            // Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
            // It is in the derived class QueryIsDestructionAuthority() that we give the client authority to network delete the object
            avatarNode->BroadcastDestruction();

            delete avatarNode;
            return;
        }

    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::deleteByAddress() systemAddress:" + std::string(systemAddress.ToString()) + " not found !");
}

//-------------------------------------------------------------------------------------
bool RakNetAvatarNode::SerializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::SerializeConstruction()");

    StringTable::Instance()->EncodeString("AvatarNode", 128, bitStream);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetAvatarNode::Serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::Serialize()");

    // Server side
    if (RakNetConnection::getSingleton()->mServer)
    {
        bitStream->Write(mSystemAddress);

        mIsLocal = (serializationContext->recipientAddress == mSystemAddress);
        bitStream->Write(mIsLocal);

        if (mEntity != 0)
            bitStream->Write(mEntity->GetNetworkID());
        else
            bitStream->Write(UNASSIGNED_NETWORK_ID);
    }
    // Client side
    else if (mIsLocal)
    {
        stringCompressor->EncodeString(mNodeId.c_str(), 16, bitStream);
        stringCompressor->EncodeString(mName.c_str(), 16, bitStream);
        bitStream->Write(mFileListTransferSetID);
    }

    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetAvatarNode::Serialize() mSystemAddress:%s, mNodeId:%s, mEntity:0x%08x(%s)", mSystemAddress.ToString(), mNodeId.c_str(), (unsigned int)mEntity, mEntity ? mEntity->GetNetworkID().systemAddress.ToString() : "");
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));

    return true;
}

//-------------------------------------------------------------------------------------
void RakNetAvatarNode::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    RakNetConnection::getSingleton()->logMessage("RakNetAvatarNode::Deserialize()");

    // Client side
    if (!RakNetConnection::getSingleton()->mServer)
    {
	    bitStream->Read(mSystemAddress);

        bitStream->Read(mIsLocal);

        NetworkID entityNetworkId;
        bitStream->Read(entityNetworkId);
        mEntity = (RakNetEntity*)RakNetConnection::getSingleton()->mNetworkIdManager.GET_OBJECT_FROM_ID(entityNetworkId);
    }
    // Server side
    else
    {
        bool isLocalOnSender = (sender == mSystemAddress);
        if (isLocalOnSender)
        {
            char output[16];
            stringCompressor->DecodeString(output, 16, bitStream);
            mNodeId = output;
            stringCompressor->DecodeString(output, 16, bitStream);
            mName = output;
            bitStream->Read(mFileListTransferSetID);
        }
    }

    char logStr[256];
    _snprintf(logStr, sizeof(logStr)-1, "RakNetAvatarNode::Deserialize() mSystemAddress:%s, mNodeId:%s, mName:%s, mIsLocal:%s, mEntity:0x%08x(%s)", mSystemAddress.ToString(), mNodeId.c_str(), mName.c_str(), mIsLocal ? "true" : "false", (unsigned int)mEntity, mEntity ? mEntity->GetNetworkID().systemAddress.ToString() : "");
    RakNetConnection::getSingleton()->logMessage(std::string(logStr));
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
