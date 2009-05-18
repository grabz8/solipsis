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

#include "Extrusion.h"

#include "wrapper.h"
#include "wrapperobj.h"

Extrusion* wrapperobjOgre;

extern "C" {

void CALLBACK newTriangleStripOgre()
{
  wrapperobjOgre->beginTriangleStripOgre();
}

void CALLBACK endTriangleStripOgre()
{
  wrapperobjOgre->endTriangleStripOgre();
}

void CALLBACK vertexOgre(double x, double y, double z)
{
  wrapperobjOgre->addVertexOgre(x, y, z);
}

void CALLBACK normalOgre(double x, double y, double z)
{
  wrapperobjOgre->addNormalOgre(x, y, z);
}

void CALLBACK texcoordsOgre(double u, double v)
{
  wrapperobjOgre->addTexCoordOgre(u, v);
}

void CALLBACK pushmatrixOgre(){
    wrapperobjOgre->pushMatrixOgre();
}

void CALLBACK popmatrixOgre(){
    wrapperobjOgre->popMatrixOgre();
}

void CALLBACK multmatrixOgre(gleDouble mat[4][4]){
    wrapperobjOgre->multMatrixOgre(mat);
}

void CALLBACK loadmatrixOgre(gleDouble mat[4][4]){
    wrapperobjOgre->loadMatrixOgre(mat);
}

};

