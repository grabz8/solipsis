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

#include <iostream>
#include <stdlib.h>

#include "error.h"


void Error(char* routine, char* text)
{
	std::cerr << "Error";
	if(routine != NULL)
		std::cerr << " (in subroutine " << routine << ")";
	std::cerr << ": " << text << "." << std::endl;
}

void Fatal(int errcode, char* routine, char* text)
{
	std::cerr << "Fatal error";
	if(routine != NULL)
		std::cerr << " (in subroutine " << routine << ")";
	std::cerr << ": " << text << "." << std::endl;
	exit(errcode);
}

void Warning(char* routine, char* text)
{
	std::cerr << "Warning";
	if(routine != NULL)
		std::cerr << " (in subroutine " << routine << ")";
	std::cerr << ": " << text << "." << std::endl;
}


