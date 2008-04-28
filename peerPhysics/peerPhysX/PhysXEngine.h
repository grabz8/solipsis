#ifndef __PhysXEngine_h__
#define __PhysXEngine_h__

#include <IPhysicsEngine.h>
#include <NxUserOutputStream.h>
#include <stdio.h>
#include <stdlib.h>

using namespace Ogre;

namespace Solipsis {

/** This class manages the PhysX engine.
*/
class PhysXEngine : public IPhysicsEngine, public NxUserOutputStream
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

    /// See NxUserOutputStream
    void reportError(NxErrorCode code, const char *message, const char* file, int line)
    {
        // this should be routed to the application specific error handling. If this gets hit
        // then you are in most cases using the SDK wrong and you need to debug your code!
        // however, code may  just be a warning or information.
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::reportError() error %d, %s", code, message);
        logMessage(reportLog);
//        exit(1);
    }
    /// See NxUserOutputStream
    NxAssertResponse reportAssertViolation(const char *message, const char *file, int line)
    {
        // this should not get hit by a properly debugged SDK!
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::reportAssertViolation() %s", message);
        logMessage(reportLog),
        assert(0);
        return NX_AR_CONTINUE;
    }
    /// See NxUserOutputStream
    void print(const char *message)
    {
        // just a information message
        char reportLog[256];
        _snprintf(reportLog, sizeof(reportLog) - 1, "PhysXEngine::OutputStream::print() %s", message);
        logMessage(reportLog);
    }
};

} // namespace Solipsis

#endif // #ifndef __PhysXEngine_h__