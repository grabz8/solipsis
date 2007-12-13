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

#include "brep.h"
#include "private.h"


// Creates a new vertex - no edges in which the vertex is used are specified,
// this is done when creating an edge
BREP_VERTEX::BREP_VERTEX()
{
	// A vertex is completely specified by its 3D coordinates
	//CloseCallback();
}

// Release all storage associated with the vertex. The vertex is supposed not
// to be used anymore in any edge. Disconnecting vertices and edges happens
// automatically when destroying edges
BREP_VERTEX::~BREP_VERTEX()
{
	// check whether the vertex is still being used in edges
	if(!wing_ring.empty())
		BrepInfo(this, "BrepDestroyVertex", "vertex still being used in edges!");
}


// Disconnect the wing from its starting vertex
// (the wing must be properly connected to a contour)
void BREP_VERTEX::DisconnectWing(BREP_WING* wing)
{
	// Sanity check...
	// Looks whether the wing is referenced in
	// the wing ring of its starting vertex
	BREP_WING_RING::iterator thewing;
	thewing = find(wing_ring.begin(), wing_ring.end(), wing);
	if(thewing == wing_ring.end())
		{
			BrepError(wing->edge, "BrepDisconnectWingFromVertex", "wing not connected to the vertex");
			return;
		}

	// Remove the wing element from the vertex wing ring
	wing_ring.erase(thewing);
}


