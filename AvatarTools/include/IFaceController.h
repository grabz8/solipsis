/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __IFaceController_h__
#define __IFaceController_h__

#include <IVoicePacketListener.h>

class PhonemeSequence;

namespace Solipsis 
{

	/**
		@brief	interface that allows control of the face of the avatar
	*/
	class IFaceController : public IVoicePacketListener
	{
	public:
		///brief Contructor
		IFaceController( void );
		///brief Destructor
		virtual ~IFaceController();

		//! @name IVoicePacketListener implementation
		//!{
			virtual void onVoicePacketReception( VoicePacket* pVoicePacket ) = 0 ;
		//!}

		/**
			@brief	updates the face

			@param	timeSinceLastFrame	in seconds
		*/
		virtual void updateFace(float timeSinceLastFrame) = 0;

		#if 0
			/**
				@brief	called whenever a sequence of phonemes have been received from this face controller

				@param	pSequence the sequence of phonemes that needs to be handled. The face controller is responsible for deletion of this sequence
			*/
			virtual void onPhonemesReception( PhonemeSequence* pSequence ) = 0;
		#endif //0
	};

}

#endif //__IFaceController_h__
