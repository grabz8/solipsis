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

#ifndef __AvatarNode_h__
#define __AvatarNode_h__

#include <map>
#include <pthread.h>
#include "Ogre.h"
#include <RakNetAvatarNode.h>
#include <RakNetConnection.h>
#include <BitStream.h>
#include "Node.h"
#include "Entity.h"
#include "TimeListener.h"

namespace Solipsis {

/** This class manages 1 avatar node.
*/
class AvatarNode : public RakNetAvatarNode, public Node, public TimeListener
{
protected:
    /// Map of owned entities
    Entity::EntityMap mOwnedEntities;

    /// Avatar entity
    /// ... Hm Hm should be RakNetAvatarNode::mEntity but it is initialized too late in deserialize() to unfreeze avatarNode in onNewEntity()
    RakNetEntity *mAvatarEntity;

public:
    /** Constructor. */
    AvatarNode();
    /** Destructor. */
    virtual ~AvatarNode();

    void onNewEntity(Entity* entity, bool sendNewEvt);
    void onUpdatedEntity(Entity* entity);
    void onLostEntity(Entity* entity, bool sendLostEvt);

    void onActionOnEntity(RakNet::BitStream *bitStream);

    bool isOwnedEntity(Entity* entity);

    /** See Replica2::Deserialize. */
	virtual void Deserialize(RakNet::BitStream *bitStream, RakNet::SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/** See Replica2::QueryIsSerializationAuthority. */
	virtual bool QueryIsSerializationAuthority(void) const;

    /** See Solipsis::Node. */
#ifdef POOL
    virtual bool processEvt(RefCntPoolPtr<XmlEvt>& xmlEvt, std::string& xmlRespStr);
#else
    virtual bool processEvt(XmlEvt* xmlEvt, std::string& xmlRespStr);
#endif
    /** See Solipsis::Node. */
#ifdef POOL
    virtual bool freeEvt(RefCntPoolPtr<XmlEvt>& xmlEvt);
#else
    virtual bool freeEvt(XmlEvt* xmlEvt);
#endif
    /** See Solipsis::Node. */
    virtual bool freeze(bool frozen);

    /** See Solipsis::TimeListener. */
    virtual bool tick(Ogre::Real timeSinceLastTick);
};

} // namespace Solipsis

#endif // #ifndef __AvatarNode_h__