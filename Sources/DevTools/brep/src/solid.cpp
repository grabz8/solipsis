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


// Creates an empty solid
BREP_SOLID::BREP_SOLID()
{
	vertices = NULL;
}

// Performs various actions to be done when the boundary
// representation of the solid has been completely specified
void BREP_SOLID::CloseCallback(const bool done)
{
	// Notify the user that the shells are complete
	//BREP_SHELL_RING::iterator shell;
	//for(shell = shells.begin(); shell != shells.end(); shell++)
	//	(*shell)->CloseCallback();

	// Notify the user that the solid is complete
	if(!done)
		DataCloseCallback(true);
}

// Release all memory associated with the solid
BREP_SOLID::~BREP_SOLID()
{
	// Inverse operations of BrepCreateSolid, and in reverse order

	// Destroy shells in the solid
	BREP_SHELL_RING::iterator shell;
	for(shell = shells.begin(); shell != shells.end(); shell++)
		{
			delete (*shell);
			shells.erase(shell);
		}

	// Next dispose of the vertex octree
	delete vertices;
}


// Connect a shell to the solid
void BREP_SOLID::ConnectShell(BREP_SHELL* shell)
{
	shell->solid = this;
	shells.push_back(shell);
}

// Disconnect a shell from the solid
void BREP_SOLID::DisconnectShell(BREP_SHELL* shell)
{
	BREP_SHELL_RING::iterator shelliterator;

	shelliterator = find(shells.begin(), shells.end(), shell);
	if(shelliterator != shells.end())
		shells.erase(shelliterator);

	shell->solid = NULL;
}


// Iterate though all contours inside the solid
void BREP_SOLID::IterateContours(void (*func)(BREP_CONTOUR*))
{
	BREP_SHELL_RING::iterator shell;
	for(shell = shells.begin(); shell != shells.end(); shell++)
		{
			BREP_FACE_RING::iterator face;
			for(face = (*shell)->faces.begin(); face != (*shell)->faces.end(); face++)
				{
					BREP_CONTOUR_RING::iterator contour;
					for(contour = (*face)->outer_contour.begin(); contour != (*face)->outer_contour.end(); contour++)
						func(*contour);
				}
		}
}


