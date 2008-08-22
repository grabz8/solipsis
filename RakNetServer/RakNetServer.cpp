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

#include <time.h>
#include "RakNetServer.h"
#include "AvatarNode.h"
#include "SiteNode.h"
#include "Entity.h"
#include <CTSystem.h>
#include <CTIO.h>
#include <Ogre.h>

using namespace RakNet;

namespace Solipsis {

RakNetServer* RakNetServer::ms_Singleton = 0;
RakNetConnection *RakNetConnection::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
RakNetServer::RakNetServer(int argc, char** argv) :
    mPort(8660),
    mMaxIncomingConnections(32),
    mSceneDemoLoaded(""),
    mMediaCachePath(""),
    mSiteNodeId("00000010"),
    mQuit(false)
{
    for (int iarg=1; iarg < argc; iarg++)
    {
        if ((strstr(argv[iarg], "-p") != 0) && (argc > iarg+1))
        {
            iarg++;
            mPort = atoi(argv[iarg]);
            continue;
        }
        if ((strstr(argv[iarg], "-c") != 0) && (argc > iarg+1))
        {
            iarg++;
            mMaxIncomingConnections = atoi(argv[iarg]);
            continue;
        }
        if ((strstr(argv[iarg], "-s") != 0) && (argc > iarg+1))
        {
            iarg++;
            mSceneDemoLoaded = argv[iarg];
            continue;
        }
        if ((strstr(argv[iarg], "-m") != 0) && (argc > iarg+1))
        {
            iarg++;
            mMediaCachePath = argv[iarg];
            continue;
        }
    }

    // Retrieve Media/Cache path
    if (mMediaCachePath.empty())
        mMediaCachePath = CommonTools::IO::getCWD() + "\\" + CommonTools::IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    ms_Singleton = this;
}

//-------------------------------------------------------------------------------------
void RakNetServer::initialize()
{
    // We are the server
    mRakNetConnection.mServer = true;
    // Set logger
    mRakNetConnection.mLogger = &mRakNetConnectionLogger;
    mRakNetConnection.logMessage("RakNetServer::initialize()");
    // Get 1 instance of the RakNet peer interface
    mRakNetConnection.mRakPeer = RakNetworkFactory::GetRakPeerInterface();
    // ObjectMemberRPC and ReplicaManager2 require that you call SetNetworkIDManager()
    mRakNetConnection.mRakPeer->SetNetworkIDManager(&mRakNetConnection.mNetworkIdManager);
    // The network ID authority is the system that creates the common numerical identifier used to lookup pointers.
    // For client/server this is the server
    // For peer to peer this would be true on every system, and you would also have call NetworkID::peerToPeerMode=true;
    mRakNetConnection.mNetworkIdManager.SetIsNetworkIDAuthority(mRakNetConnection.mServer);
    // Start RakNet, up to 32 connections
    mRakNetConnection.mSocketDescriptor.port = mPort;
    mRakNetConnection.mRakPeer->Startup(mMaxIncomingConnections, 100, &mRakNetConnection.mSocketDescriptor, 1);
    mRakNetConnection.mServerSystemAddress = mRakNetConnection.mRakPeer->GetInternalID();
    mRakNetConnection.logMessage("RakNetServer::initialize() Server started on " + std::string(mRakNetConnection.mServerSystemAddress.ToString()));
    // Attach the ReplicaManager2 plugin
    mRakNetConnection.mRakPeer->AttachPlugin(&mRakNetConnection.mReplicaManager);
    // Register our custom connection factory
    mRakNetConnection.mReplicaManager.SetConnectionFactory(&mRakNetConnection.mConnectionFactory);
    // Attach the FileListTransfer plugin
    mRakNetConnection.mRakPeer->AttachPlugin(&mRakNetConnection.mFileListTransfer);
    // The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
    mRakNetConnection.mRakPeer->SetMaximumIncomingConnections(mMaxIncomingConnections);

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is used in in Replica2::SerializeConstruct
	// The decoding is used in in Connection_RM2::Construct
	// The stringTable class will also send strings that weren't registered but this just falls back to the stringCompressor and wastes 1 extra bit on top of that
	// 2nd parameter of false means a static string so it's not necessary to copy it
	StringTable::Instance()->AddString("AvatarNode", false);
	StringTable::Instance()->AddString("SiteNode", false);
	StringTable::Instance()->AddString("Entity", false);

    // Create the site node
    bool sceneLoaded = false;
    if (mSceneDemoLoaded.empty())
        sceneLoaded = loadNodeIdFile(mSiteNodeId);
    // Simulate the scene around the avatar
    if (!sceneLoaded)
    {
        std::string xmlSiteStr;
        if (mSceneDemoLoaded.compare("Ile") == 0)
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
    <file name=\"11112223.ssf\" version=\"00000000\" />\
   </files>\
  </lod>\
 </content>\
</entity>\
";
        else if (mSceneDemoLoaded.compare("DigitalOcean1") == 0)
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
        Entity* sceneEntity = new Entity();
        sceneEntity->getXmlEntity()->fromXmlElt(xmlSiteDoc.RootElement());
        onNewEntity(*sceneEntity);
        /// Server can serialize
        sceneEntity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // Entity managed by the Replica2 plugin
        sceneEntity->SetReplicaManager(&mRakNetConnection.mReplicaManager);
        // Send out this new entity to all systems
        sceneEntity->BroadcastConstruction();

        mRakNetConnection.logMessage("RakNetServer::initialize() initializing simulation site node name:" + sceneEntity->getXmlEntity()->getName());
        SiteNode* siteNode = new SiteNode();
        siteNode->setEntity(sceneEntity);
        siteNode->setNodeId(mSiteNodeId);
        mNodes[mSiteNodeId] = siteNode;
        // Node managed by the Replica2 plugin
        siteNode->SetReplicaManager(&mRakNetConnection.mReplicaManager);
        // Send out this new node to all systems
        siteNode->BroadcastConstruction();
    }
}

//-------------------------------------------------------------------------------------
void RakNetServer::run()
{
    Packet *packet;

    mRakNetConnection.logMessage("RakNetServer::run()");

    while (!mQuit)
    {
        // process packets
        for (packet = mRakNetConnection.mRakPeer->Receive(); packet; mRakNetConnection.mRakPeer->DeallocatePacket(packet), packet = mRakNetConnection.mRakPeer->Receive())
        {
            switch (packet->data[0])
            {
            case ID_CONNECTION_ATTEMPT_FAILED:
                mRakNetConnection.logMessage("RakNetServer::run() ID_CONNECTION_ATTEMPT_FAILED");
                mQuit = true;
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                mRakNetConnection.logMessage("RakNetServer::run() ID_NO_FREE_INCOMING_CONNECTIONS");
                mQuit = true;
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                mRakNetConnection.logMessage("RakNetServer::run() ID_CONNECTION_REQUEST_ACCEPTED");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                {
                    mRakNetConnection.logMessage("RakNetServer::run() ID_NEW_INCOMING_CONNECTION from " + std::string(packet->systemAddress.ToString()));

                    AvatarNode* avatarNode = new AvatarNode();
                    avatarNode->setSystemAddress(packet->systemAddress);
                    // Node managed by the Replica2 plugin
                    avatarNode->SetReplicaManager(&mRakNetConnection.mReplicaManager);
                    // Send out this new node to all systems
                    avatarNode->BroadcastConstruction();
                }
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                mRakNetConnection.logMessage("RakNetServer::run() ID_DISCONNECTION_NOTIFICATION");
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetAvatarNode::deleteByAddress(packet->systemAddress);
                break;
            case ID_CONNECTION_LOST:
                mRakNetConnection.logMessage("RakNetServer::run() ID_CONNECTION_LOST");
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetAvatarNode::deleteByAddress(packet->systemAddress);
                break;
            case RakNetConnection::ID_ACTION_ON_ENTITY:
                mRakNetConnection.logMessage("RakNetServer::run() RakNetConnection::ID_ACTION from " + std::string(packet->systemAddress.ToString()));
                BitStream bitStream(packet->data, packet->length, false);
                bitStream.IgnoreBytes(1);
                ActionType actionType;
                bitStream.Read(actionType);
                EntityUID sourceEntityUid;
                bitStream.Read(sourceEntityUid);
                EntityUID targetEntityUid;
                bitStream.Read(targetEntityUid);
                std::string desc;
                RakNetConnection::DeserializeString(&bitStream, desc);
                if (sourceEntityUid == targetEntityUid)
                {
                    // Broadcast
                    mRakNetConnection.mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
                }
                else
                {
                    // Send to target node
                    AvatarNode *targetAvatarNode = getAvatarNodeOfEntity(targetEntityUid);
                    if (targetAvatarNode == 0)
                        mRakNetConnection.logMessage("RakNetServer::run() RakNetConnection::ID_ACTION unable to find avatar node with entity uid:" + std::string(XmlHelpers::convertEntityUIDToHexString(targetEntityUid)));
                    else
                        mRakNetConnection.mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, targetAvatarNode->getSystemAddress(), false);
                }
                break;
            }
        }

        CommonTools::System::sleep(100);
    }
}

