#include "Phoneme.h"

namespace Solipsis
{
	Phoneme::Phoneme( PhonemeType phonemeType, float duration )
	: mPhonemeType( phonemeType )
	, mDuration( duration )
	{
	}

	Phoneme::~Phoneme()
	{
	}
}