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

#ifndef LINK_H_
#define LINK_H_

#include <string>
#include "Actor.h"

/**
 * \file Link.h
 * \class Link
 * \brief Classe représentant un lien entre 2 acteurs
 * \date : October 2008
 */
class Link
{
public:

	/**
	* \brief Constructeur d'un lien
	* \param operation : le lien entre les 2 acteurs
	* \param a1 : le premier acteur
	* \param a2 : le second acteur
	*/
	Link(const std::string & operation="", Actor * a1=NULL, Actor * a2=NULL);

	/**
	* \brief Destructeur
	*/
	~Link();

	/**
	* \brief Acces au lien entre les deux acteurs
	* \return le lien entre les deux acteurs
	*/
	std::string & getLink() { return _relation; }

	/**
	* \brief Acces au premier acteur
	* \return le premier acteur
	*/
	Actor * getFirst() { return _a1; }

	/**
	* \brief Acces au second acteur
	* \return le second acteur
	*/
	Actor * getSecond() { return _a2; }

	/**
	* \brief Affecte la relation
	* \param relation : la relation
	*/	
	void setLink(std::string & relation) { _relation = relation; }

	/**
	* \brief Affecte le 1er acteur
	* \param a : le premier acteur
	*/
	void setFirst(Actor * a) { _a1 = a; }

	/**
	* \brief Affecte le 2eme acteur
	* \param a : le second acteur
	*/
	void setSecond(Actor * a) { _a2 = a; }

	void setA2Constraint(bool isConstraint)
	{
		_actor2Constraint = isConstraint;
	}
	
	bool isA2Constraint()
	{
		return _actor2Constraint;
	}
	
	/**
	* \brief Affichage d'un lien
	*/
	friend std::ostream& operator<<(std::ostream& os, Link & l);


	// -------------------------  ATTRIBUTS  -----------------------------------------------------

private :
	/** \brief Premier Acteur */
	Actor * _a1;

	/** \brief Premier Acteur */
	Actor * _a2;

	/** \brief Relation entre les 2 acteurs */
	std::string _relation;
	
	bool _actor2Constraint;
};

#endif /*LINK_H_*/
