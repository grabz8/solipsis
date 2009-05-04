/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Sebastien LAIGRE

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

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

#include <Ogre.h>
using namespace::Ogre;

#include "TagsLexer.hpp"
#include "TagsParser.hpp"
#include "antlr/TokenBuffer.hpp"
#include "AttributeComputer.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "DeclarativeModeler.h"
#include <tag.h>

#define DM_ME_SIZE 16

DeclarativeModeler::DeclarativeModeler()
: m_s( "" ), m_vme( std::vector< ME_Model >( DM_ME_SIZE ) )
{

    String secName, typeName, archName;
    ConfigFile cf;
    cf.load("declarativemodeling.cfg");

	ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while( seci.hasMoreElements() ) {
	    secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
		    typeName = i->first;
            archName = i->second;
			m_confPaths[secName].push_back( archName );
		}
	}

	if( !m_confPaths["LanguageModels"].empty() )
		init( m_confPaths["LanguageModels"].front(), m_vme ); // from postagger library

	if( !m_confPaths["Synonyms"].empty() )
		initSynonyms( m_confPaths["Synonyms"].front() );

}


DeclarativeModeler::~DeclarativeModeler()
{}

void extractTags(std::string & input, std::string & outFileName)
{
  // declaration d'une chaine qui contiendra la ligne lue
  std::string texte = input;  

  std::string tmpString = texte;

  // recuperation des tags SEULS
  std::string tags;

  int indice = tmpString.find('/');
  while(indice!=-1)
    {
      int ind_space = tmpString.find(' ');

      tags = tags + tmpString.substr(indice+1, ind_space-indice);
      tmpString = tmpString.substr(ind_space+1);
      indice = tmpString.find('/');
      ind_space = tmpString.find(' ');
      if(ind_space==-1)
        {
          tags = tags + '.';
          break;
        }
    }

  FILE* outFile = NULL;
  outFile = fopen(outFileName.c_str(), "w");
  if (outFile != NULL)
    {
      // On l'écrit dans le fichier
      fprintf(outFile,tags.c_str());
      fclose(outFile);
    }
}


int DeclarativeModeler::run( const std::string& s, std::string& errorMsg, std::string& warningMsg )
{
	if( s.empty() )
		return 1;

	m_s = s; // add a point at the end of the sentence if there is not.
	if( m_s.find( "." ) != m_s.size()-1 ) 
		m_s += ".";

	int (*pf)(int)=tolower; // change the sentence to lower case to avoid misunderstanding of bad placed caps.
	std::transform( m_s.begin(), m_s.end(), m_s.begin(), pf );

	// on tag le texte
	std::string output;
	tag( m_s, m_vme, output );

	// on extrait les tags du texte taggé // TODO changer le nom pour le passage sous windows
	// on écrit dans un fichier temporairement car ANTLR doit charger un fichier...
	std::string fichierSortie = "tmpFile";
	extractTags(output, fichierSortie);

  ANTLR_USING_NAMESPACE(antlr)
  try {
    std::srand(std::time(NULL));
    std::ifstream strm( "tmpFile" );

    if (!strm.is_open())
      {
        std::cout << "impossible de charger le fichier contenant les Tags ("<<"file"<<")"<< std::endl; 
        return -1;
      }

    // le lexer
    TagsLexer lexer(strm);
    TokenBuffer buffer(lexer);

    // le parser
    TagsParser parser(buffer);
	parser.init();

    // chargement du fichier contenant le texte initial et découpage pour accéder à chaque mot
    parser.setWords( m_s);
    parser.separatesWords(parser.getSentence());

    // parcours du texte pour récupérer les acteurs et les contraintes
    parser.program();

    // les acteurs de la scene
	std::vector< Actor* > actors = parser.getActors();

    // parcours des contraintes pour poser les équations
    std::vector<Link*> relationsInterActor = parser.getRelations();

    // on affecte les positions / couleurs aux acteurs
    AttributeComputer attComputer( actors, relationsInterActor);

    // calcul et résolution de contraintes
    int res = attComputer.compute();

	strm.close();

	std::string msg;
	if(res==-3) {
		warningMsg = "A correct size could not be computed for the actors of the scene.";
	}
	if(res==-2) {
		warningMsg = "A correct position could not be computed for the actors of the scene.";
	}
	else if(res==-1) {
		warningMsg = "A correct color could not be computed for actors of the scene. A default color has been assigned.";
	}

	m_actors = actors;
	assignModelsToActors( /* errorMsg */ );

  }
  catch( ANTLRException& e ) {
		errorMsg = e.getMessage() + "<br/>There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    return -3;
  }
  catch( std::exception& e ) {
		errorMsg = std::string( e.what() ) + "<br/>There are some unknown/ununderstandable elements in the sentence. Please reformulate.";
	    return -4;
  }
  return 0;
}

