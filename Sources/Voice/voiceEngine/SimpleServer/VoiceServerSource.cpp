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

#include "VoiceServerSource.h"
#include "VoiceServerSocket.h"
#include "VoiceServerUtil.h"

namespace SolipsisVoiceServer {

VoiceServerSource::VoiceServerSource(int fd /*= -1*/, bool deleteOnClose /*= false*/) 
  : mSocket(fd), _deleteOnClose(deleteOnClose)
{
}

VoiceServerSource::~VoiceServerSource()
{
}

void VoiceServerSource::close()
{
	if (mSocket.isValid())
	{
		Socket::Handle fd = mSocket.getHandle();
		VoiceServerUtil::log(2,"VoiceServerSource::close: closing socket %d.", (int)fd);
		mSocket.close();
		VoiceServerUtil::log(2,"VoiceServerSource::close: done closing socket %d.", (int)fd);
	}
	if (_deleteOnClose)
	{
		VoiceServerUtil::log(2,"VoiceServerSource::close: deleting this");
		_deleteOnClose = false;
		delete this;
	}
}

} // namespace SolipsisVoiceServer
