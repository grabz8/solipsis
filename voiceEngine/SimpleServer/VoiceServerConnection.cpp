#include "VoiceServerConnection.h"

#include "VoiceServerSocket.h"
#include "VoiceServer.h"
#ifndef MAKEDEPEND
# include <stdio.h>
# include <stdlib.h>
#endif

#include "voiceheader.h"
#include "voiceuuid.h"

#include <boost/pool/singleton_pool.hpp>

#include "Avatar.h"

using namespace CommonTools;

namespace SolipsisVoiceServer {

// The server delegates handling client requests to a serverConnection object.
VoiceServerConnection::VoiceServerConnection(int fd, VoiceServer* server) :
    BasicThread(""),
    _fd(fd),
	_server(server),
    mAvatar(0),
    mSendReceivePool(1, 40960),
    mSendReceivePoolMutex(PTHREAD_MUTEX_INITIALIZER)
{
  VoiceServerUtil::log(2,"VoiceServerConnection: new socket %d.", fd);

  // set name
  char name[32];
  snprintf(name, sizeof(name) - 1, "VSC%d", fd);
  setName(name);
}

VoiceServerConnection::~VoiceServerConnection()
{
  VoiceServerUtil::log(4,"VoiceServerConnection dtor.");

  if (mAvatar != 0)
    _server->removeAvatar(mAvatar->getId());

  _server->removeConnection(this);
}

void VoiceServerConnection::finalize()
{
  BasicThread::finalize();

  VoiceServerUtil::log(4,"VoiceServerConnection::finalize: socket %d, closing connection ...", _fd);
  close();
}

void VoiceServerConnection::addVSCListener(VoiceServerConnectionListener* newListener)
{
  // Check if the specified listener is scheduled for removal
  std::set<VoiceServerConnectionListener*>::iterator i = mRemovedVSCListeners.find(newListener);

  // If yes, cancel the removal. Otherwise add it to other listeners.
  if (i != mRemovedVSCListeners.end())
    mRemovedVSCListeners.erase(*i);
  else
    mVSCListeners.insert(newListener); // Insert, unique only (set)
}

void VoiceServerConnection::removeVSCListener(VoiceServerConnectionListener* oldListener)
{
  // Remove, 1 only (set)
  mRemovedVSCListeners.insert(oldListener);
}

void VoiceServerConnection::close()
{
  if (_fd != -1) {
    VoiceServerUtil::log(2,"VoiceServerConnection::close: closing socket %d.", _fd);
    VoiceServerSocket::close(_fd);
    VoiceServerUtil::log(2,"VoiceServerConnection::close: done closing socket %d.", _fd);
    _fd = -1;
  }
}

void VoiceServerConnection::run()
{
    VoiceServerUtil::log(4,"VoiceServerConnection::run: socket %d, running ...", _fd);

    while (!isStopRequested())
    {
        char packetType;
        unsigned int packetSize;
        if (recvPacketHeader(&packetType, &packetSize) <= 0)
        {
            VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive packet header", _fd);
            break;
        }
        VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, new packet rcved (%s, %d)", _fd, VoicePacketTypeName[packetType], packetSize);

        bool breakOnError = false;
        switch (packetType)
        {
        case VP_LOGIN:
            {
                VoiceUUID id;
                if (rcvLogin(&id, &mSupportedFormats) <= 0)
                {
                    VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive Login", _fd);
                    breakOnError = true;
                }
                VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, rcvLogin() mSupportedFormats = %d", _fd, mSupportedFormats);

                if (mAvatar != 0)
                    VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, avatar already logged on this connection", _fd);

                // Create new avatar if not found
                mAvatar = _server->getAvatar(id);
                if (mAvatar == 0)
                {
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) creation ...", _fd, id.getID().c_str());
                    mAvatar = _server->newAvatar(id, this);
                    if (mAvatar == 0)
                        VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, Could not create new avatar on this connection", _fd);
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) starting ...", _fd, id.getID().c_str());
                    if (!mAvatar->start())
                    {
                        VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not start new avatar", _fd);
                        breakOnError = true;
                    }
                }
                else
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) already exists", _fd, id.getID().c_str());
                break;
            }
        case VP_ENABLE_VOIP:
            if (rcvEnableVOIP(&mVOIPEnabled) <= 0)
            {
                VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive EnableVOIP", _fd);
                breakOnError = true;
            }
            VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, rcvEnableVOIP() mVOIPEnabled = %d", _fd, mVOIPEnabled);
            break;
        case VP_AUDIO_TO_SERVER:
            {
                int receivedSize = recvAudioFrames(packetSize);
                if (receivedSize <= 0)
                {
                    VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive AudioFrames", _fd);
                    breakOnError = true;
                    break;
                }
                if (receivedSize != packetSize)
                    VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, Warning: receivedSize != packetSize (%d/%d)", _fd, receivedSize, packetSize);
                VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, recvAudioFrames() receivedSize = %d", _fd, receivedSize);
                break;
            }
        default:
            VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Unknown packet header received: type %d, size %d. Shutting down connection ...", _fd, packetType, packetSize);
            breakOnError = true;
            break;
        }
        if (breakOnError) break;
    }

    VoiceServerUtil::log(4,"VoiceServerConnection::run: socket %d, end", _fd);
}

