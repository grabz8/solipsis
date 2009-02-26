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
#include "error.h"
#include "vector.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


std::vector<VERTEX*> points;

static SOLID* currentSolid;
static SHELL* currentShell;
static FACE* currentFace;
static CONTOUR* currentContour;

static bool newobject;


// Creates a vertex
static void AddVertex(VECTOR vector)
{
	VERTEX* v = new VERTEX();
	v->point = vector;

	v->DataCloseCallback();

	currentSolid->vertices->AddWithDuplicates((BREP_VERTEX*)v);
	points.push_back(v);
}


void getline(std::istream& Stream, std::string& LineBuffer)
{
	std::ostringstream buffer;

	for(char c = Stream.get(); !Stream.eof(); c = Stream.get())
	{
		if('\r' == c)
		{
			if('\n' == Stream.peek())
				Stream.get();
			break;
		}
		else if('\n' == c)
		{
			break;
		}

		buffer << c;
	}

	LineBuffer = buffer.str();
}


bool dataline(std::ifstream& file, std::string& linebuffer)
{
	// Strip comments (lines beginning with '#') and empty lines,
	// return true if buffer has been filled, return false on end of file
	while(!file.eof())
		{
			getline(file, linebuffer);

			// Skip comments ...
			if(linebuffer[0] == '#')
				continue;

			if(!linebuffer.size())
				continue;

			// Must qualify
			return true;
		}

	return false;
}


// Reads the objects and collects them into one SOLID
SOLID* ReadFile(char* filename)
{
	std::ifstream file(filename);
	if(!file.good())
		{
			Error("ReadFile", "Couldn't open file");
			return 0;
		}

	currentSolid = new SOLID();
	currentSolid->filename = filename;

	// Return if empty file
	std::string linebuffer;
	if(!dataline(file, linebuffer))
		return 0;

	// New surface
	newobject = true;

	// Extract point and polygon numbers
	std::istringstream stream(linebuffer);
	unsigned int npoints = 0;
	unsigned int npols = 0;
	stream >> npoints >> npols;

	// Load objects ...
	while(npoints && npols)
		{
			// Read points
			points.clear();
			for(unsigned int n = 0; n < npoints; n++)
				{
					// Get next data line
					if(!dataline(file, linebuffer))
						{
							std::cerr << "ERROR: Unexpected end of input file"
								<< ", point #" << n << "." << std::endl;
							return false;
						}

					// Extract a record type ...
					std::istringstream stream(linebuffer);

					VECTOR coords;
					stream >> coords.x >> coords.y >> coords.z;
					AddVertex(coords);
				}

			// Faces
			for(unsigned int n = 0; n < npols; n++)
				{
					// Get next data line
					if(!dataline(file, linebuffer))
						{
							std::cerr << "ERROR: Unexpected end of input file"
								<< ", triangle #" << n << "." << std::endl;
							return false;
						}

					// New face
					if(newobject)
					{
						currentShell = new SHELL(currentSolid);
						newobject = false;
					}

					currentFace = new FACE(currentShell);

					if(currentFace == NULL)
						{
							std::cerr << "no current face for contour" << std::endl;
							return false;
						}

					currentContour = new CONTOUR(currentFace);

					// Extract a record type ...
					std::istringstream stream(linebuffer);

					// First vertex
					unsigned long idx;
					stream >> idx;

					VERTEX* first = points[idx];
					VERTEX* cur = first;

					// Decode edge vertices into points from point map
					do
						{
							stream >> idx;

							VERTEX* prev = cur;
							cur = points[idx];
							currentContour->CreateWing((BREP_VERTEX*)prev, (BREP_VERTEX*)cur);
						}
					while(!stream.eof());

					// Close the loop
					currentContour->CreateWing((BREP_VERTEX*)cur, (BREP_VERTEX*)first);
				}

			// Go on for another one ?
			npoints = npols = 0;
			if(dataline(file, linebuffer))
				{
					std::istringstream stream(linebuffer);
					stream >> npoints >> npols;
				}
		}

	currentSolid->DataCloseCallback();

	return currentSolid;
}
