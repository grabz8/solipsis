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

#include "PeerModule.h"
#include "Ogre.h"
#include "OgreDefaultHardwareBufferManager.h"
/*#include "OgreMeshSerializer.h"
#include "OgreSkeletonSerializer.h"
#include "OgreDefaultHardwareBufferManager.h"
#include "OgreHardwareVertexBuffer.h"*/
#include <pthread.h>

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
class PEERMODULE_EXPORT OgreHelpers
{
private:
    static OgreHelpers* mSingleton;
    pthread_mutex_t mLogsMutex;

protected:
    Ogre::Root* mRoot;
    bool mRootAllocated;
    Ogre::MeshSerializer* mMeshSerializer;
    Ogre::DefaultHardwareBufferManager *mHWBufferManager;
/*    Ogre::LogManager* mLogManager;
    Ogre::ResourceGroupManager* mResourceGroupManager;
    Ogre::Math* mMath;
    Ogre::MaterialManager* mMaterialManager;
    Ogre::SkeletonManager* mSkeletonManager;
    Ogre::MeshSerializer* mMeshSerializer;
    Ogre::SkeletonSerializer* mSkeletonSerializer;
    Ogre::DefaultHardwareBufferManager *mHWBufferManager;
    Ogre::MeshManager* mMeshManager;
    Ogre::Timer* mTimer;*/

private:
    /// No instanciation
    OgreHelpers() :
        mLogsMutex(PTHREAD_MUTEX_INITIALIZER),
        mRoot(0),
        mMeshSerializer(0),
        mHWBufferManager(0)
/*        mLogManager(0),
        mResourceGroupManager(0),
        mMath(0),
        mMaterialManager(0),
        mSkeletonManager(0),
        mMeshSerializer(0),
        mSkeletonSerializer(0),
        mHWBufferManager(0),
        mMeshManager(0),
        mTimer(0)*/
    {
        mSingleton = this;
    }
    virtual ~OgreHelpers()
    {
        _shutdown();
    }
    bool _initialize();
    void _shutdown();

public:
    static bool initialize();
    static void shutdown();

    void logMessage(const std::string& message)
    {
        pthread_mutex_lock(&mLogsMutex);
        Ogre::LogManager* logManager = Ogre::LogManager::getSingletonPtr();
        if (logManager != 0)
            logManager->logMessage(message);
        pthread_mutex_unlock(&mLogsMutex);
    }

    static OgreHelpers* getSingletonPtr() { return mSingleton; }
    static OgreHelpers& getSingleton() { return *mSingleton; }

    /// Add resource locations from resources.cfg file
    static void addResourceLocations();
    /// Remove resource locations from resources.cfg file
    static void removeResourceLocations();

    /// Load 1 mesh file
    Ogre::Mesh* loadMesh(const Ogre::String& filename);

    /// Retrieve mesh informations
    void getMeshInformation(Ogre::Mesh* mesh,
        size_t &vertex_count,
        Ogre::Vector3*& vertices,
        Ogre::Vector2*& texCoords,
        size_t &index_count,
        unsigned long*& indices,
        const Ogre::Vector3& position,
        const Ogre::Quaternion& orientation,
        const Ogre::Vector3& scale);
};

} // namespace Solipsis

#endif // #ifndef __OgreHelpers_h__