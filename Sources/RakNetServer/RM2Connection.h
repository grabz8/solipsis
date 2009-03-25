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

#ifndef __RM2Connection_h__
#define __RM2Connection_h__

#include "ReplicaManagerSolipsis.h"

namespace Solipsis {

/** This class manages 1 ReplicaManager2 Solipsis connection.
@remarks One instance of Connection_RM2 is implicitly created per connection that uses ReplicaManager2. The most important function to implement is Construct() as this creates your game objects.
It is designed this way so you can override per-connection behavior in your own game classes
*/
class RM2Connection : public RakNetSolipsis::Connection_RM2
{
    /** See Connection_RM2::Construct.
    @remarks Callback used to create objects
    */
	RakNetSolipsis::Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, RakNetSolipsis::SerializationType type, RakNetSolipsis::ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision);

    /** See Connection_RM2::Construct.
    @remarks Callback when we finish downloading all objects from a new connection
    */
	virtual void DeserializeDownloadComplete(RakNet::BitStream *objectData, SystemAddress sender, RakNetSolipsis::ReplicaManager2 *replicaManager, RakNetTime timestamp, RakNetSolipsis::SerializationType serializationType);
};

/** This class manages 1 ReplicaManager2 Solipsis connection.
@remarks This is a required class factory, that creates and destroys instances of ReplicaManager2SolipsisConnection
*/
class RM2ConnectionFactory : public RakNetSolipsis::Connection_RM2Factory
{
    /** See Connection_RM2Factory::AllocConnection. */
	virtual RakNetSolipsis::Connection_RM2* AllocConnection(void) const { return new RM2Connection; }
    /** See Connection_RM2Factory::DeallocConnection. */
	virtual void DeallocConnection(RakNetSolipsis::Connection_RM2* s) const { delete s; }
};

} // namespace Solipsis

#endif // #ifndef __RM2Connection_h__