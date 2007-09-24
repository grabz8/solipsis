#include "KeyMotion.h"

KeyMotion::KeyMotion(Real impulse, Real maxSpeed, Real accelerationFactor, Real decelerationFactor) :
  mImpulse(impulse),
  mMaxSpeed(maxSpeed),
  mAccelerationFactor(accelerationFactor),
  mDecelerationFactor(decelerationFactor),
  mPressed(false),
  mMotion(0.0)
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
        if (mMotion < mMaxSpeed)
            mMotion *= mAccelerationFactor;
    }
    else
    {
        mMotion *= mDecelerationFactor;
        if (mMotion < mImpulse)
            mMotion = 0.0;
    }
}
