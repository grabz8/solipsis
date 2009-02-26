//
//  The BREP Library.
//  Copyright (C) 1996 Philippe Bekaert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// C++ version: Romain Behar (romainbehar@yahoo.com), Feb 2002

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>


// Some math
const double EPSILON = 1.0e-6;


// A vector in 3D space
class VECTOR
{
public:
	VECTOR(const double X = 0, const double Y = 0, const double Z = 0) :
		x(X),
		y(Y),
		z(Z)
	{}

	~VECTOR() {}

	VECTOR& operator = (const VECTOR& v);

	double Norm2()
	{
		return x*x+y*y+z*z;
	}

	double Norm()
	{
		return sqrt(Norm2());
	}

	void InverseScale(const double s)
	{
		if(fabs(s) < EPSILON)
			return;

		double is = 1/s;
		x *= is;
		y *= is;
		z *= is;
	}

	void Normalize()
	{
		double norm = Norm();
		InverseScale(norm);
	}

	double x;
	double y;
	double z;
};

const unsigned int XNORMAL = 0;
const unsigned int YNORMAL = 1;
const unsigned int ZNORMAL = 2;

inline VECTOR operator - (const VECTOR& a)
{ return VECTOR(-a.x, -a.y, -a.z); }

inline VECTOR& VECTOR::operator = (const VECTOR& v)
{ x = v.x; y = v.y; z = v.z; return *this; }

inline VECTOR operator + (const VECTOR& a, const VECTOR& b)
{ return VECTOR(a.x+b.x, a.y+b.y, a.z+b.z); }

inline VECTOR operator - (const VECTOR& a, const VECTOR& b)
{ return VECTOR(a.x-b.x, a.y-b.y, a.z-b.z); }

inline VECTOR operator * (const double d, const VECTOR& a)
{ return VECTOR(d*a.x, d*a.y, d*a.z); }

// Cross product
inline VECTOR operator ^ (const VECTOR& a, const VECTOR& b)
{ return VECTOR(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x); }

// Dot product
inline double operator * (const VECTOR& a, const VECTOR& b)
{ return (a.x*b.x + a.y*b.y + a.z*b.z); }

// Centre of two points
inline VECTOR MidPoint(const VECTOR& a, const VECTOR& b)
{ return 0.5*(a+b); }

// Distance between two points in 3D space
inline double VectorDist(const VECTOR& a, const VECTOR& b)
{
	return (b-a).Norm();
}

// Find the "dominant" part of the vector (eg patch-normal),
// this is used to turn the point-in-polygon test into a 2D problem
inline unsigned int VectorDominantCoord(const VECTOR& v)
{
	double ax = fabs(v.x);
	double ay = fabs(v.y);
	double az = fabs(v.z);

	if(ax >= ay && ax >= az)
		return XNORMAL;

	if(ay > ax && ay >= az)
		return YNORMAL;

	return ZNORMAL;
}

inline bool operator < (const VECTOR& v1, const VECTOR& v2)
{
	if(v1.x != v2.x)
		return v1.x < v2.x;
	if(v1.y != v2.y)
		return v1.y < v2.y;
	if(v1.z != v2.z)
		return v1.z < v2.z;

	return false;
}

inline bool operator == (const VECTOR& v1, const VECTOR& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

// Given a vector p in 3D space and an index i, which is
// XNORMAL, YNORMAL or ZNORMAL, projects the vector on
// the YZ, XZ or XY plane respectively

// 2d vector
class VEC2D
{
public:
	VEC2D(const double U = 0, const double V = 0) :
		u(U),
		v(V)
	{}

	double u;
	double v;
};

inline VEC2D VectorProject(const VECTOR& p, unsigned int index)
{
	if(index == XNORMAL)
		return VEC2D(p.y, p.z);

	if(index == YNORMAL)
		return VEC2D(p.x, p.z);

	// ZNORMAL:
	return VEC2D(p.x, p.y);
};

typedef struct RAY {
	VECTOR	pos, dir;
} RAY;

#endif // !VECTOR_H


