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

// Edge.cpp: routines for manipulating BREP_EDGEs and BREP_WINGs

#include "brep.h"
#include "private.h"

// Creates an edge connecting vertex1 to vertex2.
// It is not connected to a contour
BREP_EDGE::BREP_EDGE(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
	wing1 = new BREP_WING(vertex1);
	wing2 = new BREP_WING(vertex2);

	wing1->edge = wing2->edge = this;
}

// Release all storage associated with an edge and its vertices
// if not used in other edges as well. The given edge must already
// be disconnected from its contours
BREP_EDGE::~BREP_EDGE()
{
	// Check if the edge is still being used in a contour.
	// If so, warn the world
	if(wing1->contour || wing2->contour)
		BrepWarning(this, "~BREP_EDGE", "Edge is still used in a contour");
}


// Initialize the wing (it still needs to be connected with a contour)
BREP_WING::BREP_WING(BREP_VERTEX* v) :
	vertex(v)
{
	prev = next = 0;
	edge = 0;
	contour = 0;
}