bool DeclarativeModeler::assignModelToActor( Actor* actor /*, std::string& returnedMsg*/ )
{
	std::string actorName = actor->getName();
	
	// if group not initialised -> do it!
	// The 'resources.cfg' file has to be available and must contain a [General] section
	if( !ResourceGroupManager::getSingleton().isResourceGroupInitialised( "General" ) )
		ResourceGroupManager::getSingleton().initialiseResourceGroup( "General" );
	// if group not loaded -> do it!
	if( !ResourceGroupManager::getSingleton().isResourceGroupLoaded( "General" ) )
		ResourceGroupManager::getSingleton().loadResourceGroup( "General" );

	// search through resources for real name
	StringVectorPtr matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", actorName + ".mesh" );
	// if empty set -> search through resources for matching pattern
	if( matchingActorNames->empty() )
		matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", actorName + "*.mesh" );
	if( matchingActorNames->empty() )
		matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", "*" + actorName + ".mesh" );
	if( matchingActorNames->empty() )
		matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", "*" + actorName + "*.mesh" );

	if( matchingActorNames->empty() ) {
		// search for synonyms through resources
		int i = 0;
		bool synonymFound = false;
		while( i < m_synonyms.size() && !synonymFound ) {
			if( std::find( m_synonyms[i].begin(), m_synonyms[i].end(), actorName ) != m_synonyms[i].end() ) {
				int j = 0;
				while( j < m_synonyms[i].size() && !synonymFound ) {
					matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", m_synonyms[i][j] + ".mesh" );
					if( matchingActorNames->empty() )
						matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", m_synonyms[i][j] + "*.mesh" );
					if( matchingActorNames->empty() )
						matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", "*" + m_synonyms[i][j] + ".mesh" );
					if( matchingActorNames->empty() )
						matchingActorNames = ResourceGroupManager::getSingleton().findResourceNames( "General", "*" + m_synonyms[i][j] + "*.mesh" );
					if( !matchingActorNames->empty() )
						synonymFound = true;
					j++;
				}
			}
			i++;
		}
	}

	if( !matchingActorNames->empty() )
		actor->setModelName( matchingActorNames->front() );

	//if( !m_availableActorModels[actorName].empty() ) {
	//	actor->setModelName( m_availableActorModels[actorName][0] );
	//	//returnedMsg = "";
	//}
	else {
		//returnedMsg = "Le modeleur n'a pu trouver un modèle approprié pour l'acteur '" + actor->getName() + "'.";
		return false;
	    //MessageBox(0, returnedMsg.c_str(), "Modeleur déclaratif", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
	return true;
}

bool DeclarativeModeler::assignModelsToActors( /* std::string& returnedMsg */ )
{

	std::vector< Actor* >::iterator it;
	bool returnedValue = true;
	std::string currentReturnedMsg = "";
	for( it = m_actors.begin(); it != m_actors.end(); it++ ) {
		returnedValue |= assignModelToActor( *it ); // , currentReturnedMsg );
		//if( currentReturnedMsg != "" )
		//	returnedMsg = currentReturnedMsg;
	}
	return returnedValue;
}

//std::vector<std::string> split(const std::string&, const std::string&);
//std::vector<std::string> split(const char*, const char*);
//std::vector<std::string> split(const std::string&, const char*);
//std::vector<std::string> split(const char*, std::string&);
 
std::vector<std::string> split(const std::string& myStr, const std::string& token){
	std::vector<std::string> temp (0);
	std::string s;
	for(std::size_t i = 0; i < myStr.size(); i++){
		if( (myStr.substr(i, token.size()).compare(token) == 0)){
			temp.push_back(s);
			s.clear();
			i += token.size() - 1;
		}else{
			s.append(1, myStr[i]);
			if(i == (myStr.size() - 1))
				temp.push_back(s);
		}
	}
	return temp;
}

std::vector<std::string> split (const char* lhs, const char* rhs){
	const std::string m1 (lhs), m2 (rhs);
	return split(m1, m2);
}

std::vector<std::string> split (const char* lhs, const std::string& rhs){
	return split(lhs, rhs.c_str());
}

std::vector<std::string> split (const std::string& lhs, const char* rhs){
	return split(lhs.c_str(), rhs);
}

template<class Element>
std::ostream& displayElements(const std::vector<Element>& arg, std::ostream& output = std::cout){
	for(std::size_t i = 0; i < arg.size(); i++)
		output << arg[i] << "\n";
	return output;
}

void DeclarativeModeler::initSynonyms( const std::string& filepath ) {

	string line;
	ifstream myfile( std::string( filepath + "/synonyms.txt" ).c_str() );
	if( myfile.is_open() ) {
	    while (! myfile.eof() ) {
			getline( myfile, line );
			std::vector< std::string > synonyms = split( line, "," );
			m_synonyms.push_back( synonyms );
	    }
	    myfile.close();
    }
	else cout << "Unable to open file"; 
}