/* $ANTLR 2.7.6 (20061021): "tags.g" -> "TagsParser.cpp"$ */
#line 13 "tags.g"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "TagsParser.hpp"
int TagsParser::ind_word = 0;
int TagsParser::ind_sentence = 1;

#line 14 "TagsParser.cpp"
#include "TagsParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 1 "tags.g"
#line 20 "TagsParser.cpp"
TagsParser::TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

TagsParser::TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,8)
{
}

TagsParser::TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

TagsParser::TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,8)
{
}

TagsParser::TagsParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,8)
{
}

void TagsParser::program() {
	
	{ // ( ... )+
	int _cnt3=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			sentence();
		}
		else {
			if ( _cnt3>=1 ) { goto _loop3; } else {
#ifdef WIN32
		std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;
		// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
		}
		
		_cnt3++;
	}
	_loop3:;
	}  // ( ... )+
}

void TagsParser::sentence() {
	
	clause();
	dot();
}

int  TagsParser::clause() {
#line 377 "tags.g"
	int type = -1;
#line 73 "TagsParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ADJECTIVE:
		case NOUN:
		case NOUN_PLURAL:
		case DETERMINER:
		case CARDINAL_NUMBER:
		{
			noun_phrase();
			{
			switch ( LA(1)) {
			case VERB_3SINGULAR:
			case VERB_NO3SINGULAR:
			{
				verb();
				{
				if ((LA(1) == ADJECTIVE || LA(1) == SUBORDINATING_CONJUNCTION || LA(1) == CARDINAL_NUMBER) && (_tokenSet_1.member(LA(2))) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))) && (_tokenSet_4.member(LA(5))) && (_tokenSet_5.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
					{
					if ((LA(1) == CARDINAL_NUMBER) && (LA(2) == NOUN || LA(2) == NOUN_PLURAL) && (LA(3) == ADJECTIVE || LA(3) == SUBORDINATING_CONJUNCTION || LA(3) == CARDINAL_NUMBER) && (_tokenSet_1.member(LA(4))) && (_tokenSet_2.member(LA(5))) && (_tokenSet_3.member(LA(6))) && (_tokenSet_4.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
						number_noun();
					}
					else if ((LA(1) == ADJECTIVE || LA(1) == SUBORDINATING_CONJUNCTION || LA(1) == CARDINAL_NUMBER) && (_tokenSet_1.member(LA(2))) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))) && (_tokenSet_4.member(LA(5))) && (_tokenSet_5.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
					}
					else {
#ifdef WIN32
						std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
					    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
						return -3;

						// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
					}
					
					}
					{
					switch ( LA(1)) {
					case SUBORDINATING_CONJUNCTION:
					{
						subordinating_conjunction();
						noun_phrase();
#line 383 "tags.g"
						
													   updateNoun();
													   updateRelationship();
													   invertLastsActors();// inversion
													   type = 1;
													
#line 116 "TagsParser.cpp"
						break;
					}
					case ADJECTIVE:
					case CARDINAL_NUMBER:
					{
						{ // ( ... )+
						int _cnt11=0;
						for (;;) {
							if ((LA(1) == ADJECTIVE || LA(1) == CARDINAL_NUMBER)) {
								adjective();
							}
							else {
								if ( _cnt11>=1 ) { goto _loop11; } else {
#ifdef WIN32
									std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
								    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
									return -3;

									// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
								}
							}
							
							_cnt11++;
						}
						_loop11:;
						}  // ( ... )+
#line 390 "tags.g"
						
											         updateNoun();
											         // ajout d'adjectifs au dernie acteur rencontre
											         if(_adjectives.size()!=0)
														{
															for(unsigned int i=0 ; i<_adjectives.size() ; i++)
															{
																// si l'adjectif est du type "2 feet tall", on ajoute 2_feet devant tall
																if(_numberNounStack.compare("")!=0)
																{
																	_ensActor[_ensActor.size()-1]->setAttribute(_numberNounStack+"_"+_adjectives[i]);
																	_numberNounStack = "";
																}
																else // sinon on ajoute rien
																	_ensActor[_ensActor.size()-1]->setAttribute(_adjectives[i]);
															}
															_adjectives.clear();
														}
											         type = 2;
											
#line 157 "TagsParser.cpp"
						break;
					}
					default:
					{
#ifdef WIN32
						std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
						MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
						return -3;

						// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
					}
					}
					}
				}
				else if ((_tokenSet_6.member(LA(1))) && (_tokenSet_7.member(LA(2))) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))) && (_tokenSet_4.member(LA(5))) && (_tokenSet_5.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
					noun_phrase();
#line 413 "tags.g"
					
										 updateNoun();
							          updateRelationship();
							          invertLastsActors();// inversion
							          type = 3;
							
#line 176 "TagsParser.cpp"
				}
				else if ((LA(1) == COMPARATIVE)) {
					comparative();
					noun_phrase();
#line 421 "tags.g"
					
										updateNoun();
										updateRelationship();
										invertLastsActors();
										type = 9;
									
#line 188 "TagsParser.cpp"
				}
				else {
#ifdef WIN32
					std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
					MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
					return -3;
					// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				
				}
				break;
			}
			case DOT:
			{
				break;
			}
			default:
			{
#ifdef WIN32
					std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
					MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
					return -3;
					//throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		case EXISTENTIAL:
		{
			existential();
			verb();
			{
			noun_phrase();
			}
#line 430 "tags.g"
			
							updateNoun();
							type = 4;
						
#line 221 "TagsParser.cpp"
			{
			{
			switch ( LA(1)) {
			case SUBORDINATING_CONJUNCTION:
			{
				subordinating_conjunction();
				{
				noun_phrase();
				}
#line 436 "tags.g"
				
								   updateNoun();
								   updateRelationship();
								   invertLastsActors();// inversion
								   type = 5;
								
#line 238 "TagsParser.cpp"
				break;
			}
			case DOT:
			{
				break;
			}
			default:
			{
#ifdef WIN32
				std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
				MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
				return -3;
				// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
			break;
		}
		case PERSONAL_PRONOUN:
		{
			personal_pronoun();
			verb();
			{
			if ((LA(1) == SUBORDINATING_CONJUNCTION || LA(1) == CARDINAL_NUMBER) && (_tokenSet_6.member(LA(2))) && (_tokenSet_8.member(LA(3))) && (_tokenSet_2.member(LA(4))) && (_tokenSet_3.member(LA(5))) && (_tokenSet_4.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
				{
				switch ( LA(1)) {
				case CARDINAL_NUMBER:
				{
					number_noun();
					break;
				}
				case SUBORDINATING_CONJUNCTION:
				{
					break;
				}
				default:
				{
#ifdef WIN32
					std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
					MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
					return -3;
					// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				subordinating_conjunction();
				noun_phrase();
#line 448 "tags.g"
				
												updateNoun();
												updateRelationship();
												invertLastsActors(); // inversion
												type = 7;
										
#line 286 "TagsParser.cpp"
			}
			else if ((LA(1) == ADJECTIVE || LA(1) == CARDINAL_NUMBER) && (_tokenSet_7.member(LA(2))) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))) && (_tokenSet_4.member(LA(5))) && (_tokenSet_5.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
				{ // ( ... )+
				int _cnt19=0;
				for (;;) {
					if ((LA(1) == ADJECTIVE || LA(1) == CARDINAL_NUMBER)) {
						adjective();
					}
					else {
						if ( _cnt19>=1 ) { goto _loop19; } else {
#ifdef WIN32
							std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
							MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
							return -3;
							// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
						}
					}
					
					_cnt19++;
				}
				_loop19:;
				}  // ( ... )+
#line 455 "tags.g"
				
											updateNoun();
											if(_adjectives.size()!=0)
											{
												for(unsigned int i=0 ; i<_adjectives.size() ; i++)
												{
													if(_numberNounStack.compare("")!=0)
													{
														_ensActor[_ensActor.size()-1]->setAttribute(_numberNounStack+"_"+_adjectives[i]);
														_numberNounStack = "";
													}
													else
														_ensActor[_ensActor.size()-1]->setAttribute(_adjectives[i]);
												}
												_adjectives.clear();
											}
										
#line 321 "TagsParser.cpp"
			}
			else if ((LA(1) == COMPARATIVE)) {
				comparative();
				noun_phrase();
#line 473 "tags.g"
				
												updateNoun();
												updateRelationship();
												invertLastsActors();
												type = 9;
										
#line 333 "TagsParser.cpp"
			}
			else if ((_tokenSet_6.member(LA(1))) && (_tokenSet_7.member(LA(2))) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))) && (_tokenSet_4.member(LA(5))) && (_tokenSet_5.member(LA(6))) && (_tokenSet_5.member(LA(7))) && (_tokenSet_5.member(LA(8)))) {
				noun_phrase();
#line 480 "tags.g"
				
											updateNoun();
											updateRelationship();
											invertLastsActors();// inversion
											type = 3;
										
#line 344 "TagsParser.cpp"
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			break;
		}
		case SUBORDINATING_CONJUNCTION:
		{
			subordinating_conjunction();
			noun_phrase();
			existential();
			verb();
			noun_phrase();
#line 488 "tags.g"
			
				         updateNoun();
				         invertLastsActors(); // inversion temporaire pour calculer les relations induites par la phrase
				         updateRelationship();
				         invertLastsActors(); // remise en etat normal
				         type = 8;
				
#line 368 "TagsParser.cpp"
			break;
		}
		default:
		{
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return -3;
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 496 "tags.g"
		
		#ifdef WIN32
				std::string msg = "Incorrect sentence. Please reformulate!";
			    std::cerr << msg << std::endl;
			    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		
				return -3;
			
#line 388 "TagsParser.cpp"
	}
	return type;
}

void TagsParser::dot() {
	
	match(DOT);
#line 670 "tags.g"
	updateNoun();
				TagsParser::ind_word++; TagsParser::ind_sentence++;
#line 399 "TagsParser.cpp"
}

void TagsParser::noun_phrase() {
	
	{
	switch ( LA(1)) {
	case DETERMINER:
	{
		determiner();
		break;
	}
	case ADJECTIVE:
	case NOUN:
	case NOUN_PLURAL:
	case CARDINAL_NUMBER:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == ADJECTIVE || LA(1) == CARDINAL_NUMBER)) {
			adjective();
		}
		else {
			goto _loop23;
		}
		
	}
	_loop23:;
	} // ( ... )*
	{ // ( ... )+
	int _cnt25=0;
	for (;;) {
		if ((LA(1) == NOUN || LA(1) == NOUN_PLURAL)) {
			noun();
		}
		else {
			if ( _cnt25>=1 ) { goto _loop25; } else {
#ifdef WIN32
				std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
				MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
				return;
				// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
		}
		
		_cnt25++;
	}
	_loop25:;
	}  // ( ... )+
}

void TagsParser::verb() {
	
	try {      // for error handling
		if ((LA(1) == VERB_3SINGULAR) && (_tokenSet_9.member(LA(2)))) {
			match(VERB_3SINGULAR);
#line 512 "tags.g"
			TagsParser::ind_word++;
#line 459 "TagsParser.cpp"
		}
		else if ((LA(1) == VERB_3SINGULAR) && (LA(2) == VERB_GERUND) && (_tokenSet_9.member(LA(3)))) {
			match(VERB_3SINGULAR);
#line 515 "tags.g"
			TagsParser::ind_word++;
#line 465 "TagsParser.cpp"
			match(VERB_GERUND);
#line 517 "tags.g"
			_lastVerb = _words[TagsParser::ind_word];
							TagsParser::ind_word++;
#line 470 "TagsParser.cpp"
		}
		else if ((LA(1) == VERB_3SINGULAR) && (LA(2) == VERB_GERUND) && (LA(3) == VERB_PAST_PARTICIPLE)) {
			match(VERB_3SINGULAR);
#line 521 "tags.g"
			TagsParser::ind_word++;
#line 476 "TagsParser.cpp"
			match(VERB_GERUND);
#line 523 "tags.g"
			_lastVerb = _words[TagsParser::ind_word];
							TagsParser::ind_word++;
#line 481 "TagsParser.cpp"
			match(VERB_PAST_PARTICIPLE);
#line 526 "tags.g"
			TagsParser::ind_word++;
#line 485 "TagsParser.cpp"
		}
		else if ((LA(1) == VERB_NO3SINGULAR)) {
			match(VERB_NO3SINGULAR);
#line 529 "tags.g"
			TagsParser::ind_word++;
#line 491 "TagsParser.cpp"
		}
		else {
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return;
			// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 531 "tags.g"
		
		//		std::cout << "!!!EXCEPTION!!! :  verbe incorrect" << std::endl;
				std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
		#ifdef WIN32
				std::string msg = "Incorrect verb. Please reformulate the sentence";
			    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		#endif
		
			
#line 509 "TagsParser.cpp"
	}
}

void TagsParser::number_noun() {
	
	try {      // for error handling
		cardinal_number();
		c_noun();
#line 587 "tags.g"
		
		_numberNounStack = _words[TagsParser::ind_word-2] + "_" + _words[TagsParser::ind_word-1];
		
#line 522 "TagsParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 591 "tags.g"
		
		//		std::cout << "!!!EXCEPTION!!! :  (nombre + nom) incorrecte" << std::endl;
				std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
		#ifdef WIN32
				std::string msg = "Incorrect formulation (number + noun). Please reformulate the sentence.";
			    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		
				return;
			
#line 536 "TagsParser.cpp"
	}
}

void TagsParser::subordinating_conjunction() {
	
	try {      // for error handling
		match(SUBORDINATING_CONJUNCTION);
#line 570 "tags.g"
		
						_lastRelation = _words[TagsParser::ind_word];
						TagsParser::ind_word++;
					
#line 549 "TagsParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 575 "tags.g"
		
		//		std::cout << "!!!EXCEPTION!!! :  conjonction de coordination incorrecte" << std::endl;
				std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
		#ifdef WIN32
				std::string msg = "Incorrect coordination conjunction. Please reformulate the sentence";
			    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		
			
#line 562 "TagsParser.cpp"
	}
}

void TagsParser::adjective() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case ADJECTIVE:
		{
			match(ADJECTIVE);
#line 544 "tags.g"
			
							updateNoun();
							_adjectives.push_back(_words[TagsParser::ind_word]);
							TagsParser::ind_word++;
				  		
#line 579 "TagsParser.cpp"
			break;
		}
		case CARDINAL_NUMBER:
		{
			number_noun();
			{
			match(ADJECTIVE);
#line 550 "tags.g"
			
							updateNoun();
							_adjectives.push_back(_words[TagsParser::ind_word-2]+"_"+_words[TagsParser::ind_word-1]+"_"+_words[TagsParser::ind_word]);
							TagsParser::ind_word++;
						
#line 593 "TagsParser.cpp"
			}
			break;
		}
		default:
		{
#ifdef WIN32
			std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
			MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
			return;
			// throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 557 "tags.g"
		
		//		std::cout << "!!!EXCEPTION!!! :  adjectif incorrect" << std::endl;
				std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
		#ifdef WIN32
				std::string msg = "Incorrect adjective. Please reformulate the sentence";
			    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		
			
#line 614 "TagsParser.cpp"
	}
}

void TagsParser::comparative() {
	
	match(COMPARATIVE);
#line 635 "tags.g"
	_lastRelation = _words[TagsParser::ind_word];
					TagsParser::ind_word++;
#line 624 "TagsParser.cpp"
	match(SUBORDINATING_CONJUNCTION);
#line 638 "tags.g"
	TagsParser::ind_word++;
#line 628 "TagsParser.cpp"
}

void TagsParser::existential() {
	
	match(EXISTENTIAL);
#line 652 "tags.g"
	updateNoun();
		  			TagsParser::ind_word++;
#line 637 "TagsParser.cpp"
}

void TagsParser::personal_pronoun() {
	
	match(PERSONAL_PRONOUN);
#line 658 "tags.g"
	updateNoun();
		  			TagsParser::ind_word++;
#line 646 "TagsParser.cpp"
}

void TagsParser::determiner() {
	
	match(DETERMINER);
#line 645 "tags.g"
	updateNoun();
					_lastDeterminant = _words[TagsParser::ind_word];
					TagsParser::ind_word++;
#line 656 "TagsParser.cpp"
}

void TagsParser::noun() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case NOUN:
		{
			match(NOUN);
#line 614 "tags.g"
			_singular=true;
							_nouns.push_back(_words[TagsParser::ind_word]);
							TagsParser::ind_word++;
#line 670 "TagsParser.cpp"
			break;
		}
		case NOUN_PLURAL:
		{
			match(NOUN_PLURAL);
#line 618 "tags.g"
			_singular=false;
							_nouns.push_back(_words[TagsParser::ind_word]);
							TagsParser::ind_word++;
#line 680 "TagsParser.cpp"
			break;
		}
		default:
		{
		#ifdef WIN32
		std::string msg = "There are some unknown/ununderstandable nouns in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;

//			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex) {
#line 622 "tags.g"
		
		// 		std::cout << "!!!EXCEPTION!!! :  Nom incorrect" << std::endl;
				std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
		#ifdef WIN32
				std::string msg = "There is an incorrect noun in the sentence. Please reformulate.";
			    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		#endif
				return;
			
#line 700 "TagsParser.cpp"
	}
}

void TagsParser::cardinal_number() {
	
	match(CARDINAL_NUMBER);
#line 664 "tags.g"
	updateNoun();
					TagsParser::ind_word++;
#line 710 "TagsParser.cpp"
}

void TagsParser::c_noun() {
	
	switch ( LA(1)) {
	case NOUN:
	{
		match(NOUN);
#line 605 "tags.g"
		_singular=true;
						TagsParser::ind_word++;
#line 722 "TagsParser.cpp"
		break;
	}
	case NOUN_PLURAL:
	{
		match(NOUN_PLURAL);
#line 608 "tags.g"
		_singular=false;
						TagsParser::ind_word++;
#line 731 "TagsParser.cpp"
		break;
	}
	default:
	{
#ifdef WIN32
		std::string msg = "There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
#endif
		return;
//		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void TagsParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* TagsParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"VBZ\"",
	"\"VBG\"",
	"\"VBN\"",
	"\"VBP\"",
	"\"JJ\"",
	"\"IN\"",
	"\"NN\"",
	"\"NNS\"",
	"\"JJR\"",
	"\"DT\"",
	"\"EX\"",
	"\"PRP\"",
	"\"CD\"",
	"DOT",
	"\"CC\"",
	"\"JJS\"",
	"\"NNP\"",
	"\"NNPS\"",
	"\"POS\"",
	"\"RB\"",
	"\"RBR\"",
	"\"RBS\"",
	"\"RP\"",
	"\"TO\"",
	"\"VB\"",
	"\"VBD\"",
	"\"WDT\"",
	"\"WP\"",
	"\"WP$\"",
	"\"WRB\"",
	"ALPHA",
	"WS",
	0
};

const unsigned long TagsParser::_tokenSet_0_data_[] = { 126720UL, 0UL, 0UL, 0UL };
// "JJ" "IN" "NN" "NNS" "DT" "EX" "PRP" "CD" 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long TagsParser::_tokenSet_1_data_[] = { 208128UL, 0UL, 0UL, 0UL };
// "JJ" "NN" "NNS" "DT" "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long TagsParser::_tokenSet_2_data_[] = { 257794UL, 0UL, 0UL, 0UL };
// EOF "JJ" "IN" "NN" "NNS" "DT" "EX" "PRP" "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long TagsParser::_tokenSet_3_data_[] = { 257938UL, 0UL, 0UL, 0UL };
// EOF "VBZ" "VBP" "JJ" "IN" "NN" "NNS" "DT" "EX" "PRP" "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long TagsParser::_tokenSet_4_data_[] = { 262066UL, 0UL, 0UL, 0UL };
// EOF "VBZ" "VBG" "VBP" "JJ" "IN" "NN" "NNS" "JJR" "DT" "EX" "PRP" "CD" 
// DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long TagsParser::_tokenSet_5_data_[] = { 262130UL, 0UL, 0UL, 0UL };
// EOF "VBZ" "VBG" "VBN" "VBP" "JJ" "IN" "NN" "NNS" "JJR" "DT" "EX" "PRP" 
// "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long TagsParser::_tokenSet_6_data_[] = { 77056UL, 0UL, 0UL, 0UL };
// "JJ" "NN" "NNS" "DT" "CD" 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long TagsParser::_tokenSet_7_data_[] = { 199936UL, 0UL, 0UL, 0UL };
// "JJ" "NN" "NNS" "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long TagsParser::_tokenSet_8_data_[] = { 200448UL, 0UL, 0UL, 0UL };
// "JJ" "IN" "NN" "NNS" "CD" DOT 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long TagsParser::_tokenSet_9_data_[] = { 81664UL, 0UL, 0UL, 0UL };
// "JJ" "IN" "NN" "NNS" "JJR" "DT" "CD" 
const ANTLR_USE_NAMESPACE(antlr)BitSet TagsParser::_tokenSet_9(_tokenSet_9_data_,4);


