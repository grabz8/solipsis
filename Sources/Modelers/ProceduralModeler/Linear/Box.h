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

#ifndef Box_h
#define Box_h 1
#include <limits>

#include "Vec2.h"
#include "Vec3.h"

/**
 * 3D box which has planes parallel to the major axes and is
 * specified by two points on a diagonal.
 */

template<typename T>
class Box2;

template<typename T>
class Box3 {

  public:

    /**
     * Builds an empty Box.
     * @see isEmpty.
     template<typename T> class Box3   */
    Box3<T> () {
      makeEmpty();
    }

    /**
     * Builds a box given the minimum and maximum as individual components.
     * @param xmin The first minimum component.
     * @param ymin The second minimum component.
     * @param zmin The third minimum component.
     * @param xmax The first maximum component.
     * @param ymax The second maximum component.
     * @param zmax The third maximum component.
     */
    Box3<T> (T xmin, T ymin, T zmin, T xmax, T ymax, T zmax) {
      min.setValue(xmin, ymin, zmin);
      max.setValue(xmax, ymax, zmax);
    }

    /**
     * Builds a box given the minimum and maximum as two 3D points.
     * @param min The minimum point.
     * @param max The maximum point.
     */
    Box3<T> (const Vec3<T> &min, const Vec3<T> &max) {
      this->min = min;
      this->max = max;
    }

    /**
     * Destructor.
     */
    ~Box3<T> () {
    }

    /**
     * Returns the minimum point of this box.
     * @return The minimum point.
     * @see getMax
     */
    const Vec3<T> & getMin() const {
      return min;
    }

    /**
     * Returns the minimum X point of this box.
     * @return The minimum X point.
     * @see getXMax
     */
    T getXMin() const {
      return (min[0]);
    }

    /**
     * Returns the minimum Y point of this box.
     * @return The minimum Y point.
     * @see getYMax
     */
    T getYMin() const {
      return min[1];
    }

    /**
     * Returns the minimum Z point of this box.
     * @return The minimum Z point.
     * @see getZMax
     */
    T getZMin() const {
      return min[2];
    }

    /**
     * Returns the maximum point of this box.
     * @return The maximum point.
     * @see getMin
     */
    const Vec3<T> & getMax() const {
      return max;
    }

    /**
     * Returns the maximum X point of this box.
     * @return The maximum X point.
     * @see getXMin
     */
    T getXMax() const {
      return max[0];
    }

    /**
     * Returns the maximum Y point of this box.
     * @return The maximum Y point.
     * @see getYMin
     */
    T getYMax() const {
      return max[1];
    }

    /**
     * Returns the maximum Z point of this box.
     * @return The maximum Z point.
     * @see getZMin
     */
    T getZMax() const {
      return max[2];
    }

    /**
     * Returns the minimum point of this box as a modifiable reference.
     * @return The minimum point.
     */
    Vec3<T> & getMin() {
      return min;
    }

    /**
     * Returns the maximum point of this box as a modifiable reference.
     * @return The maximum point.
     */
    Vec3<T> & getMax() {
      return max;
    }

    /**
     * Computes the center of this box.
     * @return The geometric center of this box.
     */
    Vec3<T> getCenter() const {
      return Vec3<T> (0.5 * (min[0] + max[0]), 0.5 * (min[1] + max[1]), 0.5 * (min[2] + max[2]));
    }

    /**
     * Returns the height (Z diff) of this box.
     * @return The height.
     */
    T getHeight() const {
      return (max[2] - min[2]);
    }

    /**
     * Returns the width (X diff) of this box.
     * @return The width.
     */
    T getWidth() const {
      return (max[0] - min[0]);
    }

    /**
     * Returns the depth (Y diff) of this box.
     * @return The depth.
     */
    T getDepth() const {
      return (max[1] - min[1]);
    }

    /**
     * Extends this box to include the parameter point.
     * @param pt The point to include in this box.
     * @see extendBy
     */
    void extendBy(const Vec3<T> &pt) {

      if (isEmpty()) {
        min = max = pt;
        return;
      }

      if (pt[0] < min[0])
        min[0] = pt[0];
      if (pt[1] < min[1])
        min[1] = pt[1];
      if (pt[2] < min[2])
        min[2] = pt[2];
      if (pt[0] > max[0])
        max[0] = pt[0];
      if (pt[1] > max[1])
        max[1] = pt[1];
      if (pt[2] > max[2])
        max[2] = pt[2];
    }

