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

#ifndef __VoiceServerConnection_h__
#define __VoiceServerConnection_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#include "VoiceServerSource.h"

#include "CTBasicThread.h"

#include "voiceuuid.h"
#include "voiceheader.h"

#include <boost/pool/pool.hpp>
#include <boost/shared_ptr.hpp>

#include <set>
#include "VoiceServerSocket.h"
#include <XmlDatasBasicTypes.h> // for EntityUID

namespace SolipsisVoiceServer {

  // The server waits for client connections and provides methods
  class VoiceServer;

  class Avatar;

  class AudioDatas;
  typedef boost::shared_ptr<AudioDatas> AudioDatasPtr;

  //! A class to listen on new audio datas received by a connection
  class VoiceServerConnectionListener {
  public:
      virtual void audioDatasReceived(AudioDatasPtr& audioDatasPtr) = 0;
  };

  //! A class to handle XML RPC requests from a particular client
  class VoiceServerConnection : public CommonTools::BasicThread {
  public:

    /// Constructor
    VoiceServerConnection(int fd, VoiceServer* server);
    /// Destructor
    virtual ~VoiceServerConnection();

    /** See BasicThread. */
    void finalize();

    //! Return the file descriptor being monitored.
	const Socket & getSocket( void ) const { return mSocket; }
	Socket & getSocket( void ) { return mSocket; }

    // return true if VOIP is enabled for this client
    bool isVOIPEnabled() { return mVOIPEnabled; }

    // Registers a TimeListener which will be called back
    void addVSCListener(VoiceServerConnectionListener* newListener);

    // Removes a TimeListener from the list of listening classes.
    void removeVSCListener(VoiceServerConnectionListener* oldListener);

  private:
    // close the connection
    void close();

    /** See BasicThread. */
    void run();

    /** See BasicThread. */
    void end();

    int recvVoiceHeader(VoicePacketHeader* header);
    int rcvEnableVOIP(bool* enabled);
	/**
		@param	expectedSize	expected number of remaining bytes in the packet
	*/
    int recvAudioFrames(unsigned int expectedSize);

    friend AudioDatas;

    // release no more referenced audio datas
    void releaseAudioDatas(char* datas, int size);

  protected:

    // Socket. This should really be a SOCKET (an alias for unsigned int*) on windows...
    VoiceServerSocket mSocket;

    // The voice server that accepted this connection
    VoiceServer* _server;

    // Avatar
    Avatar* mAvatar;

    // Formats supported by the client
    int mSupportedFormats;

    // VOIP enabled for this client
    bool mVOIPEnabled;

    // Audio datas pool
    boost::pool<> mSendReceivePool;

    // Mutex on audio datas pool
    pthread_mutex_t mSendReceivePoolMutex;

    // Set of registered VoiceServerConnection listeners
    std::set<VoiceServerConnectionListener*> mVSCListeners;

    // Set of VoiceServerConnection listeners marked for removal
    std::set<VoiceServerConnectionListener*> mRemovedVSCListeners;
  };

  class AudioDatas {
  public:
      VoiceServerConnection* mConnection;
      char* mDatas;
      int mSize;

  public:
      AudioDatas(VoiceServerConnection* connection, char* datas, const int size) : mConnection(connection), mDatas(datas), mSize(size) {}
      ~AudioDatas() { mConnection->releaseAudioDatas(mDatas, mSize); }
  };
} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerConnection_h__