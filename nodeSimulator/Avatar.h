#ifndef __Avatar_h__
#define __Avatar_h__

#include "Entity.h"
#ifdef PHYSICSPLUGINS
#include "IPhysicsCharacter.h"
#endif

namespace Solipsis {

/** This class manages 1 avatar entity.
*/
class Avatar : public Entity
{
protected:
    /// Radius
    Real mRadius;
    /// Height
    Real mHeight;
#ifdef PHYSICSPLUGINS
    /// Physics Character
    IPhysicsCharacter* mPhysicsCharacter;
#endif
public: // TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO Entity class should have 1 processEvt() virtual method called by Node
    /// Dirty flag
    bool mDirty;
    XmlEntity mUpdatedXmlEntity;

public:
    /** Constructor. */
    Avatar(XmlEntity* xmlEntity);
    /** Destructor. */
    virtual ~Avatar();

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

#endif // #ifndef __Avatar_h__