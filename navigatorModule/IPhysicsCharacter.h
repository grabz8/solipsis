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

#ifndef __IPhysicsCharacter_h__
#define __IPhysicsCharacter_h__

#include "NavigatorModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics character.
@remarks
Character is assumed centered on X and Z axis, Y origin is just under feet.
*/
class NAVIGATORMODULE_EXPORT IPhysicsCharacter
{
public:
    /// Description of 1 character
    typedef struct {
        /// Position 
        Vector3 position;
        /// Radius
        Real radius;
        /// Height
        Real height;
        /// Max offset character can step up (stair's step, ...)
        Real stepOffset;
    } Desc;
public:
    IPhysicsCharacter() {}
    virtual ~IPhysicsCharacter() {}

    /** Create.
    @remarks An implementation must be supplied for this method.
    */
    virtual void create(SceneNode* node, Desc& desc) = 0;
    /** Move.
    @remarks An implementation must be supplied for this method.
    */
    virtual void move(Vector3& displacement) = 0;
    /** Get current position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void getPosition(Vector3& position) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IPhysicsCharacter_h__