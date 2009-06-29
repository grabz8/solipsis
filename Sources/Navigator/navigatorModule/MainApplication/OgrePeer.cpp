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
#include "Prerequisites.h"

#include "OgrePeer.h"
#include <CTLog.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------

OgrePeer::OgrePeer(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal) :
    mXmlEntity(xmlEntity),
    mIsLocal(isLocal),
    mLod(0),
    mAABBox(0),
    mLocalNode(0),
    mProgressBar(0)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "OgrePeer::OgrePeer() mIsLocal=%s", LOGHANDLER_LOGBOOL(isLocal));
}

//-------------------------------------------------------------------------------------
OgrePeer::~OgrePeer()
{
    if (mProgressBar) 
    {
        mProgressBar->detach();
        delete mProgressBar;
        mProgressBar = 0;
    }
    if (mAABBox)
    {
        mAABBox->detatchFromParent();
        delete mAABBox;
        mAABBox = 0;
    }
    if (mLocalNode)
    {
        mLocalNode->detachAllObjects();
        mLocalNode->getCreator()->destroySceneNode(mLocalNode->getName());
        mLocalNode = 0;
    }
}

//-------------------------------------------------------------------------------------

RefCntPoolPtr<XmlEntity>& OgrePeer::getXmlEntity()
{
    return mXmlEntity;
}

//-------------------------------------------------------------------------------------
