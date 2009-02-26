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
RakNetConnection::RakNetConnection(Connection_RM2Factory* connectionFactory, bool server, const std::string& host, unsigned short port, unsigned short maxIncomingConnections) :
    mServer(server),
    mHost(host),
    mPort(port),
    mMaxIncomingConnections(maxIncomingConnections),
    mRakPeer(0),
    mServerSystemAddress(UNASSIGNED_SYSTEM_ADDRESS),
    mConnectionFactory(connectionFactory),
    mCacheManager(0)
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
    // ObjectMemberRPC and ReplicaManager2 require that you call SetNetworkIDManager()
    mRakPeer->SetNetworkIDManager(&mNetworkIdManager);
    // The network ID authority is the system that creates the common numerical identifier used to lookup pointers.
    // For client/server this is the server
    // For peer to peer this would be true on every system, and you would also have call NetworkID::peerToPeerMode=true;
    mNetworkIdManager.SetIsNetworkIDAuthority(mServer);
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

    // Attach the ReplicaManager2 plugin
    mRakPeer->AttachPlugin(&mReplicaManager);
    // Register our custom connection factory
    mReplicaManager.SetConnectionFactory(mConnectionFactory);
    // Attach the FileListTransfer plugin
    mRakPeer->AttachPlugin(&mFileListTransfer);

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is used in in Replica2::SerializeConstruct
	// The decoding is used in in Connection_RM2::Construct
	// The stringTable class will also send strings that weren't registered but this just falls back to the stringCompressor and wastes 1 extra bit on top of that
	// 2nd parameter of false means a static string so it's not necessary to copy it
	StringTable::Instance()->AddString("Entity", false);

    // Initializing the cache
    mCacheManager = new CacheManager(this);
    mCacheManager->initialize(cachePath);
}

//-------------------------------------------------------------------------------------
bool RakNetConnection::connectClient()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "RakNetConnection::connect()");

    if (mRakPeer == 0)
        return false;
    return mRakPeer->Connect(mHost.c_str(), mPort, 0, 0, 0);
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

    // Finalizing the cache
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
