#ifndef __OgrePeerManager__
#define __OgrePeerManager__

#include "tinyxml.h"
#include "Ogre.h"
#include "IOgrePeerManagerCallbacks.h"

#ifdef PHYSICS
#include "OgreOde_Core.h"
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
	Ogre::SceneManager* mSceneMgr;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

    // Physics
#ifdef PHYSICS
    OgreOde::World* mPhysicsWorld;
    OgreOde::StepHandler* mPhysicsStepHandler;
    OgreOde::TriangleMeshGeometry* mPhysicsWorldGeometry;
#endif
};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager__