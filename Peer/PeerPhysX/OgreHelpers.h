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

#ifndef __OgreHelpers_h__
#define __OgreHelpers_h__

#include <Ogre.h>

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