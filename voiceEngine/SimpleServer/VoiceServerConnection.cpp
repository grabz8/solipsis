/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
#include <SimpleVoiceEngineProtocol.h>

using namespace CommonTools;

namespace SolipsisVoiceServer {

// The server delegates handling client requests to a serverConnection object.
VoiceServerConnection::VoiceServerConnection(int fd, VoiceServer* server) :
    BasicThread(""),
    mSocket(fd),
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

	VoiceServerUtil::log(4,"VoiceServerConnection::finalize: socket %d, closing connection ...", (int)mSocket.getHandle());
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
	if (mSocket.isValid())
	{
		Socket::Handle fd = mSocket.getHandle();
		VoiceServerUtil::log(2,"VoiceServerConnection::close: closing socket %d.", fd);
		mSocket.close();
		VoiceServerUtil::log(2,"VoiceServerConnection::close: done closing socket %d.", fd);
	}
}

void VoiceServerConnection::run()
{
	VoiceServerUtil::log(4,"VoiceServerConnection::run: socket %d, running ...", (int)mSocket.getHandle());

    while (!isStopRequested())
    {
		VoicePacketType packetType;
        unsigned int packetSize;
		if (SimpleVoiceEngineProtocol::receivePacketHeader(mSocket, &packetType, &packetSize) <= 0)
        {
            VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive packet header", (int)mSocket.getHandle());
            break;
        }
        VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, new packet rcved (%s, %d)", (int)mSocket.getHandle(), VoicePacketTypeName[packetType], packetSize);

        bool breakOnError = false;
        switch (packetType)
        {
        case VP_LOGIN:
            {
				Solipsis::EntityUID id;
				if ( SimpleVoiceEngineProtocol::receiveLogin(mSocket, id, &mSupportedFormats) <= 0)
                {
                    VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive Login", (int)mSocket.getHandle());
                    breakOnError = true;
                }
                VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, receiveLogin() mSupportedFormats = %d", (int)mSocket.getHandle(), mSupportedFormats);

                if (mAvatar != 0)
                    VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, avatar already logged on this connection", (int)mSocket.getHandle());

                // Create new avatar if not found
                mAvatar = _server->getAvatar(id);
                if (mAvatar == 0)
                {
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) creation ...", (int)mSocket.getHandle(), id.c_str());
                    mAvatar = _server->newAvatar(id, this);
                    if (mAvatar == 0)
                        VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, Could not create new avatar on this connection", (int)mSocket.getHandle());
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) starting ...", (int)mSocket.getHandle(), id.c_str());
                    if (!mAvatar->start())
                    {
                        VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not start new avatar", (int)mSocket.getHandle());
                        breakOnError = true;
                    }
                }
                else
                    VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, avatar (id = %s) already exists", (int)mSocket.getHandle(), id.c_str());
                break;
            }
        case VP_ENABLE_VOIP:
            if (rcvEnableVOIP(&mVOIPEnabled) <= 0)
            {
                VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive EnableVOIP", (int)mSocket.getHandle());
                breakOnError = true;
            }
            VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, rcvEnableVOIP() mVOIPEnabled = %d", (int)mSocket.getHandle(), mVOIPEnabled);
            break;
        case VP_AUDIO_TO_SERVER:
            {
                int receivedSize = recvAudioFrames(packetSize);
                if (receivedSize <= 0)
                {
                    VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Could not receive AudioFrames", (int)mSocket.getHandle());
                    breakOnError = true;
                    break;
                }
                if (receivedSize != packetSize)
                    VoiceServerUtil::log(2, "VoiceServerConnection::run: socket %d, Warning: receivedSize != packetSize (%d/%d)", (int)mSocket.getHandle(), receivedSize, packetSize);
                VoiceServerUtil::log(3, "VoiceServerConnection::run: socket %d, recvAudioFrames() receivedSize = %d", (int)mSocket.getHandle(), receivedSize);
                break;
            }
        default:
            VoiceServerUtil::error("VoiceServerConnection::run: socket %d, Unknown packet header received: type %d, size %d. Shutting down connection ...", (int)mSocket.getHandle(), packetType, packetSize);
            breakOnError = true;
            break;
        }
        if (breakOnError) break;
    }

    VoiceServerUtil::log(4,"VoiceServerConnection::run: socket %d, end", (int)mSocket.getHandle());
}

void VoiceServerConnection::end()
{
    delete this;
}

int VoiceServerConnection::recvVoiceHeader(VoicePacketHeader* header)
{
    int received = 0;
    int headerSize = sizeof(VoicePacketHeader);
    while (received < headerSize)
    {
		int bytesReceived = mSocket.receive( &((char*)header)[received], headerSize-received);
        received += bytesReceived;
        if (bytesReceived <= 0) return bytesReceived;
    }

    return received;
}

int VoiceServerConnection::rcvEnableVOIP(bool* enabled)
{
    char val;
	int received = mSocket.receive(&val, sizeof(val));
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
		VoiceServerUtil::log(2, "VoiceServerConnection::recvAudioFrames: socket %d, unable to receive VoicePacketHeader", (int)mSocket.getHandle());
        pthread_mutex_lock(&mSendReceivePoolMutex);
        mSendReceivePool.ordered_free(data, expectedSize);
        pthread_mutex_unlock(&mSendReceivePoolMutex);
        return size;
    }

    // Read whole audio datas
    received = 0;
    while (received < (int)(expectedSize - sizeof(VoicePacketHeader)))
    {
		int bytesReceived = mSocket.receive(data + sizeof(VoicePacketHeader) + received, expectedSize - sizeof(VoicePacketHeader) - received);
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
