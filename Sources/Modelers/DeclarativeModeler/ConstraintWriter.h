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

#ifndef CONSTRAINTWRITER_H_
#define CONSTRAINTWRITER_H_

#include <sstream>
#include <string>
#include "Actor.h"
#include "Link.h"
#include <vector>
#include "Constraints.h"

/**
 * \file ConstraintWriter.h
 * \class ConstraintWriter
 * \brief Classe permettant d'écrire les contraintes "temporaires" avant de les passer au solveur
 * Avec ces contraintes, on sait ce qu'elles induisent sur les coordonnées (ex : obj1 sur obj2 => obj1.y > obj2.y)
 * Cette classe ne sert qu'a transmettre les noms de variables au solveur et la relation entre les deux acteurs
 * L'analyse de chaque contrainte est faite dans Solver.cpp (avec un dynamic_cast...)
 * \date : October 2008
 */
class ConstraintWriter
{
public:
  /**
   * \brief Prepare la résolution des contraintes
   * \param ensActor : l'ensemble des acteurs
   * \param ensRelation : l'ensemble des relations entre les acteurs
   * \param geometrical : Indique si les contraintes sont des contraintes de taille, de position ou de couleur
   */
  ConstraintWriter(std::vector<Actor *>& actors, std::vector<Link *>& ensRelation, int variables = 0);

  /**
   * \brief Destructeur
   */
  ~ConstraintWriter();

  /**
   * \brief Decoupage PAR acteur
   */
  void dividePerActor();

  /**
   * \brief indique si la variable a déja été empilée ou non
   */
  bool contains(std::string & name);

  /**
   * \brief Indique quel acteurs sont contraints, en taille et en position, et l'affecte a chaque acteur...
   */
  void setConstraintedActors();

  /**
   * \brief Mise en place des contraintes, à passer au solveur
   */
  void establishPositionConstraint();

  /**
   * \brief Mise en place des contraintes, à passer au solveur
   */
  void establishSizeConstraint();

  /**
   * \brief Mise en place des contraintes POUR LES COULEURS, à passer au solveur
   */
  void establishColorConstraint();

  // accesseurs
  std::vector<Constraint *> & getConstraintVector() { return _constraintVector; }
  std::vector<std::string> & getVariableNames() { return _names; }

  // -------------------------  ATTRIBUTS  -----------------------------------------------------
private :
  // les relations entre eux
  std::vector<Link *> _ensRelation;

  // les contraintes interprétées par le solveur
  std::vector<Constraint *> _constraintVector;

  // l'ensemble des noms de variables
  std::vector<std::string> _names;

  std::vector<Actor*> _ensActor;
};

#endif /*CONSTRAINTWRITER_H_*/
