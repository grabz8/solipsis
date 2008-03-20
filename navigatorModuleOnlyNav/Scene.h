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
#ifdef POOL
    Scene(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, SceneNode* sceneNode, RaySceneQuery* raySceneQuery = 0);
#else
    Scene(XmlEntity* xmlEntity, bool isLocal, SceneNode* sceneNode, RaySceneQuery* raySceneQuery = 0);
#endif
    virtual ~Scene();

    // Set and get
    SceneNode* getSceneNode();

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);
    /** See OgrePeer. */
#ifdef POOL
    virtual bool update(RefCntPoolPtr<XmlEntity>& xmlEntity);
#else
    virtual bool update(XmlEntity* xmlEntity);
#endif

protected:
    void destroy();
    void convertToStaticGeometry();
    void getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList);
};

} // namespace Solipsis

#endif // #ifndef __Scene_h__