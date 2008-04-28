#ifndef __FModSpeexEngine_h__
#define __FModSpeexEngine_h__

#include "IVoiceEngine.h"
#include "voiceengine.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages the FModSpeex engine.
*/
class FModSpeexEngine : public IVoiceEngine
{
private:
    /// Logging instance
    IVoiceEngineLogger* mLogger;

    /// Voice engine
    VoiceEngine *mVoiceEngine;

public:
    FModSpeexEngine() : mVoiceEngine(0) {}

    /// @copydoc IVoiceEngine::getName
    virtual const String& getName() const;

    /// @copydoc IVoiceEngine::init
    virtual bool init();

    /// @copydoc IVoiceEngine::shutdown
    virtual bool shutdown();

    /// @copydoc IVoiceEngine::createScene
    virtual bool initSoundSystem(FMOD::System* system, size_t networkChunkSizePCM = 6000, unsigned int bufferFrameCount = 4, unsigned int frequency = 16000);

    /// @copydoc IVoiceEngine::createScene
    virtual bool shutdownSoundSystem();

    /// @copydoc IVoiceEngine::createScene
    virtual bool connect(const char* host, int port, unsigned int id);

    /// @copydoc IVoiceEngine::createScene
    virtual void disconnect();

    /// @copydoc IVoiceEngine::createScene
    virtual void update();

    /// @copydoc IVoiceEngine::createScene
    virtual void startRecording();

    /// @copydoc IVoiceEngine::createScene
    virtual void stopRecording();

    /// @copydoc IVoiceEngine::createScene
    virtual bool isRecording();

    /// @copydoc IVoiceEngine::setLogger
    virtual void setLogger(IVoiceEngineLogger* logger) { mLogger = logger; }

    /// log a message
    inline void logMessage(const std::string& message) { if (mLogger != 0) mLogger->logMessage(message); }
};

} // namespace Solipsis

#endif // #ifndef __FModSpeexEngine_h__