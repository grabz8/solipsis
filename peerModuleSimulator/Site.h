#ifndef __Site_h__
#define __Site_h__

#include "Entity.h"

namespace Solipsis {

/** This class manages 1 site entity.
*/
class Site : public Entity
{
protected:
    /// OSM filename
    std::string mOSMFilename;
    /// Collision mesh filename
    std::string mCollisionMeshFilename;
    /// Position of the entry gate
    Ogre::Vector3 mGatePosition;

public:
    /** Constructor. */
    Site(XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~Site();

    /// Get the position of the entry gate
    const Ogre::Vector3& getGatePosition() { return mGatePosition; }

#ifdef PHYSICSPLUGINS
    /** See Entity. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** See Entity. */
    virtual void destroyPhysics();
#endif

    /** See Entity. */
    virtual bool update(Ogre::Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __Site_h__