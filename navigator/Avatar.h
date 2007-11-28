#ifndef __Avatar_h__
#define __Avatar_h__

#include "OgrePeer.h"
#include "KeyMotion.h"
#include "MovableText.h"

using namespace Ogre;

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

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
    enum State {
        SNone,
        SIdle,
        SWalk,
        SRun,
        SFly,
        SSwim,
        SCount
    };
    enum MvtType {
        MT1stPerson,
        MT3rdPerson
    };

protected:
    static String mDefaultStateAnimName[SCount];
    String mStateAnimName[SCount];
    State mState;
    MvtType mMvtType;

    SceneNode* mSceneNode;
    Entity* mEntity;
    AnimationState* mAnimationState;
    MovableText* mNameLabel;
    bool mGravity;
    Real mRadius;
    Real mHeight;
#ifdef PHYSICS
    OgreOde::World* mWorld;
    OgreOde::RayGeometry* mRayGeom;
#ifdef CAPSULEGEOM
    Real mMaxUpdateTimeStep;
    OgreOde::CapsuleGeometry* mCapsuleGeom;
    OgreOde::Contact mCapsuleGeomLastContact;
    bool mCapsuleGeomContact;
#endif
#ifdef FEET
    OgreOde::Body* mFeetBody;
    bool mRayGeomContact;
    OgreOde::SphereGeometry* mFeetGeom;
    OgreOde::Contact mFeetGeomLastContact;
    bool mFeetGeomContact;
#endif
    OgreOde::TriangleMeshGeometry* mWorldGeometry;
#elif PHYSX
    NxScene* mPhysicsScene;
    ::ControllerManager* mControllerManager;
    NxCapsuleController* mCapsuleController;
    NxSceneQuery* mSceneQuery;
#else
    RaySceneQuery* mRaySceneQuery;
#endif

//    void lookAtTheGoodDirection();

public:
    Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity);
    virtual ~Avatar();

    //Set and get
    SceneNode* getSceneNode();
    Entity* getEntity();
    void setName(const String& name);
    void setNameVisibility(bool visible);

    void setState(State state);
    State getState();
    void setStateAnimName(State state, const String& name);

    void setMvtType(MvtType mvtType);
    MvtType getMvtType();

    void setGravity(bool enabled);
    bool isGravityEnabled();
#ifdef PHYSICS
    OgreOde::World* getPhysicsWorld() { return mWorld; }
    void createPhysics(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry);
    void destroyPhysics();
#elif PHYSX
    NxScene* getPhysicsScene() { return mPhysicsScene; }
    void createPhysics(NxScene* physicsScene, ::ControllerManager* controllerManager);
    void destroyPhysics();
#endif
#ifdef CAPSULEGEOM
    void setMaxUpdateTimeStep(Real maxUpdateTimeStep);
    Real getMaxUpdateTimeStep();
#endif

    virtual void update(Real timeSinceLastFrame);

    void startAnimation(const String &name, bool loop = true);
    void stopAnimation();
    void animate(Real timeSinceLastFrame);

    void movementKeyPressed(OIS::KeyCode code);
    void movementKeyReleased(OIS::KeyCode code);

#ifdef PHYSICS
protected:
    // OgreOde::CollisionListener
    bool collision(OgreOde::Contact* contact);
#elif PHYSX
protected:
    // NxUserControllerHitReport
    virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit);
    virtual NxControllerAction onControllerHit(const NxControllersHit& hit);
    // NxSceneQueryReport
    virtual NxQueryReportResult onBooleanQuery(void* userData, bool result) { return NX_SQR_ABORT_ALL_QUERIES; }
	virtual NxQueryReportResult onRaycastQuery(void* userData, NxU32 nbHits, const NxRaycastHit* hits);
	virtual NxQueryReportResult onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits) { return NX_SQR_ABORT_ALL_QUERIES; }
	virtual NxQueryReportResult onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits) { return NX_SQR_ABORT_ALL_QUERIES; }
#endif

private:
    String mIdleAnimName;
    String mWalkAnimName;
    String mRunAnimName;
    String mFlyAnimName;
    String mSwimAnimName;

    KeyMotion mUpKeyMotion;
    KeyMotion mDownKeyMotion;
    KeyMotion mLeftKeyMotion;
    KeyMotion mRightKeyMotion;
    KeyMotion mPgupKeyMotion;
    KeyMotion mPgdownKeyMotion;
};

} // namespace Solipsis

#endif // #ifndef __Avatar_h__