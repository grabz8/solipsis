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
#include "Peer.h"
#include "OgreHelpers.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
#ifdef POOL
SiteNode::SiteNode(const NodeId& nodeId, RefCntPoolPtr<XmlEntity>& xmlEntity) :
#else
SiteNode::SiteNode(const NodeId& nodeId, XmlEntity* xmlEntity) :
#endif
    Node(nodeId, "site"),
    mSite(xmlEntity, nodeId)
{
}

//-------------------------------------------------------------------------------------
SiteNode::~SiteNode()
{
}

//-------------------------------------------------------------------------------------
Site& SiteNode::getEntity()
{
    return mSite;
}

//-------------------------------------------------------------------------------------
bool SiteNode::loadFromElt(TiXmlElement* nodeElt)
{
    // Load present entities ?
    TiXmlElement* presentEntitiesElt;
    if ((presentEntitiesElt = nodeElt->FirstChildElement("presentEntities")) != 0)
    {
        for (TiXmlElement* nodeIdElt = presentEntitiesElt->FirstChildElement("nodeId"); nodeIdElt != 0; nodeIdElt = nodeIdElt->NextSiblingElement("nodeId"))
        {
            NodeId presentEntityNodeId = nodeIdElt->GetText();
            if (!Peer::getSingleton().getNodeManager()->loadNodeIdFile(presentEntityNodeId))
                return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* SiteNode::getSavedElt()
{
    OGRE_LOG("SiteNode::getSavedElt() saving entity and entities/nodes present in site node with nodeId:" + mNodeId);

    // Get root node
    TiXmlElement* nodeElt = Node::getSavedElt();
    // Add site entity
    mSite.getXmlEntity()->toXmlElt(*nodeElt);

    // Add present entities nodes ids
    TiXmlElement* presentEntitiesElt = new TiXmlElement("presentEntities");
    nodeElt->LinkEndChild(presentEntitiesElt); 
    pthread_mutex_lock(&mMutex);
    for(Entity::EntityMap::iterator entity=mPresentEntities.begin();entity!=mPresentEntities.end();++entity)
    {
        NodeId presentEntityNodeId = entity->second->getManagerNodeId();
        TiXmlElement* presentEntityNodeIdElt = new TiXmlElement("nodeId");
        presentEntitiesElt->LinkEndChild(presentEntityNodeIdElt); 
        TiXmlText* presentEntityNodeIdText = new TiXmlText(presentEntityNodeId.c_str());
        presentEntityNodeIdElt->LinkEndChild(presentEntityNodeIdText);
        // we do not save present nodes because they are owned by avatar nodes which are responsible to save them
    }
    pthread_mutex_unlock(&mMutex);

    return nodeElt;
}

//-------------------------------------------------------------------------------------
Entity::EntityMap& SiteNode::getPresentEntities()
{
    return mPresentEntities;
}

//-------------------------------------------------------------------------------------
bool SiteNode::addPresentEntity(Entity* entity)
{
    OGRE_LOG("SiteNode::addPresentEntity() uid:" + mSite.getXmlEntity()->getUidString() + " adding entity uid:" + entity->getXmlEntity()->getUidString());

    pthread_mutex_lock(&mMutex);
    mPresentEntities[entity->getXmlEntity()->getUid()] = entity;
    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool SiteNode::removePresentEntity(Entity* entity)
{
    OGRE_LOG("SiteNode::removePresentEntity() uid:" + mSite.getXmlEntity()->getUidString() + " removing entity uid:" + entity->getXmlEntity()->getUidString());

    pthread_mutex_lock(&mMutex);
    mPresentEntities.erase(entity->getXmlEntity()->getUid());
    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
