#ifndef __PhysXScene_h__
#define __PhysXScene_h__

#include "IPhysicsScene.h"
#include "PhysXPluginPrerequisites.h"

#include "NxPhysics.h"
#include "ControllerManager.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a PhysX scene.
*/
class PhysXScene : public IPhysicsScene
{
protected:
    /// Engine
    PhysXEngine* mEngine;
    /// Scene
    NxScene* mNxScene;
    /// Geometry
    NxTriangleMesh* mNxGeometry;
    /// Actor
    NxActor* mNxActor;
    /// Controller manager
    NxControllerManager* mNxControllerManager;

    /// Time since last frame in seconds
    Real mTimeSinceLastFrame;
    /// Max duration of each step in seconds
    Real mMaxStepInSeconds;

public:
    PhysXScene(PhysXEngine* engine);
    virtual ~PhysXScene();

    /// @copydoc IPhysicsScene::create
    virtual bool create();

    /// @copydoc IPhysicsScene::getDebugFlags
    virtual DebugFlag getDebugFlags() { return DF_NONE; }
    /// @copydoc IPhysicsScene::setDebugFlags
    virtual void setDebugFlags(DebugFlag debugFlags) {}

    /// @copydoc IPhysicsScene::setTiming
    virtual void setTiming(Real maxStepInSeconds);
    /// @copydoc IPhysicsScene::getTiming
    virtual void getTiming(Real& maxStepInSeconds, Real& timeSinceLastFrame);

    /// @copydoc IPhysicsScene::preStep
    virtual void preStep(Real timeSinceLastFrame);
    /// @copydoc IPhysicsScene::postStep
    virtual void postStep();

    /// @copydoc IPhysicsScene::getGravity
    virtual void getGravity(Vector3& gravity);
    /// @copydoc IPhysicsScene::setGravity
    virtual void setGravity(Vector3& gravity);

    /// @copydoc IPhysicsScene::setTerrainMesh
    virtual bool setTerrainMesh(const MeshPtr mesh,
                                const Vector3& position,
                                const Quaternion& orientation,
                                const Vector3& scale);

    /// @copydoc IPhysicsScene::createBody
    virtual IPhysicsBody* createBody();
    /// @copydoc IPhysicsScene::destroyBody
    virtual void destroyBody(IPhysicsBody* body);

    /// @copydoc IPhysicsScene::createCharacter
    virtual IPhysicsCharacter* createCharacter();
    /// @copydoc IPhysicsScene::destroyCharacter
    virtual void destroyCharacter(IPhysicsCharacter* character);

    /// Get scene
    NxScene* getNxScene() { return mNxScene; }
    /// Get actor
    NxActor* getNxActor() { return mNxActor; }
    /// Get character controller
    NxControllerManager* getNxControllerManager() { return mNxControllerManager; }
};

} // namespace Solipsis

#endif // #ifndef __PhysXScene_h__