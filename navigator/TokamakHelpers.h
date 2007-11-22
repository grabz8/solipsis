#ifndef __TokamakHelpers_h__
#define __TokamakHelpers_h__

#ifdef TOKAMAK

#include "Ogre.h"
#include "tokamak.h"

using namespace Ogre;

namespace Solipsis {

class TokamakHelpers
{
private:
    // No instanciation
    TokamakHelpers() {}

public:
    // Convert 1 mesh into 1 neTriangleMesh
    static neTriangleMesh convertMesh(const MeshPtr mesh,
        const Vector3& position,
        const Quaternion& orient,
        const Vector3& scale);
};

} // namespace Solipsis

#endif

#endif // #ifndef __TokamakHelpers_h__