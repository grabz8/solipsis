#ifndef __Avatar_h__
#define __Avatar_h__

#include <string>
#include <list>

#include "CTBasicThread.h"

#include "VoiceServerConnection.h"

namespace SolipsisVoiceServer {

  //! A class to handle avatar
  class Avatar : public CommonTools::BasicThread, public VoiceServerConnectionListener {
  public:

    //! Constructor
    Avatar(const VoiceUUID& id, VoiceServerConnection* connection);

    /** See BasicThread. */
    virtual void stop(unsigned int stopTimeoutSec = 5);

    //! Return the unique identifier
    VoiceUUID& getId() { return mId; }

    //! Return the connection.
    VoiceServerConnection* getConnection() const { return _connection; }

  protected:
    /// See VoiceServerConnectionListener::audioDatasReceived
    virtual void audioDatasReceived(AudioDatasPtr& audioDatasPtr);

  private:
    /** See BasicThread. */
    void run();

    /** See BasicThread. */
    void end();

    int sendPacketHeader(char type, unsigned int size);
    int sendAudioFrames(AudioDatasPtr& audioDatasPtr);

  protected:

    // Unique client identifier
    VoiceUUID mId;

    // The voice server connection
    VoiceServerConnection* _connection;

    // List of audio datas to send
    std::list<AudioDatasPtr> mAudioDatasPtrs;

    // Mutex on audio datas pointers list
    pthread_mutex_t mAudioDatasPtrsMutex;

    // Gate mutex
    pthread_mutex_t mAudioDatasPtrsGate;
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __Avatar_h__