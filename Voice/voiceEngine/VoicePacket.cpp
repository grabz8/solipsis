#include "VoicePacket.h"
#include <PhonemeSequence.h>
#include <assert.h>

namespace Solipsis
{
	VoicePacket::VoicePacket( const std::string & talkingAvatarUid )
		: mTalkingAvatarUid( talkingAvatarUid )
		, mNumAudioSamples( 0 )
		, mAudioFrequency( 0 )
		, mPhonemeSequence( 0 )
	{
	}

	VoicePacket::~VoicePacket()
	{
		if( mPhonemeSequence )
		{
			delete mPhonemeSequence;
			mPhonemeSequence = 0;
		}
	}

	void VoicePacket::setPhonemeSequence( PhonemeSequence* pPhonemeSequence )
	{
		if( mPhonemeSequence == pPhonemeSequence )
			return; // nothing to do

		if( mPhonemeSequence )
		{
			delete mPhonemeSequence;
			mPhonemeSequence = 0;
		}

		assert( mPhonemeSequence == 0 );
		mPhonemeSequence = pPhonemeSequence;
	}

}
