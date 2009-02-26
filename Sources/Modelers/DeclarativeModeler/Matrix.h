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

#ifndef MATRIX_H_
#define MATRIX_H_

#include <vector>
#include <cassert>
#include <iostream>
#include "Vec3.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * \file Matrix.h
 * \class Matrix<T>
 * \brief Une classe de matrice pleine
 */

template <class T> class Matrix{
public    :
  std::vector<std::vector<T> > data;

  /**
   * \brief Alloue en memoire une matrice
   * \param nb_lig Nombre de ligne
   * \param nb_col Nombre de colonne
   */
  Matrix(std::size_t nb_lig=3,std::size_t nb_col=3){
    for(std::size_t lig=0;lig<nb_lig;++lig){
      std::vector<T> tmp;
      tmp.reserve(nb_col);
      for(std::size_t col=0;col<nb_col;++col){
        tmp.push_back(0);//valeur par defaut
      }
      data.push_back(tmp);
    }
  }

  // definition d'une matrice a partir d'un quaternion (rotation définie a l'aide de son axe et de son angle...)
  Matrix(const T & x, const T & y, const T & z, const T & w)
  {
    T xx      = x * x;
    T xy      = x * y;
    T xz      = x * z;
    T xw      = x * w;

    T yy      = y * y;
    T yz      = y * z;
    T yw      = y * w;

    T zz      = z * z;
    T zw      = z * w;

    std::vector<T> tmp1;
    tmp1.reserve(3);
    tmp1.push_back( 1 - 2 * ( yy + zz ) );
    tmp1.push_back( 2 * ( xy - zw ) );
    tmp1.push_back( 2 * ( xz + yw ) );
    data.push_back(tmp1);

    std::vector<T> tmp2;
    tmp2.reserve(3);
    tmp2.push_back( 2 * ( xy + zw ) );
    tmp2.push_back( 1 - 2 * ( xx + zz ) );
    tmp2.push_back( 2 * ( yz - xw ) );
    data.push_back(tmp2);

    std::vector<T> tmp3;
    tmp3.reserve(3);
    tmp3.push_back( 2 * ( xz - yw ) );
    tmp3.push_back( 2 * ( yz + xw ) );
    tmp3.push_back(1 - 2 * ( xx + yy ) );
    data.push_back(tmp3);
  }

  // definition d'une matrice a partir des angles (Roll,Pitch,Yaw)
  Matrix(Vec3<T> v) 
  {
    std::vector<T> tmp;
    tmp.reserve(3);

    T gamma = v.x();
    T beta = v.y();
    T alpha = v.z();

    T cosgamma = fabs(cos(gamma)) < 1e-5 ? 0 :cos(gamma);
    T singamma = fabs(sin(gamma)) < 1e-5 ? 0 :sin(gamma);

    T cosbeta = fabs(cos(beta)) < 1e-5 ? 0 :cos(beta);
    T sinbeta = fabs(sin(beta)) < 1e-5 ? 0 :sin(beta);

    T cosalpha = fabs(cos(alpha)) < 1e-5 ? 0 : cos(alpha);
    T sinalpha = fabs(sin(alpha)) < 1e-5 ? 0 :sin(alpha);

    std::vector<T> tmp1;
    tmp1.reserve(3);
    tmp1.push_back(cosalpha * cosbeta);
    tmp1.push_back(cosalpha * sinbeta * singamma - sinalpha * cosgamma);
    tmp1.push_back(cosalpha * sinbeta * cosgamma + sinalpha*singamma);
    data.push_back(tmp1);

    std::vector<T> tmp2;
    tmp2.reserve(3);
    tmp2.push_back(sinalpha * cosbeta);
    tmp2.push_back(sinalpha * sinbeta*singamma + cosalpha*cosgamma);
    tmp2.push_back(sinalpha * sinbeta*cosgamma - cosalpha*singamma);
    data.push_back(tmp2);

    std::vector<T> tmp3;
    tmp3.reserve(3);
    tmp3.push_back(-sinbeta);
    tmp3.push_back(cosbeta * singamma);
    tmp3.push_back(cosbeta * cosgamma);
    data.push_back(tmp3);
  }

  Matrix(T & a_00, T & a_01, T & a_02, T & a_10, T & a_11, T & a_12, T & a_20, T & a_21, T & a_22)
  {
    std::vector<T> tmp1;
    tmp1.reserve(3);
    tmp1.push_back(a_00);
    tmp1.push_back(a_01);
    tmp1.push_back(a_02);
    data.push_back(tmp1);

    std::vector<T> tmp2;
    tmp2.reserve(3);
    tmp2.push_back(a_10);
    tmp2.push_back(a_11);
    tmp2.push_back(a_12);
    data.push_back(tmp2);

    std::vector<T> tmp3;
    tmp3.reserve(3);
    tmp3.push_back(a_20);
    tmp3.push_back(a_21);
    tmp3.push_back(a_22);
    data.push_back(tmp3);
  }

  /**
   * \brief Destructeur
   */
  ~Matrix(){}

  /**
   * \return Le nombre de ligne
   */
  inline std::size_t size1() const{
    return data.size();
  }

  /**
   * \return Le nombre de colonne
   */
  inline std::size_t size2() const{
    if (data.size()==0) return 0;
    return data[0].size();
  }

  /**
   * \brief Accesseur vers un element de la matrice
   * \param lig son numero de ligne
   * \param col son numero de colonne
   * \return la valeur stockee
   */
  inline T get(std::size_t lig,std::size_t col) const{
    assert(lig<size1());
    assert(col<size2());
    return data[lig][col];
  }

  /**
   * \brief Accesseur vers un element de la matrice
   * \param lig son numero de ligne
   * \param col son numero de colonne
   * \param val la valeur a stocker
   */
  inline void set(std::size_t lig,std::size_t col,T val){
    assert(lig<size1());
    assert(col<size2());
    data[lig][col]=val;
  }

};

