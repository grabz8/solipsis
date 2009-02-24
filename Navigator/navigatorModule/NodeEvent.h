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

#ifndef __NodeEvent_h__
#define __NodeEvent_h__

#include "time.h"
#include "Peer.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an event applied to a Solipsis node.
 */
class NodeEvent
{
public:
    enum Type {
        TNew,            // Node connected to a new peer
        TLost,           // Node lost connection with a peer
        TStatusChanged   // Current status has changed
    };
    class Datas
    {
    };
    class DatasPeerNew : public Datas
    {
    public:
        Peer* mPeer;
    };
    class DatasPeerLost : public Datas
    {
    public:
        String mNetworkId;
    };
    class DatasStatusChanged : public Datas
    {
    public:
        String mStatus;
    };

protected:
    time_t mTimestamp;
    Type mType;
    Datas* mDatas;

public:
    NodeEvent(time_t timestamp, Type type, Datas* datas);
    ~NodeEvent();

    time_t getTimestamp();
    Type getType();
    Datas* getDatas();
};

} // namespace Solipsis

#endif // #ifndef __NodeEvent_h__