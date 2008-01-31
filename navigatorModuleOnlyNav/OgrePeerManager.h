#ifndef __OgrePeerManager__
#define __OgrePeerManager__

#include <map>
#include <list>
#include "XmlDatas.h"
#include "tinyxml.h"
#include "Ogre.h"
#include "IOgrePeerManagerCallbacks.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages all Solipsis/Ogre peers.
*/
class OgrePeerManager : public FrameListener
{
public:
    typedef std::map<ObjectUID, XmlObject*> ObjectsMap;
    typedef std::map<ObjectUID, OgrePeer*> OgrePeersMap;
    typedef std::list<XmlEvt> EvtsList;

private:
    // Objects
    ObjectsMap mMyObjects;

    // <Peer's name, OgrePeer> map
    OgrePeersMap mOgrePeersMap;

    // List of events to send
    EvtsList mEvtsList;

	// Scene manager
	SceneManager* mSceneMgr;

    // Callbacks
    IOgrePeerManagerCallbacks* mCallbacks;

public:
	OgrePeerManager(SceneManager* sceneMgr = 0, IOgrePeerManagerCallbacks* callbacks = 0);
	~OgrePeerManager();

	// Set my objects
	void setMyObjects(std::list<ObjectUID> myObjects);

	// Load
	bool load(XmlObject* object);

	// Remove 1 peer according to its ID and if it is local or networked
    bool remove(const ObjectUID& objectUId, bool local);

    // Remove all peers (locals or networked)
    bool removeAll(bool local);

	// Update
    bool update(const ObjectUID& objectUId, XmlObject* object);

    /** See Ogre::FrameListener. */
    virtual bool frameStarted(const FrameEvent& evt);

	// Get/Set
	SceneManager* getSceneManager() { return mSceneMgr; }
    OgrePeersMap::iterator getOgrePeersIteratorBegin() { return mOgrePeersMap.begin(); }
    OgrePeersMap::iterator getOgrePeersIteratorEnd() { return mOgrePeersMap.end(); }
    EvtsList& getEvtsToSendList() { return mEvtsList; }

protected:
	virtual OgrePeer* createAvatarNode(XmlObject* object, TiXmlElement* xmlElt);
	virtual OgrePeer* createSceneNode(XmlObject* object, TiXmlElement* xmlElt);
};

} // namespace Solipsis

#endif // #ifndef __OgrePeerManager__