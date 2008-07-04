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
#include "ObjectNode.h"
#include "OgreHelpers.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
NodeManager::NodeManager()
{
}

//-------------------------------------------------------------------------------------
NodeManager::~NodeManager()
{
    // Destroy object nodes
    for(NodeMap::const_iterator it=mNodes.begin();it!=mNodes.end();it=mNodes.begin())
    {
        Node *node = it->second;
        OGRE_LOG("NodeManager::~NodeManager() Destroying forgotten node " + node->getNodeId() + " !");
        mNodes.erase(it->first);
        delete node;
    }
}

//-------------------------------------------------------------------------------------
bool NodeManager::loadNodeIdFile(const NodeId& nodeId)
{
    std::string nodeIdFilename = Peer::getSingleton().mMediaCachePath + "\\" + nodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());
    if (!xmlNodeIdFileDoc.LoadFile())
        return false;

    OGRE_LOG("NodeManager::loadNodeIdFile() loading node with nodeId:" + nodeId + " from " + nodeIdFilename);

    TiXmlElement* nodeElt = xmlNodeIdFileDoc.FirstChildElement("node");
    if (nodeElt == 0)
        return false;

    // Create the node according to the managed entity
    TiXmlElement* entityElt = nodeElt->FirstChildElement("entity");
    if (entityElt == 0)
        return false;
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity;
#else
    XmlEntity* xmlEntity = new XmlEntity();
#endif
    xmlEntity->fromXmlElt(entityElt);
    Node* node = 0;
    switch (xmlEntity->getType())
    {
    case ETAvatar:
        node = new AvatarNode(nodeId, xmlEntity);
        break;
    case ETSite:
        node = new SiteNode(nodeId, xmlEntity);
        break;
    case ETObject:
        node = new ObjectNode(nodeId, xmlEntity);
        break;
    }
    if (node == 0)
        return false;
    mNodes[nodeId] = node;

    return node->loadFromElt(nodeElt);
}

//-------------------------------------------------------------------------------------
bool NodeManager::saveNodeIdFile(const NodeId& nodeId)
{
    std::string nodeIdFilename = Peer::getSingleton().mMediaCachePath + "\\" + nodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());

    OGRE_LOG("NodeManager::saveNodeIdFile() saving node with nodeId:" + nodeId + " into " + nodeIdFilename);

    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;
    TiXmlElement* nodeElt = node->getSavedElt();
    if (nodeElt == 0)
        return false;
    xmlNodeIdFileDoc.LinkEndChild(nodeElt); 
    xmlNodeIdFileDoc.SaveFile();

    return true;
}

