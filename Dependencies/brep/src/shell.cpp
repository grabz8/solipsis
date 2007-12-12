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


// Creates a new empty shell within the solid
BREP_SHELL::BREP_SHELL(BREP_SOLID* solid)
{
	if(solid != NULL)
		solid->ConnectShell(this);
}

// Perform various actions to be performed when a shell
// has been completely specified
void BREP_SHELL::CloseCallback(const bool done)
{
	// Notify the user that the faces of the shell have been completely specified
	BREP_FACE_RING::iterator face;
	for(face = faces.begin(); face != faces.end(); face++)
		(*face)->CloseCallback();

	// Notify the user that the shell is complete
	if(!done)
		DataCloseCallback(true);
}

// Release all memory associated to a shell and its contained faces
BREP_SHELL::~BREP_SHELL()
{
	// Disconnect the shell from the containing solid
	if(solid != NULL)
		solid->DisconnectShell(this);

	// Destroy its faces
	BREP_FACE_RING::iterator face;
	for(face = faces.begin(); face != faces.end(); face++)
		delete (*face);
}


// Connect a face to the shell
void BREP_SHELL::ConnectFace(BREP_FACE* face)
{
	faces.push_back(face);
}

// Disconnect the face from its containing shell
void BREP_SHELL::DisconnectFace(BREP_FACE* face)
{
	// Remove it from the list
	BREP_FACE_RING::iterator faceiterator;

	faceiterator = find(faces.begin(), faces.end(), face);
	if(faceiterator != faces.end())
		faces.erase(faceiterator);

	face->shell = NULL;
}


