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

// Connect the wing to its starting vertex
// (the wing must have been connected to its contour before)
static void BrepConnectWingToVertex(BREP_WING* wing)
{
	if(wing)
		(wing->vertex)->wing_ring.push_back(wing);
}

// wing1 and wing2 should be two wings belonging to the same contour.
// This routine creates a new edge connecting the start vertex of wing1
// and wing2, splitting the contour in two. A pointer to the newly created
// wing, connecting the start vertex of wing1 to the start vertex of wing2,
// is returned
BREP_WING* BrepMakeEdgeSplitContour(BREP_WING *wing1, BREP_WING *wing2)
{
	// Test whether the wings belong to the same contour
	if(wing1->contour != wing2->contour)
		{
			BrepError(wing1->edge, "BrepMakeEdgeSplitContour", "wings must belong to the same contour");
			return 0;
		}

	// Create an edge connecting the two start vertices
	BREP_EDGE* edge = CreateEdge(wing1->vertex, wing2->vertex);
	BREP_WING* winga = edge->wing1;
	BREP_WING* wingb = edge->wing2;

  // connect the wings of the newly created edge to the contour,
//   * effectively breaking the contour in two loops: one starting
//   * with wing1 and one starting with wing2.
  winga->prev = wing1->prev;
  winga->next = wing2;
  wingb->prev = wing2->prev;
  wingb->next = wing1;
  winga->prev->next = winga->next->prev = winga;
  wingb->prev->next = wingb->next->prev = wingb;
  winga->contour = wingb->contour = wing1->contour;

  // Let the old contour point to the wing1 loop
  wing1->contour->wings = wing1;

  // Create a new contour for the wing2 loop
  wing2->contour = CreateContour(wing1->contour->face);
  wing2->contour->wings = wing2;

	// Let all wings in the wing2 loop point to the new contour
	BREP_WING* wing = wing2;
  do {
    wing->contour = wing2->contour;
    wing = wing->next;
  } while (wing != wing2);

  // Connect the newly created wings to their start vertices
  BrepConnectWingToVertex(winga);
  BrepConnectWingToVertex(wingb);

	// Notify the user that the edge has
	// been connected into two contours
	edge->CloseCallback();

	// Notify the user that two new contours have been completely specified
	//BrepUpdateContour(wing1->contour);
	(wing2->contour)->CloseCallback();
	(wing1->contour)->CloseCallback();

  // Return a pointer to the wing connecting wing1->vertex to wing2->vertex
  return winga;
}

// Wing1 and wing2 are two wings belonging to a different contour. This
// routine creates a new edge connecting the starting vertices of
// wing1 and wing2 and merges to two contours to one. A pointer to
// the newly created wing, connecting the start vertex of wing1 to the
// start vertex of wing2, is returned
BREP_WING* BrepMakeEdgeJoinContours(BREP_WING* wing1, BREP_WING* wing2)
{
  // Test whether the wings belong to a different contour
  if (wing1->contour == wing2->contour) {
    BrepError(wing1->edge, "BrepMakeEdgeJoinContours", "wings must belong to a different contour");
    return 0;
  }

  // Test whether the wings belong to the same face
  if (wing1->contour->face != wing2->contour->face) {
    BrepWarning(wing1->edge, "BrepMakeEdgeJoinContours", "joining contours from different faces");
    // It's not an error ...
  }

	// Create an edge connecting the two start vertices
	BREP_EDGE* edge = CreateEdge(wing1->vertex, wing2->vertex);
	BREP_WING* winga = edge->wing1;
	BREP_WING* wingb = edge->wing2;

  // connect the wings of the newly created edge to the contour to which
//   * wing1 belongs.
  winga->prev = wing1->prev;
  winga->next = wing2;
  wingb->prev = wing2->prev;
  wingb->next = wing1;
  winga->prev->next = winga->next->prev = winga;
  wingb->prev->next = wingb->next->prev = wingb;

	// Unless the contour of wing2 is the outer contour of its face,
	// "move" its wings to the contour of wing1, and destroy the contour
  if (wing2->contour != wing2->contour->face->outer_contour.front()) {
    winga->contour = wingb->contour = wing1->contour;

    // Disconnect wing2->contour from its wings and destroy it
    wing2->contour->wings = 0;
	delete (wing2->contour);

    // Let all wings in the former contour of wing2 point to the contour of wing1
	BREP_WING* wing;
    for (wing = wing2; wing->contour != wing1->contour; wing = wing->next)
      wing->contour = wing1->contour;
  } else {
    winga->contour = wingb->contour = wing2->contour;

    // Disconnect wing1->contour from its wings and destroy it
    wing1->contour->wings = 0;
	delete (wing1->contour);

    // Let all wings in the former contour of wing2 point to the contour of wing1
	BREP_WING* wing;
    for (wing = wing1; wing->contour != wing2->contour; wing = wing->next)
      wing->contour = wing2->contour;
  }

  // Connect the newly created wings to their start vertices
  BrepConnectWingToVertex(winga);
  BrepConnectWingToVertex(wingb);

	// Notify the user that the edge has
	// been connected into two contours
	edge->CloseCallback();

	// Notify the user that the contour has been updated.
	//BrepUpdateContour(wing1->contour);
	(winga->contour)->CloseCallback();

  // Return a pointer to the wing connecting wing1->vertex to wing2->vertex
  return winga;
}

