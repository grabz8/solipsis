#ifndef __Avatar_h__
#define __Avatar_h__

#include "OgrePeer.h"
#include "KeyMotion.h"
#include "MovableText.h"

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

#ifdef PHYSICS
#include "OgreOde_Core.h"
#endif

class Avatar : public OgrePeer
#ifdef PHYSICS
    ,
    public OgreOde::CollisionListener
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
    OgreOde::RayGeometry* mRayGeom;
#ifdef CAPSULEGEOM
    Ogre::Real mMaxUpdateTimeStep;
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
#else
    RaySceneQuery* mRaySceneQuery;
#endif

#ifdef PHYSICS
    bool collision(OgreOde::Contact* contact);
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
    void createPhysicsRayGeometry(OgreOde::World* world, OgreOde::TriangleMeshGeometry* worldGeometry);
#endif
#ifdef CAPSULEGEOM
    void setMaxUpdateTimeStep(Ogre::Real maxUpdateTimeStep);
    Ogre::Real getMaxUpdateTimeStep();
#endif

    virtual void update(Ogre::Real timeSinceLastFrame);

    void startAnimation(const String &name, bool loop = true);
    void stopAnimation();
    void animate(Ogre::Real timeSinceLastFrame);

    void movementKeyPressed(OIS::KeyCode code);
    void movementKeyReleased(OIS::KeyCode code);

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

#endif // #ifndef __Avatar_h__