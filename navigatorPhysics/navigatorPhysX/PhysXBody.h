#ifndef __IPhysXBody_h__
#define __IPhysXBody_h__

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
    virtual void createBox(SceneNode* node, const Vector3& extents);

    /// @copydoc IPhysicsBody::setPosition
    virtual void setPosition(const Vector3& position);
    /// @copydoc IPhysicsBody::setLinearVelocity
    virtual void setLinearVelocity(const Vector3& velocity);
    /// @copydoc IPhysicsBody::setAngularVelocity
    virtual void setAngularVelocity(const Vector3& velocity);
};

} // end namespace

#endif // #ifndef __IPhysXBody_h__