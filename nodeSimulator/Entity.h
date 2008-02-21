#ifndef __Entity_h__
#define __Entity_h__

#include "Ogre.h"
#include "XmlDatas.h"
#ifdef PHYSICSPLUGINS
#include "IPhysicsScene.h"
#endif

using namespace Ogre;

namespace Solipsis {

/** This class manages 1 entity by its descriptor.
*/
class Entity
{
public:
    /// <EntityUID, Entity*> map
    typedef std::map<EntityUID, Entity*> EntityMap;
    /// Xml Entity cache
    XmlEntity mUpdatedXmlEntity;

protected:
    /// Entity descriptor
    XmlEntity* mXmlEntity;
    /// Whether to apply the gravity
    bool mGravity;
#ifdef PHYSICSPLUGINS
    /// Physics scene
    IPhysicsScene* mPhysicsScene;
#endif

public:
    /** Constructor. */
    Entity(XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~Entity();

    /** Get the entity descriptor. */
    XmlEntity* getXmlEntity();
    /** Set whether the gravity is applied or not. */
    void setGravity(bool enabled);
    /** Determines whether the gravity is applied or not. */
    bool isGravityEnabled();
#ifdef PHYSICSPLUGINS
    /** Get the physics scene. */
    IPhysicsScene* getPhysicsScene();
    /** Create physics. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** Destroy physics. */
    virtual void destroyPhysics();
#endif

    /** Update. */
    virtual bool update(Real timeSinceLastFrame) = 0;
};

} // namespace Solipsis

#endif // #ifndef __Entity_h__