    /**
     * Extends this box to include the parameter box.
     * @param bb The box to include in this box.
     * @see extendBy
     */
    void extendBy(const Box3<T> &bb) {

      if (isEmpty()) {
        *this = bb;
        return;
      }

      if (bb.min[0] < min[0])
        min[0] = bb.min[0];
      if (bb.min[1] < min[1])
        min[1] = bb.min[1];
      if (bb.min[2] < min[2])
        min[2] = bb.min[2];
      if (bb.max[0] > max[0])
        max[0] = bb.max[0];
      if (bb.max[1] > max[1])
        max[1] = bb.max[1];
      if (bb.max[2] > max[2])
        max[2] = bb.max[2];
    }

    /**
     * Indicates if the parameter point is in this box.
     * @param pt The point to test
     * @return true if the point is in this box.
     */
    bool intersect(const Vec3<T> &pt) const {
      return ((pt[0] >= min[0]) && (pt[1] >= min[1]) && (pt[2] >= min[2]) && (pt[0] <= max[0]) && (pt[1] <= max[1]) && (pt[2] <= max[2]));
    }

    /**
     * Indicates if the parameter box intersects this box.
     * @param bb The box to test
     * @return true if the box intersects this one.
     */
    bool intersect(const Box3<T> &bb) const {
      return ((bb.max[0] >= min[0]) && (bb.min[0] <= max[0]) && (bb.max[1] >= min[1]) && (bb.min[1] <= max[1]) && (bb.max[2] >= min[2]) && (bb.min[2] <= max[2]));
    }

    /**
     * Computes the intersection between this box and the parameter one.
     * @param other The box to intersect.
     * @return the intersection result.
     */
    Box3<T> operator &(const Box3<T>& other) const {
      // Intersection minimum = maximum(this.min,other.min)
      Vec3<T> imin, imax;
      imin[0] = min[0] > other.min[0] ? min[0] : other.min[0];
      imin[1] = min[1] > other.min[1] ? min[1] : other.min[1];
      imin[2] = min[2] > other.min[2] ? min[2] : other.min[2];

      // Intersection maximum = minimum(this.max,other.max)
      imax[0] = max[0] < other.max[0] ? max[0] : other.max[0];
      imax[1] = max[1] < other.max[1] ? max[1] : other.max[1];
      imax[2] = max[2] < other.max[2] ? max[2] : other.max[2];

      return Box3<T> (imin, imax);
    }

    /**
     * Intersects this box and the parameter one and affect the result to this one.
     * @param other The box to intersect.
     * @return A reference to this box after modification.
     */
    Box3<T>& operator &=(const Box3<T>& other) {
      *this = *this & other;
      return (*this);
    }

    /**
     * Computes the union between this box and the parameter one.
     * @param other The box to unite.
     * @return the union result.
     */
    Box3<T> operator |(const Box3<T>& other) const {
      // Union minimum = minimum(this.min,other.min)
      Vec3<T> umin, umax;
      umin[0] = min[0] < other.min[0] ? min[0] : other.min[0];
      umin[1] = min[1] < other.min[1] ? min[1] : other.min[1];
      umin[2] = min[2] < other.min[2] ? min[2] : other.min[2];

      // Union maximum = maximum(this.max,other.max)
      umax[0] = max[0] > other.max[0] ? max[0] : other.max[0];
      umax[1] = max[1] > other.max[1] ? max[1] : other.max[1];
      umax[2] = max[2] > other.max[2] ? max[2] : other.max[2];

      return Box3<T> (umin, umax);
    }

    /**
     * Unites this box and the parameter one and affect the result to this one.
     * @param other The box to unite.
     * @return A reference to this box after modification.
     */
    Box3<T>& operator |=(const Box3<T>& other) {
      *this = *this | other;
      return (*this);
    }

    /**
     * Sets the minimum and maximum points of this box as individual components.
     * @param xmin The first minimum component.
     * @param ymin The second minimum component.
     * @param zmin The third minimum component.
     * @param xmax The first maximum component.
     * @param ymax The second maximum component.
     * @param zmax The third maximum component.
     */
    void setBounds(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax) {
      min.setValue(xmin, ymin, zmin);
      max.setValue(xmax, ymax, zmax);
    }

    /**
     * Sets the minimum and maximum given as two 3D points.
     * @param min The minimum point.
     * @param max The maximum point.
     */
    void setBounds(const Vec3<T> &pmin, const Vec3<T> &pmax) {
      this->min = pmin;
      this->max = pmax;
    }

