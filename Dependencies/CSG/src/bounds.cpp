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

#include "bounds.h"


// Check for intersection between bounding box and the given ray.
// If there is an intersection between mindist and *maxdist along
// the ray, *maxdist is replaced with the distance to the point of
// intersection, and TRUE is returned.  Otherwise, FALSE is returned.
//
// If this routine is used to check for intersection with a volume
// rather than a "hollow" box, one should first determine if
// (ray->pos + mindist * ray->dir) is inside the bounding volume, and
// call BoundsIntersect() only if it is not.
//
// This routine was taken from rayshade [PhB].
//
int BOUNDINGBOX::BoundsIntersect(RAY* ray, double mindist, double *maxdist)
{
	double t, tmin, tmax;
	double dir, pos;

	tmax = *maxdist;
	tmin = mindist;

	dir = ray->dir.x;
	pos = ray->pos.x;

	if (dir < 0) {
		t = (MIN_X - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MAX_X - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (dir > 0.) {
		t = (MAX_X - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MIN_X - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (pos < MIN_X || pos > MAX_X)
		return false;

	dir = ray->dir.y;
	pos = ray->pos.y;

	if (dir < 0) {
		t = (MIN_Y - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MAX_Y - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (dir > 0.) {
		t = (MAX_Y - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MIN_Y - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (pos < MIN_Y || pos > MAX_Y)
		return false;

	dir = ray->dir.z;
	pos = ray->pos.z;

	if (dir < 0) {
		t = (MIN_Z - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MAX_Z - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (dir > 0.) {
		t = (MAX_Z - pos) / dir;
		if (t < tmin)
			return false;
		if (t <= tmax)
			tmax = t;
		t = (MIN_Z - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return false;
			tmin = t;
		}
	} else if (pos < MIN_Z || pos > MAX_Z)
		return false;

	// If tmin == mindist, then there was no "near"
	// intersection farther than EPSILON away
	if (tmin == mindist) {
		if (tmax < *maxdist) {
			*maxdist = tmax;
			return true;
		}
	} else {
		if (tmin < *maxdist) {
			*maxdist = tmin;
			return true;
		}
	}
	return false;	// Hit, but not closer than maxdist
}


