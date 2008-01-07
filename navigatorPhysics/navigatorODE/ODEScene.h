#ifndef __IODEScene_h__
#define __IODEScene_h__

#include "IPhysicsScene.h"
#include "ODEPluginPrerequisites.h"

#include "OgreOde_Core.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages a ODE scene.
*/
class ODEScene : public IPhysicsScene, public OgreOde::CollisionListener
{
protected:
    /// Scene manager
    SceneManager* mSceneManager;
    /// World
    OgreOde::World* mWorld;
    /// Step handler
    OgreOde::StepHandler* mStepHandler;
    /// World geometry
    OgreOde::TriangleMeshGeometry* mWorldGeometry;

    /// Time since last frame in seconds
    Real mTimeSinceLastFrame;
    /// Max duration of each step in seconds
    Real mMaxStepInSeconds;

public:
    ODEScene();
    virtual ~ODEScene();

    /// @copydoc IPhysicsScene::create
    virtual bool create(SceneManager* sceneManager);

    /// @copydoc IPhysicsScene::getDebugFlags
    virtual DebugFlag getDebugFlags();
    /// @copydoc IPhysicsScene::setDebugFlags
    virtual void setDebugFlags(DebugFlag debugFlags);

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

    /// Get world
    OgreOde::World* getWorld() { return mWorld; }
    /// Get world geometry
    OgreOde::TriangleMeshGeometry* getWorldGeometry() { return mWorldGeometry; }

protected:
    /** See OgreOde::CollisionListener. */
    virtual bool collision(OgreOde::Contact* contact);
};

} // end namespace

#endif // #ifndef __IODEScene_h__