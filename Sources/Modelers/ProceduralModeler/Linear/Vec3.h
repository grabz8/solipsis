/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo

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

#ifndef Vec3_h
#define Vec3_h 1

#include <iostream>

/**
 * This class represents a 3D vector.
 */
template<typename T> class Vec2;

template<typename T> class Vec3 {

public:

  /**
   * Default constructor
   */
  Vec3<T>() : n(true) {
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = 0;     
  }

  /**
   * Constructor given an array of 3 components
   * @param v An array of 3 elements
   */
  Vec3<T>(const T v[3]) : n(false) { 
    vec[0] = v[0];
    vec[1] = v[1];
    vec[2] = v[2]; 
  }

  /**
   * Constructor given 3 individual components
   * @param x The first element.
   * @param y The second element.
   * @param z The third element.
   */
  Vec3<T>(T x, T y, T z) : n(false) {
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
  }

  Vec3<T>(const Vec3<T>& right){
    *this = right;
  }

  const Vec3<T>& operator =(const Vec3<T>& right)
  {
    vec[0] = right.vec[0];
    vec[1] = right.vec[1];
    vec[2] = right.vec[2];
    return *this;
  }

  /**
   * Returns right-handed cross product of vector and another vector.
   * @param v Another vector.
   * @return this^v.
   */
  Vec3<T> cross(const Vec3<T> &v) const {
    return Vec3(vec[1] * v.vec[2] - vec[2] * v.vec[1],
		  vec[2] * v.vec[0] - vec[0] * v.vec[2],
		  vec[0] * v.vec[1] - vec[1] * v.vec[0]);
  }

  /**
   * Returns dot (inner) product of vector and another vector
   * @param v The second vector of the dot product.
   * @return The dot product of this and v.
   */
  T dot(const Vec3<T> &v) const {
    return (vec[0] * v.vec[0] +
	    vec[1] * v.vec[1] +
	    vec[2] * v.vec[2]);
  }

  /**
   * Returns pointer to array of 3 components
   * @return The array of components
   */
  const T* getValue() const {
    return vec; 
  }


  /**
   * Returns 3 individual components
   * @param x The first component.
   * @param y The second component.
   * @param z The third component.
   */
  void getValue(T &x, T &y, T &z) const {
    x = vec[0];
    y = vec[1];
    z = vec[2];
  }

  /**
   * Returns geometric length of vector
   * @return The length of this vector.
   */
  T length() const {
    return T(sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
  }
  
  /**
   * Changes vector to be unit length
   * @return The previous length of this vector.
   */
  T normalize() {
    double len = length();
    
    if( len != 0.0 )
      (*this) *= (1.0 / len);
    
    else setValue( 0.0, 0.0, 0.0 );
    
    return len;
  }

  /**
   * Negates each component of vector in place
   */
  void negate() {
    vec[0] = -vec[0];
    vec[1] = -vec[1];
    vec[2] = -vec[2];
  }


  /**
   * Sets value of vector from array of 3 components
   * @param v An array of 3 components.
   * @return this vector.
   */
  Vec3<T>& setValue(const T v[3]) { 
    vec[0] = v[0]; 
    vec[1] = v[1]; 
    vec[2] = v[2]; 
    n = false;
    return *this; 
  }

  /**
   * Convert to an Vec2, without z component
   * @return an Vec2 vector.
   */
  Vec2<T> convert() const {
    Vec2<T> result(vec[0],vec[1]);
    return result;
  }
  



  /**
   * Sets value of vector from 3 individual components
   * @param x The first component
   * @param y The second component
   * @param z The third component
   * @return This vector
   */
  Vec3<T>& setValue(T x, T y, T z) {
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    n = false;
    return *this;
  }

 /**
   * Accesses indexed component of vector
   * @param i The index of the component. i must be 0, 1 or 2, no check is made.
   * @return The value of the component.
   */
  T& operator [](int i) {
    n = false;
    return (vec[i]);
  }

  /**
   * Constant component accessor.
   * @param i The index of the component. i must be 0, 1 or 2, no check is made.
   * @return The value of the component.
   */ 
  const T& operator [](int i) const{
    return (vec[i]);
  }

  /**
   * Component-wise scalar multiplication
   * @param d The value to multiply.
   * @return this vector.
   */
  Vec3<T>& operator *=(T d) {
    vec[0] *= d;
    vec[1] *= d;
    vec[2] *= d;

    return *this;
  }

  /**
   * Component-wise scalar division
   * @param d The value to divide.
   * @return this vector.
   */
  Vec3<T>& operator /=(T d) {
    return *this *= (1.0 / d);
  }

  /**
   * Component-wise vector addition
   * @param v The vector to add.
   * @return This vector.
   */
  Vec3<T>& operator +=(const Vec3<T>& v) {
    vec[0] += v.vec[0];
    vec[1] += v.vec[1];
    vec[2] += v.vec[2];

    return *this;
  }
  
  /**
   * Component-wise vector substraction
   * @param v The vector to substract.
   * @return This vector.
   */
  Vec3<T>& operator -=(const Vec3<T>& v) {
    vec[0] -= v.vec[0];
    vec[1] -= v.vec[1];
    vec[2] -= v.vec[2];

    return *this;
  }
  
  /**
   * Nondestructive unary negation - returns a new vector
   * @return this vector's opposite.
   */
  Vec3<T> operator -() const {
    Vec3<T> neg(0,0,0);
    neg -= *this;
    return neg;
  }
  
  /**
   * Component-wise binary scalar multiplication - returns a new vector
   * @param v the vector to multiply
   * @param d the scalar
   * @return d*v
   */
  friend Vec3<T> operator *(const Vec3<T> &v, T d) {
    return Vec3<T>(v.vec[0] * d,
		   v.vec[1] * d,
		   v.vec[2] * d);
  }

  /**
   * Component-wise binary scalar multiplication - returns a new vector
   * @param d the scalar
   * @param v the vector to multiply
   * @return d*v
   */
  friend Vec3<T> operator *(T d, const Vec3<T> &v) {
    return v*d; 
  }
  
  /**
   * Component-wise binary scalar division - returns a new vector
   * @param v the vector to multiply
   * @param d the scalar
   * @return d*v
   */
  friend Vec3<T> operator /(const Vec3<T> &v, T d) {
    return v*(1.0/d);
  }

  /**
   * Component-wise binary vector addition - returns a new vector
   * @param v1 first vector
   * @param v2 second vector
   * @return v1+v2
   */ 
  friend Vec3<T> operator +(const Vec3<T> &v1, const Vec3<T> &v2) {
    return Vec3<T>(v1.vec[0] + v2.vec[0],
		   v1.vec[1] + v2.vec[1],
		   v1.vec[2] + v2.vec[2]);
  }

  /**
   * Component-wise binary vector substraction - returns a new vector
   * @param v1 first vector
   * @param v2 second vector
   * @return v1-v2
   */
  friend Vec3<T> operator -(const Vec3<T> &v1, const Vec3<T> &v2) {
    return Vec3<T>(v1.vec[0] - v2.vec[0],
		   v1.vec[1] - v2.vec[1],
		   v1.vec[2] - v2.vec[2]);
  }

  /**
   * Equality comparison operator
   * @param v1 a vector
   * @param v2 another vector
   * @return true if the 2 vectors are equal.
   */
  friend int  operator ==(const Vec3<T> &v1, const Vec3<T> &v2) {
    return (v1.vec[0] == v2.vec[0] &&
	    v1.vec[1] == v2.vec[1] &&
	    v1.vec[2] == v2.vec[2]);
  }

  /**
   * Difference comparison operator
   * @param v1 a vector
   * @param v2 another vector
   * @return true if the 2 vectors aren't equal.
   */
  friend int  operator !=(const Vec3<T> &v1, const Vec3<T> &v2) { 
    return !(v1 == v2); 
  }

  /**
   * Lower comparison operator
   * Compares consecutively the first, second then third component.
   * @param v1 a vector
   * @param v2 another vector
   * @return true if v1<v2.
   */
  friend bool operator <(const Vec3<T> &v1, const Vec3<T> &v2) {
    return v1.vec[0] < v2.vec[0] ? true : (v1.vec[1] < v2.vec[1] ? true : (v1.vec[2] < v2.vec[2]));
  }

  /**
   * Equality comparison within given tolerance - the square of the length of the maximum distance between the two vectors.
   * @param v the vector to compare with
   * @param tolerance the tolerance to apply to this comparison
   * @return true if (this-v).length()<=tolerance
   */
  bool  equals(const Vec3<T> v, T tolerance) const {
    Vec3<T> diff = *this - v;
    return diff.length() <= tolerance;
  }

  /**
   * Indicates if this vector has been assigned values or not.
   * @return true if this vector wasn't assigned values.
   */
  bool isNull() const { return n; }


  /**
   * Converts this vector (supposed to be Zup) in Yup.
   * @return a reference to this vector modified.
   * @see toZup
   */
  Vec3<T>& toYup() {
    T tmp = vec[1];
    vec[1] = vec[2];
    vec[2] = -tmp;
    return (*this);
  }

  /**
   * Converts this vector (supposed to be Yup) in Zup.
   * @return a reference to this vector modified.
   * @see toYup
   */
  Vec3<T>& toZup() {
    T tmp = vec[2];
    vec[2] = vec[1];
    vec[1] = -tmp;
    return (*this);
  }

protected:

  /**
   * Storage for vector components
   */
  T vec[3];

  /**
   * Boolean indicating if the vector has been initialized or not.
   */
  bool n;
};


/**
 * Convenience operator to output a vector to a stl stream
 * @param s the stream to write to.
 * @param v the vector to be written.
 */ 
template<typename T>
 std::ostream & operator<< (std::ostream& s, const Vec3<T>& v) {
  if(!v.isNull())
    s << v[0] << " " << v[1] << " " << v[2];
  return s;
}


/**
 * Convenience operator to input a vector from a stl stream
 * @param s the stream to read from.
 * @param v the vector to be read.
 */
template<typename T>
std::istream & operator>> (std::istream& s, Vec3<T>& v) {
  s >> v[0] >> v[1] >> v[2];
  return s;
}

/**
 * Typedef for a vector of char.
 */
typedef Vec3<char> Vec3c;

/**
 * Typedef for a vector of int.
 */
typedef Vec3<int> Vec3i;

/**
 * Typedef for a vector of short.
 */
typedef Vec3<short> Vec3s;

/**
 * Typedef for a vector of float.
 */
typedef Vec3<float> Vec3f;

/**
 * Typedef for a vector of double.
 */
typedef Vec3<double> Vec3d;

#endif
