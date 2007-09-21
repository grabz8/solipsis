#ifndef __Scene_h__
#define __Scene_h__

#include "OgrePeer.h"

using namespace Ogre;

class Scene : public OgrePeer
{
protected:
    SceneNode* mSceneNode;
    RaySceneQuery* mRaySceneQuery;

public:
    Scene(Peer* peer, SceneNode* sceneNode, RaySceneQuery* raySceneQuery = 0);
    virtual ~Scene();

    // Set and get
    SceneNode* getSceneNode();

    virtual void update(Ogre::Real timeSinceLastFrame);
};

#endif // #ifndef __Scene_h__