    /**
     * Returns the minimum and maximum points of this box as individual components.
     * @param xmin The first minimum component.
     * @param ymin The second minimum component.
     * @param zmin The third minimum component.
     * @param xmax The first maximum component.
     * @param ymax The second maximum component.
     * @param zmax The third maximum component.
     */
    void getBounds(T &xmin, T &ymin, T &zmin, T &xmax, T &ymax, T &zmax) const {
      min.getValue(xmin, ymin, zmin);
      max.getValue(xmax, ymax, zmax);
    }

    /**
     *  Returns the minimum and maximum points of this box as two 3D points.
     * @param min The minimum point.
     * @param max The maximum point.
     */
    void getBounds(Vec3<T> &min, Vec3<T> &max) const {
      min = this->min;
      max = this->max;
    }

    /**
     * Computes the closest point of the parameter point to this box,
     * eg. the orthogonal projection of the point on this box.
     * @param point The point to project on this box.
     * @return The projected point.
     */
    Vec3<T> getClosestPoint(const Vec3<T> &point) {
      Vec3<T> result;

      // trivial cases first
      if (isEmpty())
        return point;
      else if (point == getCenter()) {
        // middle of z side
        result[0] = (max[0] + min[0]) / 2.0;
        result[1] = (max[1] + min[1]) / 2.0;
        result[2] = max[2];
      } else {
        // Find the closest point on a unit box (from -1 to 1),
        // then scale up.

        // Find the vector from center to the point, then scale
        // to a unit box.
        Vec3<T> vec = point - getCenter();
        T sizeX, sizeY, sizeZ;
        getSize(sizeX, sizeY, sizeZ);
        T halfX = sizeX / 2.0;
        T halfY = sizeY / 2.0;
        T halfZ = sizeZ / 2.0;
        if (halfX > 0.0)
          vec[0] /= halfX;
        if (halfY > 0.0)
          vec[1] /= halfY;
        if (halfZ > 0.0)
          vec[2] /= halfZ;

        // Side to snap side that has greatest magnitude in the vector.
        Vec3<T> mag;
        mag[0] = fabs(vec[0]);
        mag[1] = fabs(vec[1]);
        mag[2] = fabs(vec[2]);

        result = mag;

        // Check if beyond corners
        if (result[0] > 1.0)
          result[0] = 1.0;
        if (result[1] > 1.0)
          result[1] = 1.0;
        if (result[2] > 1.0)
          result[2] = 1.0;

        // snap to appropriate side
        if ((mag[0] > mag[1]) && (mag[0] > mag[2])) {
          result[0] = 1.0;
        } else if ((mag[1] > mag[0]) && (mag[1] > mag[2])) {
          result[1] = 1.0;
        } else if ((mag[2] > mag[0]) && (mag[2] > mag[1])) {
          result[2] = 1.0;
        } else if ((mag[0] == mag[1]) && (mag[0] == mag[2])) {
          // corner
          result = Vec3<T> (1, 1, 1);
        } else if (mag[0] == mag[1]) {
          // edge parallel with z
          result[0] = 1.0;
          result[1] = 1.0;
        } else if (mag[0] == mag[2]) {
          // edge parallel with y
          result[0] = 1.0;
          result[2] = 1.0;
        } else if (mag[1] == mag[2]) {
          // edge parallel with x
          result[1] = 1.0;
          result[2] = 1.0;
        }

        // Now make everything point the right way
        for (int i = 0; i < 3; i++)
          if (vec[i] < 0.0)
            result[i] = -result[i];

        // scale back up and move to center
        result[0] *= halfX;
        result[1] *= halfY;
        result[2] *= halfZ;

        result += getCenter();
      }

      return result;
    }

    /**
     * Returns the origin (eg. minimum) point of this box as individual components.
     * @param originX the origin's first component.
     * @param originY the origin's second component.
     * @param originZ the origin's third component.
     */
    void getOrigin(T &originX, T &originY, T &originZ) const {
      originX = min[0];
      originY = min[1];
      originZ = min[2];
    }

    /**
     * Returns the size of this box as individual components.
     * @param sizeX the size's first component.
     * @param sizeY the size's second component.
     * @param sizeZ the size's third component.
     */
    void getSize(T &sizeX, T &sizeY, T &sizeZ) const {
      sizeX = max[0] - min[0];
      sizeY = max[1] - min[1];
      sizeZ = max[2] - min[2];
    }

    /**
     * Makes this box empty, eg isEmpty is true afterwise.
     * @see isEmpty
     */
    void makeEmpty() {
      min.setValue(10, 10, 10);
      max.setValue(-10, -10, -10);
    }

