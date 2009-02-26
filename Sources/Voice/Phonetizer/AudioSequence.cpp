#include "AudioSequence.h"

namespace Solipsis
{
	AudioSequence::AudioSequence( unsigned short frequency, unsigned int numSamples, short* pAudioWaveData, bool audioWaveDataIsOwned )
		: mAudioDataIsOwned(audioWaveDataIsOwned)
		, mAudioWaveData( pAudioWaveData )
		, mFrequency( frequency )
		, mNumSamples( numSamples )
	{
	}

	AudioSequence::~AudioSequence()
	{
		if( mAudioDataIsOwned )
		{
			if( mAudioWaveData )
			{
				delete mAudioWaveData;
			}
		}
	}
}