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

// brep.h: An edge-based Boundary Representation of solids, similar
//         to the winged-edge data structure

#ifndef BREP_H
#define BREP_H

#include <list>
#include <algorithm>


// ****************** The Topological Data Structure ********************
//
// References:
//
// - Kevin Weiler, "Edge-based Data Structures for Solid Modeling in
//   Curved-Surface Environments", IEEE Computer graphics and Applications,
//   January, 1985
// - Martti Mantyla and Reijo Sulonen, "GWB: A Solid Modeler with Euler
//   Operators", IEEE Computer graphics and Applications, September 1982
// - Tony C. Woo, "A Combinatorial Analysis of Boundary Data Structure
//   Schemata", IEEE Computer graphics and Applications, March 1985
// - Andrew S. Glassner, "Maintaining Winged-Edge Models", Graphics Gems II,
//   James Arvo (editor), Academic Press, 1991
// - Mark Segal and Carlo H. Sequin, "Partitioning Polyhedral Objects into
//   Nonintersecting Parts", IEEE Computer graphics and Applications, January,
//   1988


// The boundary of a solid consists of one or more SHELLs
class BREP_SOLID;

// A shell is basically a collection of connected faces representing an
// orientable two-manifold. This means: a two-dimensional surface (in 3D
// space here), where each point on the surface has a neighbourhood
// topologically equivalent to an open disk. The surface must be orientable
// and closed. Orientable means that it is two-sided. There must be a clear
// distinction between its inside and its outside. The data structure
// here only represent the (adjacency) topology of a surface. It is up
// to the user of this data structure and library funtions to fill in
// the geometry information and make sure that it is geometrically
// consistent. That means that the assigned shape should be consistent with
// the topology

class BREP_SHELL;
typedef std::list<BREP_SHELL*> BREP_SHELL_RING;

// A face is a bounded open subset of a plane, bounded by one or more
// contours. One contour defines the outside of the face. Other contours
// describe its cavities. The contours describing cavities are called
// rings. The orientation of the outer contour must be counterclockwise as
// seen from the direction of the normal to the containing plane. Rings
// must be oriented clockwise. The cross product of a contour edge direction
// and the plane normal is always a vector that points to the outside of
// a face. Or in other words, the inside of a face is always on the left side
// of a contour. The geometric data supplied by the user must conform with
// this rule. The rings, if any, are pointed to by the outer contour.
// When building a face, the first contour being specified must be the
// outer contour (at least if your application relies on this
// interpretation)

class BREP_FACE;
typedef std::list<BREP_FACE*> BREP_FACE_RING;

// A contour represents a single closed planar polygonal curve (a loop).
// Self-intersecting contours are not allowed. Different contours may only
// intersect at common vertices or edges if at all. Here, we provide pointers
// to WINGs instead of EDGEs in order to have implicit information about
// the orientation of an edge as used in a contour. This facilitates
// iterating over the edges or vertices in a contour in a consistent manner.
// When building a contour, the edges must be specified in the right order:
// the inside of the face is always to the left of an edge. In a two-manifold,
// outer contours do not touch inner contours (they don't intersect
// and do not share vertices or edges). However, a vertex may be
// used several times in a contour and two outer or inner contours
// can share an edge, but only in opposite direction. If you need multiple
// pairs of contours sharing the same vertex, you probably want
// topologically different vertices at the same place in 3D space. Same if
// you believe you need inner and outer contours sharing a vertex

class BREP_CONTOUR;
typedef std::list<BREP_CONTOUR*> BREP_CONTOUR_RING;

// An edge is a unique connection between two vertices. There can be no more
// than one edge between two given vertices in a valid model. An edge can be
// used in two contours. The use of an edge in a contour is called a wing

class BREP_EDGE;

// A wing contains the starting vertex of the edge in the contour. The
// other vertex is the starting vertex in the other wing, since the edge should
// be used in opposite sense by the other contour sharing the edge.
// The invariant is: wing->vertex == OtherWingInEdge(wing->prev)->vertex:
// the starting point in a wing is the endpoint of the edge specified in
// wing->prev.
// The data structure contains two more pointers than a winged edge data
// structure, but the code to operate on it is significantly simpler, cfr.
// [Weiler 1985]. The data structure is more compact than in [Glassner 1991]

class BREP_WING;

// A doubly linked ring of wings indicates in what wings a vertex is being used.
// The code becomes a lot simpler and more efficient when explicitely
// remembering with each vertex what wings leave at the vertex. It also allows
// to deal easier with partial shells: shells not enclosing a region into space,
// such as a collection of coplanar faces, which is often used in synthetic
// environments

typedef std::list<BREP_WING*> BREP_WING_RING;

