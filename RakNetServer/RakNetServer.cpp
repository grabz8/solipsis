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
#include <CTLog.h>
#include <Ogre.h>

using namespace CommonTools;
using namespace RakNet;

namespace Solipsis {

RakNetServer* RakNetServer::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
RakNetServer::RakNetServer(int argc, char** argv) :
    mPort(8660),
    mMaxIncomingConnections(32),
    mMediaCachePath(""),
    mSiteNodeId("11112222"),
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
            mSiteNodeId = argv[iarg];
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
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::initialize()");

    // We are the server
    mRakNetConnection.mServer = true;
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
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "RakNetServer::initialize() Server started on %s", mRakNetConnection.mServerSystemAddress.ToString());
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

    // Initializing the cache
    mRakNetConnection.mCacheManager.initialize(mMediaCachePath);

    // Load the site node
    if (loadNodeIdFile(mSiteNodeId))
        LOGHANDLER_LOGF(LogHandler::VL_INFO, "RakNetServer::initialize() scene nodeId:%s loaded", mSiteNodeId.c_str());
    else
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "RakNetServer::initialize() unable to load scene nodeId:%s !", mSiteNodeId.c_str());
}

//-------------------------------------------------------------------------------------
void RakNetServer::run()
{
    Packet *packet;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run()");

    while (!mQuit)
    {
        // process packets
        for (packet = mRakNetConnection.mRakPeer->Receive(); packet; mRakNetConnection.mRakPeer->DeallocatePacket(packet), packet = mRakNetConnection.mRakPeer->Receive())
        {
            switch (packet->data[0])
            {
            case ID_CONNECTION_ATTEMPT_FAILED:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_ATTEMPT_FAILED");
                mQuit = true;
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_NO_FREE_INCOMING_CONNECTIONS");
                mQuit = true;
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_REQUEST_ACCEPTED");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                {
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_NEW_INCOMING_CONNECTION from %s", packet->systemAddress.ToString());
                    // Set notifications interval for big file transfer
                    mRakNetConnection.mRakPeer->SetSplitMessageProgressInterval(RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES/mRakNetConnection.mRakPeer->GetMTUSize(packet->systemAddress));
                    AvatarNode* avatarNode = new AvatarNode();
                    avatarNode->setSystemAddress(packet->systemAddress);
                    // Node managed by the Replica2 plugin
                    avatarNode->SetReplicaManager(&mRakNetConnection.mReplicaManager);
                    // Send out this new node to all systems
                    avatarNode->BroadcastConstruction();
                }
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_DISCONNECTION_NOTIFICATION");
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetAvatarNode::deleteByAddress(packet->systemAddress);
                break;
            case ID_CONNECTION_LOST:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_LOST");
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetAvatarNode::deleteByAddress(packet->systemAddress);
                break;
            case RakNetConnection::ID_REQUESTING_FILETRANSFER:
                {
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() RakNetConnection::ID_REQUESTING_FILETRANSFER from %s", packet->systemAddress.ToString());
                    BitStream bitStream(packet->data, packet->length, false);
                    bitStream.IgnoreBytes(1);
                    unsigned short fileListTransferSetID;
                    bitStream.Read(fileListTransferSetID);
                    std::string filename;
                    RakNetConnection::DeserializeString(&bitStream, filename);
                    FileVersion version;
                    bitStream.Read(version);
                    mRakNetConnection.mCacheManager.sendFile(packet->systemAddress, fileListTransferSetID, filename, version);
                }
                break;
            case RakNetConnection::ID_ACTION_ON_ENTITY:
                {
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() RakNetConnection::ID_ACTION from %s", packet->systemAddress.ToString());
                    BitStream bitStream(packet->data, packet->length, false);
                    bitStream.IgnoreBytes(1);
                    ActionType actionType;
                    bitStream.Read(actionType);
                    EntityUID sourceEntityUid;
                    RakNetConnection::DeserializeString(&bitStream, sourceEntityUid);
                    EntityUID targetEntityUid;
                    RakNetConnection::DeserializeString(&bitStream, targetEntityUid);
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
                            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() RakNetConnection::ID_ACTION unable to find avatar node with entity uid:%s", targetEntityUid.c_str());
                        else
                            mRakNetConnection.mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, targetAvatarNode->getSystemAddress(), false);
                    }
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
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::finalize()");

    // Destroy nodes
    for (NodeMap::const_iterator it = mNodes.begin(); it != mNodes.end(); it = mNodes.begin())
    {
        RakNetNode *node = it->second;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::finalize() Destroying node with nodeId %s", node->getNodeId().c_str());
        delete node;
    }

    if (mRakNetConnection.mRakPeer != 0)
    {
        mRakNetConnection.mRakPeer->Shutdown(100, 0);
        RakNetworkFactory::DestroyRakPeerInterface(mRakNetConnection.mRakPeer);
        mRakNetConnection.mRakPeer = 0;
    }

    // Finalizing the cache
    mRakNetConnection.mCacheManager.finalize();
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
            // Randomize position
            int rand = time(NULL)%9;
            xmlEntity->setPosition(xmlSceneContent->getEntryGate().mPosition + Ogre::Vector3(rand/3 - 1, 0, rand%3 - 1));
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
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onAvatarNodeIdInitialized() avatarNodeId:%s", avatarNode->getNodeId().c_str());

    NodeId avatarNodeId = avatarNode->getNodeId();
    // Load the avatar node
    bool avatarLoaded = loadNodeIdFile(avatarNodeId, avatarNode);
    if (!avatarLoaded)
    {
        // Create the avatar node
        EntityUID AvatarEntityUid;
        AvatarEntityUid = avatarNodeId + "_00000000";
        // Randomize the character
        CommonTools::IO::FilenameVector filenames;
        CommonTools::IO::getFilenames(mMediaCachePath, filenames);
        CommonTools::IO::FilenameVector safFilenames;
        for (CommonTools::IO::FilenameVector::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
            if (it->find(".saf") == it->length() - 4)
                safFilenames.push_back(*it);
        int safIdx = time(NULL)%(int)safFilenames.size();
        std::string xmlAvatarStr = "\
<entity uid=\"" + AvatarEntityUid + "\" owner=\"" + avatarNodeId + "\" type=\"0\" name=\"" + avatarNode->getName() + "\" version=\"00000000\">\
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
   </files>\
  </lod>\
 </content>\
</entity>\
";
        TiXmlDocument xmlAvatarDoc;
        xmlAvatarDoc.Parse(xmlAvatarStr.c_str());
        Entity* avatarEntity = new Entity();
        avatarEntity->getXmlEntity()->fromXmlElt(xmlAvatarDoc.RootElement());
        avatarEntity->addFilesInCacheManager();
        onNewEntity(*avatarEntity);
        /// Server can serialize
        avatarEntity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
        // Entity managed by the Replica2 plugin
        avatarEntity->SetReplicaManager(&mRakNetConnection.mReplicaManager);
        // Send out this new entity to all systems
        avatarEntity->BroadcastConstruction();

        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onAvatarNodeIdInitialized() initializing simulation avatar node name:%s", avatarEntity->getXmlEntity()->getName().c_str());
        avatarNode->setEntity(avatarEntity);
        mNodes[avatarNodeId] = avatarNode;
        avatarNode->BroadcastSerialize();
    }
}

//-------------------------------------------------------------------------------------
void RakNetServer::onAvatarNodeDestroyed(AvatarNode* avatarNode)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onAvatarNodeDestroyed() nodeId:%s", avatarNode->getNodeId().c_str());

    // Save this avatar node
    if (!saveNodeIdFile(avatarNode->getNodeId()))
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onAvatarNodeDestroyed() Unable to save node with nodeId:%s !", avatarNode->getNodeId().c_str());

    NodeMap::iterator it = mNodes.find(avatarNode->getNodeId());
    if (it != mNodes.end())
        mNodes.erase(it);
}

//-------------------------------------------------------------------------------------
void RakNetServer::onSiteNodeDestroyed(SiteNode* siteNode)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onSiteNodeDestroyed() nodeId:%s", siteNode->getNodeId().c_str());

    // Save the site node
    if (!saveNodeIdFile(siteNode->getNodeId()))
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onSiteNodeDestroyed() Unable to save node with nodeId:%s !", siteNode->getNodeId().c_str());

    NodeMap::iterator it = mNodes.find(siteNode->getNodeId());
    if (it != mNodes.end())
        mNodes.erase(it);
}

//-------------------------------------------------------------------------------------
void RakNetServer::onEntityDestroyed(Entity* entity)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::onEntityDestroyed() entity uid:%s", entity->getXmlEntity()->getUid().c_str());

    if (entity->getXmlEntity()->getType() == ETObject)
    {
        // Get the site node
        SiteNode* siteNode = (SiteNode*)mNodes[mSiteNodeId];
        // Remove this object from the site
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
SiteNode* RakNetServer::getSiteNode()
{
    // Get the site node
    return (SiteNode*)mNodes[mSiteNodeId];
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
    entity->addFilesInCacheManager();
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

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::loadNodeIdFile() loading node with nodeId:%s from %s", nodeId.c_str(), nodeIdFilename.c_str());

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

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::saveNodeIdFile() saving node with nodeId:%s into %s", nodeId.c_str(), nodeIdFilename.c_str());

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

} // namespace Solipsis