void VoiceServerConnection::end()
{
    delete this;
}

int VoiceServerConnection::recvPacketHeader(char* type, unsigned int* size)
{
    int received = 0;
    received = VoiceServerSocket::receive(_fd, type, sizeof(char));
    if (received <= 0) return received;
    received = 0;
    while (received < sizeof(unsigned int))
        received += VoiceServerSocket::receive(_fd, &((char*)size)[received], sizeof(unsigned int)-received);
    return received + sizeof(char);
}

int VoiceServerConnection::recvVoiceHeader(VoicePacketHeader* header)
{
    int received = 0;
    int headerSize = sizeof(VoicePacketHeader);
    while (received < headerSize)
    {
        int bytesReceived = VoiceServerSocket::receive(_fd, &((char*)header)[received], headerSize-received);
        received += bytesReceived;
        if (bytesReceived <= 0) return bytesReceived;
    }

    return received;
}

int VoiceServerConnection::recvUUID(VoiceUUID* id)
{
    int received = 0;
    while (received < sizeof(VoiceUUID))
        received += VoiceServerSocket::receive(_fd, &((char*)id)[received], sizeof(VoiceUUID)-received);

    return received;
}

int VoiceServerConnection::recvSupportedFormats(int* supportedFormats)
{
    int received = 0;
    while (received < sizeof(int))
        received += VoiceServerSocket::receive(_fd, &((char*)supportedFormats)[received], sizeof(int)-received);

    return received;
}

int VoiceServerConnection::rcvLogin(VoiceUUID* id, int* supportedFormats)
{
    int received = 0;
    received = recvUUID(id);
    received += recvSupportedFormats(supportedFormats);

    return received;
}

int VoiceServerConnection::rcvEnableVOIP(bool* enabled)
{
    char val;
    int received = VoiceServerSocket::receive(_fd, &val, sizeof(val));
    *enabled = (val == 1) ? true : false;
    return received;
}

int VoiceServerConnection::recvAudioFrames(unsigned int expectedSize)
{
    int size = 0;
    int received = 0;

    // Allocate full datas
    pthread_mutex_lock(&mSendReceivePoolMutex);
    char* data = (char*)mSendReceivePool.ordered_malloc(expectedSize);
    pthread_mutex_unlock(&mSendReceivePoolMutex);

    // Read audio header
    VoicePacketHeader *header = (VoicePacketHeader*)data;
    received = recvVoiceHeader(header);
    size += received;
    if (received <= 0)
    {
        VoiceServerUtil::log(2, "VoiceServerConnection::recvAudioFrames: socket %d, unable to receive VoicePacketHeader", _fd);
        pthread_mutex_lock(&mSendReceivePoolMutex);
        mSendReceivePool.ordered_free(data, expectedSize);
        pthread_mutex_unlock(&mSendReceivePoolMutex);
        return size;
    }

    // Read whole audio datas
    received = 0;
    while (received < (int)(expectedSize - sizeof(VoicePacketHeader)))
    {
        int bytesReceived = VoiceServerSocket::receive(_fd, data + sizeof(VoicePacketHeader) + received, expectedSize - sizeof(VoicePacketHeader) - received);
	    received += bytesReceived;
    	size += bytesReceived;
        if (bytesReceived <= 0) return size;
    }

    // Warn listeners new audio datas are available
    AudioDatasPtr audioDatasPtr(new AudioDatas(this, data, expectedSize));
    // Remove all marked listeners
    std::set<VoiceServerConnectionListener*>::iterator i;
    for (i = mRemovedVSCListeners.begin(); i != mRemovedVSCListeners.end(); i++)
    {
        mVSCListeners.erase(*i);
    }
    mRemovedVSCListeners.clear();
    // Tell all listeners
    for (i= mVSCListeners.begin(); i != mVSCListeners.end(); ++i)
    {
        (*i)->audioDatasReceived(audioDatasPtr);
    }

    return size;
}

void VoiceServerConnection::releaseAudioDatas(char* datas, int size)
{
    pthread_mutex_lock(&mSendReceivePoolMutex);
    mSendReceivePool.ordered_free(datas, size);
    pthread_mutex_unlock(&mSendReceivePoolMutex);
}

} // namespace SolipsisVoiceServer
