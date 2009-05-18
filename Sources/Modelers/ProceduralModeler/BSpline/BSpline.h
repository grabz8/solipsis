/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#ifndef _BSPLINE_H_
#define _BSPLINE_H_

#include <stdio.h>

#include <vector>

#include <Linear/Vec3.h>

using namespace::std;

class BSpline {
  
public:
  
  // contructors
  BSpline();
  BSpline(const vector<Vec3<double>  >&, double, bool);

  // destructor
  ~BSpline();

  const char *getClassName(){ return "BSpline"; };

  // Description:
  // Compute the coefficients for the spline.
  vector<Vec3<double>  > *compute();

  // Description:
  // Set/Get all or control points to fit with the spline
  void setControlPoints(const vector<Vec3<double>  >&);
  vector<Vec3<double>  > *getControlPoints();
  vector<Vec3<double>  > *getAllPoints();
  int getControlPointNum();
  int getAllPointNum();
  Vec3<double>  *getAllPoint(int);
  Vec3<double>  *getControlPoint(int);

  // Description:
  // Add/Remove a control point to the vector of control points
  void addControlPoint(const Vec3<double>  &);
//	 void replaceControlPoint(int, T&);
  bool removeControlPoint(const Vec3<double> &);
//   bool removeControlPoints();

  // Description:
  // Control the spline smoothness
  // 0.0 < value <= 1.0 (0.0 : very smooth, 1.0 :very sharp) 
  void setSmoothness(double);
  double getSmoothness();  

  // Description:
  // Control whether the spline is open or closed. A closed spline forms
  // a continous loop: the first and last points are the same, and
  // derivatives are continuous.
  // true if closed 
  void setClosed(bool);
  bool getClosed();

  // Description:
  // Optional
  // Set/Get the image contouring value (if image contouring)
  void setValue(int);
  int getValue();

  // Description
  // Optional
  // Set/Get color of spline (associated with above value)
  void setColor(const Vec3<double> &);
  Vec3<double>  getColor(); 

  // Description
  // Optional
  // Set/Get section of spline (associated with above value)
  void setSection(double);
  double getSection(); 

  // Description
  // Optional
  // Continuity at final points (open spline)
  void setThroughEndPoints(bool);
  bool getThroughEndPoints();
  
  // Description
  // Clear vectors of points
  void clearAll();

  // Description
  // Length of the spline
  double length();

private:
  
  Vec3<double>  color;            // color of the spline
  int value;                      // image contouring value
  double section;                 // contouring section
  bool closed;                    // spline closed or not 
  double smoothness;              // smoothness between two control points
  bool throughEndPoints;          // continuity at final points (open spline)

  vector<Vec3<double>  > *controlPointVec; 
  vector<Vec3<double>  > *calculControlPointVec;
  vector<Vec3<double>  > *allPointVec;
  
  // Description:
  // Prepare the control points of the spline
  void prepareControlPoints();

};

#endif