//-------------------------------------------------------------------------------------
void RakNetServer::finalize()
{
    mRakNetConnection.logMessage("RakNetServer::finalize()");

    // Destroy nodes
    for (NodeMap::const_iterator it = mNodes.begin(); it != mNodes.end(); it = mNodes.begin())
    {
        RakNetNode *node = it->second;
        mRakNetConnection.logMessage("RakNetServer::finalize() Destroying node with nodeId " + node->getNodeId());
        delete node;
    }

    mRakNetConnection.mRakPeer->Shutdown(100, 0);
    RakNetworkFactory::DestroyRakPeerInterface(mRakNetConnection.mRakPeer);
}

//-------------------------------------------------------------------------------------
void RakNetServer::onNewEntity(Entity& entity)
{
    if (entity.getXmlEntity()->getType() == ETAvatar)
    {
        // Get the site node
        SiteNode* siteNode = (SiteNode*)mNodes[mSiteNodeId];
        // Reset avatar orientation and displacement
#ifdef POOL
        RefCntPoolPtr<XmlEntity> xmlEntity = entity.getXmlEntity();
#else
        XmlEntity* xmlEntity = entity.getXmlEntity();
#endif
        xmlEntity->setOrientation(Ogre::Quaternion::IDENTITY);
        xmlEntity->setDisplacement(Ogre::Vector3::ZERO);
        // Move avatar on the entry gate of the scene + gravity
        if (siteNode != 0)
        {
            RefCntPoolPtr<XmlSceneContent> xmlSceneContent = siteNode->getEntity()->getXmlEntity()->getContent()->getDatas();
            xmlEntity->setPosition(xmlSceneContent->getEntryGate().mPosition);
            if (xmlSceneContent->getEntryGate().mGravity)
                xmlEntity->setFlags(xmlEntity->getFlags() | EFGravity);
            else
                xmlEntity->setFlags(xmlEntity->getFlags() & ~EFGravity);
        }
    }
    else if (entity.getXmlEntity()->getType() == ETObject)
    {
        // Get the site node
        SiteNode* siteNode = (SiteNode*)mNodes[mSiteNodeId];
        // Add this object in the site
        siteNode->addPresentEntity(&entity);
    }
}

