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

#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "Vec3.h"
#include "Vec4.h"
#include "Matrix.h"

#define PI 3.141592653589793

typedef Matrix<double> Matrixd;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

/**
 * \file Actor.h
 * \class Actor
 * \brief Classe représentant un acteur
 * \date : October 2008
 * \author Sébastien Laigre
 */
class Actor
{
public:

  // Constructeur : Initialise un acteur
  Actor(const std::string & n="");

  // Destructeur
  ~Actor();

  // Accés au nom de l'acteur
  std::string & getName() { return _name; }

  // Accés au type de l'acteur
  bool isSingular() { return _singular; }

  // Fixe le type de l'acteur
  void setSingular(bool isSingular) { _singular=false; }

  // Ajoute une propriété de l'acteur
  void setAttribute(const std::string & s);

  // Accés aux attributs de l'acteur
  std::vector<std::string> & getAttributes() { return _attributes; }

  // acces au nom de l'acteur
  std::string name();

  // Retourne la position de l'acteur
  Vec3i & getPosition() { return _position; }

  // Retourne la taille en x, taille en y, taille en z
  Vec3i & getSize() { return _size; }

  // acces au scale de l'acteur
  Vec3i & getScale() { return _scale; }

  // acces a la rotation de l'acteur
  Vec3d & getRotation() { return _rotation; }

  // acces a la matrice de rotation
  Matrixd & getRotationMatrix() { return _rotationMatrix; }

  // acces a la couleur de l'acteur
  Vec4 & getColor() { return _color; }



  // fixe la position de l'acteur
  void setPosition(int XYouZ, int value);

  // fixe la taille de l'acteur
  void setSize(Vec3i & s) { _size = s; }

  // fixe le scale de l'acteur
  void setScale(Vec3i & v) { _scale = v; }

  // fixe a la rotation de l'acteur
  void setRotation(Vec3d & r) { _rotation = r; }

  // fixe a la rotation de l'acteur
  void setRotation(Matrixd & r) { _rotationMatrix = r; }

  // initialise la couleur
  void setColor(const int r, const int g, const int b, const int a) { _color.setRGBA(r,g,b,a); }




  // initialise l'indice permettant d'identifier chaque acteur
  void setIndice(int i) { _indice = i; }


  // charge un objet dans la bibliotheque
  int load();


  // indique si la taille de l'acteur est contrainte ou non
  bool isSizeConstraint() { return _sizeConstraint; }

  // affecte _sizeConstraint
  void setSizeConstraint(bool sc) { _sizeConstraint = sc; }

  // indique si la couleur de l'acteur est contrainte ou non
  bool isColorConstraint() { return _colorConstraint; }

  // indique si la couleur de l'acteur est contrainte ou non
  void setColorConstraint(bool cc) { _colorConstraint = cc; }

  // indique si la position de l'acteur est contrainte
  bool isPositionConstraint() { return _positionConstraint; }

  // affecte _positionConstraint
  void setPositionConstraint(bool pc) { _positionConstraint = pc; }

  // indique si la position de l'acteur est contrainte
  bool isRotationConstraint() { return _positionConstraint; }

  // affecte _rotationConstraint
  void setRotationConstraint(bool rc) { _rotationConstraint = rc; }







  // affecte le scale de l'acteur
  void setScale(int XYouZ, int value);

  // affiche la couleur
  void setColor(int RGBouA, int value);




  //	// affecte l'ensemble des propriétés à l'acteur
  //	void affectAttribute();

  // affecte un attribut particulier (du genre '2_feet_tall' )
  // i correspond à l'indice de l'attribut en question...
  void treatParticularAttribute(const std::string & s);

  // converti un entier en string
  std::string toString(int i)
  {
    std::stringstream ss;
    ss<<i;
    return ss.str().c_str();
  }

  // converti un string en un objet de type T
  template<typename T>
  bool from_string( const std::string & Str, T & Dest ) const
  {
    std::istringstream iss( Str );
    return iss >> Dest != 0;
  }

  // Affichage des attributs de l'acteur 
  void displayAttributes()
  {
    for(unsigned int i=0 ; i<_attributes.size() ; i++)
      std::cout << _attributes[i]<< ", ";
  }

  // retourne la taille finale (_size*_scale / 100) 
  Vec3d getFinalSize() ;

  // Position de l'acteur
  Vec3i getFinalPosition();

  /**
   * \brief Teste l'existence de l'attribut 's' dans le vecteur d'attributs
   * \param s : l'attribut
   * \return vrai si 's' existe, faux sinon
   */
  bool containsAttribute(const std::string & s);

  void setModelName( const std::string& modelName ) { m_modelName = modelName; };

  std::string getModelName() { return m_modelName; };


  // ----------------------------------------     operateurs     ------------------------------------------------------
  /**
   * \brief Egalité de deux acteurs
   * \param a : le second acteur
   * \return vrai si les deux acteurs sont égaux, faux sinon
   */
  bool operator == (const Actor & a);

  /**
   * \brief Egalité de deux acteurs
   */
  bool operator != (const Actor & a);

  /**
   * \brief Affectation
   */
  void operator = (const Actor & a);

  /**
   * \brief Affichage d'un acteur
   * \param os : Le flux de sortie
   * \param a : l'acteur
   * \return le flux de sortie
   */
  friend std::ostream& operator<<(std::ostream& os, Actor & a);

  Vec3d DegreeToRadian(Vec3i & v)
  {
    return Vec3d(v.x()*PI/180 , v.y()*PI/180 , v.z()*PI/180);
  }

private :
  // -------------------------  ATTRIBUTS  -----------------------------------------------------

  /** \brief Le nom de l'acteur */
  std::string _name;

  /** \brief Le type de l'acteur */
  bool _singular;

  /** \brief Les attributs de l'acteur */
  std::vector<std::string> _attributes;

  /** \brief Sa position */
  Vec3i _position;

  /** \brief Son orientation */
  Vec3d _rotation;
  Vec3i _rotationDegree;

  /** \brief Sa taille */
  Vec3i _size;

  /** \brief Un scale */
  Vec3i _scale;

  /** \brief Sa couleur */
  Vec4 _color;

  /** \brief Indique si sa position est contrainte ou non */
  bool _positionConstraint;

  /** \brief Indique si sa taille est contrainte ou non */
  bool _sizeConstraint;

  /** \brief Indique si sa couleur est contrainte ou non */
  bool _colorConstraint;

  /** \brief Indique si sorientation est contrainte ou non */
  bool _rotationConstraint;

  /** \brief Un indice permettant l'unicité de l'acteur */
  int _indice;

  /** \brief La matrice de rotation */
  Matrixd _rotationMatrix;

  std::string m_modelName;
};

#endif /*ACTOR_H_*/
