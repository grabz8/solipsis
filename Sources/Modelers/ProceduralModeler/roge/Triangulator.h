/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

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

#ifndef __TRIANGULATOR_H__
#define __TRIANGULATOR_H__ 1

extern "C" {
#define REAL double
#define ANSI_DECLARATORS 1
#define VOID void
#include "triangle.h"
}

#include "Config.h"
#include <vector>
using namespace std;

#include "Dll.h"

#include "Linear/Linear.h"

namespace roge {

class /* _DLL */ Triangulator
{
  
public:

  /**
   * Constructor
   * @param externPolyline The extern polyline to triangulate inside of
   * @param internPolylines The intern polylines to triangulate outside of
   * @param terrain The terrain to triangulate with
   */
  Triangulator( const vector<Vec3d>& externPolyline);


  /**
   * Destructor
   */
  ~Triangulator();

  /**
   * Triangulation method using the Triangle library.
   * Fills the members above.
   */
  void compute();

  /**
   * Returns a list of vertices resulting of the triangulation
   */
  vector<Vec3d> getVertices() { return vertices; };
  
  /**
   * Returns a list of indexes in the list of vertices giving the structure
   * of triangles
   */ 
  vector<Vec3i> getTriangles() { return triangles; };
  
 protected:
  
  void initStruct(struct triangulateio& tio);
  void freeStruct(struct triangulateio& tio);
  
  vector<Vec3d> vertices;
  vector<Vec3i> triangles;
  
  vector<Vec3d> externPolyline;

};

}; // of namespace roge

#endif
