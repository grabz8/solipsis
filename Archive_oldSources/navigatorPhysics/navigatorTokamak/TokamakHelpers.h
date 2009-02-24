/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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