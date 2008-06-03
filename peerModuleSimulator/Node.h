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

#include <list>
#include <pthread.h>
#include "Ogre.h"
#include "IP2NClient.h"
#include "XmlDatas.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Node.
*/
class Node
{
public:
#ifdef POOL
    typedef std::list<RefCntPoolPtr<XmlEvt>> XmlEvtToHandleList;
#else
    typedef std::list<XmlEvt*> XmlEvtToHandleList;
#endif

protected:
    /// Node unique identifier
    NodeId mNodeId;
    /// Type
    String mType;
    /// Mutex
    pthread_mutex_t mEvtsMutex;
    /// List of events to handle
    XmlEvtToHandleList mEvtsToHandleList;
    /// Frozen state
    bool mFrozen;

public:
    /** Constructor. */
    Node(const NodeId& nodeId, const String& type);
    /** Destructor. */
    virtual ~Node();

    /** Gets the identifier. */
    const NodeId& getNodeId();
    /** Gets the type. */
    const String& getType();

    /** Process an event. */
    virtual bool processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr);
#ifdef POOL
    /** Get next event to handle. */
    virtual RefCntPoolPtr<XmlEvt> getNextEvtToHandle();
    /** Free event (handled event). */
    virtual bool freeEvt(RefCntPoolPtr<XmlEvt>& evt);
#else
    /** Get next event to handle. */
    virtual XmlEvt* getNextEvtToHandle();
    /** Free event (handled event). */
    virtual bool freeEvt(XmlEvt* evt);
#endif
    /** Freeze. */
    virtual bool freeze(bool frozen) { mFrozen = frozen; return true; }
};

} // namespace Solipsis

#endif // #ifndef __Node_h__