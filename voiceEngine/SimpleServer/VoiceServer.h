#ifndef __VoiceServer_h__
#define __VoiceServer_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <map>
# include <string>
#endif

#include "VoiceServerException.h"
#include "VoiceServerUtil.h"

#include "VoiceServerDispatch.h"
#include "VoiceServerSource.h"

#include <pthread.h>
#include <string>

#include "voiceuuid.h"

#include <boost/pool/pool.hpp>
#include <boost/shared_ptr.hpp>

namespace SolipsisVoiceServer {

  //! An interface allowing custom handling of error message reporting.
  class VoiceServerErrorHandler {
  public:
    //! Returns a pointer to the currently installed error handling object.
    static VoiceServerErrorHandler* getErrorHandler() 
    { return _errorHandler; }

    //! Specifies the error handler.
    static void setErrorHandler(VoiceServerErrorHandler* eh)
    { _errorHandler = eh; }

    //! Report an error. Custom error handlers should define this method.
    virtual void error(const char* msg) = 0;

  protected:
    static VoiceServerErrorHandler* _errorHandler;
  };

  //! An interface allowing custom handling of informational message reporting.
  class VoiceServerLogHandler {
  public:
    //! Returns a pointer to the currently installed message reporting object.
    static VoiceServerLogHandler* getLogHandler() 
    { return _logHandler; }

    //! Specifies the message handler.
    static void setLogHandler(VoiceServerLogHandler* lh)
    { _logHandler = lh; }

    //! Returns the level of verbosity of informational messages. 0 is no output, 5 is very verbose.
    static int getVerbosity() 
    { return _verbosity; }

    //! Specify the level of verbosity of informational messages. 0 is no output, 5 is very verbose.
    static void setVerbosity(int v) 
    { _verbosity = v; }

    //! Output a message. Custom error handlers should define this method.
    virtual void log(int level, const char* msg) = 0;

  protected:
    static VoiceServerLogHandler* _logHandler;
    static int _verbosity;
  };

  //! Returns log message verbosity. This is short for VoiceServerLogHandler::getVerbosity()
  int getVerbosity();
  //! Sets log message verbosity. This is short for VoiceServerLogHandler::setVerbosity(level)
  void setVerbosity(int level);


  //! Version identifier
  extern const char VOICESERVER_VERSION[];

  // Class representing connections to specific clients
  class VoiceServerConnection;

  // Class representing an avatar
  class Avatar;

  //! A class to handle voIP
  class VoiceServer : public VoiceServerSource {
  public:
    typedef std::map<VoiceUUID, Avatar*> AvatarMap;

  public:
    //! Create a server object.
    VoiceServer();
    //! Destructor.
    virtual ~VoiceServer();

    //! Create a socket, bind to the specified port, and
    //! set it in listen mode to make it available for clients.
    bool bindAndListen(int port, int backlog = 5);

    //! Process client requests for the specified time
    void work(double msTime);

    //! Temporarily stop processing client requests and exit the work() method.
    void exit();

    //! Close all connections with clients and the socket file descriptor
    void shutdown();

    //! Handle client connection requests
    virtual unsigned handleEvent(unsigned eventType);

    //! Remove a connection from the dispatcher
    virtual void removeConnection(VoiceServerConnection*);

    //! Retrieve an avatar.
    Avatar* getAvatar(const VoiceUUID& id);

    //! Create a new avatar.
    Avatar* newAvatar(const VoiceUUID& id, VoiceServerConnection* connection);

    //! Remove an avatar
    void removeAvatar(const VoiceUUID& id);

  protected:

    //! Accept a client connection request
    virtual void acceptConnection();

    //! Create a new connection object for processing requests from a specific client.
    virtual VoiceServerConnection* createConnection(int socket);

    // Event dispatcher
    VoiceServerDispatch _disp;

    // Supported formats
    int mSupportedFormats;

    // Mutex for avatars management
    pthread_mutex_t mAvatarMutex;

    // Map of avatars
    AvatarMap mAvatars;
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServer_h__