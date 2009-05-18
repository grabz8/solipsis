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

#ifndef Line_h
#define Line_h 1

#include <math.h>
#include "Vec3.h"
#include "Box.h"

template<typename T>
class Line {

public:


  /**
   * Default Constructor.
   * Builds a null line.
   */
  Line() {}

  /**
   * Construct a line from two points lying on the line.  If you
   * want to construct a line from a position and a direction, use
   * Line(p, p + d).
   * Line is directed from p0 to p1.
   * @param p0 First point on the line.
   * @param p1 Second point on the line.
   */
  Line(const Vec3<T> &p0, const Vec3<T> &p1) {
    setValue(p0, p1);
  }

  /**
   * Defines a line with 2 points on it.
   * @param p0 First point on the line.
   * @param p1 Second point on the line.
   */
  void setValue(const Vec3<T> &p0, const Vec3<T> &p1) {
    pos = p0;
    dir = p1 - p0;
    dir.normalize();
  }

  /**
   * Find closest points between the two lines. Return false if they are 
   * parallel, otherwise return true.
   */
  bool getClosestPoints(const Line<T>& line2,
			Vec3<T>& ptOnThis,
			Vec3<T>& ptOnLine2) const {
    double    s, t, A, B, C, D, E, F, denom;
    Vec3<T>  pos2 = line2.getPosition();
    Vec3<T>  dir2 = line2.getDirection();
    
    
    //  DERIVATION:
    //      [1] parametric descriptions of desired pts
    //          pos  + s * dir  = ptOnThis
    //          pos2 + t * dir2 = ptOnLine2
    //      [2] (By some theorem or other--)
    //          If these are closest points between lines, then line connecting 
    //          these points is perpendicular to both this line and line2.
    //          dir  . ( ptOnLine2 - ptOnThis ) = 0
    //          dir2 . ( ptOnLine2 - ptOnThis ) = 0
    //          OR...
    //          dir  . ( pos2 + t * dir2 - pos - s * dir ) = 0
    //          dir2 . ( pos2 + t * dir2 - pos - s * dir ) = 0
    //      [3] Rearrange the terms:
    //          t * [ dir  . dir2 ] - s * [dir  . dir ] = dir  . pos - dir  . pos2 
    //          t * [ dir2 . dir2 ] - s * [dir2 . dir ] = dir2 . pos - dir2 . pos2 
    //      [4] Let:
    //          A= dir  . dir2
    //          B= dir  . dir
    //          C= dir  . pos - dir . pos2
    //          D= dir2 . dir2
    //          E= dir2 . dir
    //          F= dir2 . pos - dir2 . pos2
    //          So [3] above turns into:
    //          t * A    - s * B =   C
    //          t * D    - s * E =   F
    //      [5] Solving for s gives:
    //          s = (CD - AF) / (AE - BD)
    //      [6] Solving for t gives:
    //          t = (CE - BF) / (AE - BD)
    
    A = dir.dot( dir2 );
    B = dir.dot( dir );
    C = dir.dot( pos ) - dir.dot( pos2 );
    D = dir2.dot( dir2 );
    E = dir2.dot( dir );
    F = dir2.dot( pos ) - dir2.dot( pos2 );


    denom = A * E - B * D;
    if (denom == 0.0)    // the two lines are parallel
      return false;

    s = ( C * D - A * F ) / denom;
    t = ( C * E - B * F ) / denom;
    ptOnThis  = pos  + dir  * s;
    ptOnLine2 = pos2 + dir2 * t;

    return true;
  }

  // Returns the closest point on the line to the given point.
  Vec3<T> getClosestPoint(const Vec3<T>& point) const {
    // vector from origin of this line to given point
    Vec3<T> p = point - pos;
    
    // find the length of p when projected onto this line
    // (which has direction d)
    // length = |p| * cos(angle b/w p and d) = (p.d)/|d|
    // but |d| = 1, so
    float length = p.dot(dir);
    
    // vector coincident with this line
    Vec3<T> proj = dir;
    proj *= length;
    
    Vec3<T> result = pos + proj;
    return result;
  }

  // Accessors
  const Vec3<T>& getPosition() const { return pos; }
  const Vec3<T>& getDirection() const { return dir; }


