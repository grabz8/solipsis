#ifndef __TokamakBody_h__
#define __TokamakBody_h__

#include "IPhysicsBody.h"
#include "TokamakPluginPrerequisites.h"

#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

class TokamakCollider
{
public:
    virtual bool isAnimated() = 0;
    /** Collision callback. */
    virtual void collisionCallback(neCollisionInfo& collisionInfo) = 0;
};

/** This class manages a Tokamak body.
*/
class TokamakBody : public IPhysicsBody, public TokamakCollider
{
    friend class TokamakScene;

protected:
    /// Scene
    TokamakScene* mScene;
    /// Node
    SceneNode* mNode;
    /// Body
    neRigidBody* mBody;

public:
    TokamakBody(TokamakScene* scene);
    virtual ~TokamakBody();

    /// @copydoc IPhysicsBody::createBox
    virtual void createBox(SceneNode* node, const Vector3& extents);

    /// @copydoc IPhysicsBody::setPosition
    virtual void setPosition(const Vector3& position);
    /// @copydoc IPhysicsBody::setLinearVelocity
    virtual void setLinearVelocity(const Vector3& velocity);
    /// @copydoc IPhysicsBody::setAngularVelocity
    virtual void setAngularVelocity(const Vector3& velocity);

    bool isAnimated() { return false; }
    /** Collision callback. */
    void collisionCallback(neCollisionInfo& collisionInfo) {}
};

} // namespace Solipsis

#endif // #ifndef __TokamakBody_h__