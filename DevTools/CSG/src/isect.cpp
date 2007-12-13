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

// isect: partitions pairs of faces into nonintersecting parts
//
// References:
//
// - Marc Segal and Carlo H. Sequin, "Partitioning Polyhedral Objects into
//      Nonintersecting Parts", IEEE Computer graphics and Applications,
//	Januari 1988 p 53
// - Marc Segal, "Using Tolerances to Guarantee Valid Polyhedral Modeling
//	Results", SIGGRAPH '90, p 105
//
//
// Known bugs:
//
// - endpoint determination may fail for small features, e.g. edges
//	with length of about the same order of magnitude as the vertex
//	tolerances
//
// In case of bugs: recompile this file with the symbol CHECK defined.
// Face pairs causing warnings or errors are dumped.
//
//
// Still to do:
//
// - feature merging (Segal, SIGGRAPH '90)
//
//
// Author:
//
//	Philippe Bekaert, Computer Graphics Research Group K.U.Leuven
//	Philippe.Bekaert@cs.kuleuven.ac.be
//	Aug, 29 1996
//
//
// Changelog:
//
// Sun Sep 22 23:23:37 1996  Philippe Bekaert  <philippe@strojka.cs.kuleuven.ac.be>
//
//      * non-coplanar faces containing contours that lie entirely in the
//	transversal face may cause problems. Solved??
//
// Wed Oct  9 18:10:11 1996  Philippe Bekaert  <philippe@flater.cs.kuleuven.ac.be>
//
//	* inside/outside marking of contours corrected.

#include <vector>
#include <algorithm>

#include "isect.h"
#include "geometry.h"
#include "error.h"
#include "vector.h"


// Arbitrary, but not -1, 0 or 1
const int UNKNOWN_SIDE	= 10;

// Information about intersection points of two faces:

// Endpoint types: to be combined with |

// no faces need to be cut, only for merging
// with very nearby intersection points
const unsigned int VERTEX_ENDPOINT =	1;
// Only the "other" face needs to be cut
const unsigned int PHANTOM_ENDPOINT =	2;
// Both faces need to be partitioned
const unsigned int EXIT_ENDPOINT =	4;
const unsigned int ENTRY_ENDPOINT =	8;
// Deleted endpoints after merging
const unsigned int DELETED_ENDPOINT =	16;

class ENDPOINTstruct
{
public:

	// Returns +1 if p1 comes after p2, -1 if p1 preceeds p2
	// and 0 if they are exactly at the same position
	bool operator < (const ENDPOINTstruct& p2) const
	{
		return (ti < p2.ti);
	}


	// Parameter of endpoint on intersection line
	double ti;
	// Parameter of endpoint on contributing edge
	double te;
	// Intersection point coordinate tolerance
	double tol;
	// ENTRY_ENDPOINT or EXIT_ENDPOINT or ...
	unsigned int type;
	// Wing containing the intersection point
	BREP_WING* wing;

	// For debugging: source code line identifying the
	// situation how the endpoints was contributed
	int source;
};

// The endpoint list
std::vector<ENDPOINTstruct> endpoints;
typedef std::vector<ENDPOINTstruct>::iterator ENDPOINT;
const ENDPOINT NULLENDPOINT = endpoints.end();


// Intersection line data
class ISECT_LINE
{
public:
	// Parametric line equation is t.U + V, U normalized
	VECTOR U, V;

	// Tolerance
	 double tol;
};

static ISECT_LINE isect;


// Global parameters needed during partioning

// Pair of intersecting faces being processed
static FACE *faceA, *faceB;

// Wings leaving at the exit endpoint of
// the previous cutting interval
static BREP_WING *lastwingA, *lastwingB;

// Exit endpoint parameter on the intersecting
// line of the previous cutting interval
static double lastti;

// Minimum and maximum parameter along intersection
// line of endpoints contributed by faceA and faceB
static double timinA, timaxA, timinB, timaxB;

// See Segal, SIGGRAPH '90 p 111. The coplanarity factor indicates to what
// extent two faces are coplanar. Coplanar faces have a large coplanarity
// factor. For perpendicular faces, the coplanarity factor is sqrt(2). IN
// order to decide whether two faces are to be considered coplanar, their
// coplanarity factor is compared with a minimum coplanarity factor, which
// should be a number between 100 and 1000. Smaller values force the
// algorithm to deem pairs of faces that intersect in a relatively large
// dehedral angle to be coplanar
const double MIN_COPLANARITY_FACTOR = 500;
double min_coplanarity_factor = MIN_COPLANARITY_FACTOR;

// To set the minimum allowed coplanarity factor
void SetMinCoplanarityFactor(double factor)
{
	min_coplanarity_factor = factor;
}

// When CHECK is defined, pairs of faces that cause an error or warning
// are dumped, so the situation can be examined later on
#ifdef CHECK

#include <stdio.h>
#include <stdarg.h>

static int check = false;

static void IsectWarning(char *routine, char *text, ...)
{
  va_list pvar;

  fprintf(stderr, "Warning");
  if (routine) fprintf(stderr, " (in subroutine %s)", routine);
  fprintf(stderr, ": ");

  va_start(pvar, text);
  vfprintf(stderr, text, pvar);
  va_end(pvar);

  fprintf(stderr, ".\n");
}

#define Warning IsectWarning
#define Error IsectWarning

#endif // CHECK


// Computes the coplanarity factor of two planes
static double CoplanarityFactor(FACE *f1, FACE *f2)
{
	VECTOR normal = f1->normal ^ f2->normal;
	double s = normal.Norm();

	if(s < EPSILON)
		return HUGE;

	double c = 1 - s*s;
	if(c > 0)
		c = sqrt(c);
	else
		c = 0;

	double tol = sqrt(f1->extent * f1->extent + f2->extent * f2->extent + 2 * f1->extent * f2->extent * c) / s;

	return tol / std::max(f1->extent, f2->extent);
}

// Computes the parametric equation P = V + tU of the
// intersection line of two non-parallel (face-)planes.
// Ref: Priamos Georgiades, "Plane-to-Plane Intersection",
// Graphics Gems III p 233
static ISECT_LINE ComputePlaneToPlaneIntersection(FACE* f1, FACE* f2)
{
	ISECT_LINE isect;
	isect.U = f1->normal ^ f2->normal;

	double s = (isect.U).Norm();
	double c = 1 - s*s;
	c = (c > 0 ? sqrt(c) : 0);
	isect.tol = sqrt(f1->extent * f1->extent + f2->extent * f2->extent + 2 * f1->extent * f2->extent * c) / s;
	double coplaf = isect.tol / std::max(f1->extent, f2->extent);
	if(coplaf > min_coplanarity_factor)
		{
			char text[BREP_MAX_MESSAGE_LENGTH];
			sprintf(text, "attempt to compute intersection line of parallel planes (coplanarity factor = %g, min. allowed = %g)", coplaf, min_coplanarity_factor);
			Warning("ComputePlaneToPlaneIntersection", text);
		}

	double maxabs = fabs(isect.U.x);
	int index = XNORMAL;
	double abs;
	if((abs = fabs(isect.U.y)) > maxabs) {maxabs = abs; index = YNORMAL;}
	if((abs = fabs(isect.U.z)) > maxabs) {maxabs = abs; index = ZNORMAL;}

  switch (index) {
  case XNORMAL:
    isect.V.x = 0;
    isect.V.y = (f1->normal.z * f2->d - f2->normal.z * f1->d) / isect.U.x;
    isect.V.z = (f2->normal.y * f1->d - f1->normal.y * f2->d) / isect.U.x;
    break;
  case YNORMAL:
    isect.V.y = 0;
    isect.V.z = (f1->normal.x * f2->d - f2->normal.x * f1->d) / isect.U.y;
    isect.V.x = (f2->normal.z * f1->d - f1->normal.z * f2->d) / isect.U.y;
    break;
  case ZNORMAL:
    isect.V.z = 0;
    isect.V.x = (f1->normal.y * f2->d - f2->normal.y * f1->d) / isect.U.z;
    isect.V.y = (f2->normal.x * f1->d - f1->normal.x * f2->d) / isect.U.z;
    break;
  default:
    Fatal(2, "PlaneToPlaneIntersection", "something impossible wrong");
  }

	(isect.U).InverseScale(s);

	return isect;
}