//-------------------------------------------------------------------------------------
void RakNetServer::onAvatarNodeIdInitialized(AvatarNode* avatarNode)
{
    mRakNetConnection.logMessage("RakNetServer::onAvatarNodeIdInitialized() avatarNodeId:" + avatarNode->getNodeId());

    NodeId avatarNodeId = avatarNode->getNodeId();
    // Load the avatar node
    bool avatarLoaded = loadNodeIdFile(avatarNodeId, avatarNode);
    if (!avatarLoaded)
    {
        // Create the avatar node
        EntityUID AvatarEntityUid;
        sscanf(avatarNodeId.c_str(), "%08X", &AvatarEntityUid);
        AvatarEntityUid <<= 16;
        // Randomize the character
        CommonTools::IO::FilenameVector filenames;
        CommonTools::IO::getFilenames(mMediaCachePath + "\\models", filenames);
        CommonTools::IO::FilenameVector safFilenames;
        for (CommonTools::IO::FilenameVector::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
            if (it->find(".saf") == it->length() - 4)
                safFilenames.push_back(*it);
        int safIdx = time(NULL)%(int)safFilenames.size();
        std::string xmlAvatarStr = "\
<entity uid=\"" + XmlHelpers::convertEntityUIDToHexString(AvatarEntityUid) + "\" owner=\"" + avatarNodeId + "\" type=\"0\" name=\"" + avatarNode->getName() + "\" version=\"00000000\">\
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
    <file name=\"" + safFilenames[safIdx] + "\" version=\"00000000\" />\
    <file name=\"" + XmlHelpers::convertEntityUIDToHexString(AvatarEntityUid) + ".sif\" version=\"00000000\" />\
   </files>\
  </lod>\
 </content>\
</entity>\
";
        TiXmlDocument xmlAvatarDoc;
        xmlAvatarDoc.Parse(xmlAvatarStr.c_str());
        Entity* avatarEntity = new Entity();
        avatarEntity->getXmlEntity()->fromXmlElt(xmlAvatarDoc.RootElement());
        onNewEntity(*avatarEntity);
        /// Server can serialize
        avatarEntity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // Entity managed by the Replica2 plugin
        avatarEntity->SetReplicaManager(&mRakNetConnection.mReplicaManager);
        // Send out this new entity to all systems
        avatarEntity->BroadcastConstruction();

        mRakNetConnection.logMessage("RakNetServer::onAvatarNodeIdInitialized() initializing simulation avatar node name:" + avatarEntity->getXmlEntity()->getName());
        avatarNode->setEntity(avatarEntity);
        mNodes[avatarNodeId] = avatarNode;
        avatarNode->BroadcastSerialize();
    }
}

//-------------------------------------------------------------------------------------
void RakNetServer::onAvatarNodeDestroyed(AvatarNode* avatarNode)
{
    mRakNetConnection.logMessage("RakNetServer::onAvatarNodeDestroyed() nodeId:" + avatarNode->getNodeId());

    // Save this avatar node
    if (!saveNodeIdFile(avatarNode->getNodeId()))
        mRakNetConnection.logMessage("RakNetServer::onAvatarNodeDestroyed() Unable to save node with nodeId:" + avatarNode->getNodeId() + " !");

    NodeMap::iterator it = mNodes.find(avatarNode->getNodeId());
    if (it != mNodes.end())
        mNodes.erase(it);
}

//-------------------------------------------------------------------------------------
void RakNetServer::onSiteNodeDestroyed(SiteNode* siteNode)
{
    mRakNetConnection.logMessage("RakNetServer::onSiteNodeDestroyed() nodeId:" + siteNode->getNodeId());

    // Save the site node
    if (!saveNodeIdFile(siteNode->getNodeId()))
        mRakNetConnection.logMessage("RakNetServer::onSiteNodeDestroyed() Unable to save node with nodeId:" + siteNode->getNodeId() + " !");

    NodeMap::iterator it = mNodes.find(siteNode->getNodeId());
    if (it != mNodes.end())
        mNodes.erase(it);
}

//-------------------------------------------------------------------------------------
void RakNetServer::onEntityDestroyed(Entity* entity)
{
    mRakNetConnection.logMessage("RakNetServer::onEntityDestroyed() entity uid:" + entity->getXmlEntity()->getUidString());

    if (entity->getXmlEntity()->getType() == ETObject)
    {
        // Get the site node
        SiteNode* siteNode = (SiteNode*)mNodes[mSiteNodeId];
        // Add this object in the site
        siteNode->removePresentEntity(entity);
    }
}

//-------------------------------------------------------------------------------------
AvatarNode* RakNetServer::getAvatarNodeOfEntity(const EntityUID& entityUID)
{
    for (NodeMap::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
        if (it->second->getType() == "avatar")
        {
            AvatarNode* avatarNode = (AvatarNode*)it->second;
            if (avatarNode->getEntity()->getXmlEntity()->getUid() == entityUID)
                return avatarNode;
        }

    return 0;
}

//-------------------------------------------------------------------------------------
Entity* RakNetServer::loadEntity(TiXmlElement* entityElt)
{
    Entity* entity = new Entity();
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity = entity->getXmlEntity();
#else
    XmlEntity* xmlEntity = entity->getXmlEntity();
#endif
    xmlEntity->fromXmlElt(entityElt);
    onNewEntity(*entity);
    /// Server can serialize
    entity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
    // Entity managed by the Replica2 plugin
    entity->SetReplicaManager(&mRakNetConnection.mReplicaManager);
    // Send out this new entity to all systems
    entity->BroadcastConstruction();

    return entity;
}
 
//-------------------------------------------------------------------------------------
bool RakNetServer::loadNodeIdFile(const NodeId& nodeId, RakNetNode* node)
{
    std::string nodeIdFilename = mMediaCachePath + "\\" + nodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());
    if (!xmlNodeIdFileDoc.LoadFile())
        return false;

    mRakNetConnection.logMessage("RakNetServer::loadNodeIdFile() loading node with nodeId:" + nodeId + " from " + nodeIdFilename);

    TiXmlElement* nodeElt = xmlNodeIdFileDoc.FirstChildElement("node");
    if (nodeElt == 0)
        return false;

    bool nodeAlreadyCreated = (node != 0);

    // Create the node according to the managed entity
    TiXmlElement* entityElt = nodeElt->FirstChildElement("entity");
    if (entityElt == 0)
        return false;
    // Load the entity
    Entity* entity = loadEntity(entityElt);
    if (entity == 0)
        return false;
    if (!nodeAlreadyCreated)
    {
        switch (entity->getXmlEntity()->getType())
        {
        case ETAvatar:
            node = new AvatarNode();
            break;
        case ETSite:
            node = new SiteNode();
            break;
        }
        if (node == 0)
            return false;
        node->setNodeId(nodeId);
    }
    mNodes[nodeId] = node;
    switch (entity->getXmlEntity()->getType())
    {
    case ETAvatar:
        {
            RakNetAvatarNode* avatarNode = (RakNetAvatarNode*)node;
            avatarNode->setEntity(entity);
            if (nodeAlreadyCreated)
                avatarNode->BroadcastSerialize();
            else
            {
                // Node managed by the Replica2 plugin
                avatarNode->SetReplicaManager(&mRakNetConnection.mReplicaManager);
                // Send out this new node to all systems
                avatarNode->BroadcastConstruction();
            }
            break;
        }
    case ETSite:
        {
            RakNetSiteNode* siteNode = (RakNetSiteNode*)node;
            siteNode->setEntity(entity);
            if (nodeAlreadyCreated)
                siteNode->BroadcastSerialize();
            else
            {
                // Node managed by the Replica2 plugin
                siteNode->SetReplicaManager(&mRakNetConnection.mReplicaManager);
                // Send out this new node to all systems
                siteNode->BroadcastConstruction();
            }
            break;
        }
    }

    return node->loadFromElt(nodeElt);
}

//-------------------------------------------------------------------------------------
bool RakNetServer::saveNodeIdFile(const NodeId& nodeId)
{
    std::string nodeIdFilename = mMediaCachePath + "\\" + nodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());

    mRakNetConnection.logMessage("RakNetServer::saveNodeIdFile() saving node with nodeId:" + nodeId + " into " + nodeIdFilename);

    RakNetNode* node = mNodes[nodeId];
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
void RakNetServer::RakNetConnectionLogger::logMessage(const std::string& message)
{
    char timeBuf[128];
    _strtime_s(timeBuf, 128);
    printf("%s: %s\n", timeBuf, message.c_str());
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
