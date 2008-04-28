#ifndef __IVoiceEngine_h__
#define __IVoiceEngine_h__

#include "NavigatorModule.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"

using namespace Ogre;

namespace FMOD
{
    class System;
}

namespace Solipsis {

/** This class provide logging capacities interface.
 */
class IVoiceEngineLogger
{
public:
    virtual void logMessage(const std::string& message) = 0;
};

/** This class represents a generic Voice engine.
*/
class NAVIGATORMODULE_EXPORT IVoiceEngine
{
public:
    IVoiceEngine() {}
    virtual ~IVoiceEngine() {}

    /** Get the name of the voice engine.
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

    /** Perform the initialization with the sound system. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool initSoundSystem(FMOD::System* system, size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000) = 0;

    /** Perform the finalization with the sound system. 
    @remarks An implementation must be supplied for this method.
    */
    virtual bool shutdownSoundSystem() = 0;

    /** Connect to a voice server.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool connect(const char* host, int port, unsigned int id) = 0;

    /** Disconnect from the voice server.
    @remarks An implementation must be supplied for this method.
    */
    virtual void disconnect() = 0;

    /** Update the voice engine, this should be called once per frame.
    @remarks An implementation must be supplied for this method.
    */
    virtual void update() = 0;

    /** Start recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void startRecording() = 0;

    /** Stop recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual void stopRecording() = 0;

    /** Returns true if engine is recording.
    @remarks An implementation must be supplied for this method.
    */
    virtual bool isRecording() = 0;
};

} // namespace Solipsis

#endif // #ifndef __IVoiceEngine_h__