//-------------------------------------------------------------------------------------
AvatarNode* NodeManager::login(XmlLogin* xmlLogin)
{
    try
    {
        // Authentication
        EntityUID AvatarEntityUid;
        NodeId nodeId;
        if (xmlLogin->getPwd().compare("demo") == 0)
        {
            int connection = Peer::getSingleton().allocConnection();
            if (connection == -1)
                return 0;
            char nidhex[16];
            sprintf(nidhex, "%08X", connection + 1);
            nodeId = nidhex;
            AvatarEntityUid = 0x10000 + connection + 1;
        }
        else
            return 0;

        // Load the site node on first avatar connected
        bool sceneLoaded = false;
        NodeId siteNodeId = "00000010";
        if ((nodeId.compare("00000001") == 0) && Peer::getSingleton().mSceneDemoLoaded.empty())
            sceneLoaded = loadNodeIdFile(siteNodeId);
        // Load the avatar node
        bool avatarLoaded = loadNodeIdFile(nodeId);

        // Simulate the scene around the avatar
        if (!sceneLoaded && (nodeId.compare("00000001") == 0))
        {
            std::string xmlSiteStr;
            if (Peer::getSingleton().mSceneDemoLoaded.compare("Ile") == 0)
                xmlSiteStr = "\
<entity uid=\"11112223\" owner=\"00000001\" type=\"1\" name=\"Ile\" version=\"00000000\">\
<position x=\"18.0\" y=\"-58.0\" z=\"133.0\" />\
<orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
<aabb>\
<min x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
<max x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
</aabb>\
<content>\
<sceneContent>\
<entryGate gravity=\"true\" >\
<position x=\"170.0\" y=\"-60.0\" z=\"450.0\" />\
</entryGate>\
</sceneContent>\
<lod level=\"0\">\
<sceneLodContent mainFilename=\"Ile.osm\" collision=\"Ile_COLLISION\" />\
<files>\
<file name=\"11112224.ssf\" version=\"00000000\" />\
</files>\
</lod>\
</content>\
</entity>\
";
            else if (Peer::getSingleton().mSceneDemoLoaded.compare("DigitalOcean1") == 0)
                xmlSiteStr = "\
<entity uid=\"11112224\" owner=\"00000001\" type=\"1\" name=\"DigitalOcean1\" version=\"00000000\">\
<position x=\"18.0\" y=\"-58.0\" z=\"133.0\" />\
<orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
<aabb>\
<min x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
<max x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
</aabb>\
<content>\
<sceneContent>\
<entryGate gravity=\"false\" >\
<position x=\"30.0\" y=\"-26.0\" z=\"68.0\" />\
</entryGate>\
</sceneContent>\
<lod level=\"0\">\
<sceneLodContent mainFilename=\"DigitalOcean1.osm\" />\
<files>\
<file name=\"11112224.ssf\" version=\"00000000\" />\
</files>\
</lod>\
</content>\
</entity>\
";
            else
                xmlSiteStr = "\
<entity uid=\"11112222\" owner=\"00000001\" type=\"1\" name=\"Deltastation1\" version=\"00000000\" >\
<position x=\"18.0\" y=\"-58.0\" z=\"133.0\" />\
<orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
<aabb>\
<min x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
<max x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
</aabb>\
<content>\
<sceneContent>\
<entryGate gravity=\"true\" >\
<position x=\"17.0\" y=\"-50.0\" z=\"115.0\" />\
</entryGate>\
</sceneContent>\
<lod level=\"0\">\
<sceneLodContent mainFilename=\"Deltastation1.osm\" collision=\"Delta_COLLISION\" />\
<files>\
<file name=\"11112222.ssf\" version=\"00000000\" />\
</files>\
</lod>\
</content>\
</entity>\
";
            TiXmlDocument xmlSiteDoc;
            xmlSiteDoc.Parse(xmlSiteStr.c_str());
#ifdef POOL
            RefCntPoolPtr<XmlEntity> siteXmlEntity;
#else
            XmlEntity* siteXmlEntity = new XmlEntity();
#endif
            siteXmlEntity->fromXmlElt(xmlSiteDoc.RootElement());
            OGRE_LOG("NodeManager::login() initializing simulation site node name:" + siteXmlEntity->getName());
            SiteNode* siteNode = new SiteNode(siteNodeId, siteXmlEntity);
            mNodes[siteNodeId] = siteNode;
        }

        if (!avatarLoaded)
        {
            // Create the avatar node
            std::string xmlAvatarStr = "\
<entity uid=\"" + XmlHelpers::convertEntityUIDToHexString(AvatarEntityUid) + "\" owner=\"" + nodeId + "\" type=\"0\" name=\"" + xmlLogin->getUsername() + "\" version=\"00000000\">\
 <flags bitmask=\"" + XmlHelpers::convertEntityFlagsToHexString(EFNone) + "\" />\
 <position x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
 <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
 <aabb>\
  <min x=\"-0.82447118\" y=\"-0.013709042\" z=\"-0.64538133\" />\
  <max x=\"0.82353306\" y=\"1.4500649\" z=\"0.69156337\" />\
 </aabb>\
 <content>\
  <lod level=\"0\">\
   <files>\
    <file name=\"Kevin.saf\" version=\"00000000\" />\
    <file name=\"" + XmlHelpers::convertEntityUIDToHexString(AvatarEntityUid) + ".sif\" version=\"00000000\" />\
   </files>\
  </lod>\
 </content>\
</entity>\
";
            TiXmlDocument xmlAvatarDoc;
            xmlAvatarDoc.Parse(xmlAvatarStr.c_str());
#ifdef POOL
            RefCntPoolPtr<XmlEntity> avatarXmlEntity;
#else
            XmlEntity* avatarXmlEntity = new XmlEntity();
#endif
            avatarXmlEntity->fromXmlElt(xmlAvatarDoc.RootElement());
            OGRE_LOG("NodeManager::login() initializing simulation avatar node name:" + avatarXmlEntity->getName());
            AvatarNode* avatarNode = new AvatarNode(nodeId, avatarXmlEntity);
            mNodes[nodeId] = avatarNode;
        }

        // Get 1st scene
        SiteNode* firstSiteNode = 0;
        for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
            if (node->second->getType().compare("site") == 0)
            {
                firstSiteNode = (SiteNode*)node->second;
                break;
            }
        if (firstSiteNode == 0)
        {
            OGRE_LOG("NodeManager::login() Unable to find first site node");
            return 0;
        }
        // Get the avatar
        AvatarNode* avatarNode = (AvatarNode*)mNodes[nodeId];
        if (avatarNode == 0)
        {
            OGRE_LOG("NodeManager::login() Unable to find the avatar node");
            return 0;
        }
        // stop displacement
        avatarNode->getEntity().getXmlEntity()->setDisplacement(Vector3::ZERO);
        // If scene changed
        // Move avatar on the entry gate of the scene + gravity
        if (!sceneLoaded)
        {
            RefCntPoolPtr<XmlSceneContent> xmlSceneContent = firstSiteNode->getEntity().getXmlEntity()->getContent()->getDatas();
            avatarNode->getEntity().getXmlEntity()->setPosition(xmlSceneContent->getEntryGate().mPosition);
            if (xmlSceneContent->getEntryGate().mGravity)
                avatarNode->getEntity().getXmlEntity()->setFlags(avatarNode->getEntity().getXmlEntity()->getFlags() | EFGravity);
            else
                avatarNode->getEntity().getXmlEntity()->setFlags(avatarNode->getEntity().getXmlEntity()->getFlags() & ~EFGravity);
        }

        // Add other avatars to aware of
        for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
            if (node->first != avatarNode->getNodeId())
            {
                if (node->second->getType().compare("avatar") == 0)
                {
                    AvatarNode* an = (AvatarNode*)node->second;
                    avatarNode->addAwareEntity(&an->getEntity());
                    an->incDecAwareCounter(+1);
                    an->addAwareEntity(&avatarNode->getEntity());
                }
                else if (node->second->getType().compare("site") == 0)
                {
                    SiteNode* sn = (SiteNode*)node->second;
                    avatarNode->addAwareEntity(&sn->getEntity());
                    sn->incDecAwareCounter(+1);
                }
                else if (node->second->getType().compare("object") == 0)
                {
                    ObjectNode* on = (ObjectNode*)node->second;
                    avatarNode->addAwareEntity(&on->getEntity());
                    on->incDecAwareCounter(+1);
                }
            }
        // Add its own avatar to aware of
        avatarNode->addAwareEntity(&avatarNode->getEntity());

        // Add objects present into the scene
        for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
            if (node->second->getType().compare("object") == 0)
            {
                ObjectNode* on = (ObjectNode*)node->second;
                firstSiteNode->addPresentEntity(&on->getEntity());
            }

        // Unfreeze avatar node
        avatarNode->freeze(false);

        return avatarNode;
    }
    catch (Ogre::Exception& e)
    {
        OGRE_LOG("NodeManager::login() Login failure, exception: " + e.getFullDescription());
        return 0;
    }
}

