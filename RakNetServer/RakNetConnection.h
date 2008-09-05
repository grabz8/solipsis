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
#include <RakSleep.h>
#include <StringTable.h>
#include <MessageIdentifiers.h>
#include <NetworkIDManager.h>
#include <ReplicaManager2.h>
#include <FileListTransfer.h>

#include <Ogre.h>

#include <CTLog.h>

#include <CacheManager.h>
#include "RM2Connection.h"

namespace Solipsis {

/// Default value in bytes between each progress notification for big file transfer
#define RAKNETCONNECTION_DEFAULT_SPLITMSGPROGRESSINTERVAL_BYTES 1024*1024

/** This class manages 1 RakNet connection.
*/
class RakNetConnection {
public:
    /// Application message identifiers
    enum MessageId {
        ID_ACTION_ON_ENTITY = ID_USER_PACKET_ENUM,
        ID_REQUESTING_FILETRANSFER
    };

public:
    /// Server of client ?
    bool mServer;
    /// RakNet peer interface
    RakPeerInterface *mRakPeer;
    /// Address of the RakNet server
    SystemAddress mServerSystemAddress;
    /// Socket descriptor
    SocketDescriptor mSocketDescriptor;
    /// ReplicaManager2 requires NetworkIDManager to lookup pointers from numerical.
    NetworkIDManager mNetworkIdManager;
    /// ReplicaManager2 plugin
    RakNet::ReplicaManager2 mReplicaManager;
    /// Instance of the class that creates the object we use to represent connections
    RM2ConnectionFactory mConnectionFactory;
    /// FileListTransfer plugin
    FileListTransfer mFileListTransfer;
    /// Cache manager
    CacheManager mCacheManager;

private:
    /// Singleton instance
    static RakNetConnection *ms_Singleton;

public:
    RakNetConnection() :
        mRakPeer(0) { ms_Singleton = this; }
    ~RakNetConnection() { ms_Singleton = 0; }

    static RakNetConnection* getSingletonPtr() { return ms_Singleton; }

    static void SerializeString(RakNet::BitStream *bitStream, const std::string &str);
    static void SerializeVector3(RakNet::BitStream *bitStream, const Ogre::Vector3 &v);
    static void SerializeQuaternion(RakNet::BitStream *bitStream, const Ogre::Quaternion &q);
    static void DeserializeString(RakNet::BitStream *bitStream, std::string &str);
    static void DeserializeVector3(RakNet::BitStream *bitStream, Ogre::Vector3 &v);
    static void DeserializeQuaternion(RakNet::BitStream *bitStream, Ogre::Quaternion &q);
};

} // namespace Solipsis

#endif // #ifndef __RakNetConnection_h__