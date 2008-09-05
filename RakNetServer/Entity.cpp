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

#include "Entity.h"
#include "RakNetServer.h"

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Entity::Entity() :
    RakNetEntity()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::Entity()");
}

//-------------------------------------------------------------------------------------
Entity::~Entity()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::~Entity()");
}

//-------------------------------------------------------------------------------------
void Entity::DeserializeDestruction(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::DeserializeDestruction()");
    RakNetServer::getSingleton().onEntityDestroyed(this);
}

//-------------------------------------------------------------------------------------
void Entity::Deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::Deserialize()");

    bool uidNotYetInitialized = !(mXmlEntity->getDefinedAttributes() & XmlEntity::DAUid);
    RakNetEntity::Deserialize(bitStream, serializationType, sender, timestamp);
    if (uidNotYetInitialized && (mXmlEntity->getDefinedAttributes() & XmlEntity::DAUid))
        RakNetServer::getSingleton().onNewEntity(*this);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
