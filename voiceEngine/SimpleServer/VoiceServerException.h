#ifndef __VoiceServerException_h__
#define __VoiceServerException_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

namespace SolipsisVoiceServer {

  //! A class representing an error.
  //! If server methods throw this exception, a fault response is returned
  //! to the client.
  class VoiceServerException {
  public:
    //! Constructor
    //!   @param message  A descriptive error message
    //!   @param code     An integer error code
    VoiceServerException(const std::string& message, int code=-1) :
        _message(message), _code(code) {}

    //! Return the error message.
    const std::string& getMessage() const { return _message; }

    //! Return the error code.
    int getCode() const { return _code; }

  private:
    std::string _message;
    int _code;
  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerException_h__