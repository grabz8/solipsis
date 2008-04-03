#ifndef __IPhysicsEngine_h__
#define __IPhysicsEngine_h__

#include "PeerModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

namespace Solipsis {

/** This class provide logging capacities interface.
 */
class IPhysicsEngineLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class represents a generic Physics engine.
*/
class PEERMODULE_EXPORT IPhysicsEngine
{
public:
    IPhysicsEngine() {}
    virtual ~IPhysicsEngine() {}

    /** Get the name of the physics engine.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual const Ogre::String& getName() const = 0;

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

    /** setLogger.
    @remarks An implementation must be supplied for this method.
    @param logger The logger instance
    */
    virtual void setLogger(IPhysicsEngineLogger* logger) = 0;
};

} // end namespace

#endif // #ifndef __IPhysicsEngine_h__