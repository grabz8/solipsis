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

#ifndef __IPhysicsEngine_h__
#define __IPhysicsEngine_h__

#include "NavigatorModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics engine.
*/
class NAVIGATORMODULE_EXPORT IPhysicsEngine
{
public:
    IPhysicsEngine() {}
    virtual ~IPhysicsEngine() {}

    /** Get the name of the physics engine.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual const String& getName() const = 0;

    /** Perform the initialization. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool init() = 0;

    /** Perform any tasks when the system is shut down.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdown() = 0;

    /** Create 1 scene.
    @remarks An implementation must be supplied for this method.
    */
    virtual IPhysicsScene* createScene() = 0;

    /** Destroy 1 scene.
    @remarks An implementation must be supplied for this method.
    */
    virtual void destroyScene(IPhysicsScene* scene) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IPhysicsEngine_h__