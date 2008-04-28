#ifndef __VoiceServerSource_h__
#define __VoiceServerSource_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

namespace SolipsisVoiceServer {

  //! An RPC source represents a file descriptor to monitor
  class VoiceServerSource {
  public:
    //! Constructor
    //!  @param fd The socket file descriptor to monitor.
    //!  @param deleteOnClose If true, the object deletes itself when close is called.
    VoiceServerSource(int fd = -1, bool deleteOnClose = false);

    //! Destructor
    virtual ~VoiceServerSource();

    //! Return the file descriptor being monitored.
    int getfd() const { return _fd; }
    //! Specify the file descriptor to monitor.
    void setfd(int fd) { _fd = fd; }

    //! Close the owned fd. If deleteOnClose was specified at construction, the object is deleted.
    virtual void close();

    //! Return true to continue monitoring this source
    virtual unsigned handleEvent(unsigned eventType) = 0;

  private:

    // Socket. This should really be a SOCKET (an alias for unsigned int*) on windows...
    int _fd;

    // In the server, a new source (VoiceServerConnection) is created
    // for each connected client. When each connection is closed, the
    // corresponding source object is deleted.
    bool _deleteOnClose;
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerSource_h__