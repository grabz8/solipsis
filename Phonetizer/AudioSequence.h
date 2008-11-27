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

#ifndef __AudioSequence_h__
#define __AudioSequence_h__

#ifdef PHONETIZER_EXPORTS
	#define PHONETIZER_EXPORT __declspec( dllexport )
#else
	#define PHONETIZER_EXPORT
#endif 


namespace Solipsis 
{

	/**
		@brief	a sequence of phonemes (a mouth animation)
	*/
	class PHONETIZER_EXPORT AudioSequence
	{
	public:
		/**
			@brief Contructor
		*/
		AudioSequence( unsigned short frequency, unsigned int numSamples, short* pAudioWaveData, bool audioWaveDataIsOwned );
		/**
			@brief Destructor
		*/
		~AudioSequence();

		/**
			@brief	gets the frequency of this audio sequence

			@return	the frequency of this audio sequence in hertz
		*/
		unsigned short getFrequency( void ) const {return mFrequency;}

		/**
			@brief	gets the number of samples in this audio sequence
		*/
		unsigned int getNumSamples( void ) const {return mNumSamples;}

		/**
			@brief	returns the duration of the sequence

			@return	in seconds
		*/
		float getDuration( void ) const { return (float)mNumSamples / (float)mFrequency; }

		/**
			@brief	returns the pointer to the audio samples of this sequence
		*/
		const short* getAudioWaveData( void ) const { return mAudioWaveData; }

	private:
		bool			mAudioDataIsOwned;	
		short*			mAudioWaveData;	///< uncompressed raw audio wave data (in PCM16 format). Could be owned or not.
		unsigned short	mFrequency;		///< number of samples per second
		unsigned int	mNumSamples;
	};
}

#endif //__AudioSequence_h__
