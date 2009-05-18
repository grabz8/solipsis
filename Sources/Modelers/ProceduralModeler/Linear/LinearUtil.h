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

#ifndef LinearUtil_h
#define LinearUtil_h 1

#include <iostream>
#include <math.h>

#include <vector>

#include "Vec3.h"

/**
 * Utility functions.
 * These are some useful geometric functions.
 */
namespace Linear {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const double GradeToRad = 0.015707963;
const double DegToRad = 0.017453293;
const double GradeToDeg = 0.9;
const double RadToGrade = 1/GradeToRad;
const double RadToDeg = 1/DegToRad;
const double DegToGrade = 1/GradeToDeg;

const double Centimeter = 0.01;
const double Millimeter = 0.001;

/**
 * Computes a triangle's height.
 * @param v1 Triangle's first vertex.
 * @param v2 Triangle's second vertex.
 * @param v3 Triangle's third vertex.
 * @return The height of the triangle.
 */
template<class T> inline T triangleHeight(const Vec3<T>& v1,
		const Vec3<T>& v2, const Vec3<T>& v3) {
	Vec3<T> v12, v13;
	Vec3<T> ph;
	T h, l;
	const T epsilon = 0.0001;

	// calcul de la base du triangle
	v12 = v2 - v1;
	l = v12.length();

	// test pour voir si ce vecteur n'est pas nul
	if (l > epsilon) {
		// calcul de la hauteur
		v13 = v3 - v1;
		ph = v1 + (v13.dot(v12) / v12.dot(v12)) *v12;
		h = (v3 - ph).length();
	} else {
		h = 0.0;
	}
	return h;
}

/**
 * Computes the angle between v1, v2 and v3, v1<-v2->v3.
 * @param v1 First vertex.
 * @param v2 Second vertex.
 * @param v3 Third vertex.
 * @return The angle's value
 */
template<class T> inline T angle(const Vec3<T>& v1, const Vec3<T>& v2,
		const Vec3<T>& v3) {
	Vec3<T> vector1 = v1 - v2;
	Vec3<T> vector2 = v3 - v2;
	return (acos((vector1.dot(vector2))/(vector1.length()*vector2.length())));
}

/**
 * Returns true if the three points are aligned with a tolerance.
 * @param v1 The first point.
 * @param v2 The second point.
 * @param v3 The third point.
 * @param tolerance The tolerance value
 * @return true if aligned false otherwise
 */template<class T> bool aligned(const Vec3<T>& v1, const Vec3<T>& v2,
		const Vec3<T>& v3, const T& tolerance) {
	if (v1 != v2 && v2 != v3 && v1 != v3) {
//		std::cout << "tolerance: " << tolerance << " (" << tolerance*Linear::RadToDeg << "), ANGLE: " << angle(v1, v2, v3) << " (" << angle(v1, v2, v3)*Linear::RadToDeg << ")" << std::endl;
		if (fabs(angle(v1, v2, v3)-M_PI) < tolerance)
			return true;
	}
	return false;
}

/**
 * Computes the intersection beetween two 3D segments.
 * @param p1 First vertex of first segment.
 * @param p2 Second vertex of first segment.
 * @param p3 First vertex of second segment.
 * @param p4 Second vertex of second segment.
 * @param pa Position of the intersect point on [p1p2].
 * @param pb Position of the intersect point on [p2p3].
 * @param ta Distance from p1
 * @param tb Distance from p2
 * @return true if 0 < ta < 1 and 0 < tb < 1 
 */
template<class T> bool computeIntersectionOfTwoSegments(const Vec3<T>& p1,
		const Vec3<T>& p2, const Vec3<T>& p3, const Vec3<T>& p4,
		Vec3<T> &pa, Vec3<T> &pb, T &ta, T &tb) {

	// function by P. Rourke (Graphics Gems)

	Vec3<T> p13, p43, p21;
	T d1343, d4321, d1321, d4343, d2121;
	T numer, denom;

	p13.setValue(p1[0]-p3[0], p1[1]-p3[1], p1[2]-p3[2]);
	p43.setValue(p4[0]-p3[0], p4[1]-p3[1], p4[2]-p3[2]);
	p21.setValue(p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2]);

	const T epsilon = 0.00001;
	/*     const T upsilon = 0.000001; */

	if (fabs(p43[0]) < epsilon && fabs(p43[1]) < epsilon && fabs(p43[2])
			< epsilon)
		return false;

	if (fabs(p21[0]) < epsilon && fabs(p21[1]) < epsilon && fabs(p21[2])
			< epsilon)
		return false;

	d1343 = p13[0] * p43[0] + p13[1] * p43[1] + p13[2] * p43[2];
	d4321 = p43[0] * p21[0] + p43[1] * p21[1] + p43[2] * p21[2];
	d1321 = p13[0] * p21[0] + p13[1] * p21[1] + p13[2] * p21[2];
	d4343 = p43[0] * p43[0] + p43[1] * p43[1] + p43[2] * p43[2];
	d2121 = p21[0] * p21[0] + p21[1] * p21[1] + p21[2] * p21[2];

