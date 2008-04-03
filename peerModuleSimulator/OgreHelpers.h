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
#define OGRE_LOG(message) if (Solipsis::OgreHelpers::getSingletonPtr() != 0) Solipsis::OgreHelpers::getSingletonPtr()->logMessage(message);

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
    // No instanciation
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

    Ogre::Timer* getTimer();

    Ogre::Mesh* loadMesh(const Ogre::String& filename);

    // Retrieve mesh informations
    void getMeshInformation(Ogre::Mesh* mesh,
        size_t &vertex_count,
        Ogre::Vector3*& vertices,
        Ogre::Vector2*& texCoords,
        size_t &index_count,
        unsigned long*& indices,
        const Ogre::Vector3& position,
        const Ogre::Quaternion& orientation,
        const Ogre::Vector3& scale);

protected:
    void addResourceLocations();
};

} // namespace Solipsis

#endif // #ifndef __OgreHelpers_h__