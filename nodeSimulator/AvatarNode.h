#ifndef __AvatarNode_h__
#define __AvatarNode_h__

#include <map>
#include <pthread.h>
#include "Ogre.h"
#include "Node.h"
#include "Avatar.h"
#include "TimeListener.h"

#ifdef PHYSICSPLUGINS
#include "IPhysicsScene.h"
#include "PhysicsEngineManager.h"
#endif

namespace Solipsis {

/** This class manages 1 avatar node.
*/
class AvatarNode : public Node, public TimeListener, public EntityListener
{
public:
    /// <EntityUID, XmlEntity*> map
    typedef std::map<EntityUID, XmlEntity*> XmlEntityMap;

protected:
    /// Mutex
    pthread_mutex_t mMutex;
    /// Avatar entity
    Avatar mAvatar;

    /// Map of owned entities
    Entity::EntityMap mOwnedEntities;
    /// Map of entities avatar is aware of
    Entity::EntityMap mAwareEntities;

    XmlEntityMap mXmlEntityMap;

#ifdef PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

public:
    /** Constructor. */
    AvatarNode(const NodeId& nodeId, XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~AvatarNode();

	/** Get associated avatar entity. */
    Avatar& getEntity();
	/** Get associated avatar owned entities. */
    Entity::EntityMap& getOwnedEntities();
#ifdef PHYSICSPLUGINS
    IPhysicsScene* getPhysicsScene();
#endif

    /** Add an entity to aware of. */
    bool addAwareEntity(Entity* entity);
    /** Remove an entity to aware of. */
    bool removeAwareEntity(Entity* entity);

    /** See Solipsis::Node. */
    virtual bool processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr);
    /** See Solipsis::Node. */
    virtual bool freeEvt(XmlEvt* evt);
    /** See Solipsis::Node. */
    virtual bool freeze(bool frozen);

    /** See Solipsis::TimeListener. */
    virtual bool tick(Real timeSinceLastTick);

    /** See Solipsis::EntityListener. */
    virtual bool updated(const Node& node, Entity& entity, XmlEvt& xmlEvt);
};

} // namespace Solipsis

#endif // #ifndef __AvatarNode_h__