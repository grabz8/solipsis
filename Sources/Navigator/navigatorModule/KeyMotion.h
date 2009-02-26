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
    /// Initial impulsion
    Real mImpulse;
    /// Maximum speed (eg. in m/sec)
    Real mMaxSpeed;
    /// Acceleration factor with 1/60 timing (eg. 1.5 to multiply speed by 1.5 every 1/60th sec)
    Real mAccelerationFactor;
    /// Deceleration factor with 1/60 timing (eg. 0.5 to multiply speed by 0.5 every 1/60th sec)
    Real mDecelerationFactor;
    /// State of key, if pressed or no
    bool mPressed;
    /// Current motion value
    Real mMotion;

public:
    /** Constructor.
    @remarks Stopped motion and default values without smoothing.
    */
    KeyMotion(Real impulse = 1.0, Real maxSpeed = 1.0, Real accelerationFactor = 1.0, Real decelerationFactor = 1.0);

    /** Set key state.
    @param pressed If key is pressed or not
    */
    void setState(bool pressed);

    /** Update the motion.
    @return True if key is pressed
    */
    bool isPressed();

    /** Get the motion value.
    @param timeSinceLastFrame Elapsed time in seconds since the last frame
    */
    Real getMotion();

    /** Update the motion value.
    @param timeSinceLastFrame Elapsed time in seconds since the last frame
    */
    void update(Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __KeyMotion_h__