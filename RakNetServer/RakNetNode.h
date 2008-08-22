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

#ifndef __RakNetNode_h__
#define __RakNetNode_h__

#include <ReplicaManager2.h>
#include <XmlDatas.h>

namespace Solipsis {

/** This class manages 1 node.
*/
class RakNetNode : public RakNet::Replica2
{
protected:
    /// Node unique identifier
    NodeId mNodeId;
    /// Type
    std::string mType;
    /// System address of this node
    SystemAddress mSystemAddress;

public:
    /** Constructor. */
    RakNetNode(const std::string& type);
    /** Destructor. */
    virtual ~RakNetNode();

    /** Get the identifier. */
    const NodeId& getNodeId() { return mNodeId; }
    /** Set the identifier. */
    void setNodeId(const NodeId& nodeId) { mNodeId = nodeId; }

    /** Get the type. */
    const std::string& getType() { return mType; }

    /** Get system address. */
    SystemAddress& getSystemAddress() { return mSystemAddress; }
    /** Set system address. */
    void setSystemAddress(SystemAddress& systemAddress) { mSystemAddress = systemAddress; }

    /** Load from node XML element. */
    virtual bool loadFromElt(TiXmlElement* nodeElt);
    /** Get the saved node XML element. */
    virtual TiXmlElement* getSavedElt();
};

} // namespace Solipsis

#endif // #ifndef __RakNetNode_h__