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
#include <CTLog.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Entity::Entity() :
    RakNetEntity()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::Entity()");
}

//-------------------------------------------------------------------------------------
Entity::~Entity()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::~Entity()");

    removeEntity(this);
}

//-------------------------------------------------------------------------------------
void Entity::onNewEntity()
{
    RakNetEntity::onNewEntity();
    RakNetServer::getSingleton().getSiteNode()->onNewEntity(this);
}

//-------------------------------------------------------------------------------------
void Entity::onLostEntity()
{
    RakNetServer::getSingleton().getSiteNode()->onLostEntity(this);
    RakNetEntity::onLostEntity();
}

//-------------------------------------------------------------------------------------
void Entity::deserialize(BitStream *bitStream, SerializationType serializationType, SystemAddress sender)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::deserialize()");

    RakNetEntity::deserialize(bitStream, serializationType, sender);

    if (serializationType == ST_SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM)
        addEntity(this);
}

//-------------------------------------------------------------------------------------
Replica::QueryResult Entity::isVisibleFrom(ConnectionRM* connection)
{
#ifdef LOGRAKNET
    IsVisibleFromConn::iterator it = mIsVisibleFromConn.find(connection);
    if (it == mIsVisibleFromConn.end())
    {
        mIsVisibleFromConn[connection] = -1;
        it = mIsVisibleFromConn.find(connection);
   } 
    #define LOG_ONCHANGE(q, fmt, ...) if (it->second != q) { it->second = q; LogHandler::getLogHandler()->logf(LogHandler::VL_DEBUG, fmt, __VA_ARGS__); }
#else
    #define LOG_ONCHANGE(q, fmt, ...)
#endif

    if (mXmlEntity->getType() != ETAvatar)
    {
        LOG_ONCHANGE(1, "Entity(%s)::isVisibleFrom(%s) site/object entity => returning QR_ALWAYS", mXmlEntity->getUid().c_str(), (connection == 0) ? "0" :connection->getSystemAddress().ToString());
        return QR_ALWAYS;
    }
    if (connection == 0)
    {
        LOG_ONCHANGE(0, "Entity(%s)::isVisibleFrom(0) => returning QR_NO", mXmlEntity->getUid().c_str());
        return QR_NO;
    }
    Entity* entity = (Entity*)findByAddress(connection->getSystemAddress());
    if (entity == 0)
    {
        LOG_ONCHANGE(3, "Entity(%s)::isVisibleFrom(%s) entity not found => returning QR_NO", mXmlEntity->getUid().c_str(), connection->getSystemAddress().ToString());
        return QR_NO;
    }
    if (entity == this)
    {
        LOG_ONCHANGE(4, "Entity(%s)::isVisibleFrom(%s) it s me => returning QR_YES", mXmlEntity->getUid().c_str(), connection->getSystemAddress().ToString());
        return QR_YES;
    }
    Ogre::Real distance2 = mXmlEntity->getPosition().squaredDistance(entity->getXmlEntity()->getPosition());
    if (distance2 > RakNetServer::getSingleton().getAvatarScopeDistance2())
    {
        LOG_ONCHANGE(5, "Entity(%s)::isVisibleFrom(%s) too far (d=%.2f) from %s => returning QR_NO", mXmlEntity->getUid().c_str(), connection->getSystemAddress().ToString(), distance2, entity->getXmlEntity()->getUid().c_str());
        return QR_NO;
    }

    LOG_ONCHANGE(6, "Entity(%s)::isVisibleFrom(%s) is visible (d=%.2f) from %s => returning QR_YES", mXmlEntity->getUid().c_str(), connection->getSystemAddress().ToString(), distance2, entity->getXmlEntity()->getUid().c_str());
    return QR_YES;
}

//-------------------------------------------------------------------------------------
Replica::QueryResult Entity::queryConstruction(ConnectionRM* connection)
{
#ifdef LOGRAKNET
/*    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Entity::queryConstruction() This entity (%s) should be constructed on %s ?",
        mXmlEntity->getUid().c_str(),
        connection ? (connection->getSystemAddress().ToString()) : "0");*/
#endif

    return isVisibleFrom(connection);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