//-------------------------------------------------------------------------------------
bool NodeManager::logout(const NodeId& nodeId)
{
    AvatarNode* avatarNode = (AvatarNode*)mNodes[nodeId];
    avatarNode->freeze(true);

    // Save this avatar node + entities owned
    if (!saveNodeIdFile(nodeId))
        throw Exception(Exception::ERR_INTERNAL_ERROR,
        "Unable to save node with nodeId:" + nodeId + " !",
        "NodeManager::logout");

    // Destroy the node
    mDestroyedNodeIds.insert(nodeId);

    int connection;
    sscanf(nodeId.c_str(), "%x", &connection);
    if (connection < 1)
        return false;
    if (!Peer::getSingleton().releaseConnection(connection - 1))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
bool NodeManager::update()
{
    NodeIdSet newDestroyedNodeIds;
    NodeIdSet::iterator i;
    for (i = mDestroyedNodeIds.begin(); i != mDestroyedNodeIds.end(); i++)
    {
        Node* node = mNodes[*i];
        if (node->getType().compare("avatar") == 0)
        {
            AvatarNode* avatarNode = (AvatarNode*)node;
            // Remove this avatar from avatars aware of
            for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
            {
                if (node->first != avatarNode->getNodeId())
                {
                    if (node->second->getType().compare("avatar") == 0)
                    {
                        AvatarNode* an = (AvatarNode*)node->second;
                        avatarNode->removeAwareEntity(&an->getEntity());
                        if (an->incDecAwareCounter(-1) <= 0)
                            OGRE_LOG("NodeManager::update() Avatar node aware counter corruption detected !");
                        an->removeAwareEntity(&avatarNode->getEntity());
                    }
                    else if (node->second->getType().compare("site") == 0)
                    {
                        SiteNode* sn = (SiteNode*)node->second;
                        avatarNode->removeAwareEntity(&sn->getEntity());
                        if (sn->incDecAwareCounter(-1) == 0)
                            newDestroyedNodeIds.insert(node->first);
                    }
                    else if (node->second->getType().compare("object") == 0)
                    {
                        ObjectNode* on = (ObjectNode*)node->second;
                        avatarNode->removeAwareEntity(&on->getEntity());
                        if (on->incDecAwareCounter(-1) == 0)
                            newDestroyedNodeIds.insert(node->first);
                    }
                }
            }
        }
        mNodes.erase(*i);
        delete node;
    }
    mDestroyedNodeIds.clear();
    mDestroyedNodeIds = newDestroyedNodeIds;

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
#ifdef POOL
ObjectNode* NodeManager::createObjectNode(RefCntPoolPtr<XmlEntity> xmlEntity)
#else
ObjectNode* NodeManager::createObjectNode(XmlEntity* xmlEntity)
#endif
{
    OGRE_LOG("NodeManager::createObjectNode() creating object node name:" + xmlEntity->getName());

    static NodeId objectNodeId = "00000100";

    // Create the object node
    ObjectNode* objectNode = new ObjectNode(objectNodeId, xmlEntity);
    mNodes[objectNodeId] = objectNode;

    // Put it into the 1st scene
    SiteNode* firstSiteNode = 0;
    for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
        if (node->second->getType().compare("site") == 0)
        {
            firstSiteNode = (SiteNode*)node->second;
            break;
        }
    if (firstSiteNode != 0)
        firstSiteNode->addPresentEntity(&objectNode->getEntity());

    // Add avatars to aware of
    for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
        if (node->second->getType().compare("avatar") == 0)
        {
            AvatarNode* an = (AvatarNode*)node->second;
            an->addAwareEntity(&objectNode->getEntity(), (node->first != xmlEntity->getOwner()));
            objectNode->incDecAwareCounter(+1);
        }

    objectNodeId = XmlHelpers::convertUIntToHexString(XmlHelpers::convertHexStringToUInt(objectNodeId.c_str()) + 1);

    return objectNode;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
