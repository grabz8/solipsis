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

#ifndef __KeyMotion_h__
#define __KeyMotion_h__

#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages keys states when applied to motion.
 */
class KeyMotion
{
protected:
    Real mImpulse;
    Real mMaxSpeed;
    Real mAccelerationFactor;
    Real mDecelerationFactor;
    bool mPressed;
    Real mMotion;

public:
    KeyMotion(Real impulse = 1.0, Real maxSpeed = 1.0, Real accelerationFactor = 1.0, Real decelerationFactor = 1.0);

    void setState(bool pressed);
    bool isPressed();
    Real getMotion();
    void update(Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __KeyMotion_h__