// A vertex is basically a point in 3D space

class BREP_VERTEX;
extern int VertexCompare(BREP_VERTEX* v1, BREP_VERTEX* v2);
extern int VertexCompareLocation(BREP_VERTEX* brepv1, BREP_VERTEX* brepv2);


// Tree with 8 children per node
class OctreeNode;
typedef OctreeNode BREP_VERTEX_OCTREE;

// ********************** Callback Functions **************************
// In order to maintain other than topological data with the entities,
// e.g. geometrical data such as coordinates of a vertex, plane equation
// of a face etc ..., a Close callback function is associated with
// each entity:
// it is called when the topological entity is completely specified.
// A vertex is completely specified when it is created, an edge is
// completely specified each time it is included in a contour.
// Contours, faces, shells and solids are considered completely
// specified only when the user lets it know to the library by calling
// CloseCallback().
// A Close callback is a good moment to compute plane equations
// for faces, to compute bounding boxes, to verify etc...
//
// Default callback functions: none. If a callback function is not set,
// no action is taken and nothing is changed to the data
// of an entity.

class BrepStruct
{
public:
	virtual void DataCloseCallback(bool done) = 0;

protected:
	virtual ~BrepStruct() {}
};


// Classes


class SOLID;
class BREP_SOLID :
	public BrepStruct
{
friend class SOLID;
public:
	BREP_SOLID();
	// Releases solid, also its vertices
	// if not used in other solids as well
	~BREP_SOLID();

	// Performs various actions to be done when the boundary
	// representation of the solid has been completely specified
	// (automatically calls shells->CloseCallback() for its
	// contained shells and so on)
	void CloseCallback(const bool done = false);

	// Connect a shell to the solid
	void ConnectShell(BREP_SHELL* shell);

	// Disconnect a shell from the solid
	void DisconnectShell(BREP_SHELL* shell);

	// Iterate through all contours inside the solid
	void IterateContours(void (*func)(BREP_CONTOUR*));

	//
	BREP_SHELL_RING shells;

	// Vertices, sorted in an octree for fast lookup
	BREP_VERTEX_OCTREE* vertices;
};


class SHELL;
class BREP_SHELL :
	public BrepStruct
{
friend class SHELL;
public:
	// Creates a new empty shell within the solid if solid is not a NULL
	// pointer. You will need to call CloseCallback() yourself to
	// close the shell and/or ConnectShell() to connect it
	// to a solid if you don't specify a solid to contain the shell with
	// this function
	BREP_SHELL(BREP_SOLID* solid);
	~BREP_SHELL();

	// Only needed for shells that were created
	// without specifying the enclosing solid
	void CloseCallback(const bool done = false);

	// Connect a face to the shell
	void ConnectFace(BREP_FACE* face);

	// Disconnect the face from its containing shell
	void DisconnectFace(BREP_FACE* face);

	BREP_FACE_RING faces;

	// Backpointer to the containing solid
	BREP_SOLID* solid;
};


class FACE;
class BREP_FACE :
	public BrepStruct
{
friend class FACE;
public:
	// Creates a new empty face within the shell
	BREP_FACE(BREP_SHELL* shell);
	// Release face and its contours, including
	// edges if not used in other faces as well
	~BREP_FACE();
	void CloseCallback(const bool done = false);

	// Connect a contour (the first one is supposed to be the outer contour
	void ConnectContour(BREP_CONTOUR* contour);
	// Disconnect a contour
	void DisconnectContour(BREP_CONTOUR* contour);

	// Contours form a doubly linked ring
	// in order to facilitate the splitting
	// and merging of contours. There is
	// one ring per face, containing the
	// outer contour as well as the rings
	BREP_CONTOUR_RING outer_contour;

	// Backpointer to the containing shell
	BREP_SHELL* shell;
};


class CONTOUR;
class BREP_CONTOUR :
	public BrepStruct
{
public:
	// Creates a new empty contour within the face.
	// 'face' cannot be a NULL pointer
	BREP_CONTOUR(BREP_FACE* face);
	~BREP_CONTOUR();

	void CloseCallback(const bool done = false);

	// Connect a wing
	void ConnectWing(BREP_WING* wing);

	// Disconnect a wing
	void DisconnectWing(BREP_WING* wing);

	// Create a wing into the contour
	BREP_WING* CreateWing(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);

	// Destroy the wings
	void DestroyWings();

	// Remove a vertex (the vertex is not deleted)
	void RemoveVertex(BREP_VERTEX* vertex);

	// Execute a function for every wing:
	// iterate over the wings in the contour
	// (when properly built, the inside of the face
	// containing the contour is always on the left side of an edge)
	void IterateWings(void (*func)(BREP_WING*));
	void IterateWings1A(void (*func)(BREP_WING*, void*), void* parm);

	// Iterates over the vertices in a contour.
	// Same order as when iterating over the edges
	void IterateVertices(void (*func)(BREP_VERTEX*));

	BREP_WING* wings;

	// Backpointer to the containing face
	BREP_FACE* face;
};

