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

#include <string>
#include <CTStatsManager.h>
#include <XmlDatas.h>
#include <RakNetConnection.h>
#include "RM2Connection.h"
#include "SiteNode.h"

namespace Solipsis {

/** This class manages 1 site node through a RakNet server.
*/
class RakNetServer
{
private:
    static RakNetServer* ms_Singleton;

protected:
    /// Node identifier of the site node
    NodeId mSiteNodeId;
    /// Media cache path
    std::string mMediaCachePath;
    /// Statistics path
    std::string mStatsPath;
    /// Squared-distance between avatars for visiblity scoping
    float mAvatarScopeDistance2;

    /// Instance of the class that creates the object we use to represent connections
    RM2ConnectionFactory mConnectionFactory;
    /// RakNetConnection
    RakNetConnection mRakNetConnection;

    /// Site node
    SiteNode *mSiteNode;

    /// Is running
    bool mRunning;
    /// Flag to quit
    bool mQuit;

    /// Statistics manager
    CommonTools::StatsManager mStatsManager;

public:
    /** Constructor. */
    RakNetServer(int argc, char** argv);
    ~RakNetServer();

    static RakNetServer* getSingletonPtr() { return ms_Singleton; }
    static RakNetServer& getSingleton() { return *ms_Singleton; }

    /** Initialize the server. */
    void initialize();
    /** Run the loop. */
    void run();
    /** Return true if running. */
    bool isRunning() { return mRunning; }
    /** Call this to quit the loop. */
    void quit() { mQuit = true; }
    /** Finalize the server. */
    void finalize();

    /// Get the Media/cache path
    const std::string& getMediaCachePath() { return mMediaCachePath; }
    /// Get the squared-distance between avatars for visiblity scoping
    float getAvatarScopeDistance2() { return mAvatarScopeDistance2; }
    /// Get the site node
    SiteNode* getSiteNode();

    /** Load 1 entity */
    Entity* loadEntity(TiXmlElement* entityElt);
    /** Load entities of a node */
    bool loadNode(Node* node);
    /** Save/Update entities of a node */
    bool saveNode(Node* node);
};

} // namespace Solipsis

#endif // #ifndef __RakNetServer_h__