  // Intersect the line with a box, point, line, and triangle.
  bool intersect(const Box3<T>& box,
		 Vec3<T>& enter, Vec3<T>& exit) const {
    if (box.isEmpty())
      return false;

    const Vec3<T>& pos = getPosition(),& dir = getDirection();
    const Vec3<T>& max = box.getMax(),& min = box.getMin();
    Vec3<T> points[8], inter, bary;
    int i, v0, v1, v2;
    bool front = false, valid, validIntersection = false;

    //
    // First, check the distance from the ray to the center
    // of the box.  If that distance is greater than 1/2
    // the diagonal distance, there is no intersection
    // diff is the vector from the closest point on the ray to the center
    // dist2 is the square of the distance from the ray to the center
    // rad2 is the square of 1/2 the diagonal length of the bounding box
    //
    float    t = (box.getCenter() - pos).dot(dir);
    Vec3<T>  diff(pos + t * dir - box.getCenter());
    float    dist2 = diff.dot(diff);
    float    rad2 = (max - min).dot(max - min) * .25;
    
    if (dist2 > rad2)
      return false;

    // set up the eight coords of the corners of the box
    for(i = 0; i < 8; i++)
      points[i].setValue(i&  01 ? min[0] : max[0],
			 i&  02 ? min[1] : max[1],
			 i&  04 ? min[2] : max[2]);

    // intersect the 12 triangles.
    for(i = 0; i < 12; i++) {
      switch(i) {
      case  0: v0 = 2; v1 = 1; v2 = 0; break;  // +z
      case  1: v0 = 2; v1 = 3; v2 = 1; break;
 
      case  2: v0 = 4; v1 = 5; v2 = 6; break;  // -z
      case  3: v0 = 6; v1 = 5; v2 = 7; break;
 
      case  4: v0 = 0; v1 = 6; v2 = 2; break;  // -x
      case  5: v0 = 0; v1 = 4; v2 = 6; break;

      case  6: v0 = 1; v1 = 3; v2 = 7; break;  // +x
      case  7: v0 = 1; v1 = 7; v2 = 5; break;
 
      case  8: v0 = 1; v1 = 4; v2 = 0; break;  // -y
      case  9: v0 = 1; v1 = 5; v2 = 4; break;
 
      case 10: v0 = 2; v1 = 7; v2 = 3; break;  // +y
      case 11: v0 = 2; v1 = 6; v2 = 7; break;
      }
      if ((valid = intersect(points[v0], points[v1], points[v2],
			     inter, bary, front)) == true) {
	if (front) {
	  enter = inter;
	  validIntersection = valid;
	}
	else {
	  exit = inter;
	  validIntersection = valid;
	}
      }
    }
    return validIntersection;
  }
  
  bool intersect(float angle, const Box3<T>& box) const {
    if (box.isEmpty())
      return false;

    const Vec3<T>& max = box.getMax(),& min = box.getMin();
    float  fuzz = 0.0;
    int   i;

    if (angle < 0.0)
      fuzz = - angle;

    else {
      // Find the farthest point on the bounding box (where the pick
      // cone will be largest).  The amount of fuzz at this point will
      // be the minimum we can use.  Expand the box by that amount and
      // do an intersection.
      double tanA = tan(angle);
      for(i = 0; i < 8; i++) {
	Vec3<T> point(i&  01 ? min[0] : max[0],
			i&  02 ? min[1] : max[1],
			i&  04 ? min[2] : max[2]);
	// how far is point from line origin??
	Vec3<T> diff(point - getPosition());
	double thisFuzz = sqrt(diff.dot(diff)) * tanA;

	if (thisFuzz > fuzz)
	  fuzz = thisFuzz;
      }
    }

    Box3<float> fuzzBox = box;

    fuzzBox.extendBy(Vec3<T>(min[0] - fuzz, min[1] - fuzz, min[2] - fuzz));
    fuzzBox.extendBy(Vec3<T>(max[0] + fuzz, max[1] + fuzz, max[2] + fuzz));

    Vec3<T> scratch1, scratch2;
    return intersect(fuzzBox, scratch1, scratch2);
  }

  bool intersect(float angle, const Vec3<T>& point) const {
    float t, d;

    // how far is point from line origin??
    Vec3<T> diff(point - getPosition());

    t = diff.dot(getDirection());
    if(t > 0) {
      d = sqrt(diff.dot(diff) - t*t);
      if (angle < 0.0)
	return (d < -angle);
      return ((d/t) < angle);
    }
    return false;
  }

  bool intersect(float angle, const Vec3<T>& v0,
		 const Vec3<T>& v1, Vec3<T>& pt) const {
    Vec3<T> ptOnLine;
    Vec3<T> intersection;
    Line<T> inputLine(v0, v1);
    float distance;
    bool      validIntersection = false;

    if(getClosestPoints(inputLine, ptOnLine, intersection)) {
      // check to make sure the intersection is within the line segment
      if((intersection - v0).dot(v1 - v0) < 0)
	intersection = v0;
      else if((intersection - v1).dot(v0 - v1) < 0)
	intersection = v1;

      distance = (ptOnLine - intersection).length();
      if (angle < 0.0)
	return (distance < -angle);
      validIntersection = ((distance / (ptOnLine - getPosition()).length())
			   < angle);
    }
    return validIntersection;
  }