extern BREP_CONTOUR* CreateContour(BREP_FACE* face);


class BREP_WING
{
public:
	BREP_WING(BREP_VERTEX* v);
	~BREP_WING() {}

	// Start vertex
	BREP_VERTEX* vertex;
	// ClockWise and CounterClockWise
	// next wing within outer contours
	BREP_WING* prev;
	BREP_WING* next;

	// Pointer to the containing edge
	BREP_EDGE* edge;

	// Backpointer to the containing contour
	BREP_CONTOUR* contour;

};


class BREP_EDGE
{
public:
	// Creates an edge connecting vertex1 to vertex2
	// (it is not connected to a contour)
	BREP_EDGE(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);
	~BREP_EDGE();

	// Actions to be performed when an edge has been specified completely,
	// i.e. it is enclosed in two contours
	void CloseCallback(const bool done = false) {}

	// Two wings
	BREP_WING* wing1;
	BREP_WING* wing2;
};

extern BREP_EDGE* CreateEdge(BREP_VERTEX* v1, BREP_VERTEX* v2);


class BREP_VERTEX
{
public:
	// Creates a new vertex. The new vertex is not installed it in a
	// vertex octree as vertices do not have to be sorted in an octree
	BREP_VERTEX();
	~BREP_VERTEX();

	// Disconnect the wing from its starting vertex
	void DisconnectWing(BREP_WING* wing);

	// Iterate over the wings leaving at the vertex, in arbitrary order
	void IterateWings(void (*func)(BREP_WING*))
	{
		BREP_WING_RING::iterator wing;

		for(wing = wing_ring.begin(); wing != wing_ring.end(); wing++)
			func(*wing);
	}

	// Ring of wings leaving at the vertex
	BREP_WING_RING wing_ring;
};

// Tree with 8 children per node

class OctreeNode
{
public:
	OctreeNode(BREP_VERTEX* newelement)
	{
		pelement = newelement;
		for(int i = 0; i < 8; i++)
			child[i] = 0;
	}

	~OctreeNode()
	{
		for(int i = 0; i < 8; i++)
			if(child[i])
				delete child[i];

		delete pelement;
	}

	// Attaches an existing BREP_VERTEX to the vertex octree,
	// allows duplicate elements in the octree
	OctreeNode* AddWithDuplicates(BREP_VERTEX* newelement)
	{
		OctreeNode* o = 0;
		OctreeNode* p = this;
		int cmp = 8;

		while(p)
			{
				cmp = VertexCompare(p->pelement, newelement);

				// Allow duplicate items in the octree
				o = p;
				p = p->child[cmp%8];
			}

		// P is null, o is the octree node where we should add the new child
		p = new OctreeNode(newelement);

		if(o)
			{
				// Add the new node to the old octree
				o->child[cmp%8] = p;
				return this;
			}
		else
			// New octree node
			return p;
	}

	// This routine looks up the element in the octree using the element
	// comparison function specified. If the element is found, a pointer to the
	// OCTREE cell where the element is stored is returned, else 0 is returned.
	// The VertexCompare function returns 8 if the elements are
	// equal and the branch to be investigated if they are different.
	OctreeNode* FindSubtree(BREP_VERTEX* element, bool comparelocation)
	{
		OctreeNode* p = this;

		while(p)
			{
				int cmp = comparelocation ? VertexCompareLocation(p->pelement, element) : VertexCompare(p->pelement, element);
				if(cmp >= 8)
					return p;

				p = p->child[cmp];
			}

		return 0;
	}

	// Look up a vertex in the vertex octree, return 0 if not found;
	// the nodecmp function returns 8 if the elements are equal
	// and the branch to be investigated if not
	BREP_VERTEX* Find(BREP_VERTEX* element)
	{
		OctreeNode* p = FindSubtree(element, false);

		if(p)
			return p->pelement;

		return 0;
	}

	void Iterate(void (*func)(BREP_VERTEX*))
	{
		for(int i = 0; i < 8; i++)
			if(child[i])
				child[i]->Iterate(func);

		func(pelement);
	}

private:
	BREP_VERTEX* pelement;
	OctreeNode* child[8];
};


// No error message from the library is longer than this
#define BREP_MAX_MESSAGE_LENGTH 200


//********************** Contructors *****************************

extern BREP_WING* BrepCreateWingWithoutContour(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);


//*********************** Modifiers *******************************

