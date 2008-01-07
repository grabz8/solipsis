#ifndef __IPhysXCharacter_h__
#define __IPhysXCharacter_h__

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
    /// Controller manager
    ::ControllerManager* mNxControllerManager;
    /// Capsule controller
    NxCapsuleController* mNxCapsuleController;

public:
    PhysXCharacter(PhysXScene* scene);
    virtual ~PhysXCharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(SceneNode* node, Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);
};

} // end namespace

#endif // #ifndef __IPhysXCharacter_h__