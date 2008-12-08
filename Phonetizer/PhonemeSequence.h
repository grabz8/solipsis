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

#ifndef __PhonemeSequence_h__
#define __PhonemeSequence_h__
#include <vector>
#include "Phoneme.h"
#include "SequenceCursor.h"
#include <assert.h>

#ifdef PHONETIZER_EXPORTS
	#define PHONETIZER_EXPORT __declspec( dllexport )
#else
	#define PHONETIZER_EXPORT
#endif 

namespace Solipsis 
{
	class Phoneme;
	class OutputStream;

	/**
		@brief	a sequence of phonemes (a mouth animation)
	*/
	class PHONETIZER_EXPORT PhonemeSequence
	{
	public:
		///brief Contructor
		PhonemeSequence( void );
		///brief Destructor
		~PhonemeSequence();

		/**
			@brief	returns the duration of the sequence

			@return	in seconds
		*/
		float getDuration( void ) const { return mDuration; }

		/**
			@brief	expands this sequence with a phoneme
		*/
		void appendPhoneme( const Phoneme & phoneme );

		/**
			@brief	saves this object to the given stream of data
		*/
		void serialize( std::ostream & outputStream ) const;

		/**
			@brief	returns the size of serialized data of this phoneme sequence

			@return	size in bytes
		*/
		unsigned int getSerializedSize( void ) const;

		/**
			@brief	returns the number of phonemes
		*/
		unsigned int getNumElements( void ) const { return (unsigned int) mElements.size(); }

		/**
			@brief	returns the number of phonemes
		*/
		const Phoneme & getElement( unsigned int elementIndex ) const { return mElements[elementIndex]; }

		/**
			@brief	creates a PhonemeSequence instance from a serialized representation coming from a stream
		*/
		static PhonemeSequence* createFromStream( std::istream & inputStream );

		typedef SequenceCursor<PhonemeSequence, Phoneme> Cursor;

	private:
		float		mDuration;			///< duration of the sequence in seconds
		std::vector<Phoneme>	mElements;
	};

}

#endif //__PhonemeSequence_h__
