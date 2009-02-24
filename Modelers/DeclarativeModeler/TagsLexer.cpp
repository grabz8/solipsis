/* $ANTLR 2.7.6 (20061021): "tags.g" -> "TagsLexer.cpp"$ */
#line 13 "tags.g"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "TagsParser.hpp"
// int TagsParser::ind_word = 0;
// int TagsParser::ind_sentence = 1;

#line 14 "TagsLexer.cpp"
#include "TagsLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

#line 1 "tags.g"
#line 25 "TagsLexer.cpp"
TagsLexer::TagsLexer(ANTLR_USE_NAMESPACE(std)istream& in)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
{
	initLiterals();
}

TagsLexer::TagsLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(ib,true)
{
	initLiterals();
}

TagsLexer::TagsLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(state,true)
{
	initLiterals();
}

void TagsLexer::initLiterals()
{
	literals["WRB"] = 33;
	literals["WP$"] = 32;
	literals["JJS"] = 19;
	literals["TO"] = 27;
	literals["VB"] = 28;
	literals["NN"] = 10;
	literals["NNP"] = 20;
	literals["NNS"] = 11;
	literals["RBS"] = 25;
	literals["DT"] = 13;
	literals["JJR"] = 12;
	literals["WP"] = 31;
	literals["NNPS"] = 21;
	literals["VBN"] = 6;
	literals["RB"] = 23;
	literals["PRP"] = 15;
	literals["RBR"] = 24;
	literals["VBZ"] = 4;
	literals["CD"] = 16;
	literals["IN"] = 9;
	literals["WDT"] = 30;
	literals["VBD"] = 29;
	literals["VBG"] = 5;
	literals["JJ"] = 8;
	literals["VBP"] = 7;
	literals["EX"] = 14;
	literals["POS"] = 22;
	literals["CC"] = 18;
	literals["RP"] = 26;
}

