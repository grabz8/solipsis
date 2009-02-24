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

#ifndef _DECLARATIVE_MODELER_H_
#define _DECLARATIVE_MODELER_H_

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "Actor.h"

/**
 * \file DeclarativeModeler.h
 * \brief Le modeleur, il parse le texte, récupère les acteurs et calcule leur propriétés
 * \date Octobre 2008
 */
class DeclarativeModeler
{
public:

	/**
	 * \brief Constructeur : initialise un modeleur à partir d'une chaine de caractère
	 * \param s : La chaine de caractère
	 */
	DeclarativeModeler();

	/**
	 * \brief Constructeur : initialise un modeleur à partir d'une chaine de caractère
	 * \param s : La chaine de caractère
	 */
	DeclarativeModeler( const std::string & s );

	/**
	 * \brief Destructeur
	 */
	virtual ~DeclarativeModeler(void);
	
	/**
	 * \brief Lance la modélisation
	 * \return 0 si OK, une valeur négative si problème... cf fichier AttributeComputer pour les types de retour)
	 */
	int run( const std::string& s /*, std::string& errorMsg, std::string& warningMsg */ );

	bool assignModelToActor( Actor* /*, std::string& msg */ );

	bool assignModelsToActors( /* std::string& msg */ );

	/** Accesseurs */
	std::string & getString() { return m_s;}
	std::vector<Actor*> & getActors() { return m_actors; }

protected:

	/** \brief Le texte */
	std::string m_s;

	/** \brief L'ensemble des acteurs */
	std::vector<Actor*> m_actors;

	std::map< std::string, std::vector< std::string > > m_availableActorModels;

};

#endif /*_DECLARATIVE_MODELER_H_*/
