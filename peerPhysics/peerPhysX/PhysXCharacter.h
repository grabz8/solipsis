#ifndef __PhysXCharacter_h__
#define __PhysXCharacter_h__

#include "IPhysicsCharacter.h"
#include "PhysXPluginPrerequisites.h"

#include "NxPhysics.h"
#include "NxController.h"
#include "ControllerManager.h"
#include "NxCapsuleController.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX character.
*/
class PhysXCharacter : public IPhysicsCharacter
{
protected:
    /// Scene
    PhysXScene* mScene;
    /// Description
    Desc mDesc;
    /// Capsule controller
    NxCapsuleController* mNxCapsuleController;

public:
    PhysXCharacter(PhysXScene* scene);
    virtual ~PhysXCharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);
};

} // namespace Solipsis

#endif // #ifndef __PhysXCharacter_h__