// Returns TRUE if the vertex approximately is on the intersection line.
// See Segal, SIGGRAPH '90 p 111
static int VertexOnIntersectionLine(VERTEX* v, double dist)
{
	if(fabs(dist) < v->tolerance + isect.tol)
		return true;
	else
		return false;

#ifdef NEVER
  double d, e;
  VECTOR x;

  // Numerically unstable: sometimes, about equal large numbers are subtracted
	x = v->point - isect.V;
  d = (isect.U * x);
  d *= -d;
  d += (x * x);
  e = isect.tol + v->tolerance;
  e *= e;
  return (d <= e);
#endif
}

// Computes the distance of the vertex to the plane of the face in *dist.
// Returns 0 if the vertex approximately lies on the intersection line
// of the two intersection faces being run and -1 or +1, the side w.r.t.
// the plane if not
static int VertexToFaceIntersection(BREP_VERTEX *v, FACE *f, double *dist)
{
	int side;
	*dist = f->SignedDistanceToPlane((VERTEX*)v, &side);

	if(VertexOnIntersectionLine((VERTEX*)v, *dist))
		side = 0;

	return side;
}

// Computes the parameter ti of an intersection point P = v1 + te.(v2-v1),
// on the intersection line ti.U + V of the planes of two intersecting faces
static double ProjectIntersection(VERTEX *v1, VERTEX *v2, double te)
{
	VECTOR x = v2->point - v1->point;
	x = v1->point + te*x;
	x = x - isect.V;
	return x * isect.U;
}

// Same as above, except that the intersection is at a vertex
static double ProjectVertexIntersection(VERTEX* v)
{
	VECTOR x = v->point - isect.V;
	return x * isect.U;
}

// The edge from v1 to v2 belonging to the face intersects another face. The
// intersection line of the face planes has direction vector U. This function
// returns ENTRY_ENDPOINT if the intersection point will be an entry endpoint
// of a cutting interval or EXIT_ENDPOINT if it will be an exit endpoint.
// It relies on the convention that the interior of a face is on the
// left side of each contour bounding the face, or: (v2-v1) x n, with v1 and v2
// the endpoints of a wing and n the normal to the face plane, points to the
// interior of the face. The intersection point will be an entry endpoint if
// this vector and U point to the same direction. It will be an exit endpoint
// if they point into opposite directions
static int TestEntryExit(VERTEX* v1, VERTEX* v2, FACE* f)
{
	VECTOR x = v2->point - v1->point;
	x = f->normal ^ x;

	return ((x * isect.U) > 0 ? ENTRY_ENDPOINT : EXIT_ENDPOINT);
}


// v0->v1 and v1->v2 are two edges along the intersection line of the planes
// of two faces. This function returns TRUE if the edges form a notch and
// FALSE if not. The two edges form a notch if they point into opposite
// direction. If the edges form a notch, v1 is an endpoint of a cutting interval
// along which the face containing the edges will need to be partitioned.
// In that caase, it is determined whether the endpoint is an entry or
// exit endpoint in the contributing face: v1 is
// an entry point if the edge v1->v2 points into opposite direction of the
// unit direction vector U of the intersection line. If v1 is an entry endpoint,
// ENTRY_ENDPOINT is filled in in type. EXIT_ENDPOINT if it is an exit endpoint.
// The value of type is unaltered if the edges do not form a notch, in which
// case v1 is not an endpoint
static int IsNotch(VERTEX* v0, VERTEX* v1, VERTEX* v2, int* type)
{
	VECTOR e1 = v1->point - v0->point;
	VECTOR e2 = v2->point - v1->point;

	if((e1 * e2) > 0)
		return false;

	*type = ((e2 * isect.U) < 0 ? ENTRY_ENDPOINT : EXIT_ENDPOINT);
	return true;
}


// Adds endpoint information to the list of endpoints
static void InsertEndpoint(double ti, double t, int type, BREP_WING* wing, int source)
{
	ENDPOINTstruct newendpoint;

	newendpoint.ti = ti;
	newendpoint.te = t;
	newendpoint.type = type;
	newendpoint.source = source;
	newendpoint.wing = wing;

	// See Segal SIGGRAPH '90 p 111
	newendpoint.tol = isect.tol;

	if(wing->contour->face == faceA)
		{
			if(ti < timinA) timinA = ti;
			if(ti > timaxA) timaxA = ti;
		}
	else
		{
			if(ti < timinB) timinB = ti;
			if(ti > timaxB) timaxB = ti;
		}

	endpoints.push_back(newendpoint);
}

// Wing is a "phantom" wing or a wing that results after splitting a contour.
// This routine will mark the contour to which wing belongs and the contour
// to which the wing in the reverse direction belongs whether they are
// inside or outside the other solid. This information is needed to do
// inside/outside classification for CSG operations
static void MarkContours(BREP_WING* wing)
{
	FACE* main_face = (FACE*)wing->contour->face;
	FACE* transversal_face = (main_face == faceA ? faceB : faceA);

	VECTOR p1 = ((VERTEX*)(wing->vertex))->point;
	VECTOR p2 = ((VERTEX*)(wing->next->vertex))->point;
	VECTOR u = p2 - p1;
	VECTOR nm = ((FACE*)main_face)->normal;
	// Nm = ((CONTOUR *)(wing->contour->client_data))->normal;
	VECTOR s = nm ^ u;
	VECTOR nt = ((FACE*)transversal_face)->normal;

	unsigned int mark;
	unsigned int omark;
	if((s * nt) > 0)
		{
			mark = OUTMASK;
			omark = INMASK;
		}
	else
		{
			mark = INMASK;
			omark = OUTMASK;
		}

	((CONTOUR*)(wing->contour))->SetInOut(mark);

	if(BrepEdgeOtherWing(wing)->contour)
		{
			CONTOUR* ocontour_data = (CONTOUR*)(BrepEdgeOtherWing(wing)->contour);

//			if(!(ocontour_data->InOut()))
//				{
//					ocontour_data->SetInOut(omark);
//				}
//			else if ((ocontour_data->InOut() != omark)
//      Warning("MarkContour", "contour %d: inconsistent inside/outside determination", ((CONTOUR*)(wing->contour))->EntityId());

			ocontour_data->SetInOut(omark);
		}
}

