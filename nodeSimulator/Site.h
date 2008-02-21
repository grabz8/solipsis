#ifndef __Site_h__
#define __Site_h__

#include "Entity.h"

namespace Solipsis {

/** This class manages 1 site entity.
*/
class Site : public Entity
{
public:
    /** Constructor. */
    Site(XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~Site();

#ifdef PHYSICSPLUGINS
    /** See Entity. */
    virtual void createPhysics(IPhysicsScene* physicsScene);
    /** See Entity. */
    virtual void destroyPhysics();
#endif

    /** See Entity. */
    virtual bool update(Real timeSinceLastFrame);
};

} // namespace Solipsis

#endif // #ifndef __Site_h__