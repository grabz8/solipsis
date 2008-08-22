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

#ifndef __RakNetAvatarNode_h__
#define __RakNetAvatarNode_h__

#include <list>
#include <string>
#include "RakNetNode.h"

namespace Solipsis {

class RakNetEntity;

/** This class manages 1 avatar node.
*/
class RakNetAvatarNode : public RakNetNode
{
public:
    typedef std::list<RakNetAvatarNode*> AvatarNodeList;

protected:
    /// Entity
    RakNetEntity *mEntity;

    /// Name
    std::string mName;

    /// Local avatar node (set by server)
    bool mIsLocal;

    /// Set ID (FileListTransfer) to received files from the owner of this avatar node (set by client)
    unsigned short mFileListTransferSetID;

	// All avatar nodes that have been created
	static AvatarNodeList avatarNodes;

public:
    /** Constructor. */
    RakNetAvatarNode();
    /** Destructor. */
    virtual ~RakNetAvatarNode();

    /** Get entity. */
    RakNetEntity* getEntity() { return mEntity; }
    /** Set entity. */
    void setEntity(RakNetEntity* entity) { mEntity = entity; }

    /** Get the name. */
    const std::string& getName() { return mName; }

	/** Helper function to find an avatar node by address. */
	static RakNetAvatarNode* findByAddress(SystemAddress& systemAddress);
	/** Helper function to free memory when someone disconnections. */
	static void deleteByAddress(SystemAddress& systemAddress);

	/** See Replica2::Serialize. */
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, RakNet::SerializationContext *serializationContext);
	/** See Replica2::Serialize. */
	virtual bool Serialize(RakNet::BitStream *bitStream, RakNet::SerializationContext *serializationContext);
	/** See Replica2::Deserialize. */
	virtual void Deserialize(RakNet::BitStream *bitStream, RakNet::SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);
};

} // namespace Solipsis

#endif // #ifndef __RakNetAvatarNode_h__