    /**
     * Indicates if this box is empty.
     * @return true if this box is empty.
     */
    bool isEmpty() const {
      return max[0] < min[0];
    }

    /**
     * Indicates if this box's sizes are all strictly positive.
     * @return true if all sizes are strictly positive.
     */
    bool hasVolume() const {
      return (max[0] > min[0] && max[1] > min[1] && max[2] > min[2]);
    }

    /**
     * Indicates if this box is inside parameter's one
     * @return true if this box is inside 'outsider'
     */
    bool isInside(const Box3<T> outsider) {
      return ((outsider.getXMin() <= getXMin()) && (outsider.getYMin() <= getYMin()) && (outsider.getZMin() <= getZMin()) && (outsider.getXMax() >= getXMax()) && (outsider.getYMax() >= getYMax()) && (outsider.getZMax() >= getZMax()));
    }

    /**
     * Finds the span of a box along a specified direction.
     * The span is the total distance the box occupies along a given direction.
     * The total distance is returned in the form of a minimum and maximum distance
     * from the origin of each of the corners of the box along the given direction.
     * The difference between these two values is the span.
     * @param direction The direction along which you want this box's span.
     * @param dMin The minimum distance.
     * @param dMax The maximum distance.
     */
    void getSpan(const Vec3<T> &direction, T &dMin, T &dMax) const {
      int i;
      Vec3<T> points[8];
      Vec3<T> dir = direction;

      dir.normalize();

      /* Set up the eight points at the corners of the extent */
      points[0][2] = points[2][2] = points[4][2] = points[6][2] = min[2];
      points[1][2] = points[3][2] = points[5][2] = points[7][2] = max[2];

      points[0][0] = points[1][0] = points[2][0] = points[3][0] = min[0];
      points[4][0] = points[5][0] = points[6][0] = points[7][0] = max[0];

      points[0][1] = points[1][1] = points[4][1] = points[5][1] = min[1];
      points[2][1] = points[3][1] = points[6][1] = points[7][1] = max[1];

      points[0][2] = points[2][2] = points[4][2] = points[6][2] = min[2];
      points[1][2] = points[3][2] = points[5][2] = points[7][2] = max[2];

      dMin = (std::numeric_limits<T>::max)();
      dMax = -dMin;

      for (i = 0; i < 8; i++) {
        T proj = points[i].dot(dir);
        if (proj < dMin)
          dMin = proj;
        if (proj > dMax)
          dMax = proj;
      }
    }

    Box2<T> convert() const {
      Box2<T> result(min[0], min[1], max[0], max[1]);
      return result;
    }

    /**
     * Computes the volume of this box.
     * @return The volume of this box.
     */
    T getVolume() const {
      if (isEmpty())
        return T(0);

      return (max[0] - min[0]) * (max[1] - min[1]) * (max[2] - min[2]);
    }

    /**
     * Equality operator.
     * @param b1 the first box to compare.
     * @param b2 the second box to compare.
     * @return true if the two boxes' minimum and maximum are equal.
     */
    friend bool operator ==(const Box3<T> &b1, const Box3<T> &b2) {
      return ((b1.min == b2.min) && (b1.max == b2.max));
    }

    /**
     * Unequality operator.
     * @param b1 the first box to compare.
     * @param b2 the second box to compare.
     * @return true if one of the two boxes' minimum and maximum are different.
     */
    friend bool operator !=(const Box3<T> &b1, const Box3<T> &b2) {
      return !(b1 == b2);
    }

    /**
     * Converts this box (supposed to be Zup) in Yup.
     * @return The Yup box.
     */
    Box3<T>& toYup() {
      min.toYup();
      max.toYup();
    }

    /**
     * Converts this box (supposed to be Yup) in Zup.
     * @return The Zup box.
     */
    Box3<T>& toZup() {
      min.toZup();
      max.toZup();
    }


    /**
     * Equality comparison within given tolerance
     * @param v the box to compare with
     * @param tolerance the tolerance to apply to this comparison
     * @return true if (this-v).lenght()<tolerance
     */
    bool equals(const Box3<T>& v, T tolerance) const {
      return (this->min.equals(v.min, tolerance) && this->max.equals(v.max, tolerance));
    }

  private:

    Vec3<T> min, max;

};

/**
  * Convenience operator to output a box to a stl stream
  * @param s the stream to write to.
  * @param v the box to be written.
  */
 template<typename T>
 std::ostream & operator<<(std::ostream& s, const Box3<T>& v) {
   s << v.getMin() << " " << v.getMax();
   return s;
 }


