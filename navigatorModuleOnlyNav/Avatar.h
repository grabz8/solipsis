#ifndef __Avatar_h__
#define __Avatar_h__

#include "OgrePeer.h"
#include "KeyMotion.h"
#include "MovableText.h"
#include "Event.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an avatar.
*/
class Avatar : public OgrePeer
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
    /// Updated entity
#ifdef POOL
    RefCntPoolPtr<XmlEntity> mUpdatedXmlEntity;
#else
    XmlEntity* mUpdatedXmlEntity;
#endif
    /// Default animation names
    static String mDefaultStateAnimName[SCount];
    /// Animation names
    String mStateAnimName[SCount];
    /// Current state
    State mState;
    /// Current movement type
    MvtType mMvtType;
    Vector3 mLastRealPosition;

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
    /** Constructor. */
#ifdef POOL
    Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, SceneNode* sceneNode, Entity* entity);
#else
    Avatar(XmlEntity* xmlEntity, bool isLocal, SceneNode* sceneNode, Entity* entity);
#endif
    /** Destructor. */
    virtual ~Avatar();

    /** Gets the updated entity. */
#ifdef POOL
    virtual RefCntPoolPtr<XmlEntity>& getUpdatedXmlEntity()
#else
    virtual XmlEntity* getUpdatedXmlEntity()
#endif
    {
        if (isLocal() && mUpdatedXmlEntity->getDefinedAttributes() & (
            XmlEntity::DAFlags |
            XmlEntity::DADisplacement |
            XmlEntity::DAPosition |
            XmlEntity::DAOrientation))
            return mUpdatedXmlEntity;
#ifdef POOL
        return RefCntPoolPtr<XmlEntity>::nullPtr;
#else
        return 0;
#endif
    }

    /** Get the scene node. */
    SceneNode* getSceneNode();
    /** Get the entity. */
    Entity* getEntity();
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

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);
    /** See OgrePeer. */
#ifdef POOL
    virtual bool update(RefCntPoolPtr<XmlEntity>& xmlEntity);
#else
    virtual bool update(XmlEntity* xmlEntity);
#endif

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

    /** Rotate the avatar around the Y-axis. */
    void yaw(const Radian& angle);
};

} // namespace Solipsis

#endif // #ifndef __Avatar_h__