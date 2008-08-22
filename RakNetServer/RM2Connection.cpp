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

#include "RM2Connection.h"
#include "AvatarNode.h"
#include "SiteNode.h"
#include "Entity.h"
#include <BitStream.h>
#include <StringTable.h>

using namespace RakNet;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Replica2* RM2Connection::Construct(BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
{
    char objectName[128];

    // In our two Serialize() functions above, we used StringTable to encode the name of the class. Here we read it out, and create the desired object.
    StringTable::Instance()->DecodeString(objectName, 128, replicaData);

    if (networkIDCollision) return 0;

    if (strcmp(objectName, "AvatarNode") == 0)
        return new AvatarNode;
    if (strcmp(objectName, "SiteNode") == 0)
        return new SiteNode;
    if (strcmp(objectName, "Entity") == 0)
        return new Entity;

    return 0;
}

//-------------------------------------------------------------------------------------
void RM2Connection::DeserializeDownloadComplete(BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType)
{
    // Since the system is inherently peer to peer, download notifications go both ways, not just server to client.
    // Just ignore download notifications from clients in a client/server app
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
