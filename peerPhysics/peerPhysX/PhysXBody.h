#ifndef __PhysXBody_h__
#define __PhysXBody_h__

#include "IPhysicsBody.h"
#include "PhysXPluginPrerequisites.h"

#include "NxPhysics.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX body.
*/
class PhysXBody : public IPhysicsBody
{
protected:
    /// Scene
    PhysXScene* mScene;
    /// Actor
    NxActor* mNxActor;

public:
    PhysXBody(PhysXScene* scene);
    virtual ~PhysXBody();

    /// @copydoc IPhysicsBody::createBox
    virtual void createBox(IPhysicsBodyListener* listener, const Vector3& extents);

    /// @copydoc IPhysicsBody::setPosition
    virtual void setPosition(const Vector3& position);
    /// @copydoc IPhysicsBody::setLinearVelocity
    virtual void setLinearVelocity(const Vector3& velocity);
    /// @copydoc IPhysicsBody::setAngularVelocity
    virtual void setAngularVelocity(const Vector3& velocity);
};

} // namespace Solipsis

#endif // #ifndef __PhysXBody_h__