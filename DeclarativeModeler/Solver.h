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

#ifndef SOLVER_H_
#define SOLVER_H_

#include <gecode/support.hh>
#include <gecode/minimodel.hh>
#include <gecode/int.hh>
#include <gecode/kernel.hh>
#include <gecode/search.hh>
#include "Constraints.h"
#include "Actor.h"
#include <map>
#include <vector>

/**
 * \file Solver.h
 * \class Solver
 * \brief Résoud les différentes contraintes imposées par les relation entre les acteurs
 * \brief Utilise la librairie Gecode
 * \date : October 2008
 */
class Solver : public Gecode::Space 
{

private:
  /** \brief Nombre d'inconnus */
  static int nl;

  /** \brief Le vecteur contenant les inconnus */
  Gecode::IntVarArray _le;

  /** \brief Mode de fonctionnement de gecode */
  int _BRANCHING_MODE;

  /** \brief La somme des distances _scale-100 (distance à minimiser dans le cas des contraintes de tailles */
  Gecode::IntVar _distance;

  /** \brief Indique si la contrante traitée est une contrainte de taille ou non */
  bool _sizeConstraint;

public:

  /**
   * \brief Constructeur : Initialise un solveur de contraintes
   * \param nbVariables : Le nombre d'inconnues
   * \param variablesNames : L'ensemble des variables
   * \param contraintes : Les contraintes
   * \param BRANCHING_MODE : Le mode de fonctionnement de Gecode
   * \param mode : Indique si la résolution est une résolution sur les tailles ou non
   */
  Solver(unsigned int nbVariables, std::vector<std::string> & variableNames, std::vector<Constraint*> & contraintes, int BRANCHING_MODE=0, int mode=1);

  /**
   * \brief Constructor for cloning \a s
   */
  Solver(bool share, Solver& s);

  /**
   * \brief Destructeur
   */
  virtual ~Solver()
  { }

  /**
   * \brief Copy during cloning
   */
  virtual Space* copy(bool share)
  {
    return new Solver(share,*this);
  }

  /**
   * \brief Affichage de la solution
   */
  virtual void print(std::ostream& os) 
  {
    os << "\t" <<_le << std::endl;
  }

  /**
   * \brief Ajoute la contrainte de minimisation de la somme 'sum(_scale-100), pour avoir un facteur scale le plus proche de 100 possible
   * Seulement appelée lors d'une résolution de contrainte sur les tailles
   */
  void constrain(Space* s);

  /**
   * \brief Résolution du système d'équations
   * \return -1 si aucune solution n'a été trouvée, 1 sinon.
   */
  int run();

  // les solutions
  int getValue(int i)
  {
    return _le[i].val();
  }

  /**
   * \brief Calcule le nombre d'éléments que contiendra le tableau temporaire 'values'. Ce tableau permet de calculer 
   * la somme 'sum(_scale-100), pour avoir un facteur scale le plus proche de 100 possible
   * Cette somme est minimisée dans le cas d'une résolution de contraintes sur les tailles.
   * (cf. constructeur)
   * \param contraintes : Les contraintes à satisfaire
   * \return le nombre d'éléments que contiendra le tableau temporaire 'values'
   */
  int nbConstraintToOptimize(std::vector<Constraint*> & contraintes);
};

#endif /*SOLVER_H_*/