template<typename T>
class Box2 {

  public:

    Box2<T> () {
      makeEmpty();
    }

    Box2<T> (T xmin, T ymin, T xmax, T ymax) {
      min.setValue(xmin, ymin);
      max.setValue(xmax, ymax);
    }

    Box2<T> (const Vec2<T> &_min, const Vec2<T> &_max) {
      min = _min;
      max = _max;
    }

    ~Box2<T> () {
    }

    const Vec2<T> & getMin() const {
      return min;
    }

    T getXMin() const {
      return min[0];
    }

    T getYMin() const {
      return min[1];
    }

    const Vec2<T> & getMax() const {
      return max;
    }

    T getXMax() const {
      return max[0];
    }

    T getYMax() const {
      return max[1];
    }

    Vec2<T> getCenter() const {
      return Vec2<T> (0.5 * (min[0] + max[0]), 0.5 * (min[1] + max[1]));
    }

    /**
     * Returns the height (Y diff) of this box.
     * @return The height.
     */
    T getHeight() const {
      return (max[1] - min[1]);
    }

    /**
     * Returns the width (X diff) of this box.
     * @return The width.
     */
    T getWidth() const {
      return (max[0] - min[0]);
    }

    void extendBy(const Vec2<T> &pt) {
      if (isEmpty()) {
        min = max = pt;
        return;
      }

      if (pt[0] < min[0])
        min[0] = pt[0];
      if (pt[0] > max[0])
        max[0] = pt[0];

      if (pt[1] < min[1])
        min[1] = pt[1];
      if (pt[1] > max[1])
        max[1] = pt[1];
    }

    void extendBy(const Box2<T> &r) {
      if (isEmpty())
        *this = r;

      if (r.min[0] < min[0])
        min[0] = r.min[0];
      if (r.max[0] > max[0])
        max[0] = r.max[0];
      if (r.min[1] < min[1])
        min[1] = r.min[1];
      if (r.max[1] > max[1])
        max[1] = r.max[1];
    }

    bool intersect(const Vec2<T> &pt) const {
      return ((pt[0] >= min[0]) && (pt[1] >= min[1]) && (pt[0] <= max[0]) && (pt[1] <= max[1]));
    }

    bool intersect(const Box2<T> &bb) const {
      return ((bb.max[0] >= min[0]) && (bb.min[0] <= max[0]) && (bb.max[1] >= min[1]) && (bb.min[1] <= max[1]));
    }

    /**
     * Computes the intersection between this box and the parameter one.
     * @param other The box to intersect.
     * @return the intersection result.
     */
    Box2<T> operator &(const Box2<T>& other) const {
      // Intersection minimum = maximum(this.min,other.min)
      Vec2<T> imin, imax;
      imin[0] = min[0] > other.min[0] ? min[0] : other.min[0];
      imin[1] = min[1] > other.min[1] ? min[1] : other.min[1];

      // Intersection maximum = minimum(this.max,other.max)
      imax[0] = max[0] < other.max[0] ? max[0] : other.max[0];
      imax[1] = max[1] < other.max[1] ? max[1] : other.max[1];

      return Box2<T> (imin, imax);
    }

    /**
     * Intersects this box and the parameter one and affect the result to this one.
     * @param other The box to intersect.
     * @return A reference to this box after modification.
     */
    Box2<T>& operator &=(const Box2<T>& other) {
      *this = *this & other;
      return (*this);
    }

    /**
     * Computes the union between this box and the parameter one.
     * @param other The box to unite.
     * @return the union result.
     */
    Box2<T> operator |(const Box2<T>& other) const {
      // Union minimum = minimum(this.min,other.min)
      Vec2<T> umin, umax;
      umin[0] = min[0] < other.min[0] ? min[0] : other.min[0];
      umin[1] = min[1] < other.min[1] ? min[1] : other.min[1];

      // Union maximum = maximum(this.max,other.max)
      umax[0] = max[0] > other.max[0] ? max[0] : other.max[0];
      umax[1] = max[1] > other.max[1] ? max[1] : other.max[1];

      return Box2<T> (umin, umax);
    }

    /**
     * Unites this box and the parameter one and affect the result to this one.
     * @param other The box to unite.
     * @return A reference to this box after modification.
     */
    Box2<T>& operator |=(const Box2<T>& other) {
      *this = *this | other;
      return (*this);
    }

