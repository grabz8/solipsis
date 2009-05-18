/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

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

#include <cassert>
#include <cstdio>

#include <vector>

#include "Triangulator.h"

using namespace::std;
using namespace::roge;

Triangulator::Triangulator( const vector<Vec3d>& externPol)
{
	externPolyline = externPol;
}

Triangulator::~Triangulator()
{}

void Triangulator::initStruct(struct triangulateio& tio)
{
	tio.pointlist                  = NULL;
	tio.pointattributelist         = NULL;
	tio.pointmarkerlist            = NULL;
	tio.numberofpoints             = 0;
	tio.numberofpointattributes    = 0;

	tio.trianglelist               = NULL;
	tio.triangleattributelist      = NULL;
	tio.trianglearealist           = NULL;
	tio.neighborlist               = NULL;
	tio.numberoftriangles          = 0;
	tio.numberofcorners            = 0;
	tio.numberoftriangleattributes = 0;

	tio.segmentlist                = NULL;
	tio.segmentmarkerlist          = NULL;
	tio.numberofsegments           = 0;

	tio.holelist                   = NULL;
	tio.numberofholes              = 0;

	tio.regionlist                 = NULL;
	tio.numberofregions            = 0;

	tio.edgelist                   = NULL;
	tio.edgemarkerlist             = NULL;
	tio.normlist                   = NULL;
	tio.numberofedges              = 0;
	tio.pointlist                  = NULL;
	tio.pointattributelist         = NULL;
	tio.pointmarkerlist            = NULL;
	tio.numberofpoints             = 0;
	tio.numberofpointattributes    = 0;

}

void Triangulator::freeStruct(struct triangulateio& tio)
{
	if(tio.pointlist != NULL){
		free(tio.pointlist);
		tio.pointlist = NULL;
	}
	if(tio.pointattributelist != NULL){
		free(tio.pointattributelist);
		tio.pointattributelist = NULL;
	}
	if(tio.pointmarkerlist != NULL){
		free(tio.pointmarkerlist);
		tio.pointmarkerlist = NULL;
	}
	if(tio.trianglelist != NULL){
		free(tio.trianglelist);
		tio.trianglelist = NULL;
	}
	if(tio.triangleattributelist != NULL){
		free(tio.triangleattributelist);
		tio.triangleattributelist = NULL;
	}
	if(tio.trianglearealist != NULL){
		free(tio.trianglearealist);
		tio.trianglearealist = NULL;
	}
	if(tio.neighborlist != NULL){
		free(tio.neighborlist);
		tio.neighborlist = NULL;
	}
	if(tio.segmentlist != NULL){
		free(tio.segmentlist);
		tio.segmentlist = NULL;
	}
	if(tio.segmentmarkerlist != NULL){
		free(tio.segmentmarkerlist);
		tio.segmentmarkerlist = NULL;
	}
	if(tio.holelist != NULL){
		free(tio.holelist);
		tio.holelist = NULL;
	}
	if(tio.regionlist != NULL){
		free(tio.regionlist);
		tio.regionlist = NULL;
	}
	if(tio.edgelist != NULL){
		free(tio.edgelist);
		tio.edgelist = NULL;
	}
	if(tio.edgemarkerlist != NULL){
		free(tio.edgemarkerlist);
		tio.edgemarkerlist = NULL;
	}
	if(tio.normlist != NULL){
		free(tio.normlist);
		tio.normlist = NULL;
	}
}

void Triangulator::compute()
{
	assert(!externPolyline.empty());

	unsigned int j;

	vector<Vec3d> interiorPoints;
	vector<Vec3d> borderConstraint;
	int numOfPointsInConstraint = 0;

	borderConstraint = externPolyline;
	numOfPointsInConstraint += externPolyline.size();


	// Triangulation des points int�rieurs � la polyligne

	// pr�paration de la structure pour la triangulation
	char cmd[1024];
#ifdef WIN32
	sprintf_s(cmd,"-z -p -Q");
#else
	sprintf(cmd,"-z -p -Q");
#endif
	//   sprintf(cmd,"-z -p -c -Q");

		struct triangulateio in;
		struct triangulateio out;
		initStruct(in);
		initStruct(out);
		in.segmentlist = (int *)malloc(2*externPolyline.size()*sizeof(int));
		in.pointlist = (REAL *)malloc(2*externPolyline.size()*sizeof(REAL));

		j=0;
		unsigned int l;
		double zbak = externPolyline[0][2];
		for(l = 0 ; l < externPolyline.size() ; ++l){
			in.pointlist[2*l] = externPolyline[l][0];
			in.pointlist[2*l+1] = externPolyline[l][1];
			//vertices.push_back(externPolyline[l]);
			//cout << externPolyline[l][0] << " " << externPolyline[l][1] << endl;
			in.segmentlist[2*l] = l;
			in.segmentlist[2*l+1] = l+1;

			if(l == externPolyline.size()-1){
				in.segmentlist[2*l] = l;
				in.segmentlist[2*l+1] = 0;
			}
		}
		in.numberofpoints = externPolyline.size();
		in.numberofsegments = externPolyline.size();
		out.numberofpoints  = externPolyline.size();

		// triangulation
		triangulate(cmd, &in, &out, NULL);
		
		for( int u = 0 ; u < out.numberofpoints ; ++u){
			vertices.push_back(Vec3d(out.pointlist[2*u], out.pointlist[2*u+1], zbak));
		}
		
		/*if(in.numberofpoints != out.numberofpoints){
			cout << "difference" << endl;
			for(unsigned int u = 0 ; u < in.numberofpoints ; ++u){
				cout << in.pointlist[2*u] << " " << in.pointlist[2*u+1] << endl;
			}
			for(unsigned int u = 0 ; u < out.numberofpoints ; ++u){
				cout << out.pointlist[2*u] << " " << out.pointlist[2*u+1] << endl;
			}
		}*/

		for( int k = 0; k < out.numberoftriangles; k++){
			/*if(out.trianglelist[k*out.numberofcorners] >= count + externPolyline.size() || out.trianglelist[k*out.numberofcorners+1] >= count + externPolyline.size() || out.trianglelist[k*out.numberofcorners+2] >= count + externPolyline.size())
				cout << "erreur d'indice interne" << endl;*/
			triangles.push_back(Vec3i(out.trianglelist[k*out.numberofcorners] /*+count*/,
					out.trianglelist[k*out.numberofcorners+1]/*+count*/,
					out.trianglelist[k*out.numberofcorners+2]/*+count*/));
		}
//		count += out.numberofpoints;

		// Free the memory of triangle structures.
		freeStruct(in);
		freeStruct(out); 

}
