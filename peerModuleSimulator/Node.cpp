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

#include "Node.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
Node::Node(const NodeId& nodeId, const String& type) :
    mNodeId(nodeId),
    mType(type),
    mEvtsMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
Node::~Node()
{
}

//-------------------------------------------------------------------------------------
const NodeId& Node::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
const String& Node::getType()
{
    return mType;
}

//-------------------------------------------------------------------------------------
bool Node::processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr)
{
    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
RefCntPoolPtr<XmlEvt> Node::getNextEvtToHandle()
#else
XmlEvt* Node::getNextEvtToHandle()
#endif
{
#ifdef POOL
    RefCntPoolPtr<XmlEvt> evt(RefCntPoolPtr<XmlEvt>::nullPtr);
#else
    XmlEvt* evt = 0;
#endif

    pthread_mutex_lock(&mEvtsMutex);
    if (!mEvtsToHandleList.empty())
    {
        evt = mEvtsToHandleList.front();
        mEvtsToHandleList.pop_front();
    }
    pthread_mutex_unlock(&mEvtsMutex);

    return evt;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Node::freeEvt(RefCntPoolPtr<XmlEvt>& evt)
{
    return true;
}
#else
bool Node::freeEvt(XmlEvt* evt)
{
    delete evt;

    return true;
}
#endif

//-------------------------------------------------------------------------------------

} // namespace Solipsis