ANTLR_USE_NAMESPACE(antlr)RefToken TagsLexer::nextToken()
{
	ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
	for (;;) {
		ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
		int _ttype = ANTLR_USE_NAMESPACE(antlr)Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0x2e /* '.' */ :
			{
				mDOT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x9 /* '\t' */ :
			case 0xa /* '\n' */ :
			case 0xd /* '\r' */ :
			case 0x20 /* ' ' */ :
			{
				mWS(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((LA(1) == 0x4e /* 'N' */ ) && (LA(2) == 0x4e /* 'N' */ ) && (LA(3) == 0x50 /* 'P' */ ) && (LA(4) == 0x53 /* 'S' */ ) && (true) && (true) && (true) && (true)) {
					mPROPER_NOUN_PLURAL(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4a /* 'J' */ ) && (LA(2) == 0x4a /* 'J' */ ) && (LA(3) == 0x52 /* 'R' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mCOMPARATIVE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4a /* 'J' */ ) && (LA(2) == 0x4a /* 'J' */ ) && (LA(3) == 0x53 /* 'S' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mSUPERLATIVE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4e /* 'N' */ ) && (LA(2) == 0x4e /* 'N' */ ) && (LA(3) == 0x53 /* 'S' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mNOUN_PLURAL(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4e /* 'N' */ ) && (LA(2) == 0x4e /* 'N' */ ) && (LA(3) == 0x50 /* 'P' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mPROPER_NOUN_SINGULAR(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x50 /* 'P' */ ) && (LA(2) == 0x4f /* 'O' */ ) && (LA(3) == 0x53 /* 'S' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mPOSSESSIVE_ENDING(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x50 /* 'P' */ ) && (LA(2) == 0x52 /* 'R' */ ) && (LA(3) == 0x50 /* 'P' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mPERSONAL_PRONOUN(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x52 /* 'R' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x52 /* 'R' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mADVERB_COMPARATIVE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x52 /* 'R' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x53 /* 'S' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mADVERB_SUPERLATIVE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x44 /* 'D' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mVERB_PAST(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x47 /* 'G' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mVERB_GERUND(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x4e /* 'N' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mVERB_PAST_PARTICIPLE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x50 /* 'P' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mVERB_NO3SINGULAR(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (LA(3) == 0x5a /* 'Z' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mVERB_3SINGULAR(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x57 /* 'W' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x54 /* 'T' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mWH_DETERMINER(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x57 /* 'W' */ ) && (LA(2) == 0x50 /* 'P' */ ) && (LA(3) == 0x24 /* '$' */ )) {
					mPOSSESSIVE_PRONOUN(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x57 /* 'W' */ ) && (LA(2) == 0x52 /* 'R' */ ) && (LA(3) == 0x42 /* 'B' */ ) && (true) && (true) && (true) && (true) && (true)) {
					mWH_ADVERB(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x43 /* 'C' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mCOORDINATING_CONJUNCTION(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x52 /* 'R' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mADVERB(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x52 /* 'R' */ ) && (LA(2) == 0x50 /* 'P' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mPARTICLE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x54 /* 'T' */ ) && (LA(2) == 0x4f /* 'O' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mTO(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mCARDINAL_NUMBER(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x44 /* 'D' */ ) && (LA(2) == 0x54 /* 'T' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mDETERMINER(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x45 /* 'E' */ ) && (LA(2) == 0x58 /* 'X' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mEXISTENTIAL(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x49 /* 'I' */ ) && (LA(2) == 0x4e /* 'N' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mSUBORDINATING_CONJUNCTION(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4a /* 'J' */ ) && (LA(2) == 0x4a /* 'J' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mADJECTIVE(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x4e /* 'N' */ ) && (LA(2) == 0x4e /* 'N' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mNOUN(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x56 /* 'V' */ ) && (LA(2) == 0x42 /* 'B' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mVERB(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x57 /* 'W' */ ) && (LA(2) == 0x50 /* 'P' */ ) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mWH_PRONOUN(true);
					theRetToken=_returnToken;
				}
				else if ((_tokenSet_0.member(LA(1))) && (true) && (true) && (true) && (true) && (true) && (true) && (true)) {
					mALPHA(true);
					theRetToken=_returnToken;
				}
			else {
				if (LA(1)==EOF_CHAR)
				{
					uponEOF();
					_returnToken = makeToken(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
				}
				else {
#ifdef WIN32
					std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
					MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
					theRetToken=_returnToken;
					return theRetToken;

					//throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
			}
			}
			if ( !_returnToken )
				goto tryAgain; // found SKIP token

			_ttype = _returnToken->getType();
			_ttype = testLiteralsTable(_ttype);
			_returnToken->setType(_ttype);
			return _returnToken;
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& e) {
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling RecognitionException", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return _returnToken;

				//throw ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(e);
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamIOException& csie) {
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling CharStreamIOException", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return _returnToken;
			//throw ANTLR_USE_NAMESPACE(antlr)TokenStreamIOException(csie.io);
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamException& cse) {
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling CharStreamException", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return _returnToken;
			//throw ANTLR_USE_NAMESPACE(antlr)TokenStreamException(cse.getMessage());
		}
tryAgain:;
	}
}

void TagsLexer::mALPHA(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ALPHA;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt41=0;
	for (;;) {
		switch ( LA(1)) {
		case 0x61 /* 'a' */ :
		case 0x62 /* 'b' */ :
		case 0x63 /* 'c' */ :
		case 0x64 /* 'd' */ :
		case 0x65 /* 'e' */ :
		case 0x66 /* 'f' */ :
		case 0x67 /* 'g' */ :
		case 0x68 /* 'h' */ :
		case 0x69 /* 'i' */ :
		case 0x6a /* 'j' */ :
		case 0x6b /* 'k' */ :
		case 0x6c /* 'l' */ :
		case 0x6d /* 'm' */ :
		case 0x6e /* 'n' */ :
		case 0x6f /* 'o' */ :
		case 0x70 /* 'p' */ :
		case 0x71 /* 'q' */ :
		case 0x72 /* 'r' */ :
		case 0x73 /* 's' */ :
		case 0x74 /* 't' */ :
		case 0x75 /* 'u' */ :
		case 0x76 /* 'v' */ :
		case 0x77 /* 'w' */ :
		case 0x78 /* 'x' */ :
		case 0x79 /* 'y' */ :
		case 0x7a /* 'z' */ :
		{
			matchRange('a','z');
			break;
		}
		case 0x41 /* 'A' */ :
		case 0x42 /* 'B' */ :
		case 0x43 /* 'C' */ :
		case 0x44 /* 'D' */ :
		case 0x45 /* 'E' */ :
		case 0x46 /* 'F' */ :
		case 0x47 /* 'G' */ :
		case 0x48 /* 'H' */ :
		case 0x49 /* 'I' */ :
		case 0x4a /* 'J' */ :
		case 0x4b /* 'K' */ :
		case 0x4c /* 'L' */ :
		case 0x4d /* 'M' */ :
		case 0x4e /* 'N' */ :
		case 0x4f /* 'O' */ :
		case 0x50 /* 'P' */ :
		case 0x51 /* 'Q' */ :
		case 0x52 /* 'R' */ :
		case 0x53 /* 'S' */ :
		case 0x54 /* 'T' */ :
		case 0x55 /* 'U' */ :
		case 0x56 /* 'V' */ :
		case 0x57 /* 'W' */ :
		case 0x58 /* 'X' */ :
		case 0x59 /* 'Y' */ :
		case 0x5a /* 'Z' */ :
		{
			matchRange('A','Z');
			break;
		}
		default:
		{
			if ( _cnt41>=1 ) { goto _loop41; } else {
#ifdef WIN32
				std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
				MessageBox(0, msg.c_str(), "Declarative modeling CNT41", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
				return;
				//throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
			}
		}
		}
		_cnt41++;
	}
	_loop41:;
	}  // ( ... )+
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mCOORDINATING_CONJUNCTION(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COORDINATING_CONJUNCTION;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("CC");
#line 761 "tags.g"
		std::cout << "Ne prend pas encore en compte les 'ET' POUR L'INSTANT..." << std::endl;
#ifdef WIN32
		std::string msg = "Does not take coordinating conjunctions into account. There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;

			exit(-1);
		
#line 348 "TagsLexer.cpp"
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mADVERB(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ADVERB;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("RB");
#line 767 "tags.g"
		std::cout << "Ne prend pas encore en compte les 'RB' POUR L'INSTANT..." << std::endl;
#ifdef WIN32
		std::string msg = "Does not take adverbs into account. There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;

		// exit(-1);
		
#line 367 "TagsLexer.cpp"
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPARTICLE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PARTICLE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("RP");
#line 773 "tags.g"
#ifdef WIN32
		std::string msg = "There are some unknown/ununderstandable elements (RP) in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;
		std::cout << "Ne prend pas encore en compte les 'RP' POUR L'INSTANT..." << std::endl;
			//exit(-1);
		
#line 386 "TagsLexer.cpp"
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mTO(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TO;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("TO");
#line 779 "tags.g"
#ifdef WIN32
		std::string msg = "There are some unknown/ununderstandable elements (TO) in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;
		std::cout << "Ne prend pas encore en compte les 'TO' POUR L'INSTANT..." << std::endl;
			//exit(-1);
		
#line 405 "TagsLexer.cpp"
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mCARDINAL_NUMBER(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = CARDINAL_NUMBER;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("CD");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mDETERMINER(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = DETERMINER;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("DT");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mEXISTENTIAL(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = EXISTENTIAL;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("EX");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mSUBORDINATING_CONJUNCTION(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = SUBORDINATING_CONJUNCTION;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("IN");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mADJECTIVE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ADJECTIVE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("JJ");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mCOMPARATIVE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COMPARATIVE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("JJR");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mSUPERLATIVE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = SUPERLATIVE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("JJS");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mNOUN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NOUN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("NN");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mNOUN_PLURAL(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NOUN_PLURAL;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("NNS");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPROPER_NOUN_SINGULAR(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PROPER_NOUN_SINGULAR;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("NNP");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPROPER_NOUN_PLURAL(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PROPER_NOUN_PLURAL;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("NNPS");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPOSSESSIVE_ENDING(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = POSSESSIVE_ENDING;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("POS");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPERSONAL_PRONOUN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PERSONAL_PRONOUN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("PRP");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mADVERB_COMPARATIVE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ADVERB_COMPARATIVE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("RBR");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mADVERB_SUPERLATIVE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ADVERB_SUPERLATIVE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("RBS");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VB");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB_PAST(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB_PAST;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VBD");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB_GERUND(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB_GERUND;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VBG");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB_PAST_PARTICIPLE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB_PAST_PARTICIPLE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VBN");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB_NO3SINGULAR(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB_NO3SINGULAR;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VBP");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mVERB_3SINGULAR(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = VERB_3SINGULAR;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("VBZ");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mWH_DETERMINER(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = WH_DETERMINER;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("WDT");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mWH_PRONOUN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = WH_PRONOUN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("WP");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mPOSSESSIVE_PRONOUN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = POSSESSIVE_PRONOUN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("WP$");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mWH_ADVERB(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = WH_ADVERB;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("WRB");
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mDOT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = DOT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('.' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TagsLexer::mWS(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = WS;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt74=0;
	for (;;) {
		switch ( LA(1)) {
		case 0x20 /* ' ' */ :
		{
			match(' ' /* charlit */ );
			break;
		}
		case 0x9 /* '\t' */ :
		{
			match('\t' /* charlit */ );
			break;
		}
		case 0xd /* '\r' */ :
		{
			match('\r' /* charlit */ );
#line 815 "tags.g"
			newline();
#line 802 "TagsLexer.cpp"
			break;
		}
		case 0xa /* '\n' */ :
		{
			match('\n' /* charlit */ );
#line 816 "tags.g"
			newline();
#line 810 "TagsLexer.cpp"
			break;
		}
		default:
		{
			if ( _cnt74>=1 ) { goto _loop74; } else {
#ifdef WIN32
				std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
				MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
				return;
				//throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
			}
		}
		}
		_cnt74++;
	}
	_loop74:;
	}  // ( ... )+
#line 818 "tags.g"
	_ttype = antlr::Token::SKIP;
#line 824 "TagsLexer.cpp"
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long TagsLexer::_tokenSet_0_data_[] = { 0UL, 0UL, 134217726UL, 134217726UL, 0UL, 0UL, 0UL, 0UL };
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsLexer::_tokenSet_0(_tokenSet_0_data_,8);

