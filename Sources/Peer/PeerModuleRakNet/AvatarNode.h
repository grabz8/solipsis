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

#ifndef __AvatarNode_h__
#define __AvatarNode_h__

#include <map>
#include <pthread.h>
#include "Ogre.h"
#include <BitStream.h>
#include <RakNetConnection.h>
#include <Node.h>
#include "Entity.h"
#include "TimeListener.h"

namespace Solipsis {

/** This class manages 1 avatar node.
*/
class AvatarNode : public Node, public TimeListener
{
public:

    typedef std::list<RefCntPoolPtr<XmlEvt>> XmlEvtToHandleList;

protected:
    /// Mutex on instance
    pthread_mutex_t mMutex;
    /// Mutex on events
    pthread_mutex_t mEvtsMutex;
    /// List of events to handle
    XmlEvtToHandleList mEvtsToHandleList;
    /// Frozen state
    bool mFrozen;
    /// Connection lost
    bool mConnectionLost;

protected:
    /// Map of owned entities
    RakNetEntity::RakNetEntityMap mOwnedEntities;
    /// Last site where avatar was connected
    EntityUID mLastSiteUid;

    /// Entity
    RakNetEntity *mEntity;

    /// Name
    std::string mName;

public:
    /** Constructor. */
    AvatarNode();
    /** Destructor. */
    virtual ~AvatarNode();

    /** Get entity. */
    RakNetEntity* getEntity() { return mEntity; }
    /** Set entity. */
    void setEntity(RakNetEntity* entity) { mEntity = entity; }

    /** Get the name. */
    const std::string& getName() { return mName; }
    /** Set the name. */
    void setName(const std::string& name) { mName = name; }

    /** See Solipsis::Node. */
    virtual bool loadFromElt(TiXmlElement* nodeElt);
    /** See Solipsis::Node. */
    virtual TiXmlElement* getSavedElt();

    void onNewEntity(Entity* entity);
    void onUpdatedEntity(Entity* entity);
    void onLostEntity(Entity* entity);

    void onActionOnEntity(RakNet::BitStream *bitStream);

    bool isOwnedEntity(Entity* entity);


    /** Process an event. */
    virtual bool processEvt(RefCntPoolPtr<XmlEvt>& xmlEvt, std::string& xmlRespStr);

    /** Get next event to handle. */
    virtual RefCntPoolPtr<XmlEvt> getNextEvtToHandle();
    /** Free event (handled event). */
    virtual bool freeEvt(RefCntPoolPtr<XmlEvt>& xmlEvt);

    /** Freeze. */
    virtual bool freeze(bool frozen);
    /** Is frozen ? */
    bool isFrozen() { return mFrozen; }
    /** Set connection lost. */
    virtual void setConnectionLost(bool lost);
    /** Is connection lost ? */
    bool isConnectionLost() { return mConnectionLost; }

    /** See Solipsis::TimeListener. */
    virtual bool tick(Ogre::Real timeSinceLastTick);
};

} // namespace Solipsis

#endif // #ifndef __AvatarNode_h__