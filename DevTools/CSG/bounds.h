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

#ifndef _BOUNDS_H_
#define _BOUNDS_H_

#include "vector.h"

#include <math.h>
#include <iostream>

class BOUNDINGBOX
{
public:
	BOUNDINGBOX()
	{
		MIN_X = MIN_Y = MIN_Z = HUGE;
		MAX_X = MAX_Y = MAX_Z = -HUGE;
	}

	~BOUNDINGBOX()
	{
	}

	void Enlarge(const BOUNDINGBOX& extra)
	{
		MIN_X = std::min(MIN_X, extra.MIN_X);
		MIN_Y = std::min(MIN_Y, extra.MIN_Y);
		MIN_Z = std::min(MIN_Z, extra.MIN_Z);

		MAX_X = std::max(MAX_X, extra.MAX_X);
		MAX_Y = std::max(MAX_Y, extra.MAX_Y);
		MAX_Z = std::max(MAX_Z, extra.MAX_Z);
	}

	void Enlarge(const VECTOR& point)
	{
		MIN_X = std::min(MIN_X, point.x);
		MIN_Y = std::min(MIN_Y, point.y);
		MIN_Z = std::min(MIN_Z, point.z);

		MAX_X = std::max(MAX_X, point.x);
		MAX_Y = std::max(MAX_Y, point.y);
		MAX_Z = std::max(MAX_Z, point.z);
	}

	bool OutOfBounds(const VECTOR& p)
	{
		return
			p.x < MIN_X || p.x > MAX_X ||
			p.y < MIN_Y || p.y > MAX_Y ||
			p.z < MIN_Z || p.z > MAX_Z;
	}

	int BoundsIntersect(RAY* ray, double mindist, double *maxdist);

	// Bounding-box limits
	double MIN_X;
	double MIN_Y;
	double MIN_Z;
	double MAX_X;
	double MAX_Y;
	double MAX_Z;
};


// Test whether two boundingboxes are disjunct
inline bool DisjunctBounds(const BOUNDINGBOX& b1, const BOUNDINGBOX& b2)
{
	return (b1.MIN_X >= b2.MAX_X) || (b2.MIN_X >= b1.MAX_X)
		|| (b1.MIN_Y >= b2.MAX_Y) || (b2.MIN_Y >= b1.MAX_Y)
		|| (b1.MIN_Z >= b2.MAX_Z) || (b2.MIN_Z >= b1.MAX_Z);
}

#endif // !_BOUNDS_H_


