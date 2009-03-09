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
#include "SiteNode.h"
#include "Entity.h"
#include <CTSystem.h>
#include <CTIO.h>
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Ogre.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

RakNetServer* RakNetServer::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
RakNetServer::RakNetServer(int argc, char** argv) :
    mMediaCachePath(""),
    mStatsPath("stats"),
    mRakNetConnection(&mConnectionFactory, true, "localhost", 8660, 32),
    mSiteNodeId("11112222"),
    mSiteNode(0),
    mRunning(false),
    mQuit(false)
{
    for (int iarg=1; iarg < argc; iarg++)
    {
        if ((strstr(argv[iarg], "-p") != 0) && (argc > iarg+1))
        {
            iarg++;
            mRakNetConnection.setPort(atoi(argv[iarg]));
            continue;
        }
        if ((strstr(argv[iarg], "-c") != 0) && (argc > iarg+1))
        {
            iarg++;
            mRakNetConnection.setMaxIncomingConnections(atoi(argv[iarg]));
            continue;
        }
        if ((strstr(argv[iarg], "-t") != 0) && (argc > iarg+1))
        {
            iarg++;
            mRakNetConnection.setTimeoutTime(atoi(argv[iarg]));
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
        if ((strstr(argv[iarg], "-S") != 0) && (argc > iarg+1))
        {
            iarg++;
            mStatsPath = argv[iarg];
            continue;
        }
    }

    // Retrieve Media/Cache path
    if (mMediaCachePath.empty())
        mMediaCachePath = IO::getCWD() + IO::getPathSeparator() + IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    ms_Singleton = this;

    mStatsManager.initialize(mStatsPath);
    mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_START, "");
}

//-------------------------------------------------------------------------------------
RakNetServer::~RakNetServer()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::~RakNetServer()");

    finalize();

    mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_END, "");
    mStatsManager.finalize();
}

//-------------------------------------------------------------------------------------
void RakNetServer::initialize()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::initialize()");

    // Initialize the server connection
    mRakNetConnection.initialize(mMediaCachePath);

    // Load the site node
    mSiteNode = new SiteNode();
    mSiteNode->setNodeId(mSiteNodeId);
    if (mSiteNode->loadNode(mMediaCachePath))
        LOGHANDLER_LOGF(LogHandler::VL_INFO, "RakNetServer::initialize() scene nodeId:%s loaded", mSiteNodeId.c_str());
    else
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "RakNetServer::initialize() unable to load scene nodeId:%s !", mSiteNodeId.c_str());
}

