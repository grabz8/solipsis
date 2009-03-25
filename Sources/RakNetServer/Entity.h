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

#ifndef __Entity_h__
#define __Entity_h__

#include <RakNetEntity.h>

namespace Solipsis {

/** This class manages 1 entity.
*/
class Entity : public RakNetEntity
{
public:
    /** Constructor. */
    Entity();
    /** Destructor. */
    virtual ~Entity();

    /** See RakNetEntity. */
    virtual void onNewEntity();
    /** See RakNetEntity. */
    virtual void onLostEntity();

    /** See RakNet::Replica2. */
	virtual void Deserialize(RakNet::BitStream *bitStream, RakNetSolipsis::SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/** See RakNet::Replica2. */
    RakNetSolipsis::BooleanQueryResult isVisibleFrom(RakNetSolipsis::Connection_RM2 *connection);
	/** See RakNet::Replica2. */
    RakNetSolipsis::BooleanQueryResult QueryConstruction(RakNetSolipsis::Connection_RM2 *connection);
	/** See RakNet::Replica2. */
	virtual RakNetSolipsis::BooleanQueryResult QueryVisibility(RakNetSolipsis::Connection_RM2 *connection);

#ifdef LOGRAKNET
private:
    typedef std::map<RakNetSolipsis::Connection_RM2*, int> IsVisibleFromConn;
    IsVisibleFromConn mIsVisibleFromConn;
#endif
};

} // namespace Solipsis

#endif // #ifndef __Entity_h__