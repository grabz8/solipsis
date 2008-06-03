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
#include "XmlDatas.h"
#include "tinyxml.h"
#include "Ogre.h"
#include "IOgrePeerManagerCallbacks.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis/Ogre peers.
*/
class OgrePeerManager : public FrameListener
{
public:
    typedef std::map<EntityUID, XmlEntity*> XmlEntitiesMap;
    typedef std::map<EntityUID, OgrePeer*> OgrePeersMap;
#ifdef POOL
    typedef std::list<RefCntPoolPtr<XmlEvt>> EvtsList;
#else
    typedef std::list<XmlEvt> EvtsList;
#endif

private:
    // Entities
    XmlEntitiesMap mMyXmlEntities;

    // <Peer's name, OgrePeer> map
    OgrePeersMap mOgrePeersMap;

    // List of events to send
    EvtsList mEvtsList;

    // Scene manager
    SceneManager* mSceneMgr;

    // Scene object filename
    std::string xmlObjectFilename;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

public:
    OgrePeerManager(SceneManager* sceneMgr = 0, IOgrePeerManagerCallbacks* callbacks = 0);
    ~OgrePeerManager();

    // Set my entities
    void setMyEntities(std::list<EntityUID> myEntities);

    // Load
#ifdef POOL
    bool load(RefCntPoolPtr<XmlEntity>& xmlEntity);
#else
    bool load(XmlEntity* xmlEntity);
#endif

    // Get the scene object filename
    String getXmlObjectFilename();

    // Remove 1 peer according to its ID and if it is local or networked
    bool remove(const EntityUID& entity, bool local);

    // Remove all peers (locals or networked)
    bool removeAll(bool local);

	// Update
#ifdef POOL
    bool update(RefCntPoolPtr<XmlEntity>& xmlEntity);
#else
    bool update(XmlEntity* xmlEntity);
#endif

    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

	// Get/Set
	SceneManager* getSceneManager() { return mSceneMgr; }
    OgrePeersMap::iterator getOgrePeersIteratorBegin() { return mOgrePeersMap.begin(); }
    OgrePeersMap::iterator getOgrePeersIteratorEnd() { return mOgrePeersMap.end(); }
    EvtsList& getEvtsToSendList() { return mEvtsList; }

protected:
#ifdef POOL
    virtual OgrePeer* createAvatarNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt);
    virtual OgrePeer* createSceneNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt);
    virtual OgrePeer* createObjectNode(RefCntPoolPtr<XmlEntity>& xmlEntity, TiXmlElement* xmlElt);
#else
    virtual OgrePeer* createAvatarNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt);
    virtual OgrePeer* createSceneNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt);
    virtual OgrePeer* createObjectNode(XmlEntity* xmlEntity, TiXmlElement* xmlElt);
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager_h__