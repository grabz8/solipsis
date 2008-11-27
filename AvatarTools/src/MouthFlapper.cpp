#include "MouthFlapper.h"
#include "MouthFlapperCreator.h"
#include "CharacterInstance.h"
#include <Ogre.h>
#include <VoicePacket.h>
using namespace Solipsis;

#define TRACE_MOUTH_FLAPPER_INPUT
#ifdef TRACE_MOUTH_FLAPPER_INPUT
	#include <Windows.h>
#endif


MouthFlapper::MouthFlapper( CharacterInstance* pFaceControllerOwner, MouthFlapperCreator* pCharacterFacialData )
: mJawBone(0)
, mCharacterFacialData( pCharacterFacialData )
, mLastFrameTime(0.0f)
, mEndOfSpeechTime(0.0f)
, mCurrentVoicePacket(0)
, mPhonemeCursor(0)
{
	mJawBone = pFaceControllerOwner->getEntity()->getSkeleton()->getBone(mCharacterFacialData->getJawBoneName());
	assert(mJawBone);
	mJawBone->setManuallyControlled( true );
}

MouthFlapper::~MouthFlapper()
{
	if( mCurrentVoicePacket )
	{
		assert( mPhonemeCursor );
		delete mPhonemeCursor;
		delete mCurrentVoicePacket;
	}
}

void MouthFlapper::onVoicePacketReception( VoicePacket* pVoicePacket )
{
	if( mCurrentVoicePacket )
	{
		assert( mPhonemeCursor );
		delete mPhonemeCursor;
		delete mCurrentVoicePacket;
	}

	mCurrentVoicePacket = pVoicePacket;
	mPhonemeCursor = new PhonemeSequence::Cursor( *pVoicePacket->getPhonemeSequence() );
	mEndOfSpeechTime = mLastFrameTime + pVoicePacket->getDuration();
}

enum Enum1
{
	Enum1_1,
	Enum1_2
};

enum Enum2
{
	Enum2_1,
	Enum2_2
};

void MouthFlapper::updateFace(float timeSinceLastFrame)
{
//	Phoneme::PhonemeType currentPhonemeType = Enum2_2;
	Enum1 e1 = Enum1_1;
	if(e1 == Enum2_2)
	{
		int toto = 0;
	}
	float amountOfOpen;

	if( mLastFrameTime > mEndOfSpeechTime )
	{
		// no longer speaking
		amountOfOpen = 0.0f;
	}
	else
	{
		bool bPlayingSilencePhoneme = false;
		if( mPhonemeCursor )
		{
			Phoneme::PhonemeType currentPhonemeType = mPhonemeCursor->getPointedPhonemeType();
			#ifdef TRACE_MOUTH_FLAPPER_INPUT
			{
				char message[256];
				sprintf(message, "MouthFlapper::updateFace current phoneme : %d\n", currentPhonemeType );
				OutputDebugString( message );
			}
			#endif

			if( currentPhonemeType == Phoneme::kPhonemeTypeSil )
			{
				bPlayingSilencePhoneme = true;
			}
		}

		if(bPlayingSilencePhoneme)
		{
			amountOfOpen = 0.0f;
		}
		else
		{
			const float frequency = 3.0f * Math::PI * 2.0f; // rad / second
			amountOfOpen = sinf( mLastFrameTime * frequency ) * 0.5f + 0.5f;
		}
	}
	/*
	static Ogre::Radian angle( 0.05f );
	mJawBone->rotate( Ogre::Vector3( 1.0f, 0.0f, 0.0f ), angle );
	*/
	Quaternion jawOrientation = Quaternion::Slerp( amountOfOpen, mCharacterFacialData->getClosedJawOrientation(), mCharacterFacialData->getOpenJawOrientation() );
	mJawBone->setOrientation( jawOrientation );
	mLastFrameTime += timeSinceLastFrame;

	if( mPhonemeCursor )
	{
		mPhonemeCursor->evolve( timeSinceLastFrame );
	}
}