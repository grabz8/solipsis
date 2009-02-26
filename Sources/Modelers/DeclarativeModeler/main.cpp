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

#include <fstream>

#include "DeclarativeModeler.h"

int main(int argc , char** argv)
{
	// le texte a analyser
	if( argc == 1 || argc > 2 ) {
		std::cout << "DeclarativeModeler <file_to_be_analysed>" << std::endl;
		return 0;
	}

	std::string filename = argv[1];

	// ouverture du texte a analyser
	std::ifstream stream( filename.c_str(), std::ios::out );  // on ouvre en lecture
	if( !stream ) {
		std::cerr << "Impossible d'ouvrir le fichier " << filename << "." << std::endl;
		exit( -1 );
	}

	// on met le texte a analyser dans un string
	std::string input;
	getline( stream, input );
	stream.close();

	// creation du modeleur
	DeclarativeModeler modeler;

	std::string err = "";
	std::string warn = "";
	// on lance la modélisation...
	int resultat = modeler.run( input ); //, err, warn );

	// l'ensemble des acteurs à charger...
	std::vector<Actor *> actors = modeler.getActors();

	return resultat;

}
