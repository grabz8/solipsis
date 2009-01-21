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

#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include "Vec3.h"
#include <iostream>

/**
 * \file Constraint.h
 * \class Constraint
 * \brief Une contrainte
 * \date : October 2008
 */
class Constraint
{
public:
  Constraint(){};

  virtual ~Constraint() {};
};

/**
 * \class CompPosition
 * \brief Une contrainte de positionnement d'un objet par rapport a un autre
 * \date : October 2008
 */
class CompPosition : public Constraint
{
public:
  CompPosition(Vec3<std::string> & p1, Vec3<std::string> & p2, Vec3<std::string> & t1, Vec3<std::string> & t2, std::string & rel)
  :_p1(p1), _p2(p2), _t1(t1), _t2(t2), _rel(rel)
  { }

  virtual ~CompPosition(){}

  const std::string & getRelation() { return _rel; }
  const std::string & getP1X() { return _p1.x(); }
  const std::string & getP1Y() { return _p1.y(); }
  const std::string & getP1Z() { return _p1.z(); }

  const std::string & getP2X() { return _p2.x(); }
  const std::string & getP2Y() { return _p2.y(); }
  const std::string & getP2Z() { return _p2.z(); }

  const std::string & getT1X() { return _t1.x(); }
  const std::string & getT1Y() { return _t1.y(); }
  const std::string & getT1Z() { return _t1.z(); }

  const std::string & getT2X() { return _t2.x(); }
  const std::string & getT2Y() { return _t2.y(); }
  const std::string & getT2Z() { return _t2.z(); }

private:
  // les 2 points et leurs tailles
  Vec3<std::string> _p1;
  Vec3<std::string> _p2;
  Vec3<std::string> _t1;
  Vec3<std::string> _t2;

  // la relation entre les deux variables
  std::string _rel;
};

/**
 * \class CompTailles
 * \brief Une contrainte sur les tailles
 * \date : October 2008
 */
class CompTailles : public Constraint
{
public:
  CompTailles(			
      Vec3<std::string> & t1, Vec3<std::string> & t2, Vec3<std::string> & s1, Vec3<std::string> & s2, std::string & rel)
  :_t1(t1), _t2(t2), _s1(s1), _s2(s2), _rel(rel)
  { }

      virtual ~CompTailles(){}

      const std::string & getRelation() { return _rel; }

      const std::string & getT1X() { return _t1.x(); }
      const std::string & getT1Y() { return _t1.y(); }
      const std::string & getT1Z() { return _t1.z(); }

      const std::string & getT2X() { return _t2.x(); }
      const std::string & getT2Y() { return _t2.y(); }
      const std::string & getT2Z() { return _t2.z(); }

      const std::string & getS1X() { return _s1.x(); }
      const std::string & getS1Y() { return _s1.y(); }
      const std::string & getS1Z() { return _s1.z(); }

      const std::string & getS2X() { return _s2.x(); }
      const std::string & getS2Y() { return _s2.y(); }
      const std::string & getS2Z() { return _s2.z(); }

private:
  // les 2 tailles et leurs scales
  const Vec3<std::string> _t1;
  const Vec3<std::string> _t2;
  const Vec3<std::string> _s1;
  const Vec3<std::string> _s2;	

  // la relation entre les deux variables
  std::string _rel;
};

/**
 * \class Equal
 * \brief Une contrainte d'egalité
 * \date : October 2008
 */
class Equal : public Constraint
{
public:
  Equal(std::string & t1, int v)
  :_var(t1),_val(v)
  { }

  virtual ~Equal(){}

  const std::string & getVar() const  { return _var; }

  const int getVal() const { return _val; }

private:
  // la variable
  std::string _var;

  // sa valeur
  int _val;
};

/**
 * \class NoCollision
 * \brief Une contrainte de non-intersection
 * \date : October 2008
 */
class NoCollision : public Constraint
{
public:
  NoCollision(Vec3<std::string> & p1, Vec3<std::string> & p2, Vec3<std::string> & t1, Vec3<std::string> & t2)
  :_p1(p1), _p2(p2), _t1(t1), _t2(t2)
  { }

  virtual ~NoCollision(){}

  const std::string & getP1X() { return _p1.x(); }
  const std::string & getP1Y() { return _p1.y(); }
  const std::string & getP1Z() { return _p1.z(); }

  const std::string & getP2X() { return _p2.x(); }
  const std::string & getP2Y() { return _p2.y(); }
  const std::string & getP2Z() { return _p2.z(); }

  const std::string & getT1X() { return _t1.x(); }
  const std::string & getT1Y() { return _t1.y(); }
  const std::string & getT1Z() { return _t1.z(); }

  const std::string & getT2X() { return _t2.x(); }
  const std::string & getT2Y() { return _t2.y(); }
  const std::string & getT2Z() { return _t2.z(); }

private:
  // les 2 points et leurs tailles
  const Vec3<std::string> _p1;
  const Vec3<std::string> _p2;
  const Vec3<std::string> _t1;
  const Vec3<std::string> _t2;
};

/**
 * \class CompColor
 * \brief Une contrainte sur les couleurs
 * \date : October 2008
 */
class CompColor : public Constraint
{
public:
  CompColor(std::string & r1, std::string & g1, std::string & b1, std::string & a1,
      std::string & r2, std::string & g2, std::string & b2, std::string & a2 , std::string & rel)
  : _r1(r1), _g1(g1), _b1(b1), _a1(a1), _r2(r2), _g2(g2), _b2(b2), _a2(a2), _rel(rel)
  { }

  virtual ~CompColor(){}

  const std::string & getRelation() { return _rel; }

  const std::string & getR1() { return _r1; }
  const std::string & getG1() { return _g1; }
  const std::string & getB1() { return _b1; }
  const std::string & getA1() { return _a1; }

  const std::string & getR2() { return _r2; }
  const std::string & getG2() { return _g2; }
  const std::string & getB2() { return _b2; }
  const std::string & getA2() { return _a2; }

private:
  // les variables
  std::string _r1, _g1, _b1, _a1, _r2, _g2, _b2, _a2;

  // la relation entre les 2 couleurs
  std::string _rel;

};
#endif /*CONSTRAINTS_H_*/
