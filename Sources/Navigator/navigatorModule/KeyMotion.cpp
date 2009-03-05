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
#include "KeyMotion.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
KeyMotion::KeyMotion(Real impulse, Real maxSpeed, Real accelerationFactor, Real decelerationFactor) :
  mImpulse(impulse),
  mMaxSpeed(maxSpeed),
  mAccelerationFactor(accelerationFactor),
  mDecelerationFactor(decelerationFactor),
  mPressed(false),
  mMotion(0.0f)
{
}

//-------------------------------------------------------------------------------------
void KeyMotion::setState(bool pressed)
{
    mPressed = pressed;
}

//-------------------------------------------------------------------------------------
bool KeyMotion::isPressed()
{
    return mPressed;
}

//-------------------------------------------------------------------------------------
Real KeyMotion::getMotion()
{
    return mMotion;
}

//-------------------------------------------------------------------------------------
void KeyMotion::update(Real timeSinceLastFrame)
{
    if (mPressed)
    {
        if (mMotion < mImpulse)
            mMotion = mImpulse;
/*        Real refFps = 1.0/60.0;
        while (timeSinceLastFrame > refFps)
        {
            mMotion *= mAccelerationFactor;
            timeSinceLastFrame -= refFps;
            if (mMotion > mMaxSpeed) break;
        }
        mMotion *= 1.0 + (mAccelerationFactor - 1.0)*(timeSinceLastFrame*60.0);*/
        // Approximation
        mMotion *= 1.0 + (mAccelerationFactor - 1.0)*(timeSinceLastFrame*60.0);
        if (mMotion > mMaxSpeed)
            mMotion = mMaxSpeed;
    }
    else
    {
/*        Real refFps = 1.0/60.0;
        while (timeSinceLastFrame > refFps)
        {
            mMotion *= mDecelerationFactor;
            timeSinceLastFrame -= refFps;
            if (mMotion < mImpulse) break;
        }
        mMotion *= 1.0 - (1.0 - mDecelerationFactor)*(timeSinceLastFrame*60.0);*/
        // Approximation
        mMotion *= 1.0 - std::max(1.0, (1.0 - mDecelerationFactor)*(timeSinceLastFrame*60.0));
        if (mMotion < mImpulse)
            mMotion = 0.0f;
    }
}

//-------------------------------------------------------------------------------------