// in/out-marks the contour to which wing belongs
// if it has not been marked before
static void ConditionallyMarkContours(BREP_WING *wing)
{
	if(((CONTOUR*)(wing->contour))->InOut() == 0)
		MarkContours(wing);
}

// Test each wing of the main face, starting with first_wing, for intersection with
// the transversal face. Most of the intersection points will be an endpoint of a
// cutting interval, along which the faces will need to be partitioned. Some
// however will not. Record the endpoints with information which wing contributed
// it and whether it is an entry or exit point on the contributing face.
// "first_wing" is known to be the first wing contributing an intersection point
// (SMALLER INDENTATION HERE DUE TO LOSTS OF NESTINGS)
static void DetermineEndpoints(FACE* main_face, BREP_WING* first_wing, FACE *transversal_face)
{
	// Start over looking for intersection from
	// the first wing of the first contour on
	// (Removed START_OVER #define/#endif from original BREP Library (Romain Behar)
	BREP_CONTOUR_RING::iterator contour;
	for(contour = main_face->outer_contour.begin(); contour != main_face->outer_contour.end(); contour++)
	{

		BREP_WING* wing = (*contour)->wings;

		double d2;
		int side2 = VertexToFaceIntersection(wing->vertex, transversal_face, &d2);

		// An impossible value indicating that we don't know
		// on what side of the transversal plane the starting
		// point of the previous wing lies --- this is only
		// needed in rare cases
		int side1 = UNKNOWN_SIDE;

		// Not in a phtanom interval
		bool firstphantom = false;
		bool inphantom = false;

		do
		{
			int side0 = side1;
			double d1 = d2;
			side1 = side2;
			side2 = VertexToFaceIntersection(wing->next->vertex, transversal_face, &d2);

			int type;
			double te, ti;

			if(side1 != side2 && (side1+side2) == 0)
			{
				// Intersection halfway the line segment
				// determine parameter of intersection point
				// on the intersection line of the main and
				// transversal face
				te = fabs(d1) / (fabs(d1) + fabs(d2));
				ti = ProjectIntersection((VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex), te);
				type = TestEntryExit((VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex), main_face);
				// Always an endpoint
				InsertEndpoint(ti, te, type, wing,  __LINE__);

			}
			else if(side1 == 0)
			{
				// Starting point lies in the
				// transversal plane
				te = 0;
				ti = ProjectVertexIntersection((VERTEX*)(wing->vertex));

				// We need to know whether the starting
				// vertex of the previous wing also
				// lies in the transversal plane or not
				if(side0 == UNKNOWN_SIDE)
				{
					double d0;
					side0 = VertexToFaceIntersection(wing->prev->vertex, transversal_face, &d0);

					// If the last wing lies inside the transversal plane,
					// determine whether a previous vertex has contributed
					// a phantom endpoint or not. If this "previous" vertex
					// contributes a phantom endpoint, it will be generated
					// later, when the vertex will be processed
					if(side0 == 0)
					{
						int prevside0, prevside1, prevside2, prevtype;
						double prevd0, prevd1, prevd2;
						BREP_WING* prevwing;

						// wing->vertex and wing->prev->vertex lie in the
						// transversal plane.
						// Search backward to find the last vertex that
						// is the top of a notch, or that doesn't lie in
						// the transversal plane. Normally, there is such
						// a vertex since the main face is known not to
						// be coplanar with the transversal face, and
						// contours are closed: so, either there is a
						// vertex in the contour that does not lie in the
						// transversal plane, or they all lie on the
						// intersection line, but then there must be a
						// notch (even two) somewhere. However, due to
						// the finite precision and extents, it might
						// happen that a contour of the main face is
						// judged to be coplanar with the transversal face
						prevside2 = side1;
						prevd2 = d1;

						prevside1 = side0;
						prevd1 = d0;
						prevwing = wing->prev;

						prevside0 = VertexToFaceIntersection(prevwing->prev->vertex, transversal_face, &prevd0);

						while(prevside0 == 0 && !IsNotch((VERTEX*)(prevwing->prev->vertex),
							(VERTEX*)(prevwing->vertex), (VERTEX*)(prevwing->next->vertex), &prevtype) && prevwing != wing)
						{
							prevside2 = prevside1;
							prevd2 = prevd1;

							prevside1 = prevside0;
							prevd1 = prevd0;
							prevwing = prevwing->prev;

							prevside0 = VertexToFaceIntersection(prevwing->prev->vertex, transversal_face, &prevd0);
						}

						if(prevwing == wing)
						{
							// Should not happen, but we prevent
							// infinite loops if it would happen
							Warning("DetermineEndpoints", "bad contour");
							continue;
						}

						if(prevside0 != 0)
						{
							inphantom = true;
						}
						else
						{
							// A previous vertex is the top of a notch, or the
							// the whole contour lies in the transversal plane

							// Find a vertex not lying in the transversal plane.
							while(prevside0 == 0 && prevwing != wing)
							{
								prevside2 = prevside1;
								prevd2 = prevd1;

								prevside1 = prevside0;
								prevd1 = prevd0;
								prevwing = prevwing->prev;

								prevside0 = VertexToFaceIntersection(prevwing->prev->vertex, transversal_face, &prevd0);
							}

							if(prevside0 != 0)
							{
								// We found a vertex outside the
								// transversal plane, so we have a notch
								inphantom = false;
							}
							else
							{
								// Let's see what this does...
								inphantom = true;
							}
						}

						firstphantom = inphantom;
					}
				}

				if(side0 != 0 && side2 != 0)
				{
					// Both adjacent endpoints lie outside
					// the transversal plane
					if(side0 != side2)
					{
						// The previous and next endpoint lie
						// on opposite sides
						type = TestEntryExit((VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex), main_face);
						// Always an endpoint
						InsertEndpoint(ti, te, type|VERTEX_ENDPOINT, wing, __LINE__);
					}
					else
					{
						// The previous and next endpoint
						// lie on the same side
						if(!main_face->ConvexCorner((VERTEX*)(wing->prev->vertex), (VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex)))
						{
							// If the previous and current wing make
							// a convex corner at the starting
							// vertex of the current wing, no
							// endpoints are induced. If it is a
							// concave corner, two endpoints are
							// induced: one entry and one exit,
							// at the same physical location
							InsertEndpoint(ti, te, EXIT_ENDPOINT|VERTEX_ENDPOINT, wing, __LINE__);
							InsertEndpoint(ti, te, ENTRY_ENDPOINT|VERTEX_ENDPOINT, wing, __LINE__);
						}
						else
						{
							// Not an endpoint, no partitioning of
							// the main or traversal face is
							// necessary. The intersection point is
							// stored anyways in order to merge
							// merge with other intersection points
							// that lie within tolerance form it
							InsertEndpoint(ti, te, VERTEX_ENDPOINT, wing, __LINE__);
						}
					}
				}
				else if(side0 == 0)
				{
					// Previous endpoint lies in the transversal plane
					if(side2 != 0)
					{
						// Next endpoint doesn't lie in the
						// transversal plane

						// The previous edge lies in the transversal
						// plane, the current edge not. No
						// partitioning of the main face is necessary
						// along the previous wing, but partitioning
						// of the transversal plane might be,so we
						// add a phantom endpoint, induced by the
						// previous edge, to the endpoint list
						if(inphantom)
						{
							InsertEndpoint(ti, 1, PHANTOM_ENDPOINT|VERTEX_ENDPOINT, wing->prev, __LINE__);
							inphantom = false;
						}

						if(!main_face->ConvexCorner((VERTEX*)(wing->prev->vertex), (VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex)))
						{
							// If the corner at the starting vertex
							// of the current wing is not a convex
							// corner, the starting vertex is also
							// a real endpoint: the entry of exit
							// of a cutting interval along which
							// the main face *will* need to be
							// partitioned
							type = TestEntryExit((VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex), main_face);
							InsertEndpoint(ti, te, type|VERTEX_ENDPOINT, wing, __LINE__);
						}
						else
						{
							// No endpoint needs to be inserted.
							// Not even a VERTEX endpoint

						}
					}
					else
					{
						// Both previous and next lie in
						// the transversal plane
						if(IsNotch((VERTEX*)(wing->prev->vertex), (VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex), &type))
						{
							// Two wings along the intersection
							// line, going into opposite direction:
							// a notch

							// For a normal notch, inphantom should
							// be TRUE. For a slit however,
							// inphantom may be either TRUE or FALSE
							InsertEndpoint(ti, 1, PHANTOM_ENDPOINT|VERTEX_ENDPOINT, wing->prev, __LINE__);
							inphantom = 1 - inphantom;
							InsertEndpoint(ti, te, type|VERTEX_ENDPOINT, wing, __LINE__);
						}
						else
						{
						// Two wings along the intersection line,
						// going in the same direction.
						// No partitioning of the main face is
						// necessary. Partitioning of the transversal
						// face might be necessary, but the starting
						// vertex of the current wing is not an endpoint
						InsertEndpoint(ti, te, VERTEX_ENDPOINT, wing, __LINE__);
						}
					}
				}
				else if (side2 == 0)
				{
					// Current and next endpoint, i.o.w.
					// the current edge, lies in the
					// traversal plane (but the previous
					// edge does not)
					if(!main_face->ConvexCorner((VERTEX*)(wing->prev->vertex), (VERTEX*)(wing->vertex), (VERTEX*)(wing->next->vertex)))
					{

						// If the corner at the starting vertex of
						// the current wing is not a convex corner,
						// the starting vertex is a real endpoint:
						// the entry of exit of a cutting interval
						// along which the main face *will* need to
						// be partitioned
						type = TestEntryExit((VERTEX*)(wing->prev->vertex), (VERTEX*)(wing->vertex), main_face);
						InsertEndpoint(ti, 1, type|VERTEX_ENDPOINT, wing->prev, __LINE__);

					}
					else
					{
						// No endpoint needs to be inserted.
						// Not even a VERTEX endpoint
					}

					// The main face doesn't need to be cut for the
					// current wing, but the transversal face might
					// however. Insert the starting vector as a
					// phantom endpoint into the endpoint list
					if(!inphantom)
					{
						InsertEndpoint(ti, te, PHANTOM_ENDPOINT|VERTEX_ENDPOINT, wing, __LINE__);
						inphantom = true;
					}

					// The main face doesn't need to be cut, but we can
					// mark the contour as lying inside or outside the
					// other solid.
					// Out guess might not be correct, so, don't mark
					// the contour if already marked before. If the
					// contour was not yet marked, we might mark it
					// wrong if the contour contains wings on both sides
					// of the other solid. In that case, however,
					// the contour will be split later, and the
					// fragments marked correctly
					ConditionallyMarkContours(wing);
				}

			}
			else
			{
				// Either there is no intersection, or only the end
				// vertex lies inside the transversal plane.
				// In the latter case, the intersection will be handled
				// as an intersection of the next wing with the
				// transversal plane, or has been handled as an
				// intersection of the first wing in the contour
			}

			wing = wing->next;
		}
		while(wing != (*contour)->wings);

		if(inphantom != firstphantom)
			Warning("DetermineEndpoints", "something wrong with phantom endpoints");
	}
}

