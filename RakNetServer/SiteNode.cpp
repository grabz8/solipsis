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

#include "SiteNode.h"
#include "RakNetServer.h"
#include <CTLog.h>

using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
SiteNode::SiteNode() :
    RakNetSiteNode()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SiteNode::SiteNode()");
}

//-------------------------------------------------------------------------------------
SiteNode::~SiteNode()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SiteNode::~SiteNode()");

    RakNetServer::getSingleton().onSiteNodeDestroyed(this);
}

//-------------------------------------------------------------------------------------
Entity::EntityMap& SiteNode::getPresentEntities()
{
    return mPresentEntities;
}

//-------------------------------------------------------------------------------------
void SiteNode::addPresentEntity(Entity* entity)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SiteNode::addPresentEntity() uid:%s adding entity uid:%s", mEntity->getXmlEntity()->getUid().c_str(), entity->getXmlEntity()->getUid().c_str());

    mPresentEntities[entity->getXmlEntity()->getUid()] = entity;
}

//-------------------------------------------------------------------------------------
void SiteNode::removePresentEntity(Entity* entity)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SiteNode::removePresentEntity() uid:%s removing entity uid:%s", mEntity->getXmlEntity()->getUid().c_str(), entity->getXmlEntity()->getUid().c_str());

    mPresentEntities.erase(entity->getXmlEntity()->getUid());
}

//-------------------------------------------------------------------------------------
bool SiteNode::loadFromElt(TiXmlElement* nodeElt)
{
    // Load present entities ?
    TiXmlElement* presentEntitiesElt;
    if ((presentEntitiesElt = nodeElt->FirstChildElement("presentEntities")) != 0)
    {
        for (TiXmlElement* entityElt = presentEntitiesElt->FirstChildElement("entity"); entityElt != 0; entityElt = entityElt->NextSiblingElement("entity"))
        {
            Entity* entity = RakNetServer::getSingletonPtr()->loadEntity(entityElt);
            if (entity == 0)
                return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* SiteNode::getSavedElt()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "SiteNode::getSavedElt() saving entity of site node with nodeId:%s", mNodeId.c_str());

    // Get root node
    TiXmlElement* nodeElt = RakNetNode::getSavedElt();
    // Add site entity
    mEntity->getXmlEntity()->toXmlElt(*nodeElt);

    // Add present entities
    TiXmlElement* presentEntitiesElt = new TiXmlElement("presentEntities");
    nodeElt->LinkEndChild(presentEntitiesElt); 
    for (Entity::EntityMap::iterator it = mPresentEntities.begin(); it != mPresentEntities.end(); ++it)
        it->second->getXmlEntity()->toXmlElt(*presentEntitiesElt);

    return nodeElt;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
