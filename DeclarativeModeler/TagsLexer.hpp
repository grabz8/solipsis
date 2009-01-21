#ifndef INC_TagsLexer_hpp_
#define INC_TagsLexer_hpp_

#line 2 "tags.g"

#include <iostream>
#include <antlr/RecognitionException.hpp>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Actor.h"
#include "Link.h"

#line 15 "TagsLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.6 (20061021): "tags.g" -> "TagsLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "TagsTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
class CUSTOM_API TagsLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public TagsTokenTypes
{
#line 1 "tags.g"
#line 26 "TagsLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	TagsLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	TagsLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	TagsLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mALPHA(bool _createToken);
	public: void mCOORDINATING_CONJUNCTION(bool _createToken);
	public: void mADVERB(bool _createToken);
	public: void mPARTICLE(bool _createToken);
	public: void mTO(bool _createToken);
	public: void mCARDINAL_NUMBER(bool _createToken);
	public: void mDETERMINER(bool _createToken);
	public: void mEXISTENTIAL(bool _createToken);
	public: void mSUBORDINATING_CONJUNCTION(bool _createToken);
	public: void mADJECTIVE(bool _createToken);
	public: void mCOMPARATIVE(bool _createToken);
	public: void mSUPERLATIVE(bool _createToken);
	public: void mNOUN(bool _createToken);
	public: void mNOUN_PLURAL(bool _createToken);
	public: void mPROPER_NOUN_SINGULAR(bool _createToken);
	public: void mPROPER_NOUN_PLURAL(bool _createToken);
	public: void mPOSSESSIVE_ENDING(bool _createToken);
	public: void mPERSONAL_PRONOUN(bool _createToken);
	public: void mADVERB_COMPARATIVE(bool _createToken);
	public: void mADVERB_SUPERLATIVE(bool _createToken);
	public: void mVERB(bool _createToken);
	public: void mVERB_PAST(bool _createToken);
	public: void mVERB_GERUND(bool _createToken);
	public: void mVERB_PAST_PARTICIPLE(bool _createToken);
	public: void mVERB_NO3SINGULAR(bool _createToken);
	public: void mVERB_3SINGULAR(bool _createToken);
	public: void mWH_DETERMINER(bool _createToken);
	public: void mWH_PRONOUN(bool _createToken);
	public: void mPOSSESSIVE_PRONOUN(bool _createToken);
	public: void mWH_ADVERB(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mWS(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
};

#endif /*INC_TagsLexer_hpp_*/