	denom = d2121 * d4343 - d4321 * d4321;
	if (fabs(denom) < epsilon)
		return false;

	numer = d1343 * d4321 - d1321 * d4343;

	ta = numer / denom;
	tb = (d1343 + d4321 * (ta)) / d4343;

	pa.setValue(p1[0]+ta*p21[0], p1[1]+ta*p21[1], p1[2]+ta*p21[2]);
	pb.setValue(p3[0]+tb*p43[0], p3[1]+tb*p43[1], p3[2]+tb*p43[2]);

	/*     if(ta >= (1-upsilon) || ta <= upsilon) */
	/*       return false; */
	/*     if(tb >= (1-upsilon) || tb <= upsilon) */
	/*       return false; */

	if (fabs(1-ta) < epsilon && (tb > 0.0 && tb < 1.0))
		return true;
	if (fabs(1-tb) < epsilon && (ta > 0.0 && ta < 1.0))
		return true;

	if (ta >= 1 || ta <= 0)
		return false;
	if (tb >= 1 || tb <= 0)
		return false;

	return true;
}

/** Computes the third coordinate of the cross product between 3 points
 * @param p0 The first point
 * @param p1 The second point
 * @param p3 The third point
 * @return The Z value of the cross product
 */
template<class T> T arcIsLeft(const Vec3<T>& p0, const Vec3<T>& p1,
		const Vec3<T>& p2) {
	return (p1[0]-p0[0])*(p2[1]-p0[1])-(p2[0]-p0[0])*(p1[1]-p0[1]);
}

/**
 * Computes the angle between v1, v2 and v3.
 * @param v1 First vertex.
 * @param v2 Second vertex.
 * @param v3 Third vertex.
 * @return The value of the angle
 */
template<class T> bool isVertexInPolyline(const Vec3<T>& vertex,
		const std::vector<Vec3<T> >& vertices) {
	int wn = 0; // the winding number counter
	unsigned int numOfVertices = vertices.size();

	// loop through all edges of the polygon
	unsigned int i;
	for (i = 0; i < numOfVertices; i++) {
		if (vertices[i][1] <= vertex[1]) { // start y <= v.y
			if (vertices[(i+1)%numOfVertices][1] > vertex[1])
				// an upward crossing
				if (arcIsLeft(vertices[i], vertices[(i+1)%numOfVertices], vertex) > 0)
					// P left of edge
					++wn; // have a valid up intersect
		} else { // start y > P.y (no test needed)
			if (vertices[(i+1)%numOfVertices][1] <= vertex[1])
				// a downward crossing
				if (arcIsLeft(vertices[i], vertices[(i+1)%numOfVertices], vertex) < 0)
					// P right of edge
					--wn; // have a valid down intersect
		}
	}
	return (0 != wn);
}

/** Tests if a polyline "turns" in trigonometric way
 * @param vertices The vector of points
 * @return true If trigo, false otherwise
 */
template<class T> bool isTrigo(const std::vector<Vec3<T> >& vertices) {
	unsigned int size = vertices.size();

	T sumOfAngles = 0;
	for (unsigned int i = 0; i < size; i++) {
		Vec3<T> vector1 = vertices[i] - vertices[(size+i-1)%size];
		Vec3<T> vector2 = vertices[(i+1)%size] - vertices[i];
		vector1[2] = 0.0;
		vector2[2] = 0.0;
		vector1.normalize();
		vector2.normalize();

		Vec3<T> crossProduct = vector1.cross(vector2);
		T dot = vector1.dot(vector2);

		if ((crossProduct[2] != 0 && crossProduct[2] > 0))
			sumOfAngles += acos(dot);
		else
			sumOfAngles -= acos(dot);
	}
	return (sumOfAngles > 0.0);
}

/**
 * Returns the distance from a point to a line
 * @param p1, p2 points of the line
 * @param p the point 
 * @return The distance from the point to the line
 */
template<class T> T distance(const Vec3<T>& p1, const Vec3<T>& p2,
		const Vec3<T>& p) {
	Vec3<T> p1p = p-p1;
	Vec3<T> p1p2 = p2-p1;
	T u = (p1p.dot(p1p2))/(p1p2.length()*p1p2.length());
	Vec3<T> intersection = p1+u*(p2-p1);
	return ((intersection-p).length());
}

/**
 * Returns the centroid of a ring
 * @param ring The ring
 * @return The centroid (means of ring vertices)
 */
template< class T > Vec3< T > centroid( const std::vector< Vec3< T > >& ring )
{
	Vec3< T > c( 0, 0, 0 );
	unsigned int size = ring.size();
	unsigned int i = 0; 
	while( i < size )
		c += ring[i++];
	return c/size;
}

/**
 * Converts an angle in grade to the radian one.
 * @param gangle The angle to convert in grade.
 * @return The angle's value in radians.
 */
inline double gradeToRadian(double gangle) {
	return gangle*GradeToRad;
}

}

#endif
