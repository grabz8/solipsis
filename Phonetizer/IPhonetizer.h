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

#ifndef __Phonetizer_h__
#define __Phonetizer_h__

#include <string>

namespace Solipsis 
{

	class AudioSequence;
	class PhonemeSequence;

	/**
		@brief	interface for converters that generate phoneme sequence from an audio source
	*/
	class IPhonetizer
	{
	public:
		/**
			@brief	constructor
		*/
		IPhonetizer(void) {}
		/**
			@brief	destructor
		*/
		~IPhonetizer(void) {}

		/**
			@brief	Get the name of the voice engine.

			@remarks An implementation must be supplied for this method to uniquely identify the engine.
		*/
		virtual const std::string& getName( void ) const = 0;

		/**
			@brief	generates a sequence of phonemes by analyzing the audio data

			@return null in case of error
		*/
		virtual PhonemeSequence* audioToPhonemes( const AudioSequence & audioSequence ) const = 0;

	private:
	};

}

#endif //__Phonetizer_h__
