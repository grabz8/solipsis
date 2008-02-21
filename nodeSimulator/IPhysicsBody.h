#ifndef __IPhysicsBody_h__
#define __IPhysicsBody_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics body.
*/
class PEERMODULE_EXPORT IPhysicsBodyListener
{
public:
    /** Sets the position of the body.
    */
    virtual void setPosition(const Vector3& pos) = 0;
    /** Sets the orientation of this node via a quaternion.
    */
    virtual void setOrientation(const Quaternion& q) = 0;
};

/** This class represents a generic Physics body.
*/
class PEERMODULE_EXPORT IPhysicsBody
{
public:
    IPhysicsBody() {}
    virtual ~IPhysicsBody() {}

    /** Create a box.
    @remarks An implementation must be supplied for this method.
    */
    virtual void createBox(IPhysicsBodyListener* listener, const Vector3& extents) = 0;

    /** Set position.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setPosition(const Vector3& position) = 0;
    /** Set linear velocity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setLinearVelocity(const Vector3& velocity) = 0;
    /** Set angular velocity.
    @remarks An implementation must be supplied for this method.
    */
    virtual void setAngularVelocity(const Vector3& velocity) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsBody_h__