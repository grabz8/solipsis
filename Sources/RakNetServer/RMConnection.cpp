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

#include "RMConnection.h"
#include "Entity.h"
#include <BitStream.h>
#include <StringTable.h>
#include <CTLog.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Replica* RMConnection::construct(BitStream* replicaData, SystemAddress sender, SerializationType type, ReplicationManager* replicationManager, const ReplicaUid& replicaUid)
{
    char objectName[128];

    // In our two Serialize() functions above, we used StringTable to encode the name of the class. Here we read it out, and create the desired object.
    StringTable::Instance()->DecodeString(objectName, 128, replicaData);

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RMConnection::Construct() Contruction of 1 new %s from %s", objectName, sender.ToString());
    if (strcmp(objectName, "Entity") == 0)
    {
        Entity* entity = new Entity();
        entity->setSystemAddress(sender);
        entity->setUid(replicaUid);
        return entity;
    }

    return 0;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
