#ifndef __OgrePeerManager__
#define __OgrePeerManager__

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
#include "PhysXHelpers.h"
#elif TOKAMAK
#pragma comment(lib, "tokamakdll.lib")
#include "tokamak.h"
#include "TokamakHelpers.h"
#endif

using namespace Ogre;

namespace Solipsis {

class OgrePeerManager : public FrameListener
#ifdef PHYSICS
    ,
    public OgreOde::CollisionListener
#endif
{
public:
	OgrePeerManager(SceneManager* sceneMgr = 0, IOgrePeerManagerCallbacks* callbacks = 0);
	~OgrePeerManager();

	// Load
	bool load(Peer* peer, const String xmlFile);

	// Remove 1 peer according to its ID and if it is local or networked
    bool remove(String& peerId, bool local);

    // Remove all peers (locals or networked)
    bool removeAll(bool local);

    // Ogre::FrameListener
    virtual bool frameStarted(const FrameEvent& evt);

	// Get/Set
	SceneManager* getSceneManager() { return mSceneMgr; }
    std::map<String,OgrePeer*>::iterator getOgrePeersIteratorBegin() { return mOgrePeersMap.begin(); }
    std::map<String,OgrePeer*>::iterator getOgrePeersIteratorEnd() { return mOgrePeersMap.end(); }
#ifdef PHYSICS
    OgreOde::World* getPhysicsWorld();
    OgreOde::StepHandler* getPhysicsStepHandler();
    OgreOde::TriangleMeshGeometry* getPhysicsWorldGeometry();
#elif PHYSX
    NxScene* getPhysicsScene();
    NxTriangleMesh* getPhysicsWorldGeometry();
    NxActor* getPhysicsWorldActor();
#elif TOKAMAK
    neSimulator* getPhysicsSim();
    neTriangleMesh& getPhysicsWorldGeometry();
    std::map<String, neRigidBody*>& getPhysicsBodies();
#endif

protected:
	virtual OgrePeer* createAvatarNode(Peer* peer, TiXmlElement* xmlElt);
	virtual OgrePeer* createSceneNode(Peer* peer, TiXmlElement* xmlElt);

#ifdef PHYSICS
private:
    // OgreOde::CollisionListener
    virtual bool collision(OgreOde::Contact* contact);
#endif

private:
    // <Peer's name, OgrePeer> map
    std::map<String,OgrePeer*> mOgrePeersMap;

	// Scene manager
	SceneManager* mSceneMgr;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

    // Physics
#ifdef PHYSICS
    #define physicsScale 10.0f
    OgreOde::World* mPhysicsWorld;
    OgreOde::StepHandler* mPhysicsStepHandler;
    OgreOde::TriangleMeshGeometry* mPhysicsWorldGeometry;
#elif PHYSX
    #define physicsScale 10.0f
    NxScene* mPhysicsScene;
    NxTriangleMesh* mPhysicsWorldGeometry;
    NxActor* mPhysicsWorldActor;
#elif TOKAMAK
    #define physicsScale 10.0f
    neSimulator* mPhysicsSim;
    neTriangleMesh mPhysicsWorldGeometry;
    std::map<String, neRigidBody*> mPhysicsBodies;
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager__