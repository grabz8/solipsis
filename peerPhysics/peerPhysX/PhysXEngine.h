#ifndef __PhysXEngine_h__
#define __PhysXEngine_h__

#include "IPhysicsEngine.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages the PhysX engine.
*/
class PhysXEngine : public IPhysicsEngine
{
private:
    /// Logging instance
    IPhysicsEngineLogger* mLogger;

public:
    PhysXEngine() {}

    /// @copydoc IPhysicsEngine::getName
    virtual const String& getName() const;

    /// @copydoc IPhysicsEngine::init
    virtual bool init();

    /// @copydoc IPhysicsEngine::shutdown
    virtual bool shutdown();

    /// @copydoc IPhysicsEngine::createScene
    virtual IPhysicsScene* createScene();

    /// @copydoc IPhysicsEngine::destroyScene
    virtual void destroyScene(IPhysicsScene* scene);

    /// @copydoc IPhysicsEngine::setLogger
    virtual void setLogger(IPhysicsEngineLogger* logger) { mLogger = logger; }

    /// log a message
    inline void logMessage(const std::string& message) { if (mLogger != 0) mLogger->logMessage(message); }
};

} // namespace Solipsis

#endif // #ifndef __PhysXEngine_h__