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

#include "error.h"
#include "vector.h"
#include "raytrace.h"


// (This code has been copied from rayshade 4.0.6. and adapted)
//
// Quadrants are defined as:
//        |
//   1    |   0
//        |
// -------c--------
//        |
//   2    |   3
//        |
inline int quadrant(const VEC2D& p, const VEC2D& c)
{
	if(p.u < c.u)
		if(p.v < c.v)
			return 2;
		else
			return 1;
	else
		if(p.v < c.v)
			return 3;
		else
			return 0;
}


// Returns the winding number of the contour around the point, projected to 2D.
// This code was taken from Rayshade 4.0.6
static int ContourWinding(BREP_CONTOUR* contour, const VEC2D& center, unsigned int index)
{
	// Is the point inside the polygon?
	//
	// Compute the winding number by finding the quadrant each
	// polygon point lies in with respect to the the point in
	// question, and computing a "delta" (winding number).  If we
	// end up going around in a complete circle around
	// the point (winding number is non-zero at the end), then
	// we're inside.  Otherwise, the point is outside.
	//
	// Note that we can turn this into a 2D problem by projecting
	// all the points along the axis defined by poly->index,
	// the "dominant" part of the polygon's normal vector.
	if(contour->wings == NULL || (contour->wings->prev == contour->wings->next))
		{
			// Fewer than three vertices in the contour, return zero
			return 0;
		}

	int winding = 0;

	VEC2D last = VectorProject(((VERTEX*)(contour->wings->prev->vertex))->point, index);
	int lastquad = quadrant(last, center);

	BREP_WING* wing = contour->wings;
	do
		{
			VEC2D cur = VectorProject(((VERTEX*)(wing->vertex))->point, index);
			int quad = quadrant(cur, center);

			if(quad != lastquad)
				{
					if(((lastquad + 1) & 3) == quad)
						winding++;
					else if(((quad + 1) & 3) == lastquad)
						winding--;
					else
						{
							// Find where edge crosses, center's X axis
							double right = last.u - cur.u;
							double left = (last.v - cur.v) * (center.u - last.u);

							if(left + last.v * right > right * center.v)
								winding += 2;
							else
								winding -= 2;
						}
				}

			lastquad = quad;
			last = cur;
			wing = wing->next;
		}
	while (wing != contour->wings);

	return winding;
}


static unsigned int VertexInsideContour(const VECTOR& point, BREP_CONTOUR* contour)
{
	FACE *face_data = (FACE*)(contour->face);

	unsigned int index = VectorDominantCoord(face_data->normal);
	VEC2D center = VectorProject(point, index);

	return ContourWinding(contour, center, index);
}


const double ON_THE_BORDER = 4*M_PI;

// Paulo Cezar Pinto Carvalho and Paulo Roma Cavalcanti, "Point in
// Polyhedron Testing Using Spherical Polygons", Graphics Gems V, p 42
static double GeoSolidAngle(VERTEX* v, BREP_CONTOUR* contour)
{
	// Contour with less than 3 vertices
	if(contour->wings == NULL || contour->wings->prev == contour->wings->next)
		{
			return 0;
		}

	// Test whether v->point lies within the plane of the contour. If so, test
	// whether the point is inside of outside the contour. If outside, the solid angle
	// under which the contour is seen is zero. If inside, the solid angle is
	// 2pi or -2pi depending on the orientation of the contour around the vertex
	FACE* plane = (FACE *)(contour->face);
	if(fabs((plane->normal * v->point) + plane->d) < plane->extent + v->tolerance)
		{
			unsigned int winding = VertexInsideContour(v->point, contour);

			return (winding == 0 ? 0 : /* (winding > 0 ? 2*M_PI : -2*M_PI) */ ON_THE_BORDER);
		}

	VECTOR norm = ((CONTOUR*)(contour))->normal;
	double area = 0;
	int nr_verts = 0;

	VECTOR p2 = ((VERTEX*)(contour->wings->prev->vertex))->point;
	VECTOR p1 = ((VERTEX*)(contour->wings->vertex))->point;
	VECTOR a = p2 - p1;

	BREP_WING* wing = contour->wings;
	VECTOR r1;
	for(; wing != NULL; wing = (wing->next == contour->wings ? NULL : wing->next), nr_verts++)
		{
			r1 = p1 - v->point;
			p2 = ((VERTEX*)(wing->next->vertex))->point;
			VECTOR b = p2 - p1;
			VECTOR n1 = a ^ r1;
			VECTOR n2 = r1 ^ b;

			double l1 = n1.Norm();
			double l2 = n2.Norm();
			double s = (n1 * n2) / (l1 * l2);
			double ang = acos(std::max(-1.0, std::min(1.0, s)));

			// Determinant
			s = (b^a)*norm; // == b*(a^norm)
			area += s > -EPSILON ? M_PI - ang : M_PI + ang;

			a = -b;
			p1 = p2;
		}

	area -= M_PI * (double)(nr_verts - 2);
	area = ((norm * r1) > 0) ? -area : area;

	return area;
}