// Returns TRUE if the endpoints approximately coincide
static int CoincidingEndpoints(ENDPOINT p1, ENDPOINT p2)
{
	double d = (*p2).ti - (*p1).ti; if (d < 0) d = -d;
	double tol = (*p1).tol + (*p2).tol;
	return d <= tol;
}


// Align a series of endpoints that are within each others tolerance region
static void AlignEndpoints(int start, int end)
{
	// Find a good position on the intersection line
	// for alignment and corresponding tolerance region
	int nr_vertex_endpoints = 0;
	double t_align = 0;
	double tmin = HUGE;
	double tmax = -HUGE;

	ENDPOINT p = endpoints.begin() + start;
	for(int i = start; i <= end; i++, p++)
		{
			if((*p).type != VERTEX_ENDPOINT)
				{
					if((*p).ti - (*p).tol < tmin)
						tmin = (*p).ti - (*p).tol;
					if ((*p).ti + (*p).tol > tmax)
						tmax = (*p).ti + (*p).tol;
				}
			if((*p).type & VERTEX_ENDPOINT)
				{
					t_align += (*p).ti;
					nr_vertex_endpoints++;
				}
		}

	double tol;
	if(nr_vertex_endpoints == 0)
		{
			t_align = (tmin + tmax) * 0.5;
			tol = (tmax - tmin) * 0.5;
		}
	else
		{
			double m;
			t_align /= (double)nr_vertex_endpoints;
			tol = tmax - t_align;
			m = t_align - tmin;
			if (m > tol) tol = m;
		}

	// Align the endpoints. Enlarge the tolerance region of vertex
	// endpoints to include the aligned intersection position
	p = endpoints.begin() + start;
	for(int i = start; i <= end; i++, p++)
		{
			if(p->type & VERTEX_ENDPOINT)
				{
					// Enlarge the tolerance region of the vertex to include the intersection point
					VERTEX* v;
					double d;

					if(p->te == 0)
						v = (VERTEX*)(p->wing->vertex);
					else
						{
							if(p->te != 1)
								Warning("MergeInterval", "vertex endppoint with te != 0 and != 1");
							v = (VERTEX*)(p->wing->next->vertex);
						}

					d = (t_align > p->ti ? t_align - p->ti : p->ti - t_align);
					p->tol = std::max(d, p->tol);
					if(p->tol > v->tolerance)
						v->EnlargeTolerance(p->tol);
				}
			else
				p->tol = tol;

			p->ti = t_align;
		}
}


// Secondary endpoint sorting:
// sorts the endpoints in the interval such that the endpoints induced by faceA
// all preceed the endpoints induced by faceB. Returns in firstA and lastA
// the index to the first and last endpoint induced by faceA and similar for faceB
static void SortEndpointFace(int start, int end,
			     int *firstA, int *lastA, int *firstB, int *lastB)
{
	// *firstB is the index of the first endpoint induced by faceB
	*firstB = start;
	while(*firstB <= end && endpoints[*firstB].wing->contour->face != faceB)
		(*firstB)++;

	// *LastA is the index of the last endpoint induced by faceA
	*lastA = end;
	while(*lastA >= start && endpoints[*lastA].wing->contour->face != faceA)
		(*lastA)--;

	while(*lastA > *firstB)
		{
			std::swap(endpoints[*firstB], endpoints[*lastA]);

			while(*firstB <= end && endpoints[*firstB].wing->contour->face != faceB)
				(*firstB)++;
			while(*lastA >= start && endpoints[*lastA].wing->contour->face != faceA)
				(*lastA)--;
		}

	*firstA = start;
	*lastB = end;
}

