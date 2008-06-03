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

#include "NodeEvent.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NodeEvent::NodeEvent(time_t timestamp, Type type, Datas* datas) :
    mTimestamp(timestamp),
    mType(type),
    mDatas(datas)
{
}

//-------------------------------------------------------------------------------------
NodeEvent::~NodeEvent()
{
    delete mDatas;
}

//-------------------------------------------------------------------------------------
time_t NodeEvent::getTimestamp()
{
    return mTimestamp;
}

//-------------------------------------------------------------------------------------
NodeEvent::Type NodeEvent::getType()
{
    return mType;
}

//-------------------------------------------------------------------------------------
NodeEvent::Datas* NodeEvent::getDatas()
{
    return mDatas;
}

//-------------------------------------------------------------------------------------