// Creates a slit, connecting the two specified vertices,
// in the given face. A slit is a new contour containing
// two wings: one from v1 to v2 and the second
// from v2 back to v1. A pointer to the newly created
// wing, connecting v1 to v2, is returned
BREP_WING* BrepMakeSlit(BREP_FACE *face, BREP_VERTEX *v1, BREP_VERTEX *v2)
{
	// Create a new contour in the face
	BREP_CONTOUR* contour = CreateContour(face);

	// Create an edge connecting the two vertices
	BREP_EDGE* edge = CreateEdge(v1, v2);
	BREP_WING* winga = edge->wing1;
	BREP_WING* wingb = edge->wing2;

  // connect the wings of the newly created edge to the newly created
//   * contour and to each other
  winga->prev = winga->next = wingb;
  wingb->prev = wingb->next = winga;
  winga->contour = wingb->contour = contour;
  contour->wings = winga;

  // Connect the newly created wings to their start vertices
  BrepConnectWingToVertex(winga);
  BrepConnectWingToVertex(wingb);

	// Notify the user that the edge has
	// been connected into two contours
	edge->CloseCallback();

	// Notify the user that the new contour has been completely specified
	contour->CloseCallback();

  // Return the wing connecting wing->vertex to vertex
  return winga;
}

// Creates a notch, connecting the start vertex of the wing with
// the specified vertex, and inserts it into the contour to
// which the wing belongs. A pointer to the newly created
// wing, from the start vertex of the specified wing to the
// specified vertex, is returned
BREP_WING* BrepMakeNotch(BREP_WING* wing, BREP_VERTEX* vertex)
{
	// Create an edge connecting the start vertex
	// of the wing to the specified vertex
	BREP_EDGE* edge = CreateEdge(wing->vertex, vertex);
	BREP_WING* winga = edge->wing1;
	BREP_WING* wingb = edge->wing2;

  // Connect the wings of the newly created edge to the contour
  winga->prev = wing->prev;
  winga->next = wingb;
  wingb->prev = winga;
  wingb->next = wing;
  winga->prev->next = winga->next->prev = winga;
  wingb->prev->next = wingb->next->prev = wingb;
  winga->contour = wingb->contour = wing->contour;

  // Connect the newly created wings to their start vertices
  BrepConnectWingToVertex(winga);
  BrepConnectWingToVertex(wingb);

  // notify the user that the edge has been connected into two
	// contours.
	edge->CloseCallback();

  // Notify the user that the contour has been updated.
  // BrepUpdateContour(wing->contour);

  // Return the wing connecting wing->vertex to vertex
  return winga;
}