    void setBounds(T xmin, T ymin, T xmax, T ymax) {
      min.setValue(xmin, ymin);
      max.setValue(xmax, ymax);
    }

    void setBounds(const Vec2<T> &_min, const Vec2<T> &_max) {
      min = _min;
      max = _max;
    }

    void getBounds(T &xmin, T &ymin, T &xmax, T &ymax) const {
      min.getValue(xmin, ymin);
      max.getValue(xmax, ymax);
    }

    void getBounds(Vec2<T> &_min, Vec2<T> &_max) const {
      _min = min;
      _max = max;
    }

    Vec2<T> getClosestPoint(const Vec2<T> &point) {
      Vec2<T> result;

      // trivial cases first
      if (isEmpty())
        return point;
      else if (point == getCenter()) {
        // middle of x side
        result[0] = max[0];
        result[1] = (max[1] + min[1]) / 2.0;
      } else if (min[0] == max[0]) {
        result[0] = min[0];
        result[1] = point[1];
      } else if (min[1] == max[1]) {
        result[0] = point[0];
        result[1] = min[1];
      } else {
        // Find the closest point on a unit box (from -1 to 1),
        // then scale up.

        // Find the vector from center to the point, then scale
        // to a unit box.
        Vec2<T>& vec = point - getCenter();
        T sizeX, sizeY;
        getSize(sizeX, sizeY);
        T halfX = sizeX / 2.0;
        T halfY = sizeY / 2.0;
        if (halfX > 0.0)
          vec[0] /= halfX;
        if (halfY > 0.0)
          vec[1] /= halfY;

        // Side to snap to has greatest magnitude in the vector.
        T magX = fabs(vec[0]);
        T magY = fabs(vec[1]);

        if (magX > magY) {
          result[0] = (vec[0] > 0) ? 1.0 : -1.0;
          if (magY > 1.0)
            magY = 1.0;
          result[1] = (vec[1] > 0) ? magY : -magY;
        } else if (magY > magX) {
          if (magX > 1.0)
            magX = 1.0;
          result[0] = (vec[0] > 0) ? magX : -magX;
          result[1] = (vec[1] > 0) ? 1.0 : -1.0;
        } else {
          // must be one of the corners
          result[0] = (vec[0] > 0) ? 1.0 : -1.0;
          result[1] = (vec[1] > 0) ? 1.0 : -1.0;
        }

        // scale back the result and move it to the center of the box
        result[0] *= halfX;
        result[1] *= halfY;
        result += getCenter();
      }

      return result;
    }

    void getOrigin(T &originX, T &originY) const {
      originX = min[0];
      originY = min[1];
    }

    void getSize(T &sizeX, T &sizeY) const {
      sizeX = max[0] - min[0];
      sizeY = max[1] - min[1];
    }

    T getAspectRatio() const {
      return (max[0] - min[0]) / (max[1] - min[1]);
    }

    void makeEmpty() {
      min.setValue(10, 10);
      max.setValue(-10, -10);
    }

    bool isEmpty() const {
      return max[0] < min[0];
    }

    bool hasArea() const {
      return (max[0] > min[0] && max[1] > min[1]);
    }

    Box3<T> convert(T vmin = 0, T vmax = 0) const {
      Box3<T> result(min[0], min[1], vmin, max[0], max[1], vmax);
      return result;
    }

    bool isInside(const Box2<T> outsider) {
      return ((outsider.getXMin() <= getXMin()) && (outsider.getYMin() <= getYMin()) && (outsider.getXMax() >= getXMax()) && (outsider.getYMax() >= getYMax()));
    }

    friend int operator ==(const Box2<T> &b1, const Box2<T> &b2) {
      return ((b1.min == b2.min) && (b1.max == b2.max));
    }

    friend int operator !=(const Box2<T> &b1, const Box2<T> &b2) {
      return !(b1 == b2);
    }


    /**
     * Equality comparison within given tolerance
     * @param v the box to compare with
     * @param tolerance the tolerance to apply to this comparison
     * @return true if (this-v).lenght()<tolerance
     */
    bool equals(const Box2<T>& v, T tolerance) const {
      return (this->min.equals(v.min, tolerance) && this->max.equals(v.max, tolerance));
    }

  private:

    Vec2<T> min, max;
};

/**
  * Convenience operator to output a box to a stl stream
  * @param s the stream to write to.
  * @param v the box to be written.
  */
 template<typename T>
 std::ostream & operator<<(std::ostream& s, const Box2<T>& v) {
   s << v.getMin() << " " << v.getMax();
   return s;
 }

#endif
