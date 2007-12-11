#ifndef __Avatar_h__
#define __Avatar_h__

#include "OgrePeer.h"
#include "KeyMotion.h"
#include "MovableText.h"
#include "Event.h"

using namespace Ogre;

#ifdef PHYSICS
#include "OgreOde_Core.h"
#elif PHYSX
#include "NxPhysics.h"
#include "NxController.h"
#include "ControllerManager.h"
#include "NxCapsuleController.h"
#include "PhysXHelpers.h"
#endif

namespace Solipsis {

/** This class represents an avatar.
*/
class Avatar : public OgrePeer
#ifdef PHYSICS
    ,
    public OgreOde::CollisionListener
#elif PHYSX
    ,
    public NxUserControllerHitReport,
    public NxSceneQueryReport
#endif
{
public:
    /** Enumeration denoting the state (animation, ...) */
    enum State {
        SNone,
        SIdle,
        SWalk,
        SRun,
        SFly,
        SSwim,
        SCount
    };
    /** Enumeration denoting the type of movement (rotate, straff, ...) */
    enum MvtType {
        MT1stPerson,
        MT3rdPerson
    };

protected:
    /// Default animation names
    static String mDefaultStateAnimName[SCount];
    /// Animation names
    String mStateAnimName[SCount];
    /// Current state
    State mState;
    /// Current movement type
    MvtType mMvtType;

    /// Scene node
    SceneNode* mSceneNode;
    /// Entity
    Entity* mEntity;
    /// Current animation state
    AnimationState* mAnimationState;
    /// Name label
    MovableText* mNameLabel;
    /// Whether to apply the gravity
    bool mGravity;
    /// Radius
    Real mRadius;
    /// Height
    Real mHeight;
#ifdef PHYSICS
    /// Physical world
    OgreOde::World* mWorld;
    /// World geometry
    OgreOde::TriangleMeshGeometry* mWorldGeometry;
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
    /// Capsule to compute body collisions
    OgreOde::CapsuleGeometry* mCapsuleBodyGeom;
    /// Body capsule
    OgreOde::Body* mCapsuleBody;
#elif PHYSX
    /// Physical scene
    NxScene* mPhysicsScene;
    /// Character controller manager
    ::ControllerManager* mControllerManager;
    /// Capsule controller
    NxCapsuleController* mCapsuleController;
/*    /// Scene query
    NxSceneQuery* mSceneQuery;*/
#else
    RaySceneQuery* mRaySceneQuery;
#endif

private:
    /// Animation name for Idle
    String mIdleAnimName;
    /// Animation name for Walk
    String mWalkAnimName;
    /// Animation name for Run
    String mRunAnimName;
    /// Animation name for Fly
    String mFlyAnimName;
    /// Animation name for Swim
    String mSwimAnimName;

    /// Motion for Up key
    KeyMotion mUpKeyMotion;
    /// Motion for Down key
    KeyMotion mDownKeyMotion;
    /// Motion for Left key
    KeyMotion mLeftKeyMotion;
    /// Motion for Right key
    KeyMotion mRightKeyMotion;
    /// Motion for PgUp key
    KeyMotion mPgupKeyMotion;
    /// Motion for PgDown key
    KeyMotion mPgdownKeyMotion;

public:
    /** Constructor.
    */
    Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity);
    virtual ~Avatar();

    /** Get the scene node. */
    SceneNode* getSceneNode();
    /** Get the entity. */
    Entity* getEntity();
    /** Set the name. */
    void setName(const String& name);
    /** Set whether the name is visible or not. */
    void setNameVisibility(bool visible);

    /** Set the current state. */
    void setState(State state);
    /** Get the current state. */
    State getState();
    /** Set the animation name of 1 state. */
    void setStateAnimName(State state, const String& name);

    /** Set the current movement type. */
    void setMvtType(MvtType mvtType);
    /** Get the current movement type. */
    MvtType getMvtType();

    /** Set whether the gravity is applied or not. */
    void setGravity(bool enabled);
    /** Determines whether the gravity is applied or not. */
    bool isGravityEnabled();
#ifdef PHYSICS
    /** Get the physical world. */
    OgreOde::World* getPhysicsWorld() { return mWorld; }
    /** Create physics. */
    void createPhysics(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry);
    /** Destroy physics. */
    void destroyPhysics();
    /** Set the maximum time step to update collision. */
    void setMaxUpdateTimeStep(Real maxUpdateTimeStep);
    /** Get the maximum time step to update collision. */
    Real getMaxUpdateTimeStep();
#elif PHYSX
    /** Get the physical scene. */
    NxScene* getPhysicsScene() { return mPhysicsScene; }
    /** Create physics. */
    void createPhysics(NxScene* physicsScene, ::ControllerManager* controllerManager);
    /** Destroy physics. */
    void destroyPhysics();
#endif

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);

    /** Starts 1 animation. */
    void startAnimation(const String &name, bool loop = true);
    /** Stop the current animation. */
    void stopAnimation();
    /** Animate. */
    void animate(Real timeSinceLastFrame);

    /** Compute movement according to key pressed. */
    void movementKeyPressed(KeyCode code);
    /** Compute movement according to key released. */
    void movementKeyReleased(KeyCode code);

#ifdef PHYSICS
protected:
    /** See OgreOde::CollisionListener. */
    bool collision(OgreOde::Contact* contact);
#elif PHYSX
protected:
    /** See NxUserControllerHitReport. */
    virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit) { return NX_ACTION_NONE; }
    /** See NxUserControllerHitReport. */
    virtual NxControllerAction onControllerHit(const NxControllersHit& hit) { return NX_ACTION_NONE; }
    /** See NxSceneQueryReport. */
    virtual NxQueryReportResult onBooleanQuery(void* userData, bool result) { return NX_SQR_ABORT_ALL_QUERIES; }
    /** See NxSceneQueryReport. */
    virtual NxQueryReportResult onRaycastQuery(void* userData, NxU32 nbHits, const NxRaycastHit* hits) { return NX_SQR_ABORT_ALL_QUERIES; }
    /** See NxSceneQueryReport. */
	virtual NxQueryReportResult onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits) { return NX_SQR_ABORT_ALL_QUERIES; }
    /** See NxSceneQueryReport. */
	virtual NxQueryReportResult onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits) { return NX_SQR_ABORT_ALL_QUERIES; }
#endif
};

} // namespace Solipsis

#endif // #ifndef __Avatar_h__