// Creates a wing from vertex1 to vertex2. First looks for an
// existing edge connecting the vertices. Creates such an edge if
// if there is not yet one. Returns a pointer to an unused wing in
// the edge. If there is no unused wing in this edge, complains, and
// creates a second edge between the two vertices.
// The wing still needs to be connected in a contour
BREP_WING* BrepCreateWingWithoutContour(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
	BREP_WING *wing;

	// First look if there is already an edge
	// connecting the two vertices
	BREP_EDGE* edge = BrepFindEdge(vertex1, vertex2);
	if(!edge)
		{
			// There was not yet such an edge
			// Create such an edge
			edge = CreateEdge(vertex1, vertex2);

			// The wing to be used is the first wing --- vertex1
			// is the startvertex for the wing to be created
			wing = edge->wing1;
		}
	else
		{
			// There was already such an edge
			if(edge->wing1->vertex == vertex1)
				// Vertex1 is startvertex in the first wing?
				wing = edge->wing1;
			else if(edge->wing2->vertex == vertex1)
				// Vertex1 is startvertex in the second wing
				wing = edge->wing2;
			else
				{
					// Shouldn't happen
					BrepFatal(edge, "BrepCreateWingWithoutContour", "something impossible wrong here");
					// Useless, but compiler warning
					return 0;
				}

			// Is the wing found already used in a contour?
			if(wing->contour)
				{
					BrepInfo(edge, "BrepCreateWing", "attempt to use an edge two times in the same sense - creating a duplicate edge");
					edge = CreateEdge(vertex1, vertex2);
					wing = edge->wing1;
				}
		}

	return wing;
}

// Splits only one wing. If a full edge is to be split, also its
// wing in the other contour (if any) needs to be split at the same vertex.
// (BrepSplitEdge does this).
// Returns the wing leaving at the inserted vertex
BREP_WING* BrepSplitWing(BREP_WING* wing, BREP_VERTEX* vertex)
{
	// Create two new wings, not connected to the contour
	BREP_WING* wing1 = BrepCreateWingWithoutContour(wing->vertex, vertex);
	BREP_WING* wing2 = BrepCreateWingWithoutContour(vertex, wing->next->vertex);

	// Now connect the pair of consecutive wings to the contour
	wing1->contour = wing2->contour = wing->contour;

	wing1->next = wing2;
	wing2->prev = wing1;

	if(wing->prev != wing)
		{
			// Wing is not the only wing in the contour
			wing1->prev = wing->prev;
			wing2->next = wing->next;
		}
	else
		{
			// Wing is the only wing in the contour
			wing1->prev = wing2;
			wing2->next = wing1;
		}

	// Destroy the original wing, leaves the contour with
	// a hole in it or even a contour without wings ...
	BrepDestroyWing(wing);

	// ... fill the hole
	wing1->prev->next = wing1;
	wing2->next->prev = wing2;

	// And make sure the contour knows about the news wings
	if(!wing1->contour->wings)
		wing1->contour->wings = wing1;

	// Finally, connect the new wings to their startvertex
	BrepConnectWingToVertex(wing1);
	BrepConnectWingToVertex(wing2);

	// Close the edges
	wing1->edge->CloseCallback();
	wing2->edge->CloseCallback();

	return wing2;
}

