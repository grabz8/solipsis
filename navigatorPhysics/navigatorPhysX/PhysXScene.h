#ifndef __IPhysXScene_h__
#define __IPhysXScene_h__

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
    /// Scene manager
    SceneManager* mSceneManager;
    /// Scene
    NxScene* mNxScene;
    /// Geometry
    NxTriangleMesh* mNxGeometry;
    /// Actor
    NxActor* mNxActor;
    /// Controller manager
    ::ControllerManager* mNxControllerManager;

    /// Time since last frame in seconds
    Real mTimeSinceLastFrame;
    /// Max duration of each step in seconds
    Real mMaxStepInSeconds;

public:
    PhysXScene();
    virtual ~PhysXScene();

    /// @copydoc IPhysicsScene::create
    virtual bool create(SceneManager* sceneManager);

    /// @copydoc IPhysicsScene::getDebugFlags
    virtual DebugFlag getDebugFlags() { return DebugFlag::DF_NONE; }
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
    virtual bool setTerrainMesh(const Entity& entity);

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
    ::ControllerManager* getNxControllerManager() { return mNxControllerManager; }
};

} // end namespace

#endif // #ifndef __IPhysXScene_h__