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

#include "VoiceServer.h"
#include "VoiceServerConnection.h"
#include "VoiceServerSocket.h"
#include "VoiceServerUtil.h"
#include "VoiceServerException.h"

#include "Avatar.h"

namespace SolipsisVoiceServer {

VoiceServer::VoiceServer() : 
  mSupportedFormats(0), mAvatarMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

VoiceServer::~VoiceServer()
{
  this->shutdown();
}

// Create a socket, bind to the specified port, and
// set it in listen mode to make it available for clients.
bool VoiceServer::bindAndListen(int port, int backlog /*= 5*/)
{
	mSocket.create();
	if(!mSocket.isValid())
	{
		VoiceServerUtil::error("VoiceServer::bindAndListen: Could not create socket (%s).", Socket::getLastErrorMsg().c_str());
		return false;
	}

	// Allow this port to be re-bound immediately so server re-starts are not delayed
	if ( ! mSocket.setReuseAddr())
	{
		this->close();
		VoiceServerUtil::error("VoiceServer::bindAndListen: Could not set SO_REUSEADDR socket option (%s).", Socket::getLastErrorMsg().c_str());
		return false;
	}

  // Bind to the specified port on the default interface
  if ( ! mSocket.bind(port))
  {
    this->close();
    VoiceServerUtil::error("VoiceServer::bindAndListen: Could not bind to specified port (%s).", Socket::getLastErrorMsg().c_str());
    return false;
  }

  // Set in listening mode
  if ( ! mSocket.listen(backlog))
  {
    this->close();
    VoiceServerUtil::error("VoiceServer::bindAndListen: Could not set socket in listening mode (%s).", Socket::getLastErrorMsg().c_str());
    return false;
  }

  VoiceServerUtil::log(2, "VoiceServer::bindAndListen: server listening on port %d fd %d", port, (int)mSocket.getHandle());

  // Notify the dispatcher to listen on this source when we are in work()
  _disp.addSource(this, VoiceServerDispatch::ReadableEvent);

  return true;
}

// Process client requests for the specified time
void VoiceServer::work(double msTime)
{
  VoiceServerUtil::log(2, "VoiceServer::work: waiting for a connection");
  _disp.work(msTime);
}

// Handle input on the server socket by accepting the connection
// and reading the rpc request.
unsigned VoiceServer::handleEvent(unsigned mask)
{
  acceptConnection();
  return VoiceServerDispatch::ReadableEvent;		// Continue to monitor this fd
}

// Accept a client connection request and create a connection to
// handle method calls from the client.
void VoiceServer::acceptConnection()
{
	int s = mSocket.accept();
	VoiceServerUtil::log(2, "VoiceServer::acceptConnection: socket %d", s);
	if (s < 0)
	{
		this->close();
		VoiceServerUtil::error("VoiceServer::acceptConnection: Could not accept connection (%s).", Socket::getLastErrorMsg().c_str());
	}
	else  // create a new connection
	{
		VoiceServerUtil::log(2, "VoiceServer::acceptConnection: creating a connection");
		this->createConnection(s);
	}
}

// Create a new connection object for processing requests from a specific client.
VoiceServerConnection* VoiceServer::createConnection(int s)
{
  // Specify that the connection object be deleted when it is closed
  VoiceServerConnection* connection = new VoiceServerConnection(s, this);
  if (!connection->start())
  {
    VoiceServerUtil::error("VoiceServer::createConnection: Could not create connection, start failed.");
    delete connection;
  }
  return connection;
}

void VoiceServer::removeConnection(VoiceServerConnection* sc)
{
}

// Stop processing client requests
void VoiceServer::exit()
{
  _disp.exit();
}

// Close the server socket file descriptor and stop monitoring connections
void VoiceServer::shutdown()
{
  // This closes and destroys all connections as well as closing this socket
  _disp.clear();
}

Avatar* VoiceServer::getAvatar(const Solipsis::EntityUID& id)
{
    AvatarMap::const_iterator i = mAvatars.find(id);
    if (i == mAvatars.end())
        return 0;

    return i->second;
}

Avatar* VoiceServer::newAvatar(const Solipsis::EntityUID& id, VoiceServerConnection* connection)
{
    // Create the avatar
    Avatar* avatar = new Avatar(id, connection);

    // Add it into avatars map
    pthread_mutex_lock(&mAvatarMutex);
    mAvatars.insert(std::make_pair(id, avatar));
    pthread_mutex_unlock(&mAvatarMutex);

    // All avatars listen this new avatar
    for(AvatarMap::iterator otherAvatarIt=mAvatars.begin(); otherAvatarIt!=mAvatars.end(); ++otherAvatarIt)
    {
#if !defined(ENABLE_ECHO)
        if (otherAvatarIt->second == avatar) continue;
#endif
        VoiceServerUtil::log(2, "VoiceServer::newAvatar: avatar %s is now listening avatar %s", otherAvatarIt->second->getId().c_str(), avatar->getId().c_str());
        avatar->getConnection()->addVSCListener(otherAvatarIt->second);
        VoiceServerUtil::log(2, "VoiceServer::newAvatar: avatar %s is now listening avatar %s", avatar->getId().c_str(), otherAvatarIt->second->getId().c_str());
        otherAvatarIt->second->getConnection()->addVSCListener(avatar);
    }

    return avatar;
}

void VoiceServer::removeAvatar(const Solipsis::EntityUID& id)
{
    pthread_mutex_lock(&mAvatarMutex);

    // Remove it from avatars map
    AvatarMap::iterator avatarIt = mAvatars.find(id);
    if (avatarIt != mAvatars.end())
    {
        Avatar* avatar = avatarIt->second;

        // All avatars do not listen anymore this avatar
        for(AvatarMap::iterator otherAvatarIt=mAvatars.begin(); otherAvatarIt!=mAvatars.end(); ++otherAvatarIt)
        {
#if !defined(ENABLE_ECHO)
            if (otherAvatarIt->second == avatar) continue;
#endif
            VoiceServerUtil::log(2, "VoiceServer::removeAvatar: avatar %s is no more listening avatar %s", otherAvatarIt->second->getId().c_str(), avatar->getId().c_str());
            avatar->getConnection()->removeVSCListener(otherAvatarIt->second);
            VoiceServerUtil::log(2, "VoiceServer::removeAvatar: avatar %s is no more listening avatar %s", avatar->getId().c_str(), otherAvatarIt->second->getId().c_str());
            otherAvatarIt->second->getConnection()->removeVSCListener(avatar);
        }

        mAvatars.erase(avatarIt);
    }

    pthread_mutex_unlock(&mAvatarMutex);
}

} // namespace SolipsisVoiceServer
