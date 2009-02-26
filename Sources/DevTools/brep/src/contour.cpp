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

// Creates a new empty contour within the face
BREP_CONTOUR::BREP_CONTOUR(BREP_FACE* containingface)
{
	wings = 0;
	face = containingface;

	if(containingface)
		face->ConnectContour(this);
}

// Various actions to be performed when a contour is specified completely. There
// is no iteration over the edges since the edges may occur in two contours. For
// edges, there are two constructor callbacks: CreateEdge, which is called when first
// inserting the edge in a contour, and CloseEdge callback, which is called when the
// edge is inserted a second time in a contour
void BREP_CONTOUR::CloseCallback(const bool done)
{
	// Notify the user that the contour is complete
	if(!done)
		DataCloseCallback(true);
}

// Release all storage associated with a contour and its contours,
// including edges and vertices if not used in other contours as well
BREP_CONTOUR::~BREP_CONTOUR()
{
	// Disconnect the contour from the containing face
	face->DisconnectContour(this);

	// Destroy its wings
	DestroyWings();
}


// Connect the wing as last wing in the contour
void BREP_CONTOUR::ConnectWing(BREP_WING* wing)
{
	wing->contour = this;

	if(!wings)
		{
			// First wing in contour
			wing->next = wing->prev = wing;
			wings = wing;
		}
	else
		{
			// Not first wing
			wing->next = wings;
			wing->prev = wings->prev;
			wing->prev->next = wing->next->prev = wing;
		}
}

// Disconnect the wing from the contour. Use with care: the contour
// might contain holes after disconnecting the wing! wing->next
// and wing->prev are left untouched as these fields are needed
// when disconnecting an edge from its vertices.
void BREP_CONTOUR::DisconnectWing(BREP_WING* wing)
{
	if(!wing->next || !wing->prev)
		BrepError(wing->edge, "BrepDisconnectWingFromContour", "wing improperly connected to contour");

	if(wings == wing)
		{
			// The wing is the first wing in the contour
			if(wing->next == wing)
				// It is the only wing
				wings = 0;
			else
				// Make the next wing the first wing of the contour
				wings = wing->next;
		}

	// Endpoint of wing->prev will in general not be the same as
	// startpoint of wing->next! A hole might be created in the contour
	if(wing->next)
		wing->next->prev = wing->prev;
	if(wing->prev)
		wing->prev->next = wing->next;

	wing->contour = 0;
	wing->next = wing->prev = 0;
}


// Creates a wing into the contour from vertex1 to vertex2. The
// inside of the face bounded by the contour is supposed to be on the
// left side. The wing being created is supposed to be the new last
// one in a contour, i.e. its starting vertex 'vertex1' must be the
// endvertex of the most recently created wing in the contour (if
// non-empty). It is the responsibility of the user to close the
// contour, i.e., the last edge in a contour should connect to the
// first one
BREP_WING* BREP_CONTOUR::CreateWing(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
	if(wings && BrepEdgeOtherWing(wings->prev)->vertex != vertex1)
		{
			BrepError(this, "BrepCreateWing", "the starting point of a new wing should be the endpoint of the previous wing");
			return 0;
		}

	// First create a wing that still needs to be connected with a contour
	BREP_WING* wing = BrepCreateWingWithoutContour(vertex1, vertex2);

	// Complain if the edge is not used in opposite sense in both directions
	if(wing->vertex != vertex1)
		BrepError(wing->edge, "BrepCreateWing", "edge should be used in opposite sense by the contours sharing it");

	// Connect the wing to the contour
	ConnectWing(wing);

	// Connect the wing to its start vertex
	// after it has been connected in a contour
	(wing->vertex)->wing_ring.push_back(wing);

	// Close the edge
	(wing->edge)->CloseCallback();

	return wing;
}


// Destroys the wings
void BREP_CONTOUR::DestroyWings()
{
	BREP_WING* first = wings;

	if(!first)
		return;

	BREP_WING* prev;
	for(BREP_WING* wing = first->prev; wing != first; wing = prev)
		{
			prev = wing->prev;
			BrepDestroyWing(wing);
		}

	BrepDestroyWing(first);
}


// Execute func for every wing in the contour
void BREP_CONTOUR::IterateWings(void (*func)(BREP_WING*))
{
	BREP_WING* first = wings;

	if(!first)
		return;

	BREP_WING* next = first;

	do
		{
			BREP_WING* wing = next;
			next = wing->next;
			func(wing);
		}
	while(next != first);
}

void BREP_CONTOUR::IterateWings1A(void (*func)(BREP_WING*, void*), void* parm)
{
	BREP_WING* first = wings;

	if(!first)
		return;

	BREP_WING* next = first;

	do
		{
			BREP_WING* wing = next;
			next = wing->next;
			func(wing, parm);
		}
	while(next != first);
}

// Execute func for every vertex in the contour
void BREP_CONTOUR::IterateVertices(void (*func)(BREP_VERTEX*))
{
	BREP_WING* first = wings;

	if(!first)
		return;

	BREP_WING* next = first;

	do
		{
			BREP_WING* wing = next;
			next = wing->next;
			func(wing->vertex);
		}
	while(next != first);
}


// Remove a vertex from a (closed) contour, the wings containing the vertex are
// removed as well (and their edges if not used in other contours as
// well). The vertex itself will also be removed when it is no longer
// used. If it is not the only vertex in the contour, a new wing connecting
// the neighbooring vertices is created, so the contour remains a loop
void BREP_CONTOUR::RemoveVertex(BREP_VERTEX *vertex)
{
	// Check for empty contour
	if(!wings)
		{
			BrepError(this, "BrepContourRemoveVertex", "empty contour");
			return;
		}

	// If vertex is the only vertex in the contour, remove its self-edge
	if(wings->next == wings)
		if(wings->vertex != vertex)
			{
				BrepError(this, "BrepContourRemoveVertex", "vertex not belonging to contour or contour is not closed");
				return;
			}
		else
			BrepDestroyWing(wings);

	// Else, replace the wing arriving at and the wing leaving the
	// last occurence of the vertex in the contour by a single wing
	// connecting the previous and next vertex

	// Look for the last occurence of the vertex in the contour
	BREP_WING* wing = wings->prev;
	while(wing->vertex != vertex && wing != wings)
		wing = wing->prev;

	if(wing->vertex != vertex)
		BrepError(this, "BrepContourRemoveVertex", "vertex doesn't occur in contour");
	else
		BrepJoinWings(wing->prev, wing);
}


