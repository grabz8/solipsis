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

#ifndef __RakNetEntity_h__
#define __RakNetEntity_h__

#include "ReplicationManager.h"
#include <XmlEntity.h>
#include "CacheManager.h"

namespace Solipsis {

class AvatarNode;

/** This class manages 1 RakNet entity.
*/
class RakNetEntity : public Replica, public CacheManagerCallback
{
public:
    /// Flags of replication
    typedef unsigned char ReplicaFlags;
    static const ReplicaFlags RFNone = 0;
    static const ReplicaFlags RFReady = 1;
    static const ReplicaFlags RFConstructionAuthorized = RFReady<<1;
    static const ReplicaFlags RFSerializationAuthorized = RFConstructionAuthorized<<1;

    /// <EntityUID, RakNetEntity*> map
    typedef std::map<EntityUID, RakNetEntity*> RakNetEntityMap;

protected:
    /// Entity descriptor
    RefCntPoolPtr<XmlEntity> mXmlEntity;

    /// Map of entities
    static RakNetEntityMap ms_Entities;

    /// System address of this node
    SystemAddress mSystemAddress;

    /// Replica flags
    ReplicaFlags mReplicaFlags;

    /// Last deserialized defined attributes (updated attr sent to server)
    XmlEntity::DefinedAttributes mLastDeserializedDefinedAttributes;

    /// Total number of files
    int mTotalNbfiles;

    /// List of missing files
    XmlLodContent::LodContentFileList mMissingFiles;
    /// List of uploading files
    XmlLodContent::LodContentFileList mUploadingFiles;

public:
    /** Constructor. */
    RakNetEntity();
    /** Destructor. */
    virtual ~RakNetEntity();

	/** Return the XML entity. */
    RefCntPoolPtr<XmlEntity> getXmlEntity() { return mXmlEntity; }
	/** Set the XML entity. */
    void setXmlEntity(RefCntPoolPtr<XmlEntity>& xmlEntity) { mXmlEntity = xmlEntity; }

    /** Add 1 new entity. */
    static void addEntity(RakNetEntity* entity);
    /** Remove 1 entity. */
    static void removeEntity(RakNetEntity* entity);
    /** Clean up entities. */
    static void cleanUpEntities();
    /** Helper function to get entities. */
    static RakNetEntityMap& getEntities() { return ms_Entities; }

	/** Helper function to find an entity by address and entity type. */
    static RakNetEntity* findByAddressAndType(SystemAddress& systemAddress, EntityType type);
	/** Helper function to delete an entity by address and entity type (free on disconnections). */
	static void deleteByAddressAndType(SystemAddress& systemAddress, EntityType type);
	/** Helper function to delete all entities with content not fully downloaded. */
    static void deleteEntitiesIfContentDownloadAborted();

    /** Method called when 1 entity is created. */
    virtual void onNewEntity() {}
    /** Method called when 1 entity is destroyed. */
    virtual void onLostEntity() {}

    /** Get system address. */
    SystemAddress& getSystemAddress() { return mSystemAddress; }
    /** Set system address. */
    void setSystemAddress(SystemAddress& systemAddress) { mSystemAddress = systemAddress; }

	/** Get replica flags. */
    ReplicaFlags getReplicaFlags() { return mReplicaFlags; }
	/** Add replica flags. */
    void addReplicaFlags(ReplicaFlags flags) { mReplicaFlags |= flags; }

	/** Get last deserialized defined attributes. */
    XmlEntity::DefinedAttributes getLastDeserializedDefinedAttributes() { return mLastDeserializedDefinedAttributes; }
	/** Set last deserialized defined attributes. */
    void setLastDeserializedDefinedAttributes(XmlEntity::DefinedAttributes definedAttributes) { mLastDeserializedDefinedAttributes = definedAttributes; }
	/** Add last deserialized defined attributes. */
    void addLastDeserializedDefinedAttributes(XmlEntity::DefinedAttributes definedAttributes) { mLastDeserializedDefinedAttributes |= definedAttributes; }

    /** See Replica. */
    virtual const ReplicaUid& getUid() const;
    /** See Replica. */
    virtual void setUid(const ReplicaUid& uid);
    /** See Replica. */
    virtual bool serializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
    /** See Replica. */
    virtual bool serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
    /** See Replica. */
    virtual void deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender);
    /** See Replica. */
    virtual bool queryIsReady() const;
    /** See Replica. */
    virtual bool queryIsConstructionAuthority() const;
    /** See Replica. */
    virtual bool queryIsSerializationAuthority() const;

    /** Add files in the cache manager. */
    void addFilesInCacheManager();
    /** Request files (of a sender system) from the cache manager
    @param sender The address of the system having the file to request to
    */
    void requestFilesFromCacheManager(const SystemAddress& sender);
    /** Cancel files (if they are currently downloaded/uploaded) from the cache manager
    @param removeFiles True if cancelled files must be removed from cache list and deleted
    */
    void cancelFilesFromCacheManager(bool removeFiles = false);
    /** Get missing files list
    @return List of missing files
    */
    const XmlLodContent::LodContentFileList& getMissingFiles() { return mMissingFiles; }

    /** See CacheManagerCallback. */
    virtual float onDownloadProgress(const std::string& filename, float progress);
    /** See CacheManagerCallback. */
    virtual float onUploadProgress(const std::string& filename, float progress);
};

} // namespace Solipsis

#endif // #ifndef __RakNetEntity_h__