#ifndef __OgrePeerManager_h__
#define __OgrePeerManager_h__

#include "tinyxml.h"
#include "Ogre.h"
#include "IOgrePeerManagerCallbacks.h"

#ifdef PHYSICS
#include "OgreOde_Core.h"
    #ifdef _DEBUG
    #pragma comment(lib, "OgreOde_Core_d.lib")
    #else
    #pragma comment(lib, "OgreOde_Core.lib")
    #endif
#elif PHYSX
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma comment(lib, "NxCharacter.lib")
#include "NxPhysics.h"
#include "NxController.h"
#include "ControllerManager.h"
#include "PhysXHelpers.h"
#elif TOKAMAK
#pragma comment(lib, "tokamakdll.lib")
#include "tokamak.h"
#include "TokamakHelpers.h"
#elif PHYSICSPLUGINS
#include "IPhysicsScene.h"
#include "PhysicsEngineManager.h"
#endif

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis/Ogre peers.
*/
class OgrePeerManager : public FrameListener
#ifdef PHYSICS
    ,
    public OgreOde::CollisionListener
#endif
{
public:
    typedef std::map<String, OgrePeer*> OgrePeersMap;

private:
    // <Peer's name, OgrePeer> map
    OgrePeersMap mOgrePeersMap;

    // Scene manager
    SceneManager* mSceneMgr;

    // Scene object filename
    std::string xmlObjectFilename;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

    // Physics
#ifdef PHYSICS
    OgreOde::World* mPhysicsWorld;
    OgreOde::StepHandler* mPhysicsStepHandler;
    OgreOde::TriangleMeshGeometry* mPhysicsWorldGeometry;
#elif PHYSX
    NxScene* mPhysicsScene;
    ::ControllerManager* mControllerManager;
    NxTriangleMesh* mPhysicsWorldGeometry;
    NxActor* mPhysicsWorldActor;
#elif TOKAMAK
    neSimulator* mPhysicsSim;
    neTriangleMesh mPhysicsWorldGeometry;
    std::map<String, neRigidBody*> mPhysicsBodies;
#elif PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

public:
    OgrePeerManager(SceneManager* sceneMgr = 0, IOgrePeerManagerCallbacks* callbacks = 0);
    ~OgrePeerManager();

    // Load
    bool load(Peer* peer, const String xmlFile);

    // Get the scene object filename
    String getXmlObjectFilename();

    // Remove 1 peer according to its ID and if it is local or networked
    bool remove(String& peerId, bool local);

    // Remove all peers (locals or networked)
    bool removeAll(bool local);

    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

    // Get/Set
    SceneManager* getSceneManager() { return mSceneMgr; }
    OgrePeersMap::iterator getOgrePeersIteratorBegin() { return mOgrePeersMap.begin(); }
    OgrePeersMap::iterator getOgrePeersIteratorEnd() { return mOgrePeersMap.end(); }
#ifdef PHYSICS
    OgreOde::World* getPhysicsWorld();
    OgreOde::StepHandler* getPhysicsStepHandler();
    OgreOde::TriangleMeshGeometry* getPhysicsWorldGeometry();
#elif PHYSX
    NxScene* getPhysicsScene();
    ::ControllerManager* getControllerManager();
    NxTriangleMesh* getPhysicsWorldGeometry();
    NxActor* getPhysicsWorldActor();
#elif TOKAMAK
    neSimulator* getPhysicsSim();
    neTriangleMesh& getPhysicsWorldGeometry();
    std::map<String, neRigidBody*>& getPhysicsBodies();
#elif PHYSICSPLUGINS
    IPhysicsScene* getPhysicsScene();
#endif

protected:
    virtual OgrePeer* createAvatarNode(Peer* peer, TiXmlElement* xmlElt);
    virtual OgrePeer* createSceneNode(Peer* peer, TiXmlElement* xmlElt);
    virtual OgrePeer* createObjectNode(Peer* peer, TiXmlElement* xmlElt);

#ifdef PHYSICS
protected:
    /** See OgreOde::CollisionListener. */
    virtual bool collision(OgreOde::Contact* contact);
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager_h__