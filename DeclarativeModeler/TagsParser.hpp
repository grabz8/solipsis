#ifndef INC_TagsParser_hpp_
#define INC_TagsParser_hpp_

#line 2 "tags.g"

#include <iostream>
#include <antlr/RecognitionException.hpp>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Actor.h"
#include "Link.h"

#line 15 "TagsParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.6 (20061021): "tags.g" -> "TagsParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "TagsTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

class CUSTOM_API TagsParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public TagsTokenTypes
{
// #line 46 "tags.g"

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
// #line 26 "TagsParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	TagsParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	TagsParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return TagsParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return TagsParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return TagsParser::tokenNames;
	}
	public: void program();
	public: void sentence();
	public: int  clause();
	public: void dot();
	public: void noun_phrase();
	public: void verb();
	public: void number_noun();
	public: void subordinating_conjunction();
	public: void adjective();
	public: void comparative();
	public: void existential();
	public: void personal_pronoun();
	public: void determiner();
	public: void noun();
	public: void cardinal_number();
	public: void c_noun();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 36;
#else
	enum {
		NUM_TOKENS = 36
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
};

#endif /*INC_TagsParser_hpp_*/