// Exit endpoints must preceed phantom endpoints, phantom endpoints
// must preceed entry endpoints. This routine returns +1 if the
// order needs to be changed, 0 is the endpoints have the same
// type and -1 if the order is correct
static int CompareEndpointType(ENDPOINT p1, ENDPOINT p2)
{
	if((*p1).type == (*p2).type) return 0;
	if((*p1).type & EXIT_ENDPOINT) return -1;
	if((*p2).type & EXIT_ENDPOINT) return +1;
	if((*p1).type & PHANTOM_ENDPOINT) return -1;
	if((*p2).type & PHANTOM_ENDPOINT) return +1;
	if((*p1).type & ENTRY_ENDPOINT) return -1;
	if((*p2).type & ENTRY_ENDPOINT) return +1;

	// Other irrelevant endpoint type
	return 0;
}

// Tertiary sorting: first exit endpoints, then phantom endpoints,
// then entry endpoints. Note that the series to be sorted will
// be very small (at most a few: coinciding endpoints induced by
// the same face)
static void SortEndpointType(int first, int last)
{
	for(int i = first; i < last; i++)
		for(int j = i+1; j <= last; j++)
			if(CompareEndpointType(endpoints.begin()+i, endpoints.begin()+j) > 0)
				std::swap(endpoints[i], endpoints[j]);
}


// Annihilates pairs of endpoints that would cause zero length cutting intervals
static void AnnihilateEndpoints(int start, int end)
{
  int i, first_exit=end+1, last_exit=start-1, nr_exit = 0,
         first_phantom=end+1, last_phantom=start-1, nr_phantom = 0,
         first_entry=end+1, last_entry=start-1, nr_entry = 0;

  for (i=start; i<=end; i++) {
    if (endpoints[i].type & EXIT_ENDPOINT) {
      if (i < first_exit) first_exit = i;
      if (i > last_exit) last_exit = i;
      nr_exit ++;
    }
    else if (endpoints[i].type & PHANTOM_ENDPOINT) {
      if (i < first_phantom) first_phantom = i;
      if (i > last_phantom) last_phantom = i;
      nr_phantom ++;
    }
    else if (endpoints[i].type & ENTRY_ENDPOINT) {
      if (i < first_entry) first_entry = i;
      if (i > last_entry) last_entry = i;
      nr_entry ++;
    }
    else
      endpoints[i].type |= DELETED_ENDPOINT;
  }

  while (nr_phantom >= 2) {
    endpoints[first_phantom++].type |= DELETED_ENDPOINT;
    endpoints[first_phantom++].type |= DELETED_ENDPOINT;
    nr_phantom -= 2;
  }

  while (nr_exit > 1 && nr_entry > 1) {
    endpoints[first_exit++].type |= DELETED_ENDPOINT;
    endpoints[first_entry++].type |= DELETED_ENDPOINT;
    nr_exit--; nr_entry--;
  }
}


// Given start and end index of a series of approximately coinciding endpoints,
// align the endpoints, (secondary) sort them according to their contributing
// face, (tertiary) sort them according to their type and remove endpoints that
// would lead to zero length cutting intervals
static void MergeInterval(int start, int end)
{
	int firstA, firstB, lastA, lastB;

	AlignEndpoints(start, end);
	SortEndpointFace(start, end, &firstA, &lastA, &firstB, &lastB);

	SortEndpointType(firstA, lastA);
	SortEndpointType(firstB, lastB);

	AnnihilateEndpoints(firstA, lastA);
	AnnihilateEndpoints(firstB, lastB);
}

// Aligns coinciding endpoints, (secondary) sorts them according to their
// contributing face, (tertiary) sorts them according to their type and
// removes endpoints that would lead to zero length cutting intervals
static void MergeEndpoints()
{
	// Index of start of series of approximately coinciding endpoints
	unsigned int start = 0;
	ENDPOINT prev = endpoints.begin();
	ENDPOINT p = prev+1;
	for(unsigned int i = 1; i < endpoints.size(); prev = p, i++, p++)
		if(!CoincidingEndpoints(prev, p))
			{
				if(i-1 > start)
					MergeInterval(start, i-1);

				start = i;
			}

	if(endpoints.size()-1 > start)
		MergeInterval(start, endpoints.size()-1);
}


// Sorts the endpoints contributed by the intersecting faces
static void SortEndpoints()
{
	sort(endpoints.begin(), endpoints.end());

	MergeEndpoints();
}

// Given the endpoint data p for an endpoint, this routine computes what
// vertex data correspond to it, looks up whether a vertex with this data
// already exists in the solid to which the face belongs, and if not found,
// creates such a vertex. A pointer to the vertex is returned. The face does
// not have to be the same face as the face contributing the endpoint
static BREP_VERTEX* MakeVertex(FACE* face, ENDPOINT p)
{
	VERTEX* thevertex;
	if(((*p).type & VERTEX_ENDPOINT) && (*p).wing->contour->face == face)
		{
			if((*p).te == 0)
				thevertex = (VERTEX*)((*p).wing->vertex);
			else
				thevertex = (VERTEX*)((*p).wing->next->vertex);
		}
	else
		{
			// Endpoint somewhere halfway the edge or on the other face
			VERTEX* v1 = (VERTEX *)((*p).wing->vertex);
			VERTEX* v2 = (VERTEX *)((*p).wing->next->vertex);

			// Sometimes, MakeVertex() is called to create a new vertex at
			// the end of the previous cutting interval for a face. Return
			// the previous end vertex in this case instead of creating a
			// new vertex at the same position

			if((*p).ti == lastti)
				{
					if(face == faceA && lastwingA)
						{
							return lastwingA->vertex;
						}
					else if(face == faceB && lastwingB)
						{
							return lastwingB->vertex;
						}
				}

			VERTEX* v = new VERTEX();
			v->point = isect.V + ((*p).ti)*isect.U;

			if((*p).wing->contour->face == face)
				{
					// Compute a nice interpolated normal
					v->normal = (1-(*p).te)*v1->normal;
					v->normal = v->normal + ((*p).te)*v2->normal;
					(v->normal).Normalize();
				}
			else
				{
					if((*p).type & VERTEX_ENDPOINT)
						{
							if((*p).te == 0)
								v->point = v1->point;
							else
								v->point = v2->point;
						}

					v->normal = VECTOR(0, 0, 0);
				}

			v->tolerance = (*p).tol;

			thevertex = (VERTEX*)(face->shell->solid->vertices->Find(v));
			if(!thevertex)
				{
					thevertex = v;
					thevertex->DataCloseCallback();

					face->shell->solid->vertices->AddWithDuplicates(thevertex);
				}
		}

	return thevertex;
}

