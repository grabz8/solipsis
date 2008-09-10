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

#include "NavigatorModule.h"
#include "Ogre.h"
#include <pthread.h>

using namespace Ogre;

namespace Solipsis {

// secure logMessage macro
#ifdef USE_WINDOWS_DEBUG
    #define OGRE_LOG(message) { \
        if (Solipsis::OgreHelpers::getSingletonPtr() != 0)  Solipsis::OgreHelpers::getSingletonPtr()->logMessage(message); \
        OutputDebugString((message + String("\n")).c_str()); \
    }
#else
    #define OGRE_LOG(message)  if (Solipsis::OgreHelpers::getSingletonPtr() != 0) Solipsis::OgreHelpers::getSingletonPtr()->logMessage(message);
#endif

/** This static class contains several helper methods above Ogre.
 */
class NAVIGATORMODULE_EXPORT OgreHelpers
{
private:
    static OgreHelpers mSingleton;
    pthread_mutex_t mLogsMutex;

private:
    // No instanciation
    OgreHelpers() :
        mLogsMutex(PTHREAD_MUTEX_INITIALIZER)
    {
//        mSingleton = this;
    }

public:
    void logMessage(const std::string& message)
    {
        pthread_mutex_lock(&mLogsMutex);
        Ogre::LogManager* logManager = Ogre::LogManager::getSingletonPtr();
        if (logManager != 0)
            logManager->logMessage(message);
        pthread_mutex_unlock(&mLogsMutex);
    }

    static OgreHelpers* getSingletonPtr() { return &mSingleton; }
    static OgreHelpers& getSingleton() { return mSingleton; }

    // Add resource locations from resources.cfg file
    static void addResourceLocations();
    // Remove resource locations from resources.cfg file
    static void removeResourceLocations();

    // Retrieve recursively from 1 scene node all movable objects of 1 type
    static void getMovableObjectsList(SceneNode* node, const String& movableType, std::list<MovableObject*> &movableObjectsList);
    // Remove and destroy 1 scene node and recursively all of its movable objects
    static void removeAndDestroySceneNode(SceneNode* node);

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