//-------------------------------------------------------------------------------------
void RakNetServer::run()
{
    RakPeerInterface *RakPeer = mRakNetConnection.getRakPeer();
    Packet *packet;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run()");

    mRunning = true;
    while (!mQuit)
    {
        // process packets
        for (packet = RakPeer->Receive(); packet; RakPeer->DeallocatePacket(packet), packet = RakPeer->Receive())
        {
            switch (packet->data[0])
            {
            case ID_CONNECTION_ATTEMPT_FAILED:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_ATTEMPT_FAILED from %s", packet->systemAddress.ToString());
                mQuit = true;
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_NO_FREE_INCOMING_CONNECTIONS from %s", packet->systemAddress.ToString());
                mQuit = true;
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_REQUEST_ACCEPTED from %s", packet->systemAddress.ToString());
                break;
            case ID_NEW_INCOMING_CONNECTION:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_NEW_INCOMING_CONNECTION from %s", packet->systemAddress.ToString());
                mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_CLIENT_CONNECTION, std::string(packet->systemAddress.ToString()));
                // Set notifications interval for big file transfer
                RakPeer->SetSplitMessageProgressInterval(RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES/RakPeer->GetMTUSize(packet->systemAddress));
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_DISCONNECTION_NOTIFICATION from %s", packet->systemAddress.ToString());
                mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_CLIENT_DISCONNECTION, std::string(packet->systemAddress.ToString()));
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetEntity::deleteByAddress(packet->systemAddress);
                break;
            case ID_CONNECTION_LOST:
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() ID_CONNECTION_LOST from %s", packet->systemAddress.ToString());
                mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_CLIENT_LOST, std::string(packet->systemAddress.ToString()));
                // Destruction broadcast done automatically in the destructor, from Replica2
                RakNetEntity::deleteByAddress(packet->systemAddress);
                break;
            case RakNetConnection::ID_REQUESTING_FILETRANSFER:
                {
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() RakNetConnection::ID_REQUESTING_FILETRANSFER from %s", packet->systemAddress.ToString());
                    mStatsManager.addEvent(StatsManager::SET_RELATIVE, StatsManager::SEI_SERVER_CLIENT_REQFILETRANSFER, std::string(packet->systemAddress.ToString()));
                    BitStream bitStream(packet->data, packet->length, false);
                    bitStream.IgnoreBytes(1);
                    unsigned short fileListTransferSetID;
                    bitStream.Read(fileListTransferSetID);
                    std::string filename;
                    RakNetConnection::DeserializeString(&bitStream, filename);
                    FileVersion version;
                    bitStream.Read(version);
                    mRakNetConnection.getCacheManager()->sendFile(packet->systemAddress, fileListTransferSetID, filename, version);
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
                    bool broadcast;
                    bitStream.Read(broadcast);
                    std::string desc;
                    RakNetConnection::DeserializeString(&bitStream, desc);
                    std::wstring wdesc = XmlHelpers::convertUTF8ToWString("WCHAR_T", desc);
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::run() RakNetConnection::ID_ACTION desc:%s", StringHelpers::convertWStringToString(wdesc).c_str());
                    if (broadcast || (sourceEntityUid == targetEntityUid))
                    {
                        // Broadcast
                        RakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
                    }
                    else
                    {
                        // Send to target node
                        RakNetEntity::RakNetEntityMap& entities = RakNetEntity::getEntities();
                        RakNetEntity* targetEntity = entities[targetEntityUid];
                        RakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, targetEntity->getSystemAddress(), false);
                    }
                }
                break;
            }
        }

        System::sleep(100);
    }

    mRunning = false;
}

//-------------------------------------------------------------------------------------
void RakNetServer::finalize()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::finalize()");

    // Quit the loop
    if (mRunning)
    {
        quit();
        while (mQuit)
            System::sleep(100);
    }

    // Save the site node
    if (mSiteNode != 0)
        if (!mSiteNode->saveNode(mMediaCachePath))
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetServer::finalize() Unable to save site node with nodeId:%s !", mSiteNode->getNodeId().c_str());

    // Destroy entities
    Entity::cleanUpEntities();

    // Destroy site node
    if (mSiteNode != 0)
    {
        delete mSiteNode;
        mSiteNode = 0;
    }

    // Finalizing the connection
    mRakNetConnection.finalize();
}

//-------------------------------------------------------------------------------------
SiteNode* RakNetServer::getSiteNode()
{
    return mSiteNode;
}

//-------------------------------------------------------------------------------------
Entity* RakNetServer::loadEntity(TiXmlElement* entityElt)
{
    Entity* entity = new Entity();
    entity->setSystemAddress(mRakNetConnection.getMySystemAddress());
#ifdef POOL
    RefCntPoolPtr<XmlEntity> xmlEntity = entity->getXmlEntity();
#else
    XmlEntity* xmlEntity = entity->getXmlEntity();
#endif
    xmlEntity->fromXmlElt(entityElt);
    entity->addFilesInCacheManager();
    Entity::addEntity(entity);
    // Server can serialize
    entity->addReplicaFlags(RakNetEntity::RFSerializationAuthorized);
    // Entity managed by the Replica2 plugin
    entity->SetReplicaManager(mRakNetConnection.getReplicaManager());
    // Send out this new entity to all systems
    entity->BroadcastConstruction();

    return entity;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
