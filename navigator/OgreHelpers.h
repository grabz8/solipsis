#ifndef __OgreHelpers_h__
#define __OgreHelpers_h__

#include "Ogre.h"

using namespace Ogre;

class OgreHelpers
{
private:
    // No instanciation
    OgreHelpers() {};

public:
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

    // Retrieve recursively from 1 scene node all movable objects of 1 type
    static void getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList);

    // Convert 1 string representation of Real into 1 Real
    static bool convertString2Real(const String& real, Real& r);
    // Convert 1 string representation of Vector3 (ie. x, y, z) into 1 Vector3
    static bool convertString2Vector3(const String& vector, Vector3& v);
};

#endif // #ifndef __OgreHelpers_h__