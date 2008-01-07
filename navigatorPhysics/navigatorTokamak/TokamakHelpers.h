#ifndef __TokamakHelpers_h__
#define __TokamakHelpers_h__

#include "Ogre.h"
#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

/** This static class contains several helper methods above Tokamak physics engine.
 */
class TokamakHelpers
{
private:
    // No instanciation
    TokamakHelpers() {}

public:
    /// Conversions
    static inline Vector3 neV32Vector3(neV3 v) { return Vector3(v.v[0], v.v[1], v.v[2]); }
    static inline neV3 Vector32neV3(Vector3 v)
    {
        neV3 neV;
        neV.Set(v.x, v.y, v.z);
        return neV;
    }
    static inline Quaternion neQ2Quaternion(neQ q) { return Quaternion(q.W, q.X, q.Y, q.Z); }
    static inline neQ Quaternion2neQ(Quaternion q) { return neQ(q.x, q.y, q.z, q.w); }

    // Convert 1 mesh into 1 neTriangleMesh
    static neTriangleMesh convertMesh(const MeshPtr mesh,
        const Vector3& position,
        const Quaternion& orientation,
        const Vector3& scale);
};

} // namespace Solipsis

#endif // #ifndef __TokamakHelpers_h__