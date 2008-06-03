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

#include "Avatar.h"

#include "VoiceServerSocket.h"
#include "VoiceServerUtil.h"

namespace SolipsisVoiceServer {

// The server delegates handling client requests to a serverConnection object.
Avatar::Avatar(const VoiceUUID& id, VoiceServerConnection* connection) :
    BasicThread(""),
	mId(id),
	_connection(connection),
	mAudioDatasPtrsMutex(PTHREAD_MUTEX_INITIALIZER),
    mAudioDatasPtrsGate(PTHREAD_MUTEX_INITIALIZER)
{
  VoiceServerUtil::log(2,"Avatar: socket %d", connection->getfd());

  // set name
  setName(std::string("Avatar") + mId.getID());
}

void Avatar::stop(unsigned int stopTimeoutSec)
{
    CommonTools::BasicThread::stop(stopTimeoutSec);
    pthread_mutex_unlock(&mAudioDatasPtrsGate);
}

void Avatar::run()
{
    VoiceServerUtil::log(4,"Avatar::run: socket %d, running ...", _connection->getfd());

    while (!isStopRequested())
    {
        pthread_mutex_lock(&mAudioDatasPtrsGate);

        pthread_mutex_lock(&mAudioDatasPtrsMutex);
        while (!mAudioDatasPtrs.empty())
        {
            AudioDatasPtr audioDatasToSend = mAudioDatasPtrs.front();
            mAudioDatasPtrs.pop_front();
            sendAudioFrames(audioDatasToSend);
        }
        pthread_mutex_unlock(&mAudioDatasPtrsMutex);
//        VoiceServerUtil::sleep(10);
    }

    VoiceServerUtil::log(4,"Avatar::run: socket %d, end", _connection->getfd());
}

void Avatar::end()
{
    delete this;
}

void Avatar::audioDatasReceived(AudioDatasPtr& audioDatasPtr)
{
    pthread_mutex_lock(&mAudioDatasPtrsMutex);
    mAudioDatasPtrs.push_back(audioDatasPtr);
    pthread_mutex_unlock(&mAudioDatasPtrsMutex);

    pthread_mutex_unlock(&mAudioDatasPtrsGate);
}

int Avatar::sendPacketHeader(char type, unsigned int size)
{
    char packet[sizeof(char)+sizeof(unsigned int)];
    memcpy(packet, &type, sizeof(char));
    memcpy(&packet[sizeof(char)], &size, sizeof(unsigned int));
    return VoiceServerSocket::send(_connection->getfd(), packet, sizeof(packet));
}

int Avatar::sendAudioFrames(AudioDatasPtr& audioDatasPtr)
{
    int sent = 0;
    sent += sendPacketHeader(VP_AUDIO_TO_CLIENT, sizeof(VoiceUUID) + audioDatasPtr->mSize);
    sent += VoiceServerSocket::send(_connection->getfd(), (const char*)&mId, sizeof(VoiceUUID));
    sent += VoiceServerSocket::send(_connection->getfd(), (const char*)audioDatasPtr->mDatas, audioDatasPtr->mSize);

    return sent;
}

} // namespace Avatar
