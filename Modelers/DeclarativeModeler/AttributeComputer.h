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

#ifndef ATTRIBUTECOMPUTER_H_
#define ATTRIBUTECOMPUTER_H_

#include <vector>
#include "Solver.h"
#include "ConstraintWriter.h"


/**
 * \file AttributeComputer.h
 * \class AttributeComputer
 * \brief Calcule l'ensemble des attributs de tous les acteurs de la scene
 * \date : October 2008
 */
class AttributeComputer
{
public:

  /**
   * \brief Constructeur : Initialise un calculateur d'attribut
   * \param actors : L'ensemble des acteurs de la scène
   * \param links : L'ensemble des relations entre les acteurs
   */
  AttributeComputer(std::vector<Actor*> & actors, std::vector<Link*> & links);

  /**
   * \brief Destructeur
   */
  virtual ~AttributeComputer();

  /**
   * \brief Calcule et affecte les coordonnées / couleurs pour chaque acteur en fonction des contraintes
   * \return 0 si l'opération s'est bien normalement, 
   * -3 si la résolution des contraintes sur les tailles s'est soldée par un echec
   * -2 si la résolution des contraintes sur les positions s'est soldée par un echec
   * -1 si la résolution des contraintes sur les couleurs s'est soldée par un echec
   */
  int compute();

  /**
   * \brief Retourne Vrai s'il existe un lien entre 2 acteurs
   * \param a1 : le premier acteur
   * \param a2 : le second acteur
   * \return Vrai s'il existe un lien entre 2 acteurs, faux sinon
   */
  bool existeLink(const Actor * a1, const Actor * a2);

  /**
   * \brief Tri des contraintes : contraintes géométriques / contraintes de couleur
   * \param 
   */
  void sortConstraints(std::vector<Link*> & colorLinks, std::vector<Link*> & positionLinks, std::vector<Link*> & sizeLinks);

private:
  /** \brief L'ensemble des acteurs */
  std::vector<Actor*> _actors;

  /** \brief L'ensemble des liens entre les acteurs */
  std::vector<Link*> _links;
};

#endif /*ATTRIBUTECOMPUTER_H_*/
