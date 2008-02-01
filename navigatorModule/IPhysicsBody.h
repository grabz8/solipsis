#ifndef __IPhysicsBody_h__
#define __IPhysicsBody_h__

#include "NavigatorModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics body.
*/
class NAVIGATORMODULE_EXPORT IPhysicsBody
{
public:
    IPhysicsBody() {}
    virtual ~IPhysicsBody() {}

    /** Create a box.
    @remarks An implementation must be supplied for this method.
    */
    virtual void createBox(SceneNode* node, const Vector3& extents) = 0;

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

} // namespace Solipsis

#endif // #ifndef __IPhysicsBody_h__