#include "VoiceServerSource.h"
#include "VoiceServerSocket.h"
#include "VoiceServerUtil.h"

namespace SolipsisVoiceServer {

VoiceServerSource::VoiceServerSource(int fd /*= -1*/, bool deleteOnClose /*= false*/) 
  : _fd(fd), _deleteOnClose(deleteOnClose)
{
}

VoiceServerSource::~VoiceServerSource()
{
}

void VoiceServerSource::close()
{
  if (_fd != -1) {
    VoiceServerUtil::log(2,"VoiceServerSource::close: closing socket %d.", _fd);
    VoiceServerSocket::close(_fd);
    VoiceServerUtil::log(2,"VoiceServerSource::close: done closing socket %d.", _fd);
    _fd = -1;
  }
  if (_deleteOnClose) {
    VoiceServerUtil::log(2,"VoiceServerSource::close: deleting this");
    _deleteOnClose = false;
    delete this;
  }
}

} // namespace SolipsisVoiceServer
