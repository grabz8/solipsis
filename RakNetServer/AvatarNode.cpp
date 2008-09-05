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

#include "AvatarNode.h"
#include "Entity.h"
#include "RakNetServer.h"

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
AvatarNode::AvatarNode() :
    RakNetAvatarNode()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::AvatarNode()");
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::~AvatarNode()");

    RakNetServer::getSingleton().onAvatarNodeDestroyed(this);
}

//-------------------------------------------------------------------------------------
void AvatarNode::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::Deserialize()");

    bool nodeIdNotYetInitialized = mNodeId.empty();
    RakNetAvatarNode::Deserialize(bitStream, serializationType, sender, timestamp);
    if (nodeIdNotYetInitialized && !mNodeId.empty())
        RakNetServer::getSingleton().onAvatarNodeIdInitialized(this);

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::Deserialize() mSystemAddress:%s, mNodeId:%s", mSystemAddress.ToString(), mNodeId.c_str());
}

//-------------------------------------------------------------------------------------
TiXmlElement* AvatarNode::getSavedElt()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "AvatarNode::getSavedElt() saving entity of avatar node with nodeId:%s", mNodeId.c_str());

    // Get root node
    TiXmlElement* nodeElt = RakNetNode::getSavedElt();
    // Add avatar entity
    mEntity->getXmlEntity()->toXmlElt(*nodeElt);

    return nodeElt;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
