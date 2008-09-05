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

#include "NodeManager.h"
#include "Peer.h"
#include "AvatarNode.h"
#include "SiteNode.h"
#include "OgreHelpers.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
NodeManager::NodeManager()
{
}

//-------------------------------------------------------------------------------------
NodeManager::~NodeManager()
{
    // Destroy nodes
    for(NodeMap::const_iterator it=mNodes.begin();it!=mNodes.end();it=mNodes.begin())
    {
        Node *node = it->second;
        mNodes.erase(it->first);
        delete node;
    }
}

//-------------------------------------------------------------------------------------
void NodeManager::onNewEntity(Entity* entity, bool sendNewEvt)
{
    const NodeId& avatarNodeId = Peer::getSingleton().getNodeId();
    AvatarNode* avatarNode = (AvatarNode*)mNodes[avatarNodeId];
    if (avatarNode != 0)
        avatarNode->onNewEntity(entity, sendNewEvt);
}

//-------------------------------------------------------------------------------------
void NodeManager::onUpdatedEntity(Entity* entity)
{
    const NodeId& avatarNodeId = Peer::getSingleton().getNodeId();
    AvatarNode* avatarNode = (AvatarNode*)mNodes[avatarNodeId];
    if (avatarNode != 0)
        avatarNode->onUpdatedEntity(entity);
}

//-------------------------------------------------------------------------------------
void NodeManager::onLostEntity(Entity* entity, bool sendLostEvt)
{
    const NodeId& avatarNodeId = Peer::getSingleton().getNodeId();
    AvatarNode* avatarNode = (AvatarNode*)mNodes[avatarNodeId];
    if (avatarNode != 0)
        avatarNode->onLostEntity(entity, sendLostEvt);
}

//-------------------------------------------------------------------------------------
void NodeManager::onActionOnEntity(RakNet::BitStream *bitStream)
{
    const NodeId& avatarNodeId = Peer::getSingleton().getNodeId();
    AvatarNode* avatarNode = (AvatarNode*)mNodes[avatarNodeId];
    if (avatarNode != 0)
        avatarNode->onActionOnEntity(bitStream);
}

//-------------------------------------------------------------------------------------
void NodeManager::onLostNode(const NodeId& nodeId)
{
    NodeMap::const_iterator it = mNodes.find(nodeId);
    if (it != mNodes.end())
    {
        Node *node = it->second;
        mNodes.erase(it->first);
        delete node;
    }
}

//-------------------------------------------------------------------------------------
bool NodeManager::update()
{

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool NodeManager::processEvt(const NodeId& nodeId, RefCntPoolPtr<XmlEvt>& xmlEvt, std::string& xmlRespStr)
#else
bool NodeManager::processEvt(const NodeId& nodeId, XmlEvt* xmlEvt, std::string& xmlRespStr)
#endif
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;

    return node->processEvt(xmlEvt, xmlRespStr);
}

//-------------------------------------------------------------------------------------
#ifdef POOL
RefCntPoolPtr<XmlEvt> NodeManager::getNextEvtToHandle(const NodeId& nodeId)
#else
XmlEvt* NodeManager::getNextEvtToHandle(const NodeId& nodeId)
#endif
{
    Node* node = mNodes[nodeId];
    if (node == 0)
#ifdef POOL
        return RefCntPoolPtr<XmlEvt>::nullPtr;
#else
        return 0;
#endif

    return node->getNextEvtToHandle();
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool NodeManager::freeEvt(const NodeId& nodeId, RefCntPoolPtr<XmlEvt>& xmlEvt)
#else
bool NodeManager::freeEvt(const NodeId& nodeId, XmlEvt* xmlEvt)
#endif
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;

    return node->freeEvt(xmlEvt);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
