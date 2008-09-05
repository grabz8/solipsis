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

#include <ReplicaManager2.h>
#include <XmlDatas.h>
#include <RakNetConnection.h>

namespace Solipsis {

class AvatarNode;

/** This class manages 1 avatar node.
*/
class RakNetEntity : public RakNet::Replica2, public CacheManagerCallback
{
public:
    /// Flags of replication
    typedef unsigned int ReplicaFlags;
    static const ReplicaFlags RFNone = (ReplicaFlags)0;
    static const ReplicaFlags RFSerializationAuthorized = (ReplicaFlags)1;

protected:
    /// Entity descriptor
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mXmlEntity;
#else
    XmlEntity* mXmlEntity;
#endif

    /// Replica flags
    ReplicaFlags mReplicaFlags;

    /// Last deserialized defined attributes (updated attr sent to server)
    XmlEntity::DefinedAttributes mLastDeserializedDefinedAttributes;

    /// List of missing files
    XmlLodContent::LodContentFileList mMissingFiles;

public:
    /** Constructor. */
    RakNetEntity();
    /** Destructor. */
    virtual ~RakNetEntity();

	/** Return the XML entity. */
#ifdef POOL
    RefCntPoolPtr<XmlEntity> getXmlEntity() { return mXmlEntity; }
#else
    XmlEntity* getXmlEntity() { return mXmlEntity; }
#endif
	/** Set the XML entity. */
#ifdef POOL
    void setXmlEntity(RefCntPoolPtr<XmlEntity>& xmlEntity) { mXmlEntity = xmlEntity; }
#else
    void setXmlEntity(XmlEntity* xmlEntity) { mXmlEntity = xmlEntity; }
#endif

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

	/** See Replica2::Serialize. */
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, RakNet::SerializationContext *serializationContext);
	/** See Replica2::Serialize. */
	virtual bool Serialize(RakNet::BitStream *bitStream, RakNet::SerializationContext *serializationContext);
	/** See Replica2::Deserialize. */
	virtual void Deserialize(RakNet::BitStream *bitStream, RakNet::SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/** See Replica2::QueryIsDestructionAuthority. */
	virtual bool QueryIsDestructionAuthority(void) const;
	/** See Replica2::QueryIsSerializationAuthority. */
	virtual bool QueryIsSerializationAuthority(void) const;

    /** Add files in the cache manager. */
    void addFilesInCacheManager();
    /** Request files (of a sender system) from the cache manager. */
    void requestFilesFromCacheManager(const SystemAddress& sender);

    /** See CacheManagerCallback::onTransferComplete. */
    virtual void onTransferComplete(const std::string& filename);
};

} // namespace Solipsis

#endif // #ifndef __RakNetEntity_h__