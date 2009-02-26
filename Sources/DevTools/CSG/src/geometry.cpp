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

// Geometric data for topological entities such as vertices, edges, ...

#include <math.h>

#include "geometry.h"
#include "vector.h"
#include "error.h"


// ************************* tolerances ***********************************

// Minimum (relative) vertex, edge and face tolerance
double vertex_tolerance = 1e-6;
double edge_tolerance = 1e-12;
double face_tolerance = 1e-12;

void SetMinimumVertexTolerance(double tol)
{
	vertex_tolerance = tol;
}

void SetMinimumEdgeTolerance(double tol)
{
	edge_tolerance = tol;
}

void SetMinimumFaceTolerance(double tol)
{
	face_tolerance = tol;
}


// ********************** Vertex functions *******************************

// Vertex comparison flags:
//
// The following data may be taken into consideration when comparing vertices:
// - vertex location: normally always specified.
// - vertex normal: (0,0,0) if not specified, normalized vector if specified.
// What data should be taken into account when comparing vertices is
// controlled with the vertex compare flags VCMP_LOCATION and VCMP_NORMAL.
//
// The order in which the data is taken into account by VertexCompare is
// as follows: first location is tested, than normals, than name (if relevant).
//
// Default vertex compare flags: vertex location, normal and name are
// all taken into account when comparing vertices. If two vertices to be
// compared have the same location, and the same normal, but both of them
// have no name the vertices are to be considered equal

// Compares two vertices and returns a code useful for ordering vertices
// into an octree. Returns 8 if the two vertices are to be considered
// the same vertices and 0-7, the index of an octree branch to be expored
// further, if not. This function is needed in order to sort created
// vertices into an octree for efficient later lookup. For each
// solid, a vertex octree is created at the time the solid is created
int VertexCompare(BREP_VERTEX* brepv1, BREP_VERTEX* brepv2)
{
	VERTEX* v1 = (VERTEX*)brepv1;
	VERTEX* v2 = (VERTEX*)brepv2;

	// Same coordinates, test the normals
	int code = 0;

	if (v1->normal.x > v2->normal.x + EPSILON) code += 1;
	if (v1->normal.y > v2->normal.y + EPSILON) code += 2;
	if (v1->normal.z > v2->normal.z + EPSILON) code += 4;
	if(code != 0)
		// x1 > x2 || y1 > y2 || z1 > z2
		return code;

	if(v1->normal.x < v2->normal.x - EPSILON ||
		v1->normal.y < v2->normal.y - EPSILON ||
		v1->normal.z < v2->normal.z - EPSILON)
		// Not the same normal
		return code;

	if(v1->normal.x == 0 && v1->normal.y == 0 && v1->normal.z == 0)
		// No valid normal => not the same vertex
		return 0;

	return code;
}

// This routine only compares the location of the two vertices.
// There may be multiple vertices at the same location,
// e.g. having a different normal and/or name. These vertices
// are considered different vertices by the vertex compare routine,
// but they are considered the same vertices by this one.
int VertexCompareLocation(BREP_VERTEX* brepv1, BREP_VERTEX* brepv2)
{
	VERTEX* v1 = (VERTEX*)brepv1;
	VERTEX* v2 = (VERTEX*)brepv2;

	// Two entities intersect if their tolerance region intersects, i.o.w. if
	// the distance between them is smaller than the sum of the two entity's
	// tolerances
	double tolerance = v1->tolerance + v2->tolerance;

	int code = 0;
	if(v1->point.x > v2->point.x + tolerance) code += 1;
	if(v1->point.y > v2->point.y + tolerance) code += 2;
	if(v1->point.z > v2->point.z + tolerance) code += 4;
	if(code != 0)
		// x1 > x2 || y1 > y2 || z1 > z2
		return code;

	if(v1->point.x < v2->point.x - tolerance ||
		v1->point.y < v2->point.y - tolerance ||
		v1->point.z < v2->point.z - tolerance)
		// Not the same coordinates
		return code;

	return 8;
}


// Computes a normals for the vertex if it doesn't have a normal yet
// unless normals in the input should be ignored
void VERTEX::ComputeNormal()
{
	// Compute a normal for the vertex:
	// average normal of the faces sharing the vertex
	normal = VECTOR(0, 0, 0);

	BREP_WING_RING::iterator wingiterator;
	for(wingiterator = wing_ring.begin(); wingiterator != wing_ring.end(); wingiterator++)
		{
			BREP_WING* wing = *wingiterator;
			normal = normal + ((FACE *)(wing->contour->face))->normal;
		}

	normal.Normalize();
}

void VERTEX::ComputeTolerance()
{
	double max = fabs(point.x);
	double m = fabs(point.y);

	if(m > max)
		max = m;

	m = fabs(point.z);
	if(m > max)
		max = m;

	tolerance = vertex_tolerance * max;
}

// Enlarges the tolerance region of a vertex. Probably also a good
// place to merge the vertex with other approximately coinciding vertices
void VERTEX::EnlargeTolerance(double tol)
{
	if(tol > tolerance)
		tolerance = tol;
}


// ************************ Edge functions ****************************

BREP_EDGE* CreateEdge(BREP_VERTEX* v1, BREP_VERTEX* v2)
{
	EDGE* edge = new EDGE(v1, v2);
	edge->DataCloseCallback();
	return edge;
}

void EDGE::DataCloseCallback(const bool done)
{
	// Compute tolerance, extent and near as in M. Segal, SIGGRAPH '90 p 105
	extent = ((VERTEX*)(wing1->vertex))->tolerance;
	double e = ((VERTEX*)(wing2->vertex))->tolerance;
	if(e > extent)
		extent = e;

	near_ = HUGE;
}


