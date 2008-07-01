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
#include "Node.h"
#include "Avatar.h"
#include "TimeListener.h"

#ifdef PHYSICSPLUGINS
#include "IPhysicsScene.h"
#include "PhysicsEngineManager.h"
#endif

namespace Solipsis {

/** This class manages 1 avatar node.
*/
class AvatarNode : public Node, public TimeListener, public EntityListener
{
public:
#ifdef POOL
    /// <EntityUID, RefCntPoolPtr<XmlEntity>> map
    typedef std::map<EntityUID, RefCntPoolPtr<XmlEntity>> XmlEntityMap;
#else
    /// <EntityUID, XmlEntity*> map
    typedef std::map<EntityUID, XmlEntity*> XmlEntityMap;
#endif

protected:
    /// Avatar entity
    Avatar mAvatar;

    /// Map of owned entities
    Entity::EntityMap mOwnedEntities;
    /// Map of entities avatar is aware of
    Entity::EntityMap mAwareEntities;

    /// Map of Xml entities updated
    XmlEntityMap mXmlEntityMap;

#ifdef PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

public:
    /** Constructor. */
#ifdef POOL
    AvatarNode(const NodeId& nodeId, RefCntPoolPtr<XmlEntity>& xmlEntity);
#else
    AvatarNode(const NodeId& nodeId, XmlEntity* xmlEntity);
#endif
    /** Destructor. */
    virtual ~AvatarNode();

    /** See Solipsis::Node. */
    virtual const Entity& getManagedEntity() { return mAvatar; }

	/** Get associated avatar entity. */
    Avatar& getEntity();
	/** Get owned entities. */
    Entity::EntityMap& getOwnedEntities();
#ifdef PHYSICSPLUGINS
    IPhysicsScene* getPhysicsScene();
#endif

    /** Add an entity to aware of. */
    bool addAwareEntity(Entity* entity, bool sendNewEvt = true);
    /** Remove an entity to aware of. */
    bool removeAwareEntity(Entity* entity);

    /** See Solipsis::Node. */
    virtual bool processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr);
    /** See Solipsis::Node. */
#ifdef POOL
    virtual bool freeEvt(RefCntPoolPtr<XmlEvt>& evt);
#else
    virtual bool freeEvt(XmlEvt* evt);
#endif
    /** See Solipsis::Node. */
    virtual bool freeze(bool frozen);

    /** See Solipsis::Node. */
    virtual TiXmlElement* getSavedElt();

    /** See Solipsis::TimeListener. */
    virtual bool tick(Ogre::Real timeSinceLastTick);

    /** See Solipsis::EntityListener. */
    virtual bool updated(const Node& node, Entity& entity, XmlEvt& xmlEvt);
};

} // namespace Solipsis

#endif // #ifndef __AvatarNode_h__