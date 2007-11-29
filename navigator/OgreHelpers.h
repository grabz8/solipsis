#ifndef __OgreHelpers_h__
#define __OgreHelpers_h__

#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

// secure logMessage macro
#define OGRE_LOG(message) if (LogManager::getSingletonPtr()) LogManager::getSingletonPtr()->logMessage(message);

/** This static class contains several helper methods above Ogre.
 */
class OgreHelpers
{
private:
    // No instanciation
    OgreHelpers() {};

public:
    // Retrieve recursively from 1 scene node all movable objects of 1 type
    static void getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList);

    // Convert 1 string representation of Real into 1 Real
    static bool convertString2Real(const String& real, Real& r);
    // Convert 1 string representation of Vector3 (ie. x, y, z) into 1 Vector3
    static bool convertString2Vector3(const String& vector, Vector3& v);

    // Retrieve mesh informations
    static void getMeshInformation(const MeshPtr mesh,
        size_t &vertex_count,
        Vector3*& vertices,
        Vector2*& texCoords,
        size_t &index_count,
        unsigned long*& indices,
        const Vector3& position,
        const Quaternion& orient,
        const Vector3& scale);
    // Ray/triangle intersection return boolean result, distance and barycentric u,v
    // to compute texture coordinates
    static bool getIntersection(const Ray& ray,
        const Vector3& a,
        const Vector3& b,
        const Vector3& c,
        Real& distance,
        Vector2& uv);
    // Ray/entity intersection return boolean result
    static bool isEntityHitByMouse(const Ray& ray, Entity* entity,
        Real& closestDistance,
        Vector2& closestUV,
        Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2);
};

} // namespace Solipsis

#endif // #ifndef __OgreHelpers_h__