static unsigned int VertexInSolid(VERTEX* v, SOLID* solid)
{
	if(solid->bounds.OutOfBounds(v->point))
		{
			return OUTMASK;
		}

	double Area = 0;
	BREP_SHELL_RING::iterator shell;
	for(shell = solid->shells.begin(); shell != solid->shells.end(); shell++)
		{
			BREP_FACE_RING::iterator face;
			for(face = (*shell)->faces.begin(); face != (*shell)->faces.end(); face++)
				{
					BREP_CONTOUR_RING::iterator contour;
					for(contour = (*face)->outer_contour.begin(); contour != (*face)->outer_contour.end(); contour++)
						{
							double area = GeoSolidAngle(v, *contour);
							if(area < -2*M_PI || area > 2*M_PI)
								{
									if(area != ON_THE_BORDER)
										Error("VertexInSolid", "something wrong with GeoSolidAngle()");

									return 0;
								}

							Area += area;
						}
				}
		}

	return (Area > 2*M_PI || Area < -2*M_PI) ? INMASK : OUTMASK;
}


unsigned int ContourInSolid(BREP_CONTOUR* contour, SOLID* solid)
{
	if(contour->wings == NULL)
		return OUTMASK;

	unsigned int mask = 0;
	BREP_WING* wing = contour->wings;
	do
		{
			// Take the midpoint of the wing
			VECTOR p = MidPoint(((VERTEX*)(wing->vertex))->point,
				((VERTEX*)(wing->next->vertex))->point);

			// Shift it a little bit into the face
			VECTOR u = ((VERTEX*)(wing->next->vertex))->point -
			((VERTEX*)(wing->vertex))->point;
			u = ((CONTOUR*)(contour))->normal ^ u;
			u.Normalize();

			VERTEX v = *((VERTEX*)(wing->vertex));
			v.point = p + (10*EPSILON)*u;
			v.tolerance = ((EDGE *)(wing->edge))->tolerance;

			mask = VertexInSolid(&v, solid);

			wing = wing->next;
		}
	while(wing != contour->wings && mask == 0);

	return mask;
}


