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

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <brep/brep.h>
#include "bounds.h"

#define M_PI       3.14159265358979323846

// See Mark Segal, "Using Tolerances to Guarantee Valid Polyhedral Modeling
// Results", SIGGRAPH '90 p 105 for everything about tolerances, extents and
// near values and what they are used for.

// Struct with common first field of VERTEX, EDGE, CONTOUR, ...

// All entities have an unsigned integer used for identification and
// to hold flags for various other purposes, such as to indicate whether
// a contour lies on the inside or outside of another object after
// partitioning for e.g. CSG operations (see "various other masks" below)

// Masks to indicate whether a contour lies inside or outside another
// object after partitioning with PartitionFaces() in isect.cpp
const unsigned int OUTMASK = 1;
const unsigned int INMASK = 2;


extern double vertex_tolerance;
extern double edge_tolerance;
extern double face_tolerance;


// Geometric data for a vertex
class VERTEX :
	public BREP_VERTEX
{
public:
	VERTEX()
	{
		tolerance = vertex_tolerance;
		normal = VECTOR(0, 0, 0);
	}

	~VERTEX() {}

	void DataCloseCallback(const bool done = false)
	{
		ComputeTolerance();
	}

	void ComputeNormal();
	void ComputeTolerance();

	void EnlargeTolerance(double tol);

	// Coordinates in 3D space
	VECTOR point;

	// Normal vector to the surface at the vertex
	VECTOR normal;

	// Coordinate tolerance. Two vertices nearer than
	// this should be considered one vertex
	double tolerance;
};


// Geometric data for an edge
class EDGE :
	public BREP_EDGE
{
public:
	EDGE(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2) :
		BREP_EDGE(vertex1, vertex2)
	{
		tolerance = edge_tolerance;
	}

	~EDGE() {}

	void DataCloseCallback(bool done = false);

	double tolerance;
	double extent;
	double near_;
};


// Geometric data for a contour
class CONTOUR :
	public BREP_CONTOUR
{
public:
	CONTOUR(BREP_FACE* face) :
		BREP_CONTOUR(face)
	{
		ClearMarks();
	}

	~CONTOUR() {}

	void DataCloseCallback(bool done = false)
	{
		ComputeNormal();
	}

	void ComputeNormal();

	bool IsHole();

	VECTOR normal;

	typedef enum
	{
		WRITE,
		DONTWRITE,
		WRITEREVERSE
	} OutputType;

	bool IsDontWrite() { return output == DONTWRITE; }
	bool IsWriteReverse() { return output == WRITEREVERSE; }

	void DontWrite() { output = DONTWRITE; }
	void WriteReverse() { if(output == WRITE) output = WRITEREVERSE; }

	void ClearMarks()
	{
		output = WRITE;
		inout = 0;
	}

	unsigned int InOut() { return inout; }
	void SetInOut(unsigned int v) { inout = v; }

private:
	// Indicates whether or not a contour needs to be written
	OutputType output;

	// Masks to indicate whether a contour lies inside or outside another
	// object after partitioning with PartitionFaces() in isect.cpp
	unsigned int inout;
};


// Geometric data for a face
class FACE :
	public BREP_FACE
{
public:
	FACE(BREP_SHELL* shell) :
		BREP_FACE(shell)
	{
	}

	~FACE() {}

	void DataCloseCallback(bool done = false)
	{
		ComputePlaneEquation();
	}

	// Compute the plane equation of the face with Newell's method
	void ComputePlaneEquation();

	double SignedDistanceToPlane(VERTEX* v, int* side);
	bool ConvexCorner(VERTEX* v0, VERTEX* v1, VERTEX* v2);
	double ShortestEdgeLength();

	//
	double tolerance;
	double extent;
	double near_;
	BOUNDINGBOX bounds;
	VECTOR normal;

	// Plane constant
	double d;
};


// Geometric data for a shell
class SHELL :
	public BREP_SHELL
{
public:
	SHELL(BREP_SOLID* solid) :
		BREP_SHELL(solid)
	{
	}

	~SHELL() {}

	void DataCloseCallback(bool done = false);

	BOUNDINGBOX bounds;
};


// Geometric data for a solid
class SOLID :
	public BREP_SOLID
{
public:
	SOLID() {}
	~SOLID() {}

	void DataCloseCallback(bool done = false);

	std::string filename;
	BOUNDINGBOX bounds;
};



// ******************* various functions **********************

void InitGeometry();

void SetMinimumVertexTolerance(double tol);
void SetMinimumEdgeTolerance(double tol);
void SetMinimumFaceTolerance(double tol);

extern SOLID* ReadFile(char* filename);
extern void OutputSolid(BREP_SOLID* solid, std::ostream& out);


#endif // !_GEOMETRY_H_


