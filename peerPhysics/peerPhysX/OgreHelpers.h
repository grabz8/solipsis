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
        const Quaternion& orientation,
        const Vector3& scale);
};

} // namespace Solipsis

#endif // #ifndef __OgreHelpers_h__