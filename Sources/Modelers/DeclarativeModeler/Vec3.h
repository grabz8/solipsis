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

#ifndef _VEC3_H_
#define _VEC3_H_

#include <iostream>

template<class T> 
class Vec3
{
public:
  Vec3(const T x=0, const T y=0, const T z=0)
  :_x(x), _y(y), _z(z)
  {}

  virtual ~Vec3(){}

  const T & x() const { return _x;}
  const T & y() const { return _y;}
  const T & z() const { return _z;}

  void setX(const T x) { _x = x;}
  void setY(const T y) { _y = y;}
  void setZ(const T z) { _z = z;}

  /**
   * \brief Opérateur d'affichage
   */
  friend std::ostream& operator<<(std::ostream& os, Vec3<T> & v)
  {
    os << v.x() << " " << v.y() << " " << v.z();
    return os;
  }

  /**
   * \brief Opérateur d'acces []
   * \param i : l'indice
   */
  T & operator [] (const unsigned int i)
  {
    if(i==0)
      return _x;
    else if(i==1)
      return _y;
    else
      return _z;
  }

  /**
   * \brief Opérateur division par un scalaire
   * \param u : Le premier vecteur
   * \param v : Le scalaire
   */
  friend Vec3<T> operator / (const Vec3<T> & v1, T & val)
  {
    Vec3<T>(v1.x()/val, v1.y()/val, v1.z()/val );
  }

  /**
   * \brief Opérateur + entre 2 vecteurs
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   */
  friend Vec3<T> operator + (const Vec3<T> & v1, const Vec3<T> & v2)
  {
    return Vec3<T>(v1.x()+v2.x(), v1.y()+v2.y(), v1.z()+v2.z() );
  }

  /**
   * \brief Opérateur - entre 2 vecteurs
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   */
  friend Vec3<T> operator - (const Vec3<T> & v1, const Vec3<T> & v2)
  {
    return Vec3<T>(v1.x()-v2.x(), v1.y()-v2.y(), v1.z()-v2.z() );
  }

  /**
   * \brief Produit scalaire
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   */
  friend T operator * (const Vec3<T> & u, const Vec3<T> & v)
  {
    T val = u.x()*v.x() + u.y()*v.y() + u.z()*v.z();
    return val;
  }

  /**
   * \brief Produit vectoriel
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   */
  friend Vec3<T> operator ^ (const Vec3<T> & u, const Vec3<T> & v)
  {
    T x = u.y()*v.z() - u.z()*v.y();
    T y = u.z()*v.x() - u.x()*v.z();
    T z = u.x()*v.y() - u.y()*v.x();
    return Vec3<T>(x,y,z);
  }

  /**
   * \brief Normalisation d'une vecteur
   */
  void normalize()
  {
    _x /= sqrt(_x*_x + _y*_y +_z*_z);
    _y /= sqrt(_x*_x + _y*_y +_z*_z);
    _z /= sqrt(_x*_x + _y*_y +_z*_z);
  }

  /**
   * \brief Calcule la norme d'un vecteur
   * \return La norme d'un vecteur
   */
  T norm()
  {
    return sqrt(_x*_x + _y*_y +_z*_z);
  }

  /**
   * \brief Opérateur différent
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   * \return vrai si u!=v, faux sinon
   */
  friend bool operator != (const Vec3<T> & u, const Vec3<T> & v)
  {
    return !(u==v);
  }

  /**
   * \brief Opérateur différent
   * \param u : Le premier vecteur
   * \param v : Le second vecteur
   * \return vrai si u==v, faux sinon
   */
  friend bool operator == (const Vec3<T> & u, const Vec3<T> & v)
  {
    T valx = fabs(u.x()-v.x());
    T valy = fabs(u.y()-v.y());
    T valz = fabs(u.z()-v.z());

    return valx<1e-5 && valy<1e-5 && valz<1e-5;
  }

private:
  T _x,  _y,  _z;
};

typedef Vec3<double> Vec3d;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec3<char> Vec3c;

#endif /*VEC3_H_*/
