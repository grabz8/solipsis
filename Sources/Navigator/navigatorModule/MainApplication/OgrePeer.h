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

#ifndef __OgrePeer_h__
#define __OgrePeer_h__

#include <XmlEntity.h>
#include <XmlAction.h>
#include <Ogre.h>
#include "OgreGraphicObjects/ProgressBar.h"
#include "OgreGraphicObjects/MovableBox.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a basic Ogre representation of a peer.
 */
class OgrePeer
{
protected:
    /// Associated entity
    RefCntPoolPtr<XmlEntity> mXmlEntity;

    /// Type
    bool mIsLocal;
    /// Level of detail
    int mLod;

    // When loading this box replace the bounding box
    MovableBox *mAABBox;
    // Used only for bounding box
    SceneNode *mLocalNode;

    // progress Bar for downloading/uploading
    ProgressBarWithText *mProgressBar;

public:
    /** Constructor.
    */
    OgrePeer(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal);

    virtual ~OgrePeer();

    /** Gets the entity. */
    RefCntPoolPtr<XmlEntity>& getXmlEntity();

    /** Gets the updated entity. */
    virtual RefCntPoolPtr<XmlEntity>& getUpdatedXmlEntity() { return RefCntPoolPtr<XmlEntity>::nullPtr; }

    /** Test if this peer is a local entity. */
    bool isLocal() { return mIsLocal; }

    /** Update. */
    virtual void update(Real timeSinceLastFrame) = 0;
    /** Update entity. */
    virtual bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity) = 0;

    /** Action on entity. */
    virtual bool action(RefCntPoolPtr<XmlAction>& xmlAction) = 0;
};

} // namespace Solipsis

#endif // #ifndef __OgrePeer_h__