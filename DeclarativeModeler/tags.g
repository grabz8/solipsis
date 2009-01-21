header "pre_include_hpp"
{
#include <iostream>
#include <antlr/RecognitionException.hpp>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Actor.h"
#include "Link.h"
}

header "pre_include_cpp"
{
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "TagsParser.hpp"
int TagsParser::ind_word = 0;
int TagsParser::ind_sentence = 1;
}

options {
	language="Cpp";
}










//-------------------------------------------------PARSER----------------------------------------------------------------------------
class TagsParser extends Parser;

options {
   k=8;
   defaultErrorHandler=false;
	exportVocab = Tags;
}

{
	int indice;
	
	// indice du mot
	static int ind_word;
	
	// indice de la phrase
	static int ind_sentence;

	// Le texte original
	std::string _text;

	// la derniere relation rencontree
	std::string _lastRelation;

	// le dernier determinant
	std::string _lastDeterminant;
	
	// le dernier verbe
	std::string _lastVerb;

	// l'ensemble des mots stockes dans un vecteur
	std::vector<std::string> _words;

	// l'ensemble des acteurs a charger
	std::vector<Actor *> _ensActor;

	// les adk=jectifs temporairement stockes
	std::vector<std::string> _adjectives;

	// les noms temporairement stockes
	std::vector<std::string> _nouns;

	// l'ensemble des liens
	std::vector<Link*> _ensLinks;
	
	std::string _numberNounStack;

	// le type de chaque nom
	bool _singular;

	public:

	void init() {
		ind_word = 0;
		ind_sentence = 1;
		_words.clear();
		_ensActor.clear();
		_adjectives.clear();
		_nouns.clear();
		_ensLinks.clear();
	}
	
	// initialise le texte
	void setWords(std::string & texte) {
		indice = 0;
		_text = texte;
	}

	std::string & getSentence()
	{
		return _text;
	}

	std::vector<Link*> & getRelations()
	{
		return _ensLinks;
	}

	std::vector<Actor*> & getActors()
	{
		return _ensActor;
	}

	// remplacement de certains mots dans le texte complet
	static void replace(std::string & s, const std::string & s1, const std::string & s2, const char skip=0) {
		std::string::size_type pos = 0;
		while (1) {
			std::string::size_type i = s.find(s1, pos);
			if (i == std::string::npos) break;
			if (i > 0 && s[i-1] == skip) {
				pos = i + 1;
				continue;
			}
			s.replace(i, s1.size(), s2);
			pos = i + s2.size();
		}
	}

	// collecte des mots pour les mettre dans un vecteur
	void separatesWords(std::string & s) {
		for(unsigned int i=0 ; i<s.size() ; i++)
			s[i] = ::tolower(s[i]);
		replace(_text, "' ", " ' ", '\'');
		replace(_text, "'_text ", " '_text ");
		replace(_text, "'m ", " 'm ");
		replace(_text, "'d ", " 'd ");
		replace(_text, "'ll ", " 'll ");
		replace(_text, "'re ", " 're ");
		replace(_text, "'ve ", " 've ");
		replace(_text, "n't ", " n't ");

		replace(_text, " cannot ", " can not ");
		replace(_text, " d'ye ", " d' ye ");
		replace(_text, " gimme ", " gim me ");
		replace(_text, " gonna ", " gon na ");
		replace(_text, " gotta ", " got ta ");
		replace(_text, " lemme ", " lem me ");
		replace(_text, " more'n ", " more 'n ");
		replace(_text, "'ti_text ", " 't i_text ");
		replace(_text, "'twa_text ", " 't wa_text ");
		replace(_text, " wanna ", " wanna ");

		replace(_text, ".", " . ", 0);
		replace(_text, "  ", " ", 0);

		replace(s, "in front of", "in_front_of");
		replace(s, "to the right of", "to_the_right_of");
		replace(s, "to the left of", "to_the_left_of");

		replace(s, "on the right of", "on_the_right_of");
		replace(s, "on the left of", "on_the_left_of");

		replace(s, "on the top left of", "on_the_top_left_of");
		replace(s, "on the top right of", "on_the_top_right_of");
		replace(s, "on the top of", "on_the_top_of");

		replace(s, "at the top left of", "at_the_top_left_of");
		replace(s, "at the top right of", "at_the_top_right_of");
		replace(s, "at the top of", "at_the_top_of");

		replace(s, "at the bottom left of", "at_the_bottom_left_of");
		replace(s, "at the bottom right of", "at_the_bottom_right_of");
		replace(s, "at the bottom of", "at_the_bottom_of");

		replace(s, "next to", "next_to");

		replace(s, "facing left", "facing_left");
		replace(s, "facing right", "facing_right");
		replace(s, "facing up", "facing_up");
		replace(s, "facing down", "facing_down");
		replace(s, "facing front", "facing_front");
		replace(s, "facing back", "facing_back");

		std::string tmpString = _text;

		int indice = tmpString.find(' ');
		while(indice!=std::string::npos)
		{
			if(tmpString[0]==' '){
				tmpString = tmpString.substr(1);
			}
			
			std::string word = tmpString.substr(0, indice);

			_words.push_back(word);
			tmpString = tmpString.substr(indice+1);
			
			indice = tmpString.find(' ');
			if(indice==std::string::npos){
				break;
			}
		}
    }

	void invertLastsActors() {
		int size = _ensActor.size();
		if(size>=2)
		{
			Actor * tmp = _ensActor[size-2];
			// l'avant dernier = dernier
			_ensActor[size-2] = _ensActor[size-1];
			// le dernier = avant dernier
			_ensActor[size-1] = tmp;
		}
	}

	void displayStack()
	{
		if(_ensActor.size()!=0)
		{
			std::cout << "\n\n PILE = ";
			for(unsigned int i=0 ; i<_ensActor.size() ; i++)
			{
				std::cout << *(_ensActor[i]);
				std::cout << " / ";
			}
			std::cout <<"\n";
		}
	}

	void displayRelations()
	{
		if(_ensLinks.size()!=0)
		{
			std::cout << "\n RELATIONS = \n";
			for(unsigned int i=0 ; i<_ensLinks.size() ; i++)
			{
				std::cout << *(_ensLinks[i]) ;
				std::cout <<"\n";
			}
		}
	}

	// acces au dernier acteur correspondant
	Actor * getLastCorrespondingActor(std::vector<Actor* > & actorList , Actor * a) {
		if(actorList.size()==0)
			return NULL;
		
		for(int i=actorList.size()-1 ; i>=0 ; i--)
		{
			if(*actorList[i]==*a)
				return actorList[i];
		}
		return NULL;
	}

	// mise a jour d'un acteur (remise au dessus de la pile...)
	void updateActor(std::vector<Actor *> & ensActor, Actor * actorToFind) {
		std::vector<Actor *> tmpString;
		unsigned int indDernier=0;
		for(unsigned int i=0 ; i<_ensActor.size() ; i++)
		{
			if( *_ensActor[i] == *actorToFind )
				indDernier=i;
		}

		// on recopie tous les autres acteurs
		for(unsigned int i=0 ; i<_ensActor.size() ; i++)
		{
			if(i!=indDernier)
				tmpString.push_back(_ensActor[i]);
		}

		tmpString.push_back(actorToFind);
		_ensActor = tmpString;
	}

	std::string getFinalNoun(std::vector<std::string> & nounVector)
	{
		std::string res = "";

		for(unsigned int i=0 ; i<nounVector.size() ; i++)
			res = res + "_" + nounVector[i];

		res = res.substr(1);

		return res;
	}

	void updateNoun()
	{
		if(_nouns.size()!=0)
		{
			std::string name = getFinalNoun(_nouns);
			_nouns.clear();
			Actor * a = new Actor(name);
			a->setSingular(_singular);

			if(_adjectives.size()!=0)
			{
				for(unsigned int i=0 ; i<_adjectives.size() ; i++)
					a->setAttribute(_adjectives[i]);

				_adjectives.clear();
			}

			// si le determinant est 'a' on le cree
			if(_lastDeterminant.compare("a")==0 || _lastDeterminant.compare("an")==0 || 
			_lastDeterminant.compare("another")==0 || _lastDeterminant.compare("")==0)
			{
				a->setIndice(indice);
				a->load();
				indice++;
				_ensActor.push_back(a);
			}
			else if(_lastDeterminant.compare("the")==0) // si le determinant est 'the'
			{
				Actor * foundActor = getLastCorrespondingActor(_ensActor,a);
				if(foundActor==NULL) // si l'acteur n'existe pas
				{
					a->setIndice(indice);
					a->load();
					indice++;
					_ensActor.push_back(a);
				}
				else // l'acteur existe, on remet son adresse par dessus de la pile
				{
					updateActor(_ensActor, foundActor);
				}
			}
			_lastDeterminant="";
		}
		
	}

	void updateRelationship()
	{
		// prendre les 2 derniers acteurs
		if(_ensActor.size()>=2)
		{
			Actor * a1 = _ensActor[_ensActor.size()-2];
			Actor * a2 = _ensActor[_ensActor.size()-1];

			Link * l;
			// si la relation est un IN (par ex. 2 feet ON)
			if(_lastRelation.compare("")!=0)
			{
				if(_numberNounStack.compare("")!=0)
				{
					l = new Link(_numberNounStack+"_"+_lastRelation,a1,a2);
					_numberNounStack = "";
				}
				else
					l = new Link(_lastRelation,a1,a2);
				_lastRelation = "";
			}
			else // la relation est un verbe (par ex. is facing)
			{
				l = new Link(_lastVerb,a1,a2);
				_lastVerb = "";
			}
			_ensLinks.push_back(l);
		}
	}
}