  bool intersect(const Vec3<T>& v0,
		 const Vec3<T>& v1,
		 const Vec3<T>& v2,
		 Vec3<T>& pt, Vec3<T>& barycentric,
		 bool& front) const {
    //////////////////////////////////////////////////////////////////
    //
    // The method used here is described by Didier Badouel in Graphics
    // Gems (I), pages 390 - 393.
    //
    //////////////////////////////////////////////////////////////////
    
#define EPSILON 1e-10

    //
    // (1) Compute the plane containing the triangle
    //
    Vec3<T> v01 = v1 - v0;
    Vec3<T> v12 = v2 - v1;
    Vec3<T> norm = v12.cross(v01); // Un-normalized normal
    // Normalize normal to unit length, and make sure the length is
    // not 0 (indicating a zero-area triangle)
    if (norm.normalize() < EPSILON)
      return false;

    //
    // (2) Compute the distance t to the plane along the line
    //
    float d = getDirection().dot(norm);
    if (d < EPSILON && d > -EPSILON)
      return false;   // Line is parallel to plane
    float t = norm.dot(v0 - getPosition()) / d;

    // Note: we DO NOT ignore intersections behind the eye (t < 0.0)

    //
    // (3) Find the largest component of the plane normal. The other
    //     two dimensions are the axes of the aligned plane we will
    //     use to project the triangle.
    //
    float xAbs = norm[0] < 0.0 ? -norm[0] : norm[0];
    float yAbs = norm[1] < 0.0 ? -norm[1] : norm[1];
    float zAbs = norm[2] < 0.0 ? -norm[2] : norm[2];
    int  axis0, axis1;

    if (xAbs > yAbs && xAbs > zAbs) {
      axis0 = 1;
      axis1 = 2;
    }
    else if (yAbs > zAbs) {
      axis0 = 2;
      axis1 = 0;
    }
    else {
      axis0 = 0;
      axis1 = 1;
    }

    //
    // (4) Determine if the projected intersection (of the line and
    //     the triangle's plane) lies within the projected triangle.
    //     Since we deal with only 2 components, we can avoid the
    //     third computation.
    //
    float intersection0 = getPosition()[axis0] + t * getDirection()[axis0];
    float intersection1 = getPosition()[axis1] + t * getDirection()[axis1];

    Vec2<float> diff0, diff1, diff2;
    bool isInter = false;
    float alpha, beta;

    diff0[0] = intersection0 - v0[axis0];
    diff0[1] = intersection1 - v0[axis1];
    diff1[0] = v1[axis0]     - v0[axis0];
    diff1[1] = v1[axis1]     - v0[axis1];
    diff2[0] = v2[axis0]     - v0[axis0];
    diff2[1] = v2[axis1]     - v0[axis1];

    // Note: This code was rearranged somewhat from the code in
    // Graphics Gems to provide a little more numeric
    // stability. However, it can still miss some valid intersections
    // on very tiny triangles.
    isInter = false;
    beta = ((diff0[1] * diff1[0] - diff0[0] * diff1[1]) /
	    (diff2[1] * diff1[0] - diff2[0] * diff1[1]));
    if (beta >= 0.0 && beta <= 1.0) {
      alpha = -1.0;
      if (diff1[1] < -EPSILON || diff1[1] > EPSILON) 
	alpha = (diff0[1] - beta * diff2[1]) / diff1[1];
      else
	alpha = (diff0[0] - beta * diff2[0]) / diff1[0];
      isInter = (alpha >= 0.0 && alpha + beta <= 1.0);
    }

    //
    // (5) If there is an intersection, set up the barycentric
    //     coordinates and figure out if the front was hit.
    //
    /*
    if (isInter) {
      barycentric.setValue(1.0 - (alpha + beta), alpha, beta);
      front = (getDirection().dot(norm) < 0.0);
      intersection = getPosition() + t * getDirection();
    }
    */

    return isInter;

#undef EPSILON
  }

  Line<T>& toYup() {
    pos.toYup();
    dir.toYup();
    return (*this);
  }

  Line<T>& toZup() {
    pos.toZup();
    dir.toZup();
    return (*this);
    
  }

private:
  // Parametric description:
  //  l(t) = pos + t * dir
  Vec3<T> pos;
  Vec3<T> dir;

};

#endif
