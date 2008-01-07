#ifndef __ITokamakBody_h__
#define __ITokamakBody_h__

#include "IPhysicsBody.h"
#include "TokamakPluginPrerequisites.h"

#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a Tokamak body.
*/
class TokamakBody : public IPhysicsBody
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

    /** Collision callback. */
    void collisionCallback(neCollisionInfo& collisionInfo) {}
};

} // end namespace

#endif // #ifndef __ITokamakBody_h__