program : (sentence)+ ;//{displayRelations();};

sentence : clause dot ;//{displayStack();};

clause returns [int type]
	: noun_phrase
		(verb // verbe facultatif pour pouvoir dire "a ball"-> creation d'un ballon.
		( (number_noun)? // on doit tester d'abord si c'est pas "NN VBZ 2 feet on NN"
				(
					subordinating_conjunction noun_phrase // cas "a ball is (2 feet) on the blue ball"
							{
							   updateNoun();
							   updateRelationship();
							   invertLastsActors();// inversion
							   type = 1;
							}
					| (adjective)+ // cas "a ball is blue"
					      {
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
					      }
				)
		
		 |noun_phrase // cas "a ball is (facing) the blue ball"
		      {
					 updateNoun();
		          updateRelationship();
		          invertLastsActors();// inversion
		          type = 3;
		      }

		 |	comparative noun_phrase
				{
					updateNoun();
					updateRelationship();
					invertLastsActors();
					type = 9;
				}
		)
	   )?
	| existential verb (noun_phrase) // cas "there is a ball"
			{
				updateNoun();
				type = 4;
			}
		(
			(subordinating_conjunction (noun_phrase) // cas "there is a ball on the blue ball"
				{
				   updateNoun();
				   updateRelationship();
				   invertLastsActors();// inversion
				   type = 5;
				}
			)?
		)
	| personal_pronoun verb
			(
				(number_noun)? // on doit tester d'abord si c'est pas "NN VBZ 2 feet on NN"
					 subordinating_conjunction noun_phrase // "it is ON the ball"
						{
								updateNoun();
								updateRelationship();
								invertLastsActors(); // inversion
								type = 7;
						}
					| (adjective)+ // cas "it is blue"
						{
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
						}
					| comparative noun_phrase
						{
								updateNoun();
								updateRelationship();
								invertLastsActors();
								type = 9;
						}
					|noun_phrase // cas "a ball is (facing) the blue ball"
						{
							updateNoun();
							updateRelationship();
							invertLastsActors();// inversion
							type = 3;
						}
			)
	| subordinating_conjunction noun_phrase existential verb noun_phrase // cas "On the desk there is a ball"
	     {
	         updateNoun();
	         invertLastsActors(); // inversion temporaire pour calculer les relations induites par la phrase
	         updateRelationship();
	         invertLastsActors(); // remise en etat normal
	         type = 8;
	     }
