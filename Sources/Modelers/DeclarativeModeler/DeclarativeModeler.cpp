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

	std::cout << "Constructor." << std::endl;
	std::vector< std::string > properties;
	properties.push_back( "Sphere.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["sphere"] = properties; 
	m_availableActorModels["ball"] = properties; 
	properties.clear();
	properties.push_back( "Box.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["box"] = properties; 
	m_availableActorModels["cube"] = properties; 
	properties.clear();
	properties.push_back( "Prism.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["prism"] = properties; 
	properties.clear();
	properties.push_back( "Cylinder.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["cylinder"] = properties; 
	properties.clear();
	properties.push_back( "Plane.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["plane"] = properties; 
	properties.clear();
	properties.push_back( "Ring.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["ring"] = properties; 
	m_availableActorModels["torus"] = properties;
	properties.clear();
	properties.push_back( "Tube.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["tube"] = properties; 

	init( "./models/", m_vme );

	std::cout << "Constructor done." << std::endl;

}

DeclarativeModeler::DeclarativeModeler( const std::string & s )
: m_s( s ), m_vme( std::vector< ME_Model >( DM_ME_SIZE ) )
{

	std::vector< std::string > properties;
	properties.push_back( "Sphere.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["sphere"] = properties; 
	m_availableActorModels["ball"] = properties; 
	properties.clear();
	properties.push_back( "Box.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["box"] = properties; 
	m_availableActorModels["cube"] = properties; 
	properties.clear();
	properties.push_back( "Prism.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["prism"] = properties; 
	properties.clear();
	properties.push_back( "Cylinder.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["cylinder"] = properties; 
	properties.clear();
	properties.push_back( "Plane.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["plane"] = properties; 
	properties.clear();
	properties.push_back( "Ring.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["ring"] = properties; 
	m_availableActorModels["torus"] = properties;
	properties.clear();
	properties.push_back( "Tube.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["tube"] = properties; 
	properties.clear();
	properties.push_back( "knot.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["knot"] = properties; 
	properties.clear();
	properties.push_back( "ninja.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["ninja"] = properties; 
	properties.clear();
	properties.push_back( "tudorhouse.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["house"] = properties; 
	properties.clear();
	properties.push_back( "column.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["column"] = properties; 

	init( "./models/", m_vme );

}

DeclarativeModeler::~DeclarativeModeler(void)
{ }

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


int DeclarativeModeler::run( const std::string& s /*, std::string& errorMsg, std::string& warningMsg */ )
{
	m_s = s;

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
        exit(-1);
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
    std::vector<Actor*> actors = parser.getActors();

    // parcours des contraintes pour poser les équations
    std::vector<Link*> relationsInterActor = parser.getRelations();

    // on affecte les positions / couleurs aux acteurs
    AttributeComputer attComputer(actors, relationsInterActor);

    // calcul et résolution de contraintes
    int res = attComputer.compute();

	strm.close();

	std::string msg;
//#ifdef WIN32
//	if(res==-3) {
//		warningMsg = "Le modeleur n'a pu déterminer une taille correcte pour les acteurs de la scène.";
//	    //MessageBox(0, msg.c_str(), "Modeleur déclaratif", MB_OK | MB_ICONERROR | MB_TASKMODAL);
//	}
//	if(res==-2) {
//		warningMsg = "Le modeleur n'a pu déterminer une position correcte pour les acteurs de la scène.";
//	    //MessageBox(0, msg.c_str(), "Modeleur déclaratif", MB_OK | MB_ICONERROR | MB_TASKMODAL);
//	}
//	else if(res==-1) {
//		warningMsg = "Le modeleur n'a pu déterminer une couleur correcte pour les acteurs de la scène.\n Une couleur par défaut a leur a été assignée.";
//	    //MessageBox(0, msg.c_str(), "Modeleur déclaratif", MB_OK | MB_ICONERROR | MB_TASKMODAL);
//	}
//#endif

	m_actors = actors;

	assignModelsToActors( /* errorMsg */ );

	////return 0;
  }
  catch( ANTLRException& e ) {
//    std::cerr << "ANTLRException : " << e.getMessage() << std::endl;
//#ifdef WIN32
//		errorMsg = e.getMessage() + "\nThere are some unknown/ununderstandable elements in the sentence. Please reformulate.";
//	    //MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
//#endif
//    return -3;
//  }
//  catch( std::exception& e )
//  {
//    std::cerr << "exception : " << e.what() << std::endl;
//#ifdef WIN32
//	errorMsg = std::string( e.what() ) + "\nThere are some unknown/ununderstandable elements in the sentence. Please reformulate.";
//	    //MessageBox(0, msg.c_str(), "Declarative modeling", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
//#endif
//    return -4;
  }
  return 0;
}

bool DeclarativeModeler::assignModelToActor( Actor* actor /*, std::string& returnedMsg*/ )
{
	std::string actorName = actor->getName();
	if( !m_availableActorModels[actorName].empty() ) {
		actor->setModelName( m_availableActorModels[actorName][0] );
		//returnedMsg = "";
	}
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