/**
 * \brief Operateur << pour les matrices
 * \param o Le flux de sortie
 * \param m la matrice a ecrire
 */
template <class T>
std::ostream& operator << (std::ostream& o,const Matrix<T> & m){
  for(std::size_t lig=0;lig<m.size1();++lig){
    for(std::size_t col=0;col<m.size2();++col){
      o<<m.get(lig,col)<<" ";
    }
    o<<std::endl;
  }
  return o;
}

/**
 * \brief Addition matricielle
 * \param m1 la premiere matrice
 * \param m2 la deuxieme matrice
 * \return la somme
 */
template <class T>
Matrix<T> operator+ (const Matrix<T> & m1,const Matrix<T> & m2){
  assert(m1.size1()==m1.size1());
  assert(m1.size2()==m1.size2());
  std::size_t nb_lig=m1.size1();
  std::size_t nb_col=m1.size2();
  Matrix<T> sum(nb_lig,nb_col);
  for(std::size_t lig=0;lig<nb_lig;++lig){
    for(std::size_t col=0;col<nb_col;++col){
      sum.set(lig,col,m1.get(lig,col)+m2.get(lig,col));
    }
  }
  return sum;
}

/**
 * \brief Multiplication matricielle
 * \param m1 la premiere matrice
 * \param m2 la deuxieme matrice
 * \return le produit
 */
template <class T>
Matrix<T> operator* (const Matrix<T> & m1,const Matrix<T> & m2)
{
  assert(m1.size2()==m2.size1());
  std::size_t nb_lig=m1.size1();
  std::size_t nb_col=m2.size2();
  Matrix<T> prod(nb_lig,nb_col);
  for(std::size_t lig=0;lig<nb_lig;++lig){
    for(std::size_t col=0;col<nb_col;++col){
      //calcul du terme (lig,col)
      T tmp=0;
      for(std::size_t i=0;i<m1.size2();++i){
        tmp+=m1.get(lig,i)*m2.get(i,col);
      }
      prod.set(lig,col,tmp);
    }
  }
  return prod;
}

/**
 * \brief Multiplication d'une matrice par un vecteur
 * \param m1 la premiere matrice
 * \param v le vecteur
 * \return le vecteur résultat
 */
template <class T>
Vec3<T> operator * (const Matrix<T> & m1,const Vec3<T> & v)
{

  assert(m1.size2()==3);
  std::size_t nb_lig=m1.size1();
  std::size_t nb_col=3;

  Vec3<T> prod;
  for(std::size_t lig=0;lig<nb_lig;++lig){
    T tmp=0;
    for(std::size_t col=0;col<nb_col;++col)
      {
        if(col==0)
          tmp+=m1.data[lig][col]* v.x();
        else if(col==1)
          tmp+=m1.data[lig][col]* v.y();
        else 	if(col==2)
          tmp+=m1.data[lig][col]* v.z();
      }

    if(lig==0)
      prod.setX(abs(tmp));
    else if(lig==1)
      prod.setY(abs(tmp));
    else if(lig==2)
      prod.setZ(abs(tmp));
  }
  return prod;
}


#endif /*MATRIX_H_*/
