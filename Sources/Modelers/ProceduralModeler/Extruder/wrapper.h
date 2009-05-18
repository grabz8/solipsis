/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author MERLET Jonathan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef WRAPPER_H
#define WRAPPER_H

/*
 *
 * Wrapper functions for Extrusion
 *
 */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#define CALLBACK
#endif
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

void CALLBACK newTriangleStripOgre();

void CALLBACK endTriangleStripOgre();

void CALLBACK vertexOgre(double x, double y, double z);

void CALLBACK normalOgre(double x, double y, double z);

void CALLBACK texcoordsOgre(double u, double v);

void CALLBACK pushmatrixOgre();

void CALLBACK popmatrixOgre();

void CALLBACK multmatrixOgre(gleDouble[4][4]);

void CALLBACK loadmatrixOgre(gleDouble[4][4]);

#ifdef __cplusplus
};
#endif

#endif

