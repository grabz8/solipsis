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

#ifndef __MouthFlapper_h__
#define __MouthFlapper_h__

#include "IFaceController.h"
#include <PhonemeSequence.h>

namespace Ogre
{
	class Bone;
}

namespace Solipsis 
{
	class CharacterInstance;
	class MouthFlapperCreator;

	/**
		@brief	a simple face controller that just performs very basic mouth flapping
	*/
	class MouthFlapper : public IFaceController
	{
	public:
		///brief Contructor
		MouthFlapper( CharacterInstance* pFaceControllerOwner, MouthFlapperCreator* pCharacterFacialData );
		///brief Destructor
		virtual ~MouthFlapper();

		//!@name IFaceController implementation
		//!{
			virtual void onVoicePacketReception( VoicePacket* pVoicePacket ) ;
			virtual void updateFace(float timeSinceLastFrame);
			//virtual void onPhonemesReception( PhonemeSequence* pSequence );
		//!}

	private:
		Ogre::Bone*	mJawBone;	///< reference to the jaw bone of the character instance
		MouthFlapperCreator* mCharacterFacialData; ///< the creator of this face controller, which also stores all facial data that are common to all character instances of this character type
		float	mLastFrameTime; ///< in seconds
		float	mEndOfSpeechTime; ///< in seconds
		VoicePacket* mCurrentVoicePacket; ///< owned
		PhonemeSequence::Cursor*	mPhonemeCursor; ///< owned
	};

}

#endif //__MouthFlapper_h__
