#include "PhonemeSequence.h"
#include "Phoneme.h"
#include <ostream>
#include <istream>
#include <assert.h>

namespace Solipsis
{
	PhonemeSequence::PhonemeSequence( void )
		: mDuration(0.0f)
	{
	}

	PhonemeSequence::~PhonemeSequence()
	{
	}

	void PhonemeSequence::appendPhoneme( const Phoneme & phoneme )
	{
		mElements.push_back( phoneme );

		// update the duration of the sequence
		mDuration += phoneme.getDuration();
	}

	void PhonemeSequence::serialize( std::ostream & outputStream ) const
	{
		//outputStream << mPhonemes.size();
		unsigned int numPhonemes = (unsigned int)mElements.size();
		outputStream.write( (char*)&numPhonemes, sizeof(unsigned int) );
		std::vector<Phoneme>::const_iterator it;
		for(it = mElements.begin() ; it < mElements.end() ; ++it )
		{
			float phonemeDuration = it->getDuration();
			outputStream.write( (char*)&phonemeDuration, sizeof(float) );
			unsigned char audioType = it->getAudioType();
			outputStream.write( (char*)&audioType, sizeof(unsigned char) );
			unsigned short phonemeType = it->getPhonemeType();
			outputStream.write( (char*)&phonemeType, sizeof(unsigned short) );

			/*
			outputStream << it->getDuration();
			outputStream << it->getAudioType();
			outputStream << it->getPhonemeType();
			*/
		}
	}

	unsigned int PhonemeSequence::getSerializedSize( void ) const
	{
		const unsigned int sizeOfAPhoneme = sizeof(float) + sizeof(unsigned char) + sizeof( unsigned short );
		unsigned int size = sizeof(unsigned int) + sizeOfAPhoneme * (unsigned int)mElements.size();
		return size;
	}

	PhonemeSequence* PhonemeSequence::createFromStream( std::istream & inputStream )
	{
		PhonemeSequence* pPhonemeSequence = new PhonemeSequence();
		unsigned int numPhonemes;
		inputStream.read( (char*)&numPhonemes, sizeof(unsigned int) );
		unsigned int phonemeIndex;
		for(phonemeIndex = 0 ; phonemeIndex < numPhonemes; ++phonemeIndex )
		{
			float phonemeDuration;
			unsigned char audioTypeAsChar;
			unsigned short phonemeTypeAsShort;
			inputStream.read( (char*)&phonemeDuration, sizeof(float) );
			inputStream.read( (char*)&audioTypeAsChar, sizeof(unsigned char) );
			inputStream.read( (char*)&phonemeTypeAsShort, sizeof(unsigned short) );
			Phoneme::PhonemeType phonemeType = (Phoneme::PhonemeType)phonemeTypeAsShort;
			pPhonemeSequence->appendPhoneme( Phoneme( phonemeType, phonemeDuration ) );
		}
		return pPhonemeSequence;
	}
}