/*

const int CULL_NONE = 0;
const int CULL_FRONT = 1;
const int CULL_BACK = 2;


static int icount = 0;
static int cull_flags = CULL_NONE;

static BREP_FACE *face1, *face2;

void DontIntersectFaces(BREP_FACE *f1, BREP_FACE *f2)
{
  face1 = f1;
  face2 = f2;
}

static unsigned RaytraceCull(unsigned flags)
{
  unsigned old_flags = cull_flags;
  cull_flags = flags;
  return old_flags;
}

// Returns winding number of intersection point in the face. The winding number
// is zero for points outside the face and != zero for contained points
int IntersectionInsideFace(BREP_FACE *face, RAY *ray, double dist)
{
  FACE *face_data = (FACE *)(face);
  VEC2D center;
  int winding, index;

  switch ((index = VectorDominantCoord(face_data->normal))) {
  case XNORMAL:
    center.u = ray->pos.y + dist * ray->dir.y;
    center.v = ray->pos.z + dist * ray->dir.z;
    break;
  case YNORMAL:
    center.v = ray->pos.z + dist * ray->dir.z;
    center.u = ray->pos.x + dist * ray->dir.x;
    break;
  default:	// ZNORMAL
    center.u = ray->pos.x + dist * ray->dir.x;
    center.v = ray->pos.y + dist * ray->dir.y;
  }

  winding = 0;
	BREP_CONTOUR_RING::iterator contour;
	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		{
			int w = ContourWinding(*contour, center, index);
			winding += w;
		}

  return winding;
}

static int BoundsIntersected(BOUNDINGBOX& bounds, RAY *ray, double mindist, double maxdist)
{
  VECTOR vtmp;

	vtmp = ray->pos + mindist*ray->dir;
	if(bounds.OutOfBounds(vtmp))
		return bounds.BoundsIntersect(ray, mindist, &maxdist);
  return true;
}

static BREP_FACE *FaceIntersect(BREP_FACE *face, RAY *ray, double mindist, double *maxdist)
{
  FACE *face_data = (FACE *)(face);
  double dist, d;

  if (face == face1 || face == face2)
    return NULL;

  // Test against face bounding box
  if (!BoundsIntersected(face_data->bounds, ray, mindist, *maxdist))
    return NULL;

  // Fill ray.pos in the face plane equation
  d = (face_data->normal * ray->pos) + face_data->d;
  if (cull_flags != CULL_NONE) {
    if ((d < 0. && (cull_flags & CULL_BACK)) ||
	(d > 0. && (cull_flags & CULL_FRONT)))
      return NULL;
  }

  // Compute intersection distance along the ray
  dist = (face_data->normal * ray->dir);
  if (fabs(dist) < EPSILON)
    return NULL;

  dist = - d / dist;
  if (dist < mindist || dist > *maxdist)
    return NULL;

  if (IntersectionInsideFace(face, ray, dist)) {
    *maxdist = dist;
    icount++;
    return face;
  }

  return NULL;
}

static BREP_FACE *ShellIntersect(BREP_SHELL *shell, RAY *ray, double mindist, double *maxdist,
		     int shadowtest)
{
  BREP_FACE *hit;

  //if (shell->IS_HIDDEN())
  //  return NULL;

  if (!BoundsIntersected(((SHELL *)(shell->client_data))->bounds, ray, mindist, *maxdist))
    return NULL;

  hit = NULL;

	BREP_FACE_RING::iterator face;
	for(face = shell->faces.begin(); face != shell->faces.end(); face++)
		{
			BREP_FACE *f = FaceIntersect(*face, ray, mindist, maxdist);
			if(f)
				{
					if(shadowtest)
						return f;
					else
						hit = f;
				}
		}

  return hit;
}

static BREP_FACE *SolidIntersect(BREP_SOLID *solid, RAY *ray, double mindist, double *maxdist,
			  int shadowtest)
{
  BREP_FACE *hit;

  if (!BoundsIntersected(((SOLID *)(solid->client_data))->bounds, ray, mindist, *maxdist))
    return NULL;

  hit = NULL;
	BREP_SHELL_RING::iterator shell;
	for(solid->shells.begin(); shell != solid->shells.end(); shell++)
		{
			BREP_FACE *f = ShellIntersect(*shell, ray, mindist, maxdist, shadowtest);
			if(f)
				{
					if(shadowtest)
						return f;
					else
						hit = f;
				}
		}

  return hit;
}

#ifdef NEVER
static double WingLength(BREP_WING *wing)
{
  VERTEX *v1 = (VERTEX *)(wing->vertex->client_data),
         *v2 = (VERTEX *)(wing->next->vertex->client_data);

  return VectorDist(&(v1->point), &(v2->point))
         - ((EDGE *)(wing->edge->client_data))->extent;
}

static BREP_WING *FindGoodWing(BREP_SOLID *solid)
{
  BREP_SHELL *shell;
  BREP_FACE *face;
  BREP_CONTOUR *contour;
  BREP_WING *wing;

  for (shell = solid->shells; shell; shell = (shell->next == solid->shells ? NULL : shell->next)) {
    for (face = shell->faces; face; face = (face->next == shell->faces ? NULL : face->next)) {
      for (contour = face->outer_contour; contour; contour = (contour->next == face->outer_contour ? NULL : contour->next)) {
	for (wing = contour->wings; wing; wing = wing->next) {
	  if (WingLength(wing) > 0.)
	    return wing;
	}
      }
    }
  }

  return NULL;
}

// Simple application of raytracing: testing whether a vertex is inside the solid
int VertexInSolid(BREP_VERTEX *vertex, BREP_SOLID *solid)
{
  RAY ray;
  VECTOR p, u;
  BREP_WING *wing;
  double dist;

  VECTORCOPY(((VERTEX *)(vertex->client_data))->point, ray.pos);

  // an arbitrary ray direction suffices, but only for a closed solid. If the solid is
//   * not closed, the result would depend on the ray direction. To handle non-closed
//   * solids, we prefer a point-in-polyhedron inclusion test based on solid angles.

  // Find an arbitrary point on an arbitrary face of the solid
  wing = FindGoodWing(solid);
  if (!wing) {
    Error("VertexInSolid", "bad solid 1");
    return 0;
  }

  // Take the midpoint of the edge
  MIDPOINT(((VERTEX *)(wing->vertex->client_data))->point,
	   ((VERTEX *)(wing->next->vertex->client_data))->point,
	   p);

  // Move it a little inward to the face
  VECTORDIFF(((VERTEX *)(wing->next->vertex->client_data))->point,
	     ((VERTEX *)(wing->vertex->client_data))->point,
	     u);
  VECTORCROSSPRODUCT(((FACE *)(wing->contour->face->client_data))->normal,
		     u, u);
  VECTORNORMALIZE(u);
  VECTORSUMSCALED(p, 1.5 * ((EDGE *)(wing->edge->client_data))->extent, u, p);

  VECTORDIFF(p, ray.pos, ray.dir);
  dist = VECTORNORM(ray.dir) - EPSILON;
  VECTORSCALEINVERSE(dist, ray.dir, ray.dir);

  DontIntersectFaces(wing->contour->face, NULL);
  icount=1;	// One intersection with the face being excluded from testing

  dist = HUGE;
  SolidIntersect(solid, &ray, ((VERTEX *)(vertex->client_data))->tolerance, &dist, false);

  DontIntersectFaces(NULL, NULL);

  return icount&1 ? OUTMASK : INMASK;
}
#endif
*/


