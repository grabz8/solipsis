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

#include "RakNetConnection.h"
#include "ReplicationManager.h"
#include "CacheManager.h"
#include <StringTable.h>
#include <StringCompressor.h>
#include <CTLog.h>
#include <CTSystem.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

RakNetConnection *RakNetConnection::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
RakNetConnection::RakNetConnection(ConnectionRMFactory* connectionRMFactory, bool server, const std::string& host, unsigned short port, unsigned short maxIncomingConnections) :
    mServer(server),
    mHost(host),
    mPort(port),
    mMaxIncomingConnections(maxIncomingConnections),
    mTimeoutTimeMS(15*1000),
    mRakPeer(0),
    mServerSystemAddress(UNASSIGNED_SYSTEM_ADDRESS),
    mConnectionRMFactory(connectionRMFactory),
    mCacheManager(0),
    mClientConnected(false)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::RakNetConnection()");

    ms_Singleton = this;
}

//-------------------------------------------------------------------------------------
RakNetConnection::~RakNetConnection()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::~RakNetConnection()");

    finalize();

    ms_Singleton = 0;
}

//-------------------------------------------------------------------------------------
void RakNetConnection::initialize(const std::string& cachePath)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::initialize()");

    // Get 1 instance of the RakNet peer interface
    mRakPeer = RakNetworkFactory::GetRakPeerInterface();
    // Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable packet
    // packet is either the internal ping or any user reliable packet
    mRakPeer->SetTimeoutTime(mTimeoutTimeMS, UNASSIGNED_SYSTEM_ADDRESS);
    // Start RakNet
    if (mServer)
    {
        // Start RakNet, up to 32 connections
        mSocketDescriptor.port = mPort;
        mRakPeer->Startup(mMaxIncomingConnections, 100, &mSocketDescriptor, 1);
        mServerSystemAddress = mRakPeer->GetInternalID();
        LOGHANDLER_LOGF(LogHandler::VL_INFO, "RakNetConnection::initialize() Server started on %s", mServerSystemAddress.ToString());
        // The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
        mRakPeer->SetMaximumIncomingConnections(mMaxIncomingConnections);
    }
    else
    {
        mSocketDescriptor.port = 0;
        mRakPeer->Startup(1, 100, &mSocketDescriptor, 1);
        // server system address must be set by client when connection request is accepted
    }

    // Attach the FileListTransfer plugin
    mRakPeer->AttachPlugin(&mFileListTransfer);

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is used in in Replica2::SerializeConstruct
	// The decoding is used in in Connection_RM2::Construct
	// The stringTable class will also send strings that weren't registered but this just falls back to the stringCompressor and wastes 1 extra bit on top of that
	// 2nd parameter of false means a static string so it's not necessary to copy it
	StringTable::Instance()->AddString("Entity", false);

    // Initializing the replication manager
    mReplicationManager = new ReplicationManager(this);
    // Register our custom connection factory
    mReplicationManager->setConnectionFactory(mConnectionRMFactory);
    // Set server as authority
    if (mServer)
        mReplicationManager->setAuthority(mServerSystemAddress);
    // Set the query construction/destruction interval time
//    mReplicationManager->setIntervalQueryConstructionMs(1*1000.0f);

    // Initializing the cache manager
    mCacheManager = new CacheManager(this);
    mCacheManager->initialize(cachePath);
}

//-------------------------------------------------------------------------------------
bool RakNetConnection::connectClient()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::connect()");

    if (mRakPeer == 0)
        return false;
    mClientConnected = mRakPeer->Connect(mHost.c_str(), mPort, 0, 0, 0);
    return mClientConnected;
}

//-------------------------------------------------------------------------------------
bool RakNetConnection::disconnectClient()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::disconnectClient()");

    if (mRakPeer == 0)
        return false;

    if (mReplicationManager != 0)
        mReplicationManager->onCloseConnection(mServerSystemAddress);

    mRakPeer->CloseConnection(mServerSystemAddress, true);
    mClientConnected = false;

    return true;
}

//-------------------------------------------------------------------------------------
void RakNetConnection::finalize()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::finalize()");

    if (mRakPeer != 0)
    {
        mRakPeer->Shutdown(100, 0);
        RakNetworkFactory::DestroyRakPeerInterface(mRakPeer);
        mRakPeer = 0;
    }

    // Finalizing the replication manager
    if (mReplicationManager != 0)
    {
        mReplicationManager->onShutdown();
        delete mReplicationManager;
        mReplicationManager = 0;
    }

    // Finalizing the cache manager
    if (mCacheManager != 0)
    {
        mCacheManager->finalize();
        delete mCacheManager;
        mCacheManager = 0;
    }
}

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeString(BitStream *bitStream, const std::string &str)
{
    stringCompressor->EncodeString(str.c_str(), 256, bitStream);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeVector3(BitStream *bitStream, const Ogre::Vector3 &v)
{
    bitStream->Write(v.x);
    bitStream->Write(v.y);
    bitStream->Write(v.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::SerializeQuaternion(BitStream *bitStream, const Ogre::Quaternion &q)
{
    bitStream->Write(q.w);
    bitStream->Write(q.x);
    bitStream->Write(q.y);
    bitStream->Write(q.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeString(BitStream *bitStream, std::string &str)
{
    char output[256];
    stringCompressor->DecodeString(output, 256, bitStream);
    str = output;
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeVector3(BitStream *bitStream, Ogre::Vector3 &v)
{
    bitStream->Read(v.x);
    bitStream->Read(v.y);
    bitStream->Read(v.z);
}

//-------------------------------------------------------------------------------------
void RakNetConnection::DeserializeQuaternion(BitStream *bitStream, Ogre::Quaternion &q)
{
    bitStream->Read(q.w);
    bitStream->Read(q.x);
    bitStream->Read(q.y);
    bitStream->Read(q.z);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
