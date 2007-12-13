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

#include "geometry.h"

#include <map>
#include <vector>

namespace
{

std::vector< std::vector<unsigned int> > faces;
std::vector<unsigned int> currentface;

typedef std::map<BREP_VERTEX*, unsigned long> PointMap;
PointMap points;

unsigned long vertexid = 0;


unsigned long GetVertexId(BREP_VERTEX *vertex)
{
	PointMap::iterator mid = points.find(vertex);
	if(mid != points.end())
		return mid->second;

	// Point has no ID yet
	unsigned long vid = vertexid++;
	points[vertex] = vid;

	return vid;
}

void OutputVertexId(BREP_VERTEX *vertex)
{
	currentface.push_back(GetVertexId(vertex));
}

} // namespace



void OutputReverseContour(BREP_CONTOUR* contour)
{
	if(!contour->wings)
		return;

	BREP_WING* wing = contour->wings->prev;
	do
		{
			OutputVertexId(wing->vertex);
			wing = wing->prev;
		}
	while(wing != contour->wings->prev);
}

void DoOutputContour(CONTOUR* contour)
{
	if(contour->IsDontWrite())
		return;

	currentface.clear();

	if(contour->IsWriteReverse())
		OutputReverseContour(contour);
	else
		contour->IterateVertices(OutputVertexId);

	faces.push_back(currentface);
}

static void OutputFaceWithHoles(BREP_FACE *face)
{
	// Count the number of contours to be written
	int nrcontours = 0;

	BREP_CONTOUR_RING::iterator contour;
	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		if(!((CONTOUR*)(*contour))->IsDontWrite())
			nrcontours++;

	// No contours to be written
	if(nrcontours == 0)
		return;

	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		{
			CONTOUR* c = (CONTOUR*)(*contour);

			//if(contour->IsHole())
			DoOutputContour(c);
		}
}

static int FaceWithHoles(BREP_FACE* face)
{
	BREP_CONTOUR_RING::iterator contour;
	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		if(!((CONTOUR*)(*contour))->IsDontWrite() && ((CONTOUR*)(*contour))->IsHole())
			return true;

	return false;
}

void OutputFace(BREP_FACE* face)
{
	BREP_CONTOUR_RING::iterator contour;

	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		{
			// For all wings in contour ...
			if(*contour != NULL && (*contour)->wings != NULL)
				{
					BREP_WING* first = (*contour)->wings;
					BREP_WING* iwing;
					BREP_WING* next;
					for(iwing = first, next = iwing->next; iwing != NULL; iwing = (next == first) ? NULL : next, next = next->next)
						if(iwing->vertex->wing_ring.size())
							GetVertexId(iwing->vertex);
				}
		}

	if(FaceWithHoles(face))
		OutputFaceWithHoles(face);
	else
		{
			for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
				DoOutputContour((CONTOUR*)(*contour));
		}
}

void OutputShell(BREP_SHELL* shell)
{
	BREP_FACE_RING::iterator face;
	for(face = shell->faces.begin(); face != shell->faces.end(); face++)
		OutputFace(*face);
}

void OutputSolid(BREP_SOLID* solid, std::ostream& out)
{
	points.clear();
	vertexid = 0;
	faces.clear();
	currentface.clear();

	// Get solid information
	BREP_SHELL_RING::iterator shell;
	for(shell = solid->shells.begin(); shell != solid->shells.end(); shell++)
		OutputShell(*shell);

	// Output solid information
	out << "#" << ((SOLID*)solid)->filename << std::endl;
	out << points.size() << " " << faces.size() << std::endl;

	// Output solid with 0-indexed points
	std::vector<VECTOR> vectors;
	vectors.resize(vertexid);
	for(PointMap::iterator mp = points.begin(); mp != points.end(); mp++)
		{
			vectors[mp->second] = ((VERTEX*)(mp->first))->point;
		}

	for(unsigned long n = 0; n < vertexid; n++)
		{
			VECTOR v = vectors[n];
			out << v.x << " " << v.y << " " << v.z << std::endl;
		}

	for(unsigned int n = 0; n < faces.size(); n++)
		{
			out << faces[n][0];

			for(unsigned int f = 1; f < faces[n].size(); f++)
				out << " " << faces[n][f];

			out << std::endl;
		}
}


