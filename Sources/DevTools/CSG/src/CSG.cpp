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


#include "CSG.h"


// Creates a vertex
void CSG::AddVertex(VECTOR vector)
{
	VERTEX* v = new VERTEX();
	v->point = vector;

	v->DataCloseCallback();

	currentSolid->vertices->AddWithDuplicates((BREP_VERTEX*)v);
	points.push_back(v);
}


// Reads the objects and collects them into one SOLID
SOLID* CSG::dataLoad(float* vertex, 
					 unsigned int numVertex, 
					 unsigned int vertexDeclSize, 
					 unsigned int* index, 
					 unsigned int numIndex )
{
	currentSolid = new SOLID();
	currentSolid->filename = "";

	// New surface
	newobject = true;

	// Extract point and polygon numbers
	unsigned int npoints = numVertex;
	unsigned int npols = numIndex;

	// Load objects ...
	while(npoints && npols)
	{
		// Read points
		points.clear();
		for(unsigned int n = 0; n < npoints; n++)
		{
			// Extract a record type ...
			VECTOR coords;
			coords.x = *vertex++;
			coords.y = *vertex++;
			coords.z = *vertex++;
			AddVertex(coords);

			vertex += vertexDeclSize - 3;
		}

		// Faces
		unsigned int* indexStart = index;
		for(unsigned int n = 0; n < npols; n++)
		{
			// New face
			if(newobject)
			{
				currentShell = new SHELL(currentSolid);
				newobject = false;
			}

			currentFace = new FACE(currentShell);

			currentContour = new CONTOUR(currentFace);

			// First vertex
			unsigned long idx;
			idx = *index++;

			VERTEX* first = points[idx];
			VERTEX* cur = first;

			// Decode edge vertices into points from point map
			do
			{
				idx = *index++;

				VERTEX* prev = cur;
				cur = points[idx];
				currentContour->CreateWing((BREP_VERTEX*)prev, (BREP_VERTEX*)cur);
			}
			while( indexStart + numIndex >= index );

			// Close the loop
			currentContour->CreateWing((BREP_VERTEX*)cur, (BREP_VERTEX*)first);
		}

		npoints = npols = 0;
	}

	currentSolid->DataCloseCallback();

	return currentSolid;
}
