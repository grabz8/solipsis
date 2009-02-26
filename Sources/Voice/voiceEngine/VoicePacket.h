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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __VoicePacket_h__
#define __VoicePacket_h__

#include "DllExport.h"
#include <string>

namespace Solipsis 
{
	class PhonemeSequence;

	/**
		@brief	a packet containing audio data + phoneme data for a short time range
		
	*/
	class VOICEENGINE_EXPORT VoicePacket
	{
	public:

		///brief Contructor
		VoicePacket( const std::string & talkingAvatarUid );
		///brief Destructor
		~VoicePacket();

		const std::string & getTalkingAvatarUid( void ) const {return mTalkingAvatarUid;}

		void setNumAudioSamples( unsigned int numAudioSamples ) { mNumAudioSamples = numAudioSamples; }
		unsigned int getNumAudioSamples( void ) const { return mNumAudioSamples; }

		void setAudioFrequency( unsigned int audioFrequency ) { mAudioFrequency = audioFrequency; }
		unsigned int getAudioFrequency( void ) const { return mAudioFrequency; }

		/**
			@brief	returns the duration of the voice packet in seconds
		*/
		float getDuration( void ) const { return (float)mNumAudioSamples/(float)mAudioFrequency; }

		/**
			@brief	assigns the given phoneme sequence to this voice packet
		*/
		void setPhonemeSequence( PhonemeSequence* pPhonemeSequence );

		/**
			@brief	gives access to the phoneme sequence
		*/
		const PhonemeSequence* getPhonemeSequence( void ) const { return mPhonemeSequence; }
		PhonemeSequence* getPhonemeSequence( void ) { return mPhonemeSequence; }

	private:
		std::string	mTalkingAvatarUid;
		unsigned int mNumAudioSamples;	///< number of audio samples in this packet
		unsigned int mAudioFrequency;		///< frequency of the audio
		PhonemeSequence*	mPhonemeSequence; ///< the phonemes associated with this voice packet (owned)
	};

}

#endif //__VoicePacket_h__
