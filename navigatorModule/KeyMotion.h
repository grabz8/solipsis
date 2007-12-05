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