exception // for alternate
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
#ifdef WIN32
		std::string msg = "Incorrect sentence. Please reformulate!";
	    std::cerr << msg << std::endl;
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif

		return -3;
	}
;

noun_phrase : (determiner)? (adjective)* (noun)+
;

verb
	:	VERB_3SINGULAR
			{ TagsParser::ind_word++; }
		
	|  VERB_3SINGULAR
			{ TagsParser::ind_word++; }
		VERB_GERUND
			{ _lastVerb = _words[TagsParser::ind_word];
				TagsParser::ind_word++; }

	|  VERB_3SINGULAR
			{ TagsParser::ind_word++; }
	   VERB_GERUND
			{ _lastVerb = _words[TagsParser::ind_word];
				TagsParser::ind_word++; }
		VERB_PAST_PARTICIPLE
			{ TagsParser::ind_word++; }

	|	VERB_NO3SINGULAR
		{ TagsParser::ind_word++; }
exception // for alternate
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
//		std::cout << "!!!EXCEPTION!!! :  verbe incorrect" << std::endl;
		std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
#ifdef WIN32
		std::string msg = "Incorrect verb. Please reformulate the sentence";
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif

	}
;

adjective
	:	ADJECTIVE
	  		{
				updateNoun();
				_adjectives.push_back(_words[TagsParser::ind_word]);
				TagsParser::ind_word++;
	  		}
	|	number_noun (ADJECTIVE
			{
				updateNoun();
				_adjectives.push_back(_words[TagsParser::ind_word-2]+"_"+_words[TagsParser::ind_word-1]+"_"+_words[TagsParser::ind_word]);
				TagsParser::ind_word++;
			}
	  	) // nombre + nom + adjectif (ex. 2 feet tall)
