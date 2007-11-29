#ifndef __Scene_h__
#define __Scene_h__

#include "OgrePeer.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a 3D scene.
 */
class Scene : public OgrePeer
{
protected:
    SceneNode* mSceneNode;
    StaticGeometry* mStaticGeometry;
    RaySceneQuery* mRaySceneQuery;

public:
    Scene(Peer* peer, SceneNode* sceneNode, RaySceneQuery* raySceneQuery = 0);
    virtual ~Scene();

    // Set and get
    SceneNode* getSceneNode();

    /** These methods implement OgrePeer
    */
    virtual void update(Real timeSinceLastFrame);

protected:
    void destroy();
    void convertToStaticGeometry();
    void getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList);
};

} // namespace Solipsis

#endif // #ifndef __Scene_h__