#ifndef __IODEBody_h__
#define __IODEBody_h__

#include "IPhysicsBody.h"
#include "ODEPluginPrerequisites.h"

#include "OgreOde_Core.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a ODE body.
*/
class ODEBody : public IPhysicsBody
{
protected:
    /// Scene
    ODEScene* mScene;
    /// Body
    OgreOde::Body* mBody;

public:
    ODEBody(ODEScene* scene);
    virtual ~ODEBody();

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

#endif // #ifndef __IODEBody_h__