/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Guillaume Raffy (Proservia)

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

#ifndef __SimpleVoiceEngineProtocol_h__
#define __SimpleVoiceEngineProtocol_h__

#include <XmlDatasBasicTypes.h>
#include "voiceheader.h"

class Socket;

/**
	@brief	utility class that deals with the simple voice engine protocol
*/
class SimpleVoiceEngineProtocol
{
public:
	/**
		@brief receives a voice identifier from the socket

		@return	the number of bytes read from the socket
	*/
	static int receiveVoiceUid( Socket & socket, Solipsis::EntityUID & voiceUid );

	/**
		@brief reads the login data from the socket

		@return	the number of bytes read from the socket
	*/
	static int receiveLogin( Socket & socket, Solipsis::EntityUID & voiceId, int* supportedFormats);

	/**
		@brief reads the supported formats from the socket

		@return	the number of bytes read from the socket
	*/
	static int receiveSupportedFormats(Socket & socket, int* supportedFormats);

	/**
		@brief sends the header of a voice packet over the network using the socket, which is supposed to be opened

		@param	type	type of voice packet
		@param	size	size of the packet's data following the packet header (in bytes)	
		@return	the number of bytes read from the socket
	*/
	static int sendPacketHeader(Socket & socket, VoicePacketType type, unsigned int size);

	/**
		@return	the number of bytes read from the socket
	*/
	static int receivePacketHeader(Socket & socket, VoicePacketType* type, unsigned int* size);

};

#endif	//	__SimpleVoiceEngineProtocol_h__