// Creates a BREP_VERTEX corresponding to the intersection point and
// belonging to the solid that contains the face that contributed the
// intersection point. Unless this vertex is the endpoint of the
// contributing wing, the contributing edge is split (on both sides
// if used in two contours). The, possibly new, wing leaving at the
// intersection point is returned. This routine also replaces all
// other references to the split wing in the (global) endpoint list.
// Since the intersection point contributed by a wing is unique (within
// the tolerances of course), the edge line parameter (te) of the
// updates endpoints is set to zero: the intersection point is the starting
// point of the newly created wing that is referenced
static BREP_WING* MakeVertexSplitEdge(ENDPOINT p)
{
	BREP_VERTEX* vertex = MakeVertex((FACE*)(*p).wing->contour->face, p);

	BREP_WING* wing;
	if(vertex == (*p).wing->vertex)
		wing = (*p).wing;
	else if (vertex == (*p).wing->next->vertex)
		wing = (*p).wing->next;
	else
		{
			BREP_WING* otherwing = BrepEdgeOtherWing((*p).wing);
			BREP_WING* newotherwing = NULL;

			if(otherwing->contour)
				{
					newotherwing = BrepSplitWing(otherwing, vertex);
				}

			wing = BrepSplitWing((*p).wing, vertex);

			// Replace all references to wings of the split edge in the endpoint list.
			for(unsigned int i = 0; i < endpoints.size(); i++)
				{
					if(endpoints[i].wing == (*p).wing)
						{
							endpoints[i].wing = wing;
							// Should be the same intersection point
							endpoints[i].te = 0;
						}
					else if(endpoints[i].wing == otherwing)
						{
							endpoints[i].wing = newotherwing;
							// Should be the same intersection point
							endpoints[i].te = 0;
						}
				}
		}

	return wing;
}

// Introduces a cut from the starting vertex of wing1 to the starting vertex of
// wing2. Assumes that the starting vertex of wing1 comes before the starting
// vertex of wing2 on the intersection line. Returns a wing leaving at the
// exit end of the cut. Also replaces contributing wings in the global
// endpoint list to belong to the right contour for further processing
static BREP_WING* MakeCut(BREP_WING* wing1, BREP_WING* wing2)
{
	BREP_WING* wing;
	if(wing1->contour == wing2->contour)
		{
			wing = BrepMakeEdgeSplitContour(wing1, wing2);

			// Update the endpoints list: the exitant edge at p2 must be an
			// edge on the contour that is still to be processed. Failing to
			// do so may cause the split of contour to be merged again with
			// the original contour when processing the next cutting interval
			// if it starts at the exit endpoint of the one being processed.
			// This situation occurs may occur when an isolated vertex of one
			// face lies in the other face. Cfr. Segal IEEE CG&A jan. 88 p 60
			if(((FACE*)wing2->contour->face)->ConvexCorner((VERTEX*)(wing1->vertex), (VERTEX*)(wing2->vertex), (VERTEX*)(wing2->next->vertex)))
				{
					BREP_WING* owing = BrepEdgeOtherWing(wing);
					for(unsigned int i = 0; i < endpoints.size(); i++)
						{
							if(endpoints[i].wing == wing2)
								{
									endpoints[i].wing = owing;
								}
						}

						wing2 = owing;
				}
		}
	else
		{
			wing = BrepMakeEdgeJoinContours(wing1, wing2);
		}

	MarkContours(wing);

	// The possibly updated wing leaving at the exit endpoint
	return wing2;
}


// Introduces a notch from the starting vertex of wing1 to v2. Assumes that
// the starting vertex of wing1 comes before v2 on the intersection line
static BREP_WING* MakeNotch1(BREP_WING* wing1, BREP_VERTEX* v2)
{
	BREP_WING* wing = BrepMakeNotch(wing1, v2);

	MarkContours(wing);

	// Wing leaving at (exit endpoint) v2
	return wing->next;
}

// Introduces a notch from v1 to the starting vertex of wing2. Assumes that
// v1 comes before the starting vertex of wing2 on the intersection line
static BREP_WING* MakeNotch2(BREP_VERTEX* v1, BREP_WING* wing2)
{
	BREP_WING* wing = BrepMakeNotch(wing2, v1);

	MarkContours(wing);
	return wing2;
}


// Introduces a slit in the face between the two vertices.
// Assumes that v1 comes before v2 on the intersection line
static BREP_WING* MakeSlit(FACE* face, BREP_VERTEX* v1, BREP_VERTEX* v2)
{
	BREP_WING* wing = BrepMakeSlit(face, v1, v2);

	MarkContours(wing);

	// Wing leaving at (exit endpoint) v2 going to v1
	return wing->next;
}



// The routines Cut(), Notch() and Slit() introduce a cut, notch or slit in a
// face between endpoints p1 and p2. They return a BREP_WING leaving at the exit end
// of the cut, notch or slit, i.e. with highest parameter along the intersection
// line. This vertex may be the start vertex of a next cut, notch or slit.

// Both endpoints are contributed by the face. This routine will insert a cut in
// the face. Assumes that p1 preceeds p2, i.o.w. p1 is the entry endpoint and p2
// is the exit endpoint
static BREP_WING* Cut(FACE* face, ENDPOINT p1, ENDPOINT p2)
{
	BREP_WING* wing1 = MakeVertexSplitEdge(p1);
	if(wing1 == (*p2).wing)
		{
			// The point at which p1->wing was to be split
			// was its end vertex, so MakeVertexSplitEdge(p1)
			// returns p2->wing

			BREP_WING* wing2 = MakeVertexSplitEdge(p2);

			return wing2;
		}

	BREP_WING* wing2 = MakeVertexSplitEdge(p2);

	if((*p1).ti < (*p2).ti)
		return MakeCut(wing1, wing2);
	else
		return MakeCut(wing2, wing1);
}


// Only the first endpoint was contributed by the face, the second not.
// This routine inserts a notch in the face, splitting p1->wing in two at
// the intersection point and connecting to the new vertex defined by p2
static BREP_WING* Notch(FACE* face, ENDPOINT p1, ENDPOINT p2)
{
	BREP_WING* wing = MakeVertexSplitEdge(p1);
	BREP_VERTEX* v = MakeVertex(face, p2);

	if((lastwingA && v == lastwingA->vertex) ||
		(lastwingB && v == lastwingB->vertex))
		{
			// A notch to the end vertex of the previous
			// cutting interval cuts the contour
			BREP_WING* wing2 = ((lastwingA && v == lastwingA->vertex) ? lastwingA : lastwingB);
			if(p1->ti < p2->ti)
				return MakeCut(wing, wing2);
			else
				return MakeCut(wing2, wing);
		}

	if((*p1).ti < (*p2).ti)
		return MakeNotch1(wing, v);
	else
		return MakeNotch2(v, wing);
}

// None of the endpoints was contributed by the face. This routine inserts a slit
// in the face. It is assumed that p1 preceeds p2 on the intersection line, i.o.w.
// p1 is the entry endpoint and p2 is the exit endpoint
static BREP_WING* Slit(FACE* face, ENDPOINT p1, ENDPOINT p2)
{
	BREP_VERTEX* v1 = MakeVertex(face, p1);
	BREP_VERTEX* v2 = MakeVertex(face, p2);

	if((lastwingA && v1 == lastwingA->vertex) ||
		(lastwingB && v1 == lastwingB->vertex))
		{
			// A slit including the endpoint of a previous
			// cutting interval is a notch
			BREP_WING *wing1 = ((lastwingA && v1 == lastwingA->vertex) ? lastwingA : lastwingB);
			return MakeNotch1(wing1, v2);
		}

	return MakeSlit(face, v1, v2);
}

