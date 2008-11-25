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

#ifndef __Node_h__
#define __Node_h__

#include <XmlDatas.h>

namespace Solipsis {

/** This class defines 1 node.
*/
class Node
{
protected:
    /// Node unique identifier
    NodeId mNodeId;
    /// Type
    std::string mType;

public:
    /** Constructor. */
    Node(const std::string& type);
    /** Destructor. */
    virtual ~Node();

    /// Get the identifier
    const NodeId& getNodeId() { return mNodeId; }
    /// Set the identifier
    void setNodeId(const NodeId& nodeId) { mNodeId = nodeId; }

    /// Get the type
    const std::string& getType() { return mType; }

    /** Load from node XML element
    @param nodeElt The XML node element
    */
    virtual bool loadFromElt(TiXmlElement* nodeElt);
    /** Get the saved node XML element
    @returns The XML node element
    */
    virtual TiXmlElement* getSavedElt();

    /** Load the node
    @param mediaCachePath The media cache path
    */
    bool loadNode(const std::string& mediaCachePath);
    /** Save/Update the node
    @param mediaCachePath The media cache path
    */
    bool saveNode(const std::string& mediaCachePath);
};

} // namespace Solipsis

#endif // #ifndef __Node_h__
