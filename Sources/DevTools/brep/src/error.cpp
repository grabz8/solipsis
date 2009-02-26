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

// Communication of informational, warning, error and fatal error messages

#include "brep.h"

#include <iostream>
#include <stdio.h>

// The default message callback routines ignore the client data
// Default callback for showing informational messages
static void brep_info(void *client_data, char *message)
{
#ifndef DEBUG
  // Don't show purely informational messages
#else // DEBUG
	std::cerr << message << std::endl;
#endif // DEBUG
}

// Default callback for showing warning messages
static void brep_warning(void *client_data, char *message)
{
	std::cerr << "warning: " << message << std::endl;
}

// Default callback for showing error messages
static void brep_error(void *client_data, char *message)
{
	std::cerr << "error: " << message << std::endl;
}

// Default callback for showing fatal error messages
static void brep_fatal(void *client_data, char *message)
{
	std::cerr << "fatal error: " << message << std::endl;
	exit(-1);
}

// Prints an informational message
void BrepInfo(void* client_data, char* routine, char* text)
{
	char buf[BREP_MAX_MESSAGE_LENGTH];

	sprintf(buf, text);

#ifdef DEBUG
	if(routine != NULL)
		sprintf(buf+strlen(buf), " (%s)", routine);
#endif

	brep_info(client_data, buf);
}

// Prints a warning message
void BrepWarning(void* client_data, char* routine, char* text)
{
	char buf[BREP_MAX_MESSAGE_LENGTH];

	sprintf(buf, "%s", text);

#ifdef DEBUG
	if(routine != NULL)
		sprintf(buf+strlen(buf), " (%s)", routine);
#endif

	brep_warning(client_data, buf);
}

// Prints an error message
void BrepError(void* client_data, char* routine, char* text)
{
	char buf[BREP_MAX_MESSAGE_LENGTH];

	sprintf(buf, "%s", text);

#ifdef DEBUG
	if(routine != NULL)
		sprintf(buf+strlen(buf), " (%s)", routine);
#endif

	brep_error(client_data, buf);
}

// Prints a fatal error message
void BrepFatal(void* client_data, char* routine, char* text)
{
	char buf[BREP_MAX_MESSAGE_LENGTH];

	sprintf(buf, "%s", text);

#ifdef DEBUG
	if(routine != NULL)
		sprintf(buf+strlen(buf), " (%s)", routine);
#endif

	brep_fatal(client_data, buf);
}


