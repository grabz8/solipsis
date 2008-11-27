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

#ifndef __Phoneme_h__
#define __Phoneme_h__

#ifdef PHONETIZER_EXPORTS
	#define PHONETIZER_EXPORT __declspec( dllexport )
#else
	#define PHONETIZER_EXPORT
#endif 

namespace Solipsis 
{

	/**
		@brief	the semantics associated with a very short (around 100 ms) audio sequence.
		
		It usually labels an elementary brick of a speech. It could represent a phoneme or silence or noise, etc...
	*/
	class PHONETIZER_EXPORT Phoneme
	{
	public:

		enum AudioType
		{
			kAudioTypeSpeech,
			kAudioTypeMusic,
			kAudioTypeNoise,
			kAudioTypeSilence,
			kAudioTypeOther
		};

		enum PhonemeType
		{
			// this list comes from Patrice Collen and Jean-Bernard Rault phonetisation system

			// voyelles
			kPhonemeTypeA,		// plat
			kPhonemeTypeI,		// pile
			kPhonemeTypeU,		// rue
			kPhonemeTypeO,		// encore, forte, alors
			kPhonemeTypeAU,		// bEAU, tôt, sEAU
			kPhonemeTypeEU,		// pEU, le?
			kPhonemeTypeOE,		// hEUre
			kPhonemeTypeEI,		// blé
			kPhonemeTypeAI,		// lAIt
			kPhonemeTypeOU,		// roue

			// voyelles nasales
			kPhonemeTypeAN,		// blanc
			kPhonemeTypeON,		// bon
			kPhonemeTypeIN,		// lin, brun

			// semi-voyelles
			kPhonemeTypeY,		// action, bien, hier
			kPhonemeTypeW,		// boite, web, oui, coin
			kPhonemeTypeUY,		// lui

			// liquides
			kPhonemeTypeL,		// Lent
			kPhonemeTypeR,		// Rue

			// nasales
			kPhonemeTypeM,		// Mot
			kPhonemeTypeN,		// Nous

			// fricatives sourdes
			kPhonemeTypeF,		// Fer
			kPhonemeTypeS,		// aSSis
			kPhonemeTypeCH,		// CHou

			// fricatives sonores
			kPhonemeTypeV,		// Verre
			kPhonemeTypeZ,		// aSie
			kPhonemeTypeJ,		// Joue

			// occlusives sourdes
			kPhonemeTypeP,		// Peu
			kPhonemeTypeT,		// Ton
			kPhonemeTypeK,		// Cou

			// occlusives sonores
			kPhonemeTypeB,		// Basse
			kPhonemeTypeD,		// Doux
			kPhonemeTypeG,		// Goût

			// spéciaux
			kPhonemeTypeSil,	// silence
			kPhonemeTypeii,		// inspiration
			kPhonemeTypesp,		// short pause
		};
		//typedef unsigned short int IpaNumber; ///< from international phonetics alphabet number chart (http://en.wikipedia.org/wiki/International_Phonetic_Alphabet, http://www.bas.uni-muenchen.de/studium/skripten/IPA/IPANumberChart96.pdf)

		///brief Contructor
		Phoneme( PhonemeType phonemeType, float duration );
		///brief Destructor
		~Phoneme();

		/**
			@brief	gets the duration of this audio semantics unit
		*/
		float	getDuration( void ) const { return mDuration; }

		PhonemeType getPhonemeType( void ) const { return mPhonemeType; }

		AudioType	getAudioType( void ) const { return mAudioType; }

	private:
		AudioType	mAudioType;	///< the global category for this audio sequence
		PhonemeType	mPhonemeType;	///< only valid if the audio type is speech. Undefined otherwise.
		float	mDuration;	///< duration of the phoneme in seconds
	};

}

#endif //__Phoneme_h__
