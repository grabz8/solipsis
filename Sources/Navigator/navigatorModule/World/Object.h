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

#ifndef __Object_h__
#define __Object_h__

#include "MainApplication/OgrePeer.h"
#include <Selection.h>
#include "OgreGraphicObjects/MovableBox.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an object.
*/
class Object : public OgrePeer
{
protected:
    /// Resource group name
    String mResourceGroup;
    /// Resource location
    String mResourceLocation;
    /// Modeler Object3D
    Object3D* mObject3D;

public:
    /** Constructor. */
    Object(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, Object3D* object3D = 0);

    /** Destructor. */
    virtual ~Object();

	/** Called when the object was saved. */
	void onObjectSave();

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);
    /** See OgrePeer. */
    virtual bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity);

    /** See OgrePeer. */
    virtual bool action(RefCntPoolPtr<XmlAction>& xmlAction);

    // when loading this box replace the bounding box
    MovableBox * mpBox;
    // used only for bounding box
    SceneNode * mLocalNode;
};

} // namespace Solipsis

#endif // #ifndef __Object_h__