// Splits only one wing. If a full edge is to be split, also its
// wing in the other contour (if any) needs to be split at the same vertex.
// (BrepSplitEdge does this).
// Returns the wing leaving at the inserted vertex.
extern BREP_WING* BrepSplitWing(BREP_WING* wing, BREP_VERTEX* vertex);

// Joins two wings at there common endpoint. The wings are supposed to
// be consecutive wings, i.e. wing1->next = wing2 (and wing2->prev = wing1)
// in a closed contour. The common vertex is wing2->vertex,
// the start vertex of the second wing (which is supposed to be
// the endpoint of the first wing as well). Returns the newly
// created wing.
extern BREP_WING* BrepJoinWings(BREP_WING* wing1, BREP_WING* wing2);

// wing1 and wing2 should be two wings belonging to the same contour.
// This routine creates a new edge connecting the start vertex of wing1
// and wing2, splitting the contour in two. A pointer to the newly created
// wing, connecting the start vertex of wing1 to the start vertex of wing2,
// is returned.
extern BREP_WING* BrepMakeEdgeSplitContour(BREP_WING* wing1, BREP_WING* wing2);

// Wing1 and wing2 are two wings belonging to a different contour. This
// routine creates a new edge connecting the starting vertices of
// wing1 and wing2 and merges to two contours to one. A pointer to
// the newly created wing, connecting the start vertex of wing1 to the
// start vertex of wing2, is returned.
extern BREP_WING* BrepMakeEdgeJoinContours(BREP_WING* wing1, BREP_WING* wing2);

// Creates a notch, connecting the start vertex of the wing with
// the specified vertex, ans inserts it into the contour to
// which the wing belongs. A pointer to the newly created
// wing, from the start vertex of the specified wing to the
// specified vertex, is returned.
extern BREP_WING* BrepMakeNotch(BREP_WING* wing, BREP_VERTEX* vertex);

// Creates a slit, connecting the two specified vertices,
// in the given face. A slit is a new contour containing
// two wings: one from v1 to v2 and the second
// from v2 back to v1. A pointer to the newly created
// wing, connecting v1 to v2, is returned.
extern BREP_WING* BrepMakeSlit(BREP_FACE* face, BREP_VERTEX* v1, BREP_VERTEX* v2);


//******************* Selectors and Iterators *************************
// adding and removing elements in the ring being iterated over is restricted
// to certain conditions: the first element being iterated over must remain
// in the ring and the next element of an element being passed must remain
// the same as well.

// Returns the other wing in the edge
extern BREP_WING* BrepEdgeOtherWing(BREP_WING* wing);

// Looks whether an edge connecting the two vertices already
// exists, Returns it if it exists. Returns NULL if not.
extern BREP_EDGE* BrepFindEdge(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);


//*********************** Destructors ****************************

// remove a wing from a contour, release the storage associated with the
// edge if it is not used in other contours as well. This routine will
// in general create a hole in a contour: the endpoint of a previous edge
// might not be the starting point of the next one where an edge has been
// deleted. The vertices are not deleted.
extern void BrepDestroyWing(BREP_WING* wing);


//********************* vertex octrees ***************************
// vertices can also be sorted and stored into an octree in order to make
// searching for a vertex significantly faster.

// Before trying to store vertices in an octree, first a routine should
// be specified to compare the client data of two vertices.
// The routine specified should return a code with the following meaning:
//
//	0:  x1 <= x2 , y1 <= y2, z1 <= z2 but not all are equal
//	1:  x1 >  x2 , y1 <= y2, z1 <= z2
//	2:  x1 <= x2 , y1 >  y2, z1 <= z2
//	3:  x1 >  x2 , y1 >  y2, z1 <= z2
//	4:  x1 <= x2 , y1 <= y2, z1 >  z2
//	5:  x1 >  x2 , y1 <= y2, z1 >  z2
//	6:  x1 <= x2 , y1 >  y2, z1 >  z2
//	7:  x1 >  x2 , y1 >  y2, z1 >  z2
//	8:  x1 == x2 , y1 == y2, z1 == z2
//
// in other words:
//
//	code&1 == 1 if x1 > x2 and 0 otherwise
//	code&2 == 1 if y1 > y2 and 0 otherwise
//	code&4 == 1 if z1 > z2 and 0 otherwise
//	code&8 == 1 if x1 == x2 and y1 == y2 and z1 == z2

// Iterator over all edge-wings between vertices
// at the locations specified by v1data and v2data
extern void BrepIterateWingsBetweenLocations(BREP_VERTEX* v1, BREP_VERTEX* v2,
	BREP_VERTEX_OCTREE* vertices, void (*func)(BREP_WING*));

#endif // BREP_H


