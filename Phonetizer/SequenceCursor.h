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

#ifndef __SequenceCursor_h__
#define __SequenceCursor_h__
#include <vector>
#include "Phoneme.h"
#include <assert.h>

#ifdef PHONETIZER_EXPORTS
	#define PHONETIZER_EXPORT __declspec( dllexport )
#else
	#define PHONETIZER_EXPORT
#endif 

namespace Solipsis 
{
	/**
		@brief	class that represents a reading cursor into a sequence of elements (eg phonemes or visemes)

		Each element has a duration.
	*/
	template <class SEQUENCE, class ELEMENT_TYPE> class PHONETIZER_EXPORT SequenceCursor
	{
	public:
		/**
			@brief	constructor

			@param	sequence the sequence this cursor points into
		*/
		SequenceCursor( const SEQUENCE & sequence );

		/**
			@brief	sets the current time of the cursor

			@param	time	in seconds

			@note	this method is usually used to initialise the time of the cursor.
		*/
		void setTime( float time );

		/**
			@brief	gets the current cursor's position in seconds
		*/
		float getTime( void ) const { return mCurrentTime; }

		/**
			@brief	advances the cursor
		*/
		void evolve( float timeStepDuration );

		/**
			@brief	gets the element this cursor points to
		*/
		const ELEMENT_TYPE getPointedElement( void ) const;

		/**
			@brief	gets the index of the element this cursor points to
		*/
		unsigned int getPointedElementIndex( void ) const;

		/**
			@brief	gets the start time of the pointed element
		*/
		float getPointedElementStartTime( void ) const;

	private:
		const SEQUENCE& mSequence;	///< phoneme sequence pointed by this cursor
		float	mCurrentTime;						///< time from the beginning of the phoneme sequence (in seconds)
		unsigned int mCurrentPhonemeIndex;			///< the cursor is currently inside this phoneme
		float	mCurrentPhonemeStartTime;			///<
	};

	template <class SEQUENCE, class ELEMENT_TYPE>
	SequenceCursor<SEQUENCE, ELEMENT_TYPE>::SequenceCursor( const SEQUENCE & sequence )
	: mSequence( sequence )
	, mCurrentTime( 0.0f )
	, mCurrentPhonemeIndex( 0 )
	, mCurrentPhonemeStartTime( 0.0f )
	{
	}

	template <class SEQUENCE, class ELEMENT_TYPE>	
	void SequenceCursor<SEQUENCE, ELEMENT_TYPE>::setTime( float time )
	{
		if( time > 0.0f )
		{
			mCurrentTime = 0.0f;
			mCurrentPhonemeIndex = 0;
			mCurrentPhonemeStartTime = 0.0f;
			evolve( time );
		}
		else
		{
			mCurrentTime = time;
			mCurrentPhonemeIndex = 0;
			mCurrentPhonemeStartTime = 0.0f;
		}
	}


	template <class SEQUENCE, class ELEMENT_TYPE>
	void SequenceCursor<SEQUENCE, ELEMENT_TYPE>::evolve( float timeStepDuration )
	{
		assert( timeStepDuration > 0.0f );
		mCurrentTime += timeStepDuration;
		if( mCurrentTime > 0.0f )
		{
			while( (mCurrentTime - mCurrentPhonemeStartTime) > mSequence.getElement( mCurrentPhonemeIndex ).getDuration() )
			{
				if( mCurrentPhonemeIndex == (mSequence.getNumElements() - 1) )
				{
					// the end of the phoneme sequence has been reached
					break;
				}
				mCurrentPhonemeStartTime += mSequence.getElement( mCurrentPhonemeIndex ).getDuration();
				mCurrentPhonemeIndex++;
			}
		}
	}

	template <class SEQUENCE, class ELEMENT_TYPE>
	const ELEMENT_TYPE SequenceCursor<SEQUENCE, ELEMENT_TYPE>::getPointedElement( void ) const
	{
		return mSequence.getElement( mCurrentPhonemeIndex );
	}

	template <class SEQUENCE, class ELEMENT_TYPE>
	unsigned int SequenceCursor<SEQUENCE, ELEMENT_TYPE>::getPointedElementIndex( void ) const
	{
		return mCurrentPhonemeIndex;
	}

	template <class SEQUENCE, class ELEMENT_TYPE>
	float SequenceCursor<SEQUENCE, ELEMENT_TYPE>::getPointedElementStartTime( void ) const
	{
		return mCurrentPhonemeStartTime;
	}

}

#endif //__SequenceCursor_h__