// Joins two wings at there common endpoint. The wings are supposed to
// be consecutive wings, i.e. wing1->next = wing2 (and wing2->prev = wing1)
// in a closed contour. The common vertex is wing2->vertex,
// the start vertex of the second wing (which is supposed to be
// the endpoint of the first wing as well)
BREP_WING* BrepJoinWings(BREP_WING* wing1, BREP_WING* wing2)
{
	// Create a new wing connecting the startpoint
	// of wing1 with the endpoint of wing2
	BREP_WING* wing = BrepCreateWingWithoutContour(wing1->vertex, wing2->next->vertex);

	// Connect it to the contour
	wing->contour = wing1->contour;

	if(wing1->prev != wing2)
		{
			// The two wings were not the only two in the contour
			wing->prev = wing1->prev;
			wing->next = wing2->next;
		}
	else
		{
			// The two wings were the only two in the contour
			wing->prev = wing->next = wing;
		}

	// Destroy the two wings, this leaves a hole in the contour,
	// or if the wings were the only two, a contour without wings ...
	BrepDestroyWing(wing1);
	BrepDestroyWing(wing2);

	// ... fill the hole
	wing->prev->next = wing;
	wing->next->prev = wing;

	// Make sure the contour knows about the new wing
	if(!wing->contour->wings)
		wing->contour->wings = wing;

	// Connect the new wing to its starvertex
	BrepConnectWingToVertex(wing);

	// Close the edge
	wing->edge->CloseCallback();

	return wing;
}

// Returns the other wing in the edge
BREP_WING* BrepEdgeOtherWing(BREP_WING* wing)
{
	return (wing == wing->edge->wing1 ?
		wing->edge->wing2 :
		wing->edge->wing1);
}

// Remove a wing from a contour, release the storage associated with the
// edge and its vertices if it is not used in other contours as well.
// Inverse of BrepCreateWing(), but also destroys unused vertices
void BrepDestroyWing(BREP_WING* wing)
{
	// First disconnect the wing from its starting vertex
	wing->vertex->DisconnectWing(wing);

	// Then disconnect the wing from its contour
	wing->contour->DisconnectWing(wing);

	// Destroy the edge if not being used in any contour anymore
	if(!wing->edge->wing1->contour && !wing->edge->wing2->contour)
		delete wing->edge;
}

// Looks whether an edge connecting the two vertices already
// exists, returns it if it exists, returns NULL if not
BREP_EDGE* BrepFindEdge(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
	BREP_WING_RING ring;
	BREP_WING_RING::iterator wingiterator;

	// There is an edge if there is a wing from vertex2 to vertex1 ...
	ring = vertex2->wing_ring;
	for(wingiterator = ring.begin(); wingiterator != ring.end(); wingiterator++)
		{
			BREP_WING* wing = *wingiterator;
			if(BrepEdgeOtherWing(wing)->vertex == vertex1)
				return wing->edge;
		}

	// ... or from vertex1 to vertex2
	ring = vertex1->wing_ring;
	for(wingiterator = ring.begin(); wingiterator != ring.end(); wingiterator++)
		{
			BREP_WING* wing = *wingiterator;
			if(BrepEdgeOtherWing(wing)->vertex == vertex1)
				return wing->edge;
		}

	// No edge connecting the two vertices was found
	return 0;
}


// Iterator over all edge-wings between vertices at
// the locations specified by v1 and v2

void (*wing_func)(BREP_WING*);

static void BrepIterateWings(BREP_VERTEX *vertex)
{
	vertex->IterateWings(wing_func);
}

// Looks up the first vertex in the vertex octree at the location as specified in the
// given vertex data. There may be multiple vertices at the same location (e.g. having
// a different normal and/or a different name). These vertices should normally be
// stored as a suboctree of the octree containing all vertices. A pointer to the
// top of this suboctree is returned if there are vertices at the given location.
// 0 is returned if there are no vertices at the given location
void BrepIterateWingsBetweenLocations(BREP_VERTEX* v1, BREP_VERTEX* v2,
	BREP_VERTEX_OCTREE* vertices, void (*func)(BREP_WING*))
{
	BREP_VERTEX_OCTREE* v1octree = vertices->FindSubtree(v1, true);
	if(!v1octree)
		return;

	BREP_VERTEX_OCTREE* v2octree = vertices->FindSubtree(v2, true);
	if(!v2octree)
		return;

	wing_func = func;
	v1octree->Iterate(BrepIterateWings);
	v2octree->Iterate(BrepIterateWings);
}


