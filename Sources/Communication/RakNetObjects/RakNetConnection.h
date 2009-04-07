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

#ifndef __RakNetConnection_h__
#define __RakNetConnection_h__

#include <string>
#include <RakPeerInterface.h>
#include <RakNetworkFactory.h>
#include <MessageIdentifiers.h>
#include <FileListTransfer.h>
#include <Ogre.h>

namespace Solipsis {

class ReplicationManager;
class ConnectionRMFactory;
class CacheManager;

/// Default value in bytes between each progress notification for big file transfer
#define RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES 1024*1024

/** This class manages 1 Solipsis RakNet connection.
*/
class RakNetConnection {
public:
    /// Application message identifiers
    enum MessageId {
        // Replication Manager class messages
        ID_RM_CONSTRUCTION = ID_USER_PACKET_ENUM,
        ID_RM_DESTRUCTION,
        ID_RM_SERIALIZE,

        // Cache manager class messages
        ID_CM_REQUESTING_FILETRANSFER,

        // Application messages
        ID_ACTION_ON_ENTITY
    };

protected:
    /// Server or client ?
    bool mServer;
    /// Host address
    std::string mHost;
    /// Port
    unsigned short mPort;
    /// Maximum incoming connections
    unsigned short mMaxIncomingConnections;
    /// Timeout in milliseconds before declaring 1 connection lost
    unsigned int mTimeoutTimeMS;
    /// RakNet peer interface
    RakPeerInterface *mRakPeer;
    /// Address of the RakNet server
    SystemAddress mServerSystemAddress;
    /// Socket descriptor
    SocketDescriptor mSocketDescriptor;
    /// FileListTransfer plugin
    FileListTransfer mFileListTransfer;
    /// Instance of the class that creates the object we use to represent connections
    ConnectionRMFactory *mConnectionRMFactory;
    /// Replication manager
    ReplicationManager *mReplicationManager;
    /// Cache manager
    CacheManager *mCacheManager;
    /// Client connected ?
    bool mClientConnected;

private:
    /// Singleton instance
    static RakNetConnection *ms_Singleton;

public:
    /** Constructor
    @param connectionRMFactory The Replication manager factory used to create instances
    @param server True if this connection is used as server, False with client connection
    @param host The server host
    @param port The server port
    @param maxIncomingConnections The maximum number of connections supported by this server
    */
    RakNetConnection(ConnectionRMFactory* connectionRMFactory, bool server = false, const std::string& host = "localhost", unsigned short port = 8660, unsigned short maxIncomingConnections = 32);
    /** Destructor */
    ~RakNetConnection();

    /// Get the singleton instance
    static RakNetConnection* getSingletonPtr() { return ms_Singleton; }

    /// Return true if it is a server connection
    bool isServer() { return mServer; }
    /// Get host address
    const std::string& getHost() { return mHost; }
    /// Set host address
    void setHost(const std::string& host) { mHost = host; }
    /// Get port
    unsigned short getPort() { return mPort; }
    /// Set port
    void setPort(unsigned short port) { mPort = port; }
    /// Get maximum incoming connections
    unsigned short getMaxIncomingConnections() { return mMaxIncomingConnections; }
    /// Set maximum incoming connections
    void setMaxIncomingConnections(unsigned short maxIncomingConnections) { mMaxIncomingConnections = maxIncomingConnections; }
    /// Get the timeout in milliseconds before declaring 1 connection lost
    unsigned int getTimeoutTime() { return mTimeoutTimeMS; }
    /// Set the timeout in milliseconds before declaring 1 connection lost
    void setTimeoutTime(unsigned int timeoutTimeMS) { mTimeoutTimeMS = timeoutTimeMS; }
    /// Get RakNet peer interface
    RakPeerInterface* getRakPeer() { return mRakPeer; }
    /// Get my address (external system)
    SystemAddress getMySystemAddress() { return mRakPeer->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS); }
    /// Set address of the RakNet server when connected as client
    void setServerSystemAddress(const SystemAddress& serverSystemAddress) { if (!mServer) mServerSystemAddress = serverSystemAddress; }
    /// Get address of the RakNet server
    const SystemAddress& getServerSystemAddress() { return mServerSystemAddress; }
    /// Get the Replication manager
    ReplicationManager* getReplicationManager() { return mReplicationManager; }
    /// Get FileListTransfer plugin
    FileListTransfer* getFileListTransfer() { return &mFileListTransfer; }
    /// Get the cache manager
    CacheManager* getCacheManager() { return mCacheManager; }

    /// Return true if connection is initialized
    bool isInitialized() { return (mRakPeer != 0); }

    /** Initialize the connection.
    @param cachePath The path used by the internal cache manager
    */
    virtual void initialize(const std::string& cachePath);
    /** Connect the client. */
    virtual bool connectClient();
    /** Disconnect the client. */
    virtual bool disconnectClient();
    /** Return true if client is connected. */
    bool isClientConnected() { return mClientConnected; }
    /** Finalize the connection. */
    virtual void finalize();

public:
    /// Helper method to serialize 1 string.
    static void SerializeString(RakNet::BitStream *bitStream, const std::string &str);
    /// Helper method to serialize 1 Vector3.
    static void SerializeVector3(RakNet::BitStream *bitStream, const Ogre::Vector3 &v);
    /// Helper method to serialize 1 Quaternion.
    static void SerializeQuaternion(RakNet::BitStream *bitStream, const Ogre::Quaternion &q);
    /// Helper method to deserialize 1 string.
    static void DeserializeString(RakNet::BitStream *bitStream, std::string &str);
    /// Helper method to deserialize 1 Vector3.
    static void DeserializeVector3(RakNet::BitStream *bitStream, Ogre::Vector3 &v);
    /// Helper method to deserialize 1 Quaternion.
    static void DeserializeQuaternion(RakNet::BitStream *bitStream, Ogre::Quaternion &q);
};

} // namespace Solipsis

#endif // #ifndef __RakNetConnection_h__