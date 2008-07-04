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

namespace Solipsis {

//-------------------------------------------------------------------------------------
#ifdef POOL
Entity::Entity(RefCntPoolPtr<XmlEntity>& xmlEntity, const NodeId& managerNodeId) :
#else
Entity::Entity(XmlEntity* xmlEntity, const NodeId& managerNodeId) :
#endif
    mXmlEntity(xmlEntity),
    mManagerNodeId(managerNodeId)
#ifdef PHYSICSPLUGINS
    ,mPhysicsScene(0)
#endif
{
    applyGravity(true);
}

//-------------------------------------------------------------------------------------
Entity::~Entity()
{
#ifdef PHYSICSPLUGINS
    destroyPhysics();
#endif

#ifdef POOL
#else
    delete mXmlEntity;
#endif
}

//-------------------------------------------------------------------------------------
#ifdef POOL
const RefCntPoolPtr<XmlEntity>& Entity::getXmlEntity() const
#else
XmlEntity* Entity::getXmlEntity()
#endif
{
    return mXmlEntity;
}

//-------------------------------------------------------------------------------------
const NodeId& Entity::getManagerNodeId()
{
    return mManagerNodeId;
}

//-------------------------------------------------------------------------------------
void Entity::applyGravity(bool applied)
{
    if (!applied)
        mGravity = false;
    else
        mGravity = mXmlEntity->getFlags() & EFGravity;
}

//-------------------------------------------------------------------------------------
void Entity::setGravity(bool enabled)
{
    mGravity = enabled;
}

//-------------------------------------------------------------------------------------
bool Entity::isGravityEnabled()
{
    return mGravity;
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* Entity::getPhysicsScene()
{
    return mPhysicsScene;
}

//-------------------------------------------------------------------------------------
void Entity::createPhysics(IPhysicsScene* physicsScene)
{
    destroyPhysics();

    mPhysicsScene = physicsScene;
}

//-------------------------------------------------------------------------------------
void Entity::destroyPhysics()
{
    mPhysicsScene = 0;
}
#endif

//-----------------------------------------------------------------------
void Entity::addEntityListener(EntityListener* newListener)
{
	mEntityListeners.insert(newListener);
}

//-----------------------------------------------------------------------
void Entity::removeEntityListener(EntityListener* oldListener)
{
	mEntityListeners.erase(oldListener);
}

//-----------------------------------------------------------------------
#ifdef POOL
void Entity::throwEvtToEntityListeners(const Node& node, Entity& entity, RefCntPoolPtr<XmlEvt>& xmlEvt)
#else
void Entity::throwEvtToEntityListeners(const Node& node, Entity& entity, XmlEvt* xmlEvt)
#endif
{
    for (std::set<EntityListener*>::iterator i = mEntityListeners.begin(); i != mEntityListeners.end(); ++i)
        (*i)->onEvt(node, entity, xmlEvt);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
