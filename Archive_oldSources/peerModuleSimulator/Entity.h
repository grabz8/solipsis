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

#include "Ogre.h"
#include "XmlDatas.h"
#ifdef PHYSICSPLUGINS
#include "IPhysicsScene.h"
#endif

namespace Solipsis {

class Node;
class Entity;

/** An interface class defining a listener which can be used to receive
    notifications of updated entities.
*/
class EntityListener
{
public:
    /** Called when an entity treated an event.
        @return
            True to go ahead, false otherwise.
    */
#ifdef POOL
    virtual bool onEvt(const Node& node, Entity& entity, RefCntPoolPtr<XmlEvt>& xmlEvt) = 0;
#else
    virtual bool onEvt(const Node& node, Entity& entity, XmlEvt* xmlEvt) = 0;
#endif
};

/** This class manages 1 entity by its descriptor.
*/
class Entity
{
public:
    /// <EntityUID, Entity*> map
    typedef std::map<EntityUID, Entity*> EntityMap;
    /// Xml Entity cache
    XmlEntity mUpdatedXmlEntity;

protected:
    /// Entity descriptor
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mXmlEntity;
#else
    XmlEntity* mXmlEntity;
#endif
    /// Node identifier of the node managing this entity
    NodeId mManagerNodeId;
    /// Whether to apply the gravity
    bool mGravity;
#ifdef PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

    /// Set of registered entity listeners
    std::set<EntityListener*> mEntityListeners;

public:
    /** Constructor. */
#ifdef POOL
    Entity(RefCntPoolPtr<XmlEntity>& xmlEntity, const NodeId& managerNodeId);
#else
    Entity(XmlEntity* xmlEntity, const NodeId& managerNodeId);
#endif
    /** Destructor. */
    virtual ~Entity();

    /** Get the entity descriptor. */
#ifdef POOL
    const RefCntPoolPtr<XmlEntity>& getXmlEntity() const;
#else
    XmlEntity* getXmlEntity();
#endif
    /** Get the node identifier of the node managing this entity. */
    const NodeId& getManagerNodeId();
    /** Apply or not the gravity. */
    void applyGravity(bool enabled);
    /** Set whether the gravity is activated or not. */
    void setGravity(bool enabled);
    /** Determines whether the gravity is applied or not. */
    bool isGravityEnabled();
#ifdef PHYSICSPLUGINS
    /** Get the physics scene. */
    IPhysicsScene* getPhysicsScene();
    /** Create physics. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** Destroy physics. */
    virtual void destroyPhysics();
#endif

    /** Removes a EntityListener from the list of listening classes. */
    void addEntityListener(EntityListener* newListener);
    /** Removes a EntityListener from the list of listening classes. */
    void removeEntityListener(EntityListener* oldListener);
    /** Throw an event to list of listening classes. */
#ifdef POOL
    void throwEvtToEntityListeners(const Node& node, Entity& entity, RefCntPoolPtr<XmlEvt>& xmlEvt);
#else
    void throwEvtToEntityListeners(const Node& node, Entity& entity, XmlEvt* xmlEvt);
#endif

    /** Update. */
    virtual bool update(Ogre::Real timeSinceLastFrame) = 0;
};

} // namespace Solipsis

#endif // #ifndef __Entity_h__