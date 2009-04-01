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

#ifndef __OgrePeerManager_h__
#define __OgrePeerManager_h__

#include <map>
#include <list>
#include <tinyxml.h>
#include <Ogre.h>

#include "XmlDatas.h"
#include "World/Modeler.h"
#include "IOgrePeerManagerCallbacks.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis/Ogre peers.
*/
class OgrePeerManager : public FrameListener, public IModelerCallbacks
{
public:
    typedef std::map<EntityUID, XmlEntity*> XmlEntitiesMap;
    typedef std::map<EntityUID, OgrePeer*> OgrePeersMap;

    typedef std::list<RefCntPoolPtr<XmlEvt>> EvtsList;


private:
    // My node identifier
    NodeId mNodeId;

    // <EntityUID, OgrePeer> map
    OgrePeersMap mOgrePeersMap;

    // <EntityUID, OgrePeer> map of reserved EntityUID
    OgrePeersMap mReservedOgrePeersMap;

    // User avatar
    OgrePeer* mUserAvatar;

    // List of events to send
    EvtsList mEvtsList;

    // Scene manager
    SceneManager* mSceneMgr;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

public:
    OgrePeerManager(SceneManager* sceneMgr = 0, IOgrePeerManagerCallbacks* callbacks = 0);
    ~OgrePeerManager();

    // Get my node identifier
    const NodeId& getNodeId();
    // Set my node identifier
    void setNodeId(const NodeId& nodeId);

    // Load
    bool load(RefCntPoolPtr<XmlEntity>& xmlEntity);

    // Get the scene object filename
    const String& getXmlObjectFilename();

    // Remove 1 peer according to its ID
    bool remove(const EntityUID& entity);

    // Remove all peers (locals or networked)
    void removeAll(bool local);
    // Clean up
    void cleanUp();

	// Update
    bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity);


	// Action
    bool action(RefCntPoolPtr<XmlAction>& xmlAction);


    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

	/** Get 1 new entity uid. */
    EntityUID getNewEntityUID();
	/** Retrieve 1 peer according to its entity uid. */
    OgrePeer* getOgrePeer(const EntityUID& entityUID);
	/** Return true if 1 peer is owned by me. */
    bool isOgrePeerOwned(OgrePeer* peer);

    /** See Solipsis::IModelerCallbacks. */
	virtual bool onObject3DSave(const String& sofFilename, Object3D* object3D);
    /** See Solipsis::IModelerCallbacks. */
	virtual bool onObject3DDelete(Object3D* object3D);
	/** See Solipsis::IModelerCallbacks. */
	virtual bool isObject3DOwned(Object3D* object3D);

	/** Called when the user avatar was saved. */
	bool onUserAvatarSave();

	// Get/Set
	SceneManager* getSceneManager() { return mSceneMgr; }
    OgrePeersMap::iterator getOgrePeersIteratorBegin() { return mOgrePeersMap.begin(); }
    OgrePeersMap::iterator getOgrePeersIteratorEnd() { return mOgrePeersMap.end(); }
    EvtsList& getEvtsToSendList() { return mEvtsList; }

protected:

    virtual OgrePeer* createAvatarNode(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual OgrePeer* createSceneNode(RefCntPoolPtr<XmlEntity>& xmlEntity);
    virtual OgrePeer* createObjectNode(RefCntPoolPtr<XmlEntity>& xmlEntity);

};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager_h__