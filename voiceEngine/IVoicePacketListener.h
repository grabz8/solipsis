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

#ifndef __IVoicePacketListener_h__
#define __IVoicePacketListener_h__

#include "DllExport.h"

namespace Solipsis
{
	class VoicePacket;

	/**
		@brief	interface for objects that need to do something when a voice packet is received
	*/
	class VOICEENGINE_EXPORT IVoicePacketListener
	{
	public:
		IVoicePacketListener() {}
		virtual ~IVoicePacketListener() {}

		/**
			@brief	action to perform when a voice packet is received

			@param	pVoicePacket	the voice packet that has just been received and that this method needs to handle
									From now on, the life of this voice packet is under the responsability of this method
									(this method needs to ensure its deletion at some point)
		*/
		virtual void onVoicePacketReception( VoicePacket* pVoicePacket ) = 0 ;
	};

} // namespace Solipsis

#endif // #ifndef __IVoicePacketListener_h__