exception // for alternate
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
//		std::cout << "!!!EXCEPTION!!! :  adjectif incorrect" << std::endl;
		std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
#ifdef WIN32
		std::string msg = "Incorrect adjective. Please reformulate the sentence";
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif

	}
;

subordinating_conjunction
	:	SUBORDINATING_CONJUNCTION
			{
				_lastRelation = _words[TagsParser::ind_word];
				TagsParser::ind_word++;
			}
exception
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
//		std::cout << "!!!EXCEPTION!!! :  conjonction de coordination incorrecte" << std::endl;
		std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
#ifdef WIN32
		std::string msg = "Incorrect coordination conjunction. Please reformulate the sentence";
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif

	}
;

number_noun : cardinal_number c_noun
{
_numberNounStack = _words[TagsParser::ind_word-2] + "_" + _words[TagsParser::ind_word-1];
}
exception
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
//		std::cout << "!!!EXCEPTION!!! :  (nombre + nom) incorrecte" << std::endl;
		std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
#ifdef WIN32
		std::string msg = "Incorrect formulation (number + noun). Please reformulate the sentence.";
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif

		return;
	}
;

c_noun
	:	NOUN
			{ _singular=true;
				TagsParser::ind_word++; }
	|	NOUN_PLURAL
			{ _singular=false;
				TagsParser::ind_word++; }
;

noun
	:	NOUN
			{ _singular=true;
				_nouns.push_back(_words[TagsParser::ind_word]);
				TagsParser::ind_word++; }
	|	NOUN_PLURAL
			{ _singular=false;
				_nouns.push_back(_words[TagsParser::ind_word]);
				TagsParser::ind_word++; }
