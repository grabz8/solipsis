#ifndef __ITokamakCharacter_h__
#define __ITokamakCharacter_h__

#include "IPhysicsCharacter.h"
#include "TokamakPluginPrerequisites.h"

#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a Tokamak character.
*/
class TokamakCharacter : public IPhysicsCharacter
{
protected:
    /// Scene
    TokamakScene* mScene;
    /// Description
    Desc mDesc;
    /// Node
    SceneNode* mNode;
    /// Capsule body
    neAnimatedBody* mCapsuleBody;
    /// Capsule body contact detected ?
    bool mCapsuleBodyContact;

public:
    TokamakCharacter(TokamakScene* scene);
    virtual ~TokamakCharacter();

    /// @copydoc IPhysicsCharacter::create
    virtual void create(SceneNode* node, Desc& desc);
    /// @copydoc IPhysicsCharacter::move
    virtual void move(Vector3& displacement);
    /// @copydoc IPhysicsCharacter::getPosition
    virtual void getPosition(Vector3& position);

    /** Collision callback. */
    void collisionCallback(neCollisionInfo& collisionInfo);

protected:
    /** Move the character by step. */
    void moveStep(Vector3& displacement);
};

} // end namespace

#endif // #ifndef __ITokamakCharacter_h__