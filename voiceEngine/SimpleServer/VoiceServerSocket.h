#ifndef __VoiceServerSocket_h__
#define __VoiceServerSocket_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

namespace SolipsisVoiceServer {

  //! A platform-independent socket API.
  class VoiceServerSocket {
  public:

    //! Creates a stream (TCP) socket. Returns -1 on failure.
    static int socket();

    //! Closes a socket.
    static void close(int socket);

    //! Sets a stream (TCP) socket to perform non-blocking IO. Returns false on failure.
    static bool setNonBlocking(int socket);

    //! Read binary datas from the specified socket. Returns read size.
    static int receive(int socket, char* buffer, int size);

    //! Write binary datas to the specified socket. Returns written size.
    static int send(int socket, const char* buffer, int size);


    // The next four methods are appropriate for servers.

    //! Allow the port the specified socket is bound to to be re-bound immediately so 
    //! server re-starts are not delayed. Returns false on failure.
    static bool setReuseAddr(int socket);

    //! Bind to a specified port
    static bool bind(int socket, int port);

    //! Set socket in listen mode
    static bool listen(int socket, int backlog);

    //! Accept a client connection request
    static int accept(int socket);

    //! Connect a socket to a server (from a client)
    static bool connect(int socket, std::string& host, int port);

    //! Returns last errno
    static int getError();

    //! Returns message corresponding to last error
    static std::string getErrorMsg();

    //! Returns message corresponding to error
    static std::string getErrorMsg(int error);
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerSocket_h__