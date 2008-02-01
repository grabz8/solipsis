#ifndef __ODECharacter_h__
#define __ODECharacter_h__

#include "IPhysicsCharacter.h"
#include "ODEPluginPrerequisites.h"

#include "OgreOde_Core.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a ODE character.
*/
class ODECharacter : public IPhysicsCharacter, public OgreOde::CollisionListener
{
protected:
    /// Scene
    ODEScene* mScene;
    /// Description
    Desc mDesc;
    /// Ray to compute feet contact
    OgreOde::RayGeometry* mRayGeom;
    /// Last ray geometry contact
    OgreOde::Contact mRayGeomLastContact;
    /// Ray geometry contact detected ?
    bool mRayGeomContact;
    /// Maximum time step to update collision
    Real mMaxUpdateTimeStep;
    /// Capsule to compute body contact
    OgreOde::CapsuleGeometry* mCapsuleGeom;
    /// Last capsule geometry contact
    OgreOde::Contact mCapsuleGeomLastContact;
    /// Capsule geometry contact detected ?
    bool mCapsuleGeomContact;
    /// Capsule transform geometry
    OgreOde::TransformGeometry* mCapsuleBodyTrans;
    /// Capsule to compute body collisions
    OgreOde::CapsuleGeometry* mCapsuleBodyGeom;
    /// Body capsule
    OgreOde::Body* mCapsuleBody;

public:
    ODECharacter(ODEScene* scene);
    virtual ~ODECharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(SceneNode* node, Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);

protected:
    /** Move the character by step. */
    void moveStep(Vector3& displacement);

    /** See OgreOde::CollisionListener. */
    bool collision(OgreOde::Contact* contact);
};

} // namespace Solipsis

#endif // #ifndef __ODECharacter_h__