// ********************** Contour functions ******************************


BREP_CONTOUR* CreateContour(BREP_FACE* face)
{
	CONTOUR* contour = new CONTOUR(face);
	//contour->DataCloseCallback();
	return contour;
}


bool CONTOUR::IsHole()
{
	FACE* f = (FACE*)(face);
	return (f->normal * normal) <= 0;
}

// Newells method for computing the normal on the plane defined by the contour.
// Ref: F. Tampieri, Graphics Gems III p 231
void CONTOUR::ComputeNormal()
{
	normal = VECTOR(0, 0, 0);

	VECTOR next = ((VERTEX*)(wings->vertex))->point;
	for(BREP_WING* wing = wings; wing; wing = (wing->next == wings ? NULL : wing->next))
		{
			VECTOR cur = next;
			next = ((VERTEX *)(wing->next->vertex))->point;

			normal.x += (cur.y - next.y) * (cur.z + next.z);
			normal.y += (cur.z - next.z) * (cur.x + next.x);
			normal.z += (cur.x - next.x) * (cur.y + next.y);
		}

	normal.Normalize();
}


// ************************** Face functions *****************************

// For computing the plane equation of a face with Newells method,
// including face tolerance, see
// F. Tampieri, Graphics Gems III p 231
// M. Segal, IEEE CG&A Jan. 1988 p 53
static FACE* face_data;
static double dmin, dmax, emax;

static void do_close_face_vertex(BREP_VERTEX *vertex)
{
  VERTEX *vertex_data = (VERTEX*)(vertex);

	double d = -(face_data->normal * vertex_data->point);
	if(d < dmin) dmin = d;
	if(d > dmax) dmax = d;

	if(vertex_data->tolerance > emax)
		emax = vertex_data->tolerance;

	face_data->bounds.Enlarge(vertex_data->point);
}

// Computes the plane equation of the face using Newells method
void FACE::ComputePlaneEquation()
{
	face_data = this;
	normal = ((CONTOUR*)(outer_contour.front()))->normal;

	// Maximum vertex tolerance
	emax = 0;
	// Minimum plane constant
	dmin = HUGE;
	// Maximum plane constant
	dmax = -HUGE;

	BREP_CONTOUR_RING::iterator contour;
	for(contour = outer_contour.begin(); contour != outer_contour.end(); contour++)
		(*contour)->IterateVertices(do_close_face_vertex);

	d = (dmin + dmax)/2.;
	tolerance = std::max((dmax - dmin)/2., face_tolerance);
	extent = face_data->tolerance + emax;
	near_ = HUGE;

	bounds.MIN_X -= extent;
	bounds.MIN_Y -= extent;
	bounds.MIN_Z -= extent;
	bounds.MAX_X += extent;
	bounds.MAX_Y += extent;
	bounds.MAX_Z += extent;
}

// Returns the signed distance from the vertex with given
// geometric data to the face with given geometric data.
// Fills in on what side (+1 or -1) of the plane of the face
// the vertex lies. If the vertex lies within the extent
// region of the plane (i.e. the distance to the plane is
// less than the extent of the plane), 0 is filled in in side
double FACE::SignedDistanceToPlane(VERTEX* v, int* side)
{
	double dist = (normal * v->point) + d;

	if(dist < -extent)
		*side = -1;
	else if(dist > extent)
		*side = +1;
	else
		*side = 0;

	return dist;
}

// v0->v1 and v1->v2 are two edges of the boundary of the face with given
// data. This routine returns TRUE if the corner at vertex v1 is a convex corner,
// and returns FALSE if it is a concave corner. Note that our conventions
// about edge directions are different than in Segals papers. In our case, the
// inside of a face is always on the left side of the boundary. Same situation
// for outer and inner contours
bool FACE::ConvexCorner(VERTEX* v0, VERTEX* v1, VERTEX* v2)
{
	VECTOR e1 = v1->point - v0->point;
	VECTOR e2 = v2->point - v1->point;
	VECTOR x = e1 ^ e2;
	return (x * normal) > 0;
}

// ShortestEdgeLength
static double minlen;

static void TestDecreaseEdgeLength(BREP_WING* wing)
{
	BREP_VERTEX* v1 = wing->vertex;
	BREP_VERTEX* v2 = wing->next->vertex;
	VECTOR p1 = ((VERTEX*)(v1))->point;
	VECTOR p2 = ((VERTEX*)(v2))->point;

	double len = VectorDist(p1, p2);
	if(len < minlen)
		minlen = len;
}

double FACE::ShortestEdgeLength()
{
	minlen = HUGE;
	outer_contour.front()->IterateWings(TestDecreaseEdgeLength);

	return minlen;
}



// ********************* Shell functions *********************************

void SHELL::DataCloseCallback(const bool done)
{
	// CloseCallback()
	if(!done)
		CloseCallback();

	BREP_FACE_RING::iterator face;
	for(face = faces.begin(); face != faces.end(); face++)
		bounds.Enlarge(((FACE*)(*face))->bounds);
}


// *********************** Solid functions ***************************

void SOLID::DataCloseCallback(const bool done)
{
	//CloseCallback();
	BREP_SHELL_RING::iterator shell;
	for(shell = shells.begin(); shell != shells.end(); shell++)
		((SHELL*)(*shell))->DataCloseCallback();

	for(shell = shells.begin(); shell != shells.end(); shell++)
		bounds.Enlarge(((SHELL*)(*shell))->bounds);
}


