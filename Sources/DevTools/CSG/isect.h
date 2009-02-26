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

// isect.*: partitioning faces into non intersecting parts,
//
// References:
// - Mark Segal abd Carlo H. Sequin, "Partitioning Polyhedral Objects into
//	Nonintersecting Parts", IEEE CG&A January 1988 p 53
// - Mark Segal, "Using Tolerances to Guarantee Valid Polyhedral Modeling Results",
//	SIGGRAPH 1990 p 105

#ifndef _ISECT_H_
#define _ISECT_H_

#include "geometry.h"

// faceA and faceB are to BREP_FACES with appropriate geometric data
// (see geometry.[ch]). This routine checks whether the faces are coplanar,
// intersect transversally or don't intersect at all. In the first case,
// COPLANAR_ISECT is returned. In the second case, TRANSVERSAL_ISECT.
// In the last case NO_ISECT. If the faces intersect
// transversally, the faces are partitionned along the intersection line.
// If a contour of a face is split in two, a flag OUTMASK or INMASK in the contours
// id is set indicating whether the contour is on the "outside" of the other face or
// on the "inside". The flags are defined in geometry.h. This flag can be
// used to classify objects w.r.t. each other for e.g. CSG operations
const int NO_ISECT = 0;
const int COPLANAR_ISECT = 1;
const int TRANSVERSAL_ISECT = 2;
int PartitionFaces(FACE* faceA, FACE* faceB);

// Sets the minimum allowed coplanarity factor of two faces (see Segal, SIGGRAPH '90
// p 111). The coplanarity factor indicates to what extent two faces are coplanar:
// truely coplanar faces have an infinitely large coplanarity factor. For
// perpendicular faces however, the coplanarity factor is sqrt(2). In
// order to decide whether two faces are to be considered coplanar, their
// coplanarity factor is compared with a minimum coplanarity factor, which
// should be a number between 100 and 1000. Smaller values force the algorithm
// to deem pairs of faces that intersect in a relatively large dehedral
// angle to be coplanar. 500 is a nice default value
void SetMinCoplanarityFactor(double factor);

#endif // !_ISECT_H_