// This routine inserts cuts, notches and slits in the faces as indicated by
// the endpoint multiplicities. There is at least one and at most two entry or exit
// endpoints for a cutting interval: at least from one and at most from both faces.
// If there is both an entry and exit endpoint for a given face, the face needs
// to be cut. If there is only one entry or exit endpoint for a face, a notch
// needs to be inserted. If a face didn't contribute neither an entry and neither
// an exit endpoint, a slit needs to be inserted in the face (In that case, the other
// face has contributed both entry and exit endpoint and will be cut). Phantom
// endpoints should come in pairs. If not, there is something wrong with the endpoint
// determination or sorting. A pair of phantom endpoints induces a cut, notch or slit
// in the face that didn't contribute the endpoints, but the face that contributed
// them is left intact. This routine is called with the last encountered entry and
// first encountered exit points on both faces at the start and end of a cutting
// interval.
static void CuttingInterval(ENDPOINT entryA, ENDPOINT entryB, ENDPOINT exitA, ENDPOINT exitB)
{
	// entry and exit endpoints of the cutting interval
	ENDPOINT ci_startA, ci_startB, ci_endA, ci_endB;
	ENDPOINT entry, exit;

	// Check if an entry and exit point is given for both faces.
	if(entryA == NULLENDPOINT || entryB == NULLENDPOINT || exitA == NULLENDPOINT || exitB == NULLENDPOINT)
		{
			Warning("CuttingInterval", "missing endpoints");
			return;
		}

	// Compare the last encountered entry points on face A and face B:
	// if they are at the same position, keep both as starting
	// points of a cutting interval. If not, save only the one with
	// largest parameter along the intersection line
	ci_startA = ci_startB = NULLENDPOINT;
	if((*entryA).ti >= (*entryB).ti)
		ci_startA = entryA;
	if((*entryB).ti >= (*entryA).ti)
		ci_startB = entryB;

	entry = (ci_startA != NULLENDPOINT ? ci_startA : ci_startB);

	// Compare the endpoints. Keep both if they are at the same position, or only
	// the one at with smallest parameter along the intersection line if different
	ci_endA = ci_endB = NULLENDPOINT;
	if((*exitA).ti <= (*exitB).ti)
		ci_endA = exitA;
	if((*exitB).ti <= (*exitA).ti)
		ci_endB = exitB;

	exit = (ci_endA != NULLENDPOINT ? ci_endA : ci_endB);

	if(CoincidingEndpoints(entry, exit))
		{
			// Don't create zero length features
			return;
		}

	if((*entry).ti > (*exit).ti)
		{
			Warning("CuttingInterval", "cutting interval determination incorrect");
			return;
		}

	// Check if phantom endpoints come in pairs
	if((entryA->type & PHANTOM_ENDPOINT) != (exitA->type & PHANTOM_ENDPOINT) ||
		(entryB->type & PHANTOM_ENDPOINT) != (exitB->type & PHANTOM_ENDPOINT))
		{
			Warning("CuttingInterval", "phantom endpoints should come in pairs");
		}

	// If not a pair of phantom endpoints, it must be
	// a pair of one entry and one exit endpoint
	else if (!(entryA->type & ENTRY_ENDPOINT) != !(exitA->type & EXIT_ENDPOINT) ||
		!(entryB->type & ENTRY_ENDPOINT) != !(exitB->type & EXIT_ENDPOINT))
		{
			Warning("CuttingInterval", "entry/exit property of endpoints incorrectly determined");
		}

#ifdef CHECK
  if (check) {
    return;
  }
#endif // CHECK

	// Partition face A
	if(!(entryA->type & PHANTOM_ENDPOINT) && !(exitA->type & PHANTOM_ENDPOINT))
		{
			if(ci_startA != NULLENDPOINT && ci_endA != NULLENDPOINT)
				lastwingA = Cut(faceA, ci_startA, ci_endA);
			else if(ci_startA != NULLENDPOINT)
				lastwingA = Notch(faceA, ci_startA, ci_endB);
			else if(ci_endA != NULLENDPOINT)
				lastwingA = Notch(faceA, ci_endA, ci_startB);
			else
				lastwingA = Slit(faceA, ci_startB, ci_endB);
		}

	// Partition face B
	if(!(entryB->type & PHANTOM_ENDPOINT) && !(exitB->type & PHANTOM_ENDPOINT))
		{
			if(ci_startB != NULLENDPOINT && ci_endB != NULLENDPOINT)
				lastwingB = Cut(faceB, ci_startB, ci_endB);
			else if(ci_startB != NULLENDPOINT)
				lastwingB = Notch(faceB, ci_startB, ci_endA);
			else if(ci_endB != NULLENDPOINT)
				lastwingB = Notch(faceB, ci_endB, ci_startA);
			else
				lastwingB = Slit(faceB, ci_startA, ci_endA);
		}

	lastti = (*exit).ti;
}

// Traverses the intersection line, determines entry and exit
// endpoints of cutting intervals, and processes cutting intervals
static void TraverseIntersectionline()
{
	// Initialize intersection line parameter of and wings in faceA and faceB
	// leaving at the exit endpoint of the previous cutting interval
	lastwingA = lastwingB = NULL;
	lastti = -HUGE;

	// Last encountered entry and exit endpoint on face A and B
	ENDPOINT entryA = NULLENDPOINT;
	ENDPOINT entryB = NULLENDPOINT;
	ENDPOINT exitA = NULLENDPOINT;
	ENDPOINT exitB = NULLENDPOINT;

	// Potential entry and exit endpoint for cutting interval
	ENDPOINT ci_startA = NULLENDPOINT;
	ENDPOINT ci_startB = NULLENDPOINT;
	ENDPOINT ci_endA = NULLENDPOINT;
	ENDPOINT ci_endB = NULLENDPOINT;

	// Flags indicating whether we are inside/outside faceA and faceB
	bool inA = false;
	bool inB = false;
	// Flag indicating whether we are currently in a cutting interval
	bool incut = false;

	// Loop over the sorted endpoints
	ENDPOINT endpoint = endpoints.begin();
	for(unsigned int i = 0; i < endpoints.size(); i++, endpoint++)
		{
			// Skip deleted endpoints or purely VERTEX endpoints,
			// which are just there for alignement
			if((endpoint->type & DELETED_ENDPOINT) || (endpoint->type == VERTEX_ENDPOINT))
				continue;

			// Flip the state of the inA or inB flag, update the last
			// encountered entry or exit endpoint for the face accordingly.
			// Forget the last exit endpoint when an entry endpoint is
			// found and vice versa.
			if(endpoint->wing->contour->face == faceA)
				{
					inA = 1 - inA;
					if(inA)
						{
							entryA = endpoint;
							exitA = NULLENDPOINT;
						}
					else
						{
							entryA = NULLENDPOINT;
							exitA = endpoint;
						}
				}
			else if(endpoint->wing->contour->face == faceB)
				{
					inB = 1 - inB;
					if(inB)
						{
							entryB = endpoint;
							exitB = NULLENDPOINT;
						}
					else
						{
							entryB = NULLENDPOINT;
							exitB = endpoint;
						}
				}
			else
				{
					// This should never occur
					Warning("TraverseIntersectionline", "endpoint present from unknown face");
					// Ignore it
					continue;
				}

			if(entryA != NULLENDPOINT && entryB != NULLENDPOINT)
				{
					// Start of a new cutting interval
					ci_startA = entryA;
					ci_startB = entryB;
					incut = true;
				}
			else if(incut && (exitA != NULLENDPOINT || exitB != NULLENDPOINT))
				{
					// End of cutting interval

					// Look ahead to find the first exit endpoint
					// induced by the other face
					ci_endA = exitA;
					for(unsigned int j = i+1; ci_endA == NULLENDPOINT && j < endpoints.size(); j++)
						{
							ENDPOINT endq = endpoints.begin()+j;
							if((*endq).wing->contour->face == faceA &&
								(((*endq).type & PHANTOM_ENDPOINT) || (endq->type & EXIT_ENDPOINT)))
								ci_endA = endq;
						}

					ci_endB = exitB;
					for(unsigned int j = i+1; ci_endB == NULLENDPOINT && j < endpoints.size(); j++)
						{
							ENDPOINT endq = endpoints.begin()+j;
							if((*endq).wing->contour->face == faceB &&
								(((*endq).type & PHANTOM_ENDPOINT) || (endq->type & EXIT_ENDPOINT)))
								ci_endB = endq;
						}

					CuttingInterval(ci_startA, ci_startB, ci_endA, ci_endB);
					incut = false;
				}
		}

	// After traversal of the intersection line,
	// we should be outside of both faces again
	if(inA || inB)
		Warning("TraverseIntersectionline", "something wrong with endpoint determination");
}

