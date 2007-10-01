#ifndef __Avatar_h__
#define __Avatar_h__

#include "OgrePeer.h"
#include "KeyMotion.h"
#include "MovableText.h"

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

class Avatar : public OgrePeer
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

    bool mGravity;

    SceneNode* mSceneNode;
    Entity* mEntity;
    AnimationState* mAnimationState;
    RaySceneQuery* mRaySceneQuery;
    MovableText* mNameLabel;

//    void lookAtTheGoodDirection();

/*    bool RaycastFromPoint(Ray& ray,
        RaySceneQueryResult& query_result,
        String& entity_name,
        Vector3& result);
    void GetMeshInformation(const Ogre::MeshPtr mesh,
        size_t &vertex_count,
        Ogre::Vector3* &vertices,
        size_t &index_count,
        unsigned long* &indices,
        const Ogre::Vector3 &position,
        const Ogre::Quaternion &orient,
        const Ogre::Vector3 &scale);
*/
public:
    Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity, RaySceneQuery* raySceneQuery = 0);
    virtual ~Avatar();

    //Set and get
    SceneNode* getSceneNode();
    Entity* getEntity();
    void setName(const String& name);
    void setNameVisibility(bool visible);
    void setGravity(bool enabled);

    void setState(State state);
    State getState();
    void setStateAnimName(State state, const String& name);

    void setMvtType(MvtType mvtType);
    MvtType getMvtType();

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