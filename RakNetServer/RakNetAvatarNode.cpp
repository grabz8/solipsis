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
using namespace CommonTools;

namespace Solipsis {

RakNetAvatarNode::AvatarNodeList RakNetAvatarNode::avatarNodes;

//-------------------------------------------------------------------------------------
RakNetAvatarNode::RakNetAvatarNode() :
    RakNetNode("avatar"),
    mEntity(0),
    mName(""),
    mIsLocal(false)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::RakNetAvatarNode()");

    avatarNodes.push_back(this);
}

//-------------------------------------------------------------------------------------
RakNetAvatarNode::~RakNetAvatarNode()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::~RakNetAvatarNode()");

    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if (*it == this)
        {
            avatarNodes.erase(it);
            break;
        }

    if (mEntity != 0)
    {
        // Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
        if (RakNetConnection::getSingletonPtr()->mServer)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::~RakNetAvatarNode() mEntity->BroadcastDestruction");
            mEntity->BroadcastDestruction();
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::~RakNetAvatarNode() delete mEntity");
            delete mEntity;
        }
    }
}

//-------------------------------------------------------------------------------------
RakNetAvatarNode* RakNetAvatarNode::findByAddress(SystemAddress& systemAddress)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::findByAddress() systemAddress:%s", systemAddress.ToString());

    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if ((*it)->getSystemAddress() == systemAddress)
        {
            RakNetAvatarNode *avatarNode = (RakNetAvatarNode*)*it;
            return avatarNode;
        }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::findByAddress() systemAddress:%s not found !", systemAddress.ToString());
    return 0;
}

//-------------------------------------------------------------------------------------
void RakNetAvatarNode::deleteByAddress(SystemAddress& systemAddress)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::deleteByAddress() systemAddress:%s", systemAddress.ToString());

    std::string avatarNodesListStr;
    for (AvatarNodeList::iterator itz = avatarNodes.begin(); itz != avatarNodes.end(); ++itz)
        avatarNodesListStr += " " + (*itz)->mNodeId;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::deleteByAddress() avatarNodesListStr:%s", avatarNodesListStr.c_str());
    for (AvatarNodeList::iterator it = avatarNodes.begin(); it != avatarNodes.end(); ++it)
        if ((*it)->getSystemAddress() == systemAddress)
        {
            RakNetAvatarNode *avatarNode = (RakNetAvatarNode*)*it;
            // Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
            avatarNode->BroadcastDestruction();
            delete avatarNode;
            return;
        }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::deleteByAddress() systemAddress:%s not found !", systemAddress.ToString());
}

//-------------------------------------------------------------------------------------
bool RakNetAvatarNode::SerializeConstruction(BitStream *bitStream, SerializationContext *serializationContext)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::SerializeConstruction()");

    StringTable::Instance()->EncodeString("AvatarNode", 128, bitStream);

    return true;
}

//-------------------------------------------------------------------------------------
bool RakNetAvatarNode::Serialize(BitStream *bitStream, SerializationContext *serializationContext)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::Serialize()");

    // Server side
    if (RakNetConnection::getSingletonPtr()->mServer)
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
    }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "RakNetAvatarNode::Serialize() mSystemAddress:%s, mNodeId:%s, mEntity:0x%08x(%s)",
        mSystemAddress.ToString(),
        mNodeId.c_str(),
        (unsigned int)mEntity,
        mEntity ? mEntity->GetNetworkID().systemAddress.ToString() : "");

    return true;
}

//-------------------------------------------------------------------------------------
void RakNetAvatarNode::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetAvatarNode::Deserialize()");

    // Client side
    if (!RakNetConnection::getSingletonPtr()->mServer)
    {
	    bitStream->Read(mSystemAddress);

        bitStream->Read(mIsLocal);

        NetworkID entityNetworkId;
        bitStream->Read(entityNetworkId);
        mEntity = (RakNetEntity*)RakNetConnection::getSingletonPtr()->mNetworkIdManager.GET_OBJECT_FROM_ID(entityNetworkId);
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
        }
    }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG,
        "RakNetAvatarNode::Deserialize() mSystemAddress:%s, mNodeId:%s, mName:%s, mIsLocal:%s, mEntity:0x%08x(%s)",
        mSystemAddress.ToString(),
        mNodeId.c_str(),
        mName.c_str(),
        LOGHANDLER_LOGBOOL(mIsLocal),
        (unsigned int)mEntity,
        mEntity ? mEntity->GetNetworkID().systemAddress.ToString() : "");
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