// Partitions a pair of possibly transversal intersecting
// faces along the intersection line
static void TransversalIntersection(FACE* _faceA, BREP_WING* first_wingA,
				    FACE* _faceB, BREP_WING* first_wingB)
{
	// Global variables not to have to pass them as a parameter every time
	faceA = _faceA;
	faceB = _faceB;

	// Compute the parametric equation t.U + V and
	// tolerance of the face intersection line
	isect = ComputePlaneToPlaneIntersection(faceA, faceB);

	// Determine cutting interval endpoints = intersections
	// of an edge of one face with the plane of the other
	endpoints.clear();
	timinA = timinB = HUGE;
	timaxA = timaxB = -HUGE;
	DetermineEndpoints(faceA, first_wingA, faceB);
	DetermineEndpoints(faceB, first_wingB, faceA);

	// If all endpoints of one face come before the endpoints
	// contributed by the other face, there is no intersection
	if(timinA > timaxB || timinB > timaxA)
		{
			return;
		}

	// Sort endpoint list
	SortEndpoints();

	// Traverse intersection line, insert cuts, notches and
	// slits in the faces as indicated by the endpoints
#ifdef CHECK
  check = true;
  TraverseIntersectionline();
  check = false;
#endif // CHECK
	TraverseIntersectionline();
}

// faceA and faceB are two faces for which the coplanarity factor exceeds the
// allowed minimum. This routine will return TRUE if faceA really can
// be considered to be within the plane of faceB:  that is, when
// two vertices in faceA lies on opposite sides of the plane of faceB, or
// when any vertex of faceA lies within a distance equal to the extent of faceB
static int TestCoplanarIntersection(FACE* faceA, FACE* faceB)
{
	// Some value != 0
	int side = UNKNOWN_SIDE;

	// Check the signed distance of all vertices of faceA to the plane of faceB
	if(faceA->outer_contour.empty())
		// Face without contours, ignore it
		return false;

	BREP_CONTOUR_RING::iterator contour;
	for(contour = faceA->outer_contour.begin(); contour != faceA->outer_contour.end(); contour++)
		if((*contour)->wings)
			{
				BREP_WING* wing = (*contour)->wings;
				faceB->SignedDistanceToPlane((VERTEX*)(wing->vertex), &side);
				if(side == 0)
					// Vertex within the extent of the plane of faceB
					return true;

				wing = wing->next;

				while(wing != (*contour)->wings)
					{
						int prevside = side;
						faceB->SignedDistanceToPlane((VERTEX*)(wing->vertex), &side);

						if(side != prevside || side == 0)
						// The starting vertex of wing was on the
						// other side of the plane of faceB than
						// the starting vertex of wing->prev or
						// the vertex was within the extent of
						// the plane of faceB
							return true;

						wing = wing->next;
					}

				// There's no need to check the last wing:
				// if the starting vertex of the last wing is on
				// the other side of the starting vertex of the first,
				// there is a crossing in one of the other wings as well
			}

	return false;
}

// faceA and faceB are two faces for which the coplanarity factor is lower than the
// minimum allowed coplanarity factor. This routine classifies faceA w.r.t. faceB:
// If an edge of faceA has endpoints on different
// sides of the plane of faceB, a pointer to the crossing edge is returned.
// If faceA is entirely on the same side of the plane of faceB NULL is returned.
// Two faces faceA and faceB can only intersect transversally if both faces
// have an edge that crosses the plane of the other.
static BREP_WING* TestTransversalIntersection(FACE* faceA, FACE* faceB)
{
	// Some value != 0
	int side = UNKNOWN_SIDE;

	// Check the signed distance of all vertices of faceA to the plane of faceB
	BREP_CONTOUR_RING::iterator contour;
	if(faceA->outer_contour.empty())
		// Face without contours, ignore it
		return NULL;

	for(contour = faceA->outer_contour.begin(); contour != faceA->outer_contour.end(); contour++)
		{
			BREP_WING* wing;

			if((*contour)->wings)
				{
					wing = (*contour)->wings;
					faceB->SignedDistanceToPlane((VERTEX*)(wing->vertex), &side);
					if(side == 0)
						return wing;

					wing = wing->next;

					while(wing != (*contour)->wings)
						{
							int prevside = side;
							faceB->SignedDistanceToPlane((VERTEX*)(wing->vertex), &side);

							if(side == 0 || side != prevside)
								// The starting vertex of wing was
								// on the other side of the plane
								// of faceB than the starting
								// vertex of wing->prev
								return wing->prev;

							wing = wing->next;
						}

					// There's no need to check the last wing: if the starting
					// vertex of the last wing is on the other side of the
					// starting vertex of the first,
					// there is a crossing in one of the other wings as well
				}
		}

	// No need to classify faceB w.r.t. to faceA
	return NULL;
}


// Given two FACES, with appropriate geometric data (see geometry.[ch]), this
// routine splits the faces into nonintersecting parts. Plane equations and tolerances
// have been computed when reading in the objects, in CloseFaceCallback()
int PartitionFaces(FACE* faceA, FACE* faceB)
{
//  if (faceA->ShortestEdgeLength() < minedgelength ||
//      faceB->ShortestEdgeLength() < minedgelength)
//    return NO_ISECT;	// Do not consider small faces

	if(CoplanarityFactor(faceA, faceB) > min_coplanarity_factor)
		{
			if(TestCoplanarIntersection(faceA, faceB) ||
				TestCoplanarIntersection(faceB, faceA))
				return COPLANAR_ISECT;
		}
	else
		{
			BREP_WING* wingA = TestTransversalIntersection(faceA, faceB);
			BREP_WING* wingB = TestTransversalIntersection(faceB, faceA);
			if(wingA != NULL && wingB != NULL)
				TransversalIntersection(faceA, wingA, faceB, wingB);

			return TRANSVERSAL_ISECT;
		}

	return NO_ISECT;
}


