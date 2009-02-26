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


// Creates a new empty face within the shell
BREP_FACE::BREP_FACE(BREP_SHELL* containingshell)
{
	shell = containingshell;

	if(shell != NULL)
		shell->ConnectFace(this);
}

// Various actions to be performed when a face is specified completely
void BREP_FACE::CloseCallback(const bool done)
{
	// Notify the user that the contours of the face
	// have been completely specified
	BREP_CONTOUR_RING::iterator contour;
	for(contour = outer_contour.begin(); contour != outer_contour.end(); contour++)
		(*contour)->CloseCallback();

	// Notify the user that the face is complete
	if(!done)
		DataCloseCallback(true);
}

// Release all storage associated with a face and its contours,
// including edges and vertices if not used in other faces as well
BREP_FACE::~BREP_FACE()
{
	// Disconnect the face from the containing shell
	if(shell != NULL)
		shell->DisconnectFace(this);

	// Destroy its contours
	BREP_CONTOUR_RING::iterator contour;
	for(contour = outer_contour.begin(); contour != outer_contour.end(); contour++)
		delete (*contour);
}

// Connect a contour
void BREP_FACE::ConnectContour(BREP_CONTOUR* contour)
{
	if(contour != NULL)
		outer_contour.push_back(contour);
}

// Disconnect a contour
void BREP_FACE::DisconnectContour(BREP_CONTOUR* contour)
{
	BREP_CONTOUR_RING::iterator contouriterator;

	contouriterator = find(outer_contour.begin(), outer_contour.end(), contour);
	if(contouriterator != outer_contour.end())
		{
			(*contouriterator)->face = 0;
			outer_contour.erase(contouriterator);
		}
	//else
		// BrepInfo(this, "BrepDisconnectContourFromFace", "outer contour disconnected from face");
}


