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