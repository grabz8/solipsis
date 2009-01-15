#include "DummyPhonetizer.h"

#include <PhonemeSequence.h>
#include <Phoneme.h>
#include <AudioSequence.h>

//#define TRACE_PHONETIZER_OUTPUT

#ifdef TRACE_PHONETIZER_OUTPUT
	#include <Windows.h>
#endif

namespace Solipsis
{
	DummyPhonetizer::DummyPhonetizer( void )
	{
	}
	
	DummyPhonetizer::~DummyPhonetizer()
	{
	}

	const std::string& DummyPhonetizer::getName( void ) const
	{
		static std::string sPhonetizerName = "Dummy Phonetizer";
		return sPhonetizerName;
	}

	PhonemeSequence* DummyPhonetizer::audioToPhonemes( const AudioSequence & audioSequence ) const
	{
		// a dummy sequence of phonemes that is outputted when the sound pressure is high enough
		// this sequece should output "blablablabla"
		const unsigned int numDummyPhonemes = 3;
		static Phoneme::PhonemeType dummyPhonemes[numDummyPhonemes] =
		{
			Phoneme::kPhonemeTypeB,
			Phoneme::kPhonemeTypeL,
			Phoneme::kPhonemeTypeA
		};

		PhonemeSequence* pPhonemeSequence = new PhonemeSequence();
		unsigned int numRemainingSamples = audioSequence.getNumSamples();
		const short* pRemainingSamples = audioSequence.getAudioWaveData();
		const float meanPhonemeDuration = 0.2f;
		const unsigned int meanNumSamplesInPhoneme = (unsigned int) ((float)audioSequence.getFrequency() * meanPhonemeDuration);
		unsigned int dummyPhonemeIndex = 0;
		while(numRemainingSamples > 0)
		{
			unsigned int numSamplesInPhoneme = meanNumSamplesInPhoneme <= numRemainingSamples ? meanNumSamplesInPhoneme : numRemainingSamples;
			short minSoundPressureInPhoneme = 0x7fff; // max short value
			short maxSoundPressureInPhoneme = -0x8000; // min short value
			for(unsigned int sampleIndex = 0; sampleIndex < numSamplesInPhoneme; ++sampleIndex )
			{
				short soundPressure = *pRemainingSamples;
				if( soundPressure < minSoundPressureInPhoneme )
				{
					minSoundPressureInPhoneme = soundPressure;
				}
				if( soundPressure > maxSoundPressureInPhoneme )
				{
					maxSoundPressureInPhoneme = soundPressure;
				}

				pRemainingSamples++;
			}
			static const int silenceThreshold = 5000;
			Phoneme::PhonemeType phonemeType;
			#ifdef TRACE_PHONETIZER_OUTPUT
			{
				char message[256];
				sprintf(message, "DummyPhonetizer::audioToPhonemes sound pressure %d - %d\n", minSoundPressureInPhoneme, maxSoundPressureInPhoneme );
				OutputDebugString( message );
			}
			#endif
			if( ( (int)maxSoundPressureInPhoneme - (int)minSoundPressureInPhoneme ) > silenceThreshold )
			{
				phonemeType = dummyPhonemes[ dummyPhonemeIndex ];
				dummyPhonemeIndex = (dummyPhonemeIndex+1) % numDummyPhonemes ;
			}
			else
			{
				dummyPhonemeIndex = 0; // restart dummy sequence from the beginning after a silence
				phonemeType = Phoneme::kPhonemeTypeSil;
			}
			float phonemeDuration = (float)numSamplesInPhoneme / (float)audioSequence.getFrequency();
			Phoneme phoneme( phonemeType, phonemeDuration );
			pPhonemeSequence->appendPhoneme( phoneme );

			#ifdef TRACE_PHONETIZER_OUTPUT
			{
				char message[256];
				sprintf(message, "DummyPhonetizer::audioToPhonemes appending phoneme : %d\n", phonemeType );
				OutputDebugString( message );
			}
			#endif
			numRemainingSamples -= numSamplesInPhoneme;
		}
		return pPhonemeSequence;
	}

}