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

#ifndef __RMConnection_h__
#define __RMConnection_h__

#include <ReplicationManager.h>

namespace Solipsis {

/** This class manages 1 ReplicationManager Solipsis connection.
@remarks One instance of ConnectionRM is implicitly created per connection that uses ReplicationManager. The most important function to implement is Construct() as this creates your game objects.
It is designed this way so you can override per-connection behavior in your own game classes
*/
class RMConnection : public ConnectionRM
{
    /** See ConnectionRM::Construct.
    @remarks Callback used to create objects
    */
	Replica* construct(RakNet::BitStream* replicaData, SystemAddress sender, SerializationType type, ReplicationManager* replicationManager, const ReplicaUid& replicaUid);
};

/** This class manages 1 ReplicationManager Solipsis connection.
@remarks This is a required class factory, that creates and destroys instances of ReplicationManager Solipsis connection
*/
class RMConnectionFactory : public ConnectionRMFactory
{
    /** See ConnectionRMFactory::newConnection. */
	virtual ConnectionRM* newConnection() const { return new RMConnection; }
    /** See ConnectionRMFactory::freeConnection. */
	virtual void freeConnection(ConnectionRM* connection) const { delete connection; }
};

} // namespace Solipsis

#endif // #ifndef __RMConnection_h__