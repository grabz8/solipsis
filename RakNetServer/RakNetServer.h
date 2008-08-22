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

#ifndef __RakNetServer_h__
#define __RakNetServer_h__

#include <map>
#include <string>
#include <XmlDatas.h>
#include "RakNetConnection.h"
#include "RakNetNode.h"

namespace Solipsis {

class Entity;
class AvatarNode;
class SiteNode;

/** This class manages 1 site node through a RakNet server.
*/
class RakNetServer
{
public:
    /// <NodeId, Node*> map
    typedef std::map<NodeId, RakNetNode*> NodeMap;

protected:
    static RakNetServer* ms_Singleton;

protected:
    /// Server address
    std::string mHost;
    /// Server port
    int mPort;
    /// Maximum incoming connections
    int mMaxIncomingConnections;

    /// RakNetConnection
    RakNetConnection mRakNetConnection;

    /// Map of nodes
    NodeMap mNodes;
    /// Node identifier of the site node
    NodeId mSiteNodeId;

    /// Flag to quit
    bool mQuit;

    /// Scene loaded
    std::string mSceneDemoLoaded;
    /// Media cache path
    std::string mMediaCachePath;

public:
    /** Constructor. */
    RakNetServer(int argc, char** argv);

    static RakNetServer* getSingletonPtr() { return ms_Singleton; }
    static RakNetServer& getSingleton() { return *ms_Singleton; }

    /** Initialize the server. */
    void initialize();
    /** Run the loop. */
    void run();
    /** Call this to quit the loop. */
    void quit() { mQuit = true; }
    /** Finalize the server. */
    void finalize();

    /** Called when 1 new entity is created, avatar entities are resetted on the entry gate */
    void onNewEntity(Entity& entity);
    /** Called when 1 avatar node is initialized (nodeId defined) */
    void onAvatarNodeIdInitialized(AvatarNode* avatarNode);
    /** Called when 1 avatar node is destroyed */
    void onAvatarNodeDestroyed(AvatarNode* avatarNode);
    /** Called when 1 site node is destroyed */
    void onSiteNodeDestroyed(SiteNode* siteNode);
    /** Called when 1 entity is destroyed */
    void onEntityDestroyed(Entity* entity);

    /** Retrieve the avatar node according to its entity UID */
    AvatarNode* getAvatarNodeOfEntity(const EntityUID& entityUID);

    /** Load 1 entity */
    Entity* loadEntity(TiXmlElement* entityElt);
    /** Load entities of a nodeId */
    bool loadNodeIdFile(const NodeId& nodeId, RakNetNode* node = 0);
    /** Save/Update entities of a nodeId */
    bool saveNodeIdFile(const NodeId& nodeId);

protected:
    class RakNetConnectionLogger : public IRakNetConnectionLogger {
        /** See IRakNetConnectionLogger. */
        virtual void logMessage(const std::string& message);
    };
    RakNetConnectionLogger mRakNetConnectionLogger;
};

} // namespace Solipsis

#endif // #ifndef __RakNetServer_h__