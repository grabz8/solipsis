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

#ifndef __NodeManager_h__
#define __NodeManager_h__

#include "Ogre.h"
#include "IP2NClient.h"
#include "XmlDatas.h"
#include "Node.h"
#include "AvatarNode.h"
#include "SiteNode.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis nodes.
*/
class NodeManager
{
public:
    /// <NodeId, Node*> map
    typedef std::map<NodeId, Node*> NodeMap;
    /// <NodeId> set
    typedef std::set<NodeId> NodeIdSet;

private:
    /// Map of nodes
    NodeMap mNodes;
    /// Set of destroyed nodes Ids
    NodeIdSet mDestroyedNodeIds;

public:
	NodeManager();
	virtual ~NodeManager();

    /** Login. */
	AvatarNode* login(XmlLogin* xmlLogin);

    /** logout. */
    bool logout(const NodeId& nodeId);

    /** update. */
    bool update();

    /** Process an event. */
    bool processEvt(const NodeId& nodeId, XmlEvt& xmlEvt, std::string& xmlRespStr);
#ifdef POOL
    /** Get next event to handle. */
    RefCntPoolPtr<XmlEvt> getNextEvtToHandle(const NodeId& nodeId);
    /** Free event (handled event). */
    bool freeEvt(const NodeId& nodeId, RefCntPoolPtr<XmlEvt>& evt);
#else
    /** Get next event to handle. */
    XmlEvt* getNextEvtToHandle(const NodeId& nodeId);
    /** Free event (handled event). */
    bool freeEvt(const NodeId& nodeId, XmlEvt* evt);
#endif
};

} // namespace Solipsis

#endif // #ifndef __NodeManager_h__