exception // for alternate
	catch [ANTLR_USE_NAMESPACE(antlr)RecognitionException & ex] {
// 		std::cout << "!!!EXCEPTION!!! :  Nom incorrect" << std::endl;
		std::cout << "Impossible to parse your "<<TagsParser::ind_sentence << "th sentence. Please reformulate it." << std::endl;
#ifdef WIN32
		std::string msg = "Incorrect noun. Please reformulate the sentence";
	    MessageBox(0, msg.c_str(), "TAG PARSER", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
		return;
	}
;

comparative
	:	COMPARATIVE
			{ _lastRelation = _words[TagsParser::ind_word];
				TagsParser::ind_word++; }
		SUBORDINATING_CONJUNCTION
			{ TagsParser::ind_word++; }
;

// superlative : SUPERLATIVE ;

determiner
	:	DETERMINER
			{ updateNoun();
				_lastDeterminant = _words[TagsParser::ind_word];
				TagsParser::ind_word++; }
;

existential
	:	EXISTENTIAL
			{ updateNoun();
	  			TagsParser::ind_word++; }
;

personal_pronoun
	:	PERSONAL_PRONOUN
			{ updateNoun();
	  			TagsParser::ind_word++; }
;

cardinal_number
	:	CARDINAL_NUMBER
			{ updateNoun();
				TagsParser::ind_word++; }
;

dot
	:	DOT
		{ updateNoun();
			TagsParser::ind_word++; TagsParser::ind_sentence++; }
;











































//-------------------------------------------------LEXER----------------------------------------------------------------------------
class TagsLexer extends Lexer;

options {
   k=8;
   defaultErrorHandler=false;
	exportVocab = Tags;
}

tokens
{
	COORDINATING_CONJUNCTION = "CC";
	CARDINAL_NUMBER = "CD" ;
	DETERMINER = "DT" ;
	EXISTENTIAL = "EX" ;
	SUBORDINATING_CONJUNCTION = "IN" ;
	ADJECTIVE = "JJ" ;
	COMPARATIVE = "JJR" ;
	SUPERLATIVE = "JJS" ;
	NOUN = "NN" ;
	NOUN_PLURAL = "NNS" ;
	PROPER_NOUN_SINGULAR = "NNP" ;
	PROPER_NOUN_PLURAL = "NNPS" ;
	POSSESSIVE_ENDING = "POS" ;
	PERSONAL_PRONOUN = "PRP" ;
	ADVERB = "RB" ;
	ADVERB_COMPARATIVE = "RBR" ;
	ADVERB_SUPERLATIVE = "RBS" ;
	PARTICLE = "RP" ;
	TO = "TO" ;
	VERB = "VB" ;
	VERB_PAST = "VBD" ;
	VERB_GERUND = "VBG" ;
	VERB_PAST_PARTICIPLE = "VBN" ;
	VERB_NO3SINGULAR = "VBP" ;
	VERB_3SINGULAR = "VBZ" ;
	WH_DETERMINER = "WDT" ;
	WH_PRONOUN = "WP" ;
	POSSESSIVE_PRONOUN = "WP$" ;
	WH_ADVERB = "WRB" ;
}

ALPHA   : ('a'..'z'|'A'..'Z')+ ;

COORDINATING_CONJUNCTION : "CC"
	{	std::cout << "Ne prend pas encore en compte les 'ET' POUR L'INSTANT..." << std::endl;
		exit(-1);
	}
;

ADVERB : "RB"
	{	std::cout << "Ne prend pas encore en compte les 'RB' POUR L'INSTANT..." << std::endl;
		exit(-1);
	}
;

PARTICLE : "RP"
	{	std::cout << "Ne prend pas encore en compte les 'RP' POUR L'INSTANT..." << std::endl;
		exit(-1);
	}
;

TO : "TO"
	{	std::cout << "Ne prend pas encore en compte les 'TO' POUR L'INSTANT..." << std::endl;
		exit(-1);
	}
;

	CARDINAL_NUMBER : "CD" ;
	DETERMINER : "DT" ;
	EXISTENTIAL : "EX" ;
	SUBORDINATING_CONJUNCTION : "IN" ;
	ADJECTIVE : "JJ" ;
	COMPARATIVE : "JJR" ;
	SUPERLATIVE : "JJS" ;
	NOUN : "NN" ;
	NOUN_PLURAL : "NNS" ;
	PROPER_NOUN_SINGULAR : "NNP" ;
	PROPER_NOUN_PLURAL : "NNPS" ;
	POSSESSIVE_ENDING : "POS" ;
	PERSONAL_PRONOUN : "PRP" ;
	ADVERB_COMPARATIVE : "RBR" ;
	ADVERB_SUPERLATIVE : "RBS" ;
	VERB : "VB" ;
	VERB_PAST : "VBD" ;
	VERB_GERUND : "VBG" ;
	VERB_PAST_PARTICIPLE : "VBN" ;
	VERB_NO3SINGULAR : "VBP" ;
	VERB_3SINGULAR : "VBZ" ;
	WH_DETERMINER : "WDT" ;
	WH_PRONOUN : "WP" ;
	POSSESSIVE_PRONOUN : "WP$" ;
	WH_ADVERB : "WRB" ;

DOT : '.' ;

WS     :
    (' '
    | '\t'
    | '\r' { newline(); } 
    | '\n'      { newline(); }
    )+
    { _ttype = antlr::Token::SKIP; }
  ;
