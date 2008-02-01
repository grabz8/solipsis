#ifndef __IPhysicsEngine_h__
#define __IPhysicsEngine_h__

#include "NavigatorModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents a generic Physics engine.
*/
class NAVIGATORMODULE_EXPORT IPhysicsEngine
{
public:
    IPhysicsEngine() {}
    virtual ~IPhysicsEngine() {}

    /** Get the name of the physics engine.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual const String& getName() const = 0;

    /** Perform the initialization. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool init() = 0;

    /** Perform any tasks when the system is shut down.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdown() = 0;

    /** Create 1 scene.
    @remarks An implementation must be supplied for this method.
    */
    virtual IPhysicsScene* createScene() = 0;

    /** Destroy 1 scene.
    @remarks An implementation must be supplied for this method.
    */
    virtual void destroyScene(IPhysicsScene* scene) = 0;
};

} // namespace Solipsis

#endif // #ifndef __IPhysicsEngine_h__