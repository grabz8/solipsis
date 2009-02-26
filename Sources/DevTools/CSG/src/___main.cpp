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

#include <time.h>
#include <vector>

#include "brep.h"
#include "geometry.h"
#include "isect.h"
#include "error.h"
#include "raytrace.h"
#include "vector.h"

clock_t start_clock;
int verbose;

typedef enum
{
	UNION,
	INTERSECTION,
	DIFFERENCE
} SelectionType;

SelectionType Selection;

// The algorithm requires to remember coplanar face pairs until partitioning
// and inside/outside mark propagation has been done
typedef std::pair<BREP_FACE*, BREP_FACE*> COPLANAR_FACES;
typedef std::vector<COPLANAR_FACES> COPLANAR_FACES_LIST;
COPLANAR_FACES_LIST coplanar_faces;


// Given a pair of coplanar faces, IterateMatchingContours() calls a given
// function for each pair of matching contours. Two contours match after
// partitioning as soon as they have one wing connecting the same vertices
// and their enclosed region is on the same side.
// See M. Segal, IEEE CG&A Jan '88 p 61-62
static void (*matching_contour_func)(BREP_CONTOUR*, BREP_CONTOUR*);
static BREP_CONTOUR* matching_contourA;
static BREP_FACE* matching_contour_faceB;

static void do_matching_contours(BREP_WING* wingB)
{
	BREP_CONTOUR* contourB = wingB->contour;

	if(contourB == NULL || contourB->face != matching_contour_faceB)
		return;

	// wingB has endpoints at the same location as the first wing of
	// matching_contourA. The contours match if the enclosed region is on the same
	// side, i.o.w. the first vertex of wingB is at the same location as the
	// first vertex of the first wing of matching_contourA
	if(VertexCompareLocation(matching_contourA->wings->vertex, wingB->vertex) == 8)
		matching_contour_func(matching_contourA, contourB);
}

static void IterateMatchingContours(BREP_FACE* faceA, BREP_FACE* faceB,
				    void (*func)(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB))
{
	void (*old_matching_contour_func)(BREP_CONTOUR*, BREP_CONTOUR*) = matching_contour_func;
	matching_contour_func = func;
	matching_contour_faceB = faceB;

	BREP_CONTOUR_RING::iterator contourA;
	for(contourA = faceA->outer_contour.begin(); contourA != faceA->outer_contour.end(); contourA++)
		if((*contourA)->wings && (*contourA)->wings->next != (*contourA)->wings->prev)
			{
				matching_contourA = *contourA;
				BrepIterateWingsBetweenLocations((*contourA)->wings->vertex,
					(*contourA)->wings->next->vertex,
					faceB->shell->solid->vertices,
					do_matching_contours);
			}

	matching_contour_func = old_matching_contour_func;
}

// Clears the contour masks
static void unset_mark(BREP_CONTOUR* contour)
{
	((CONTOUR*)(contour))->ClearMarks();
}

// Returns the inside/outside mask of the contour
static unsigned int inoutmark(BREP_CONTOUR* contour)
{
	return ((CONTOUR*)(contour))->InOut();
}

// Marks a contour with the given mask
static void mark_contour(BREP_CONTOUR* contour, unsigned int mask)
{
	((CONTOUR*)(contour))->SetInOut(mask);
}

// ContourA and contourB are matching contours in coplanar faces
// Keep one if they have the same orientation
static void keep_one_same(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB)
{
	if(((CONTOUR*)(contourA))->normal * ((CONTOUR*)(contourB))->normal > 0)
		{
			// Same orientation
			unset_mark(contourA);
			//((CONTOUR*)(contourA))->Write();
			unset_mark(contourB);
			((CONTOUR*)(contourB))->DontWrite();
		}
	else
		{
			// Opposite orientation
			unset_mark(contourA);
			((CONTOUR*)(contourA))->DontWrite();
			unset_mark(contourB);
			((CONTOUR*)(contourB))->DontWrite();
		}
}

// ContourA and contourB are matching contours in coplanar faces
// Keep one if they have opposite orientation.
static void keep_one_opposite(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB)
{
	if(((CONTOUR*)(contourA))->normal * ((CONTOUR*)(contourB))->normal > 0)
		{
			// Same orientation
			unset_mark(contourA);
			((CONTOUR*)(contourA))->DontWrite();
			unset_mark(contourB);
			((CONTOUR*)(contourB))->DontWrite();
		}
	else
		{
			// Opposite orientation
			unset_mark(contourA);
			//((CONTOUR*)(contourA))->Write();
			unset_mark(contourB);
			((CONTOUR*)(contourB))->DontWrite();
		}
}

// Partition pairs of faces belonging to shellA and shellB
static void partition_shells(BREP_SHELL* shellA, BREP_SHELL* shellB)
{
	BREP_FACE_RING::iterator faceA, faceB;
	for(faceA = shellA->faces.begin(); faceA != shellA->faces.end(); faceA++)
		for(faceB = shellB->faces.begin(); faceB != shellB->faces.end(); faceB++)
			{
				FACE* faA = (FACE*)((*faceA));
				FACE* faB = (FACE*)((*faceB));
				if(!DisjunctBounds(faA->bounds, faB->bounds))
					if(PartitionFaces(faA, faB) == COPLANAR_ISECT)
						// Record a pair of coplanar faces
						coplanar_faces.push_back(COPLANAR_FACES(*faceA, *faceB));
			}
}

/*
void do_isect(SOLID* solid)
{
  // COPLANAR_FACELIST* copl;

	BREP_SHELL_RING::iterator shellA;
	for(shellA = solid->shells.begin(); shellA != solid->shells.end(); shellA++)
		{
			BREP_SHELL_RING::iterator shellB = shellA;
			shellB++;
			for(; shellB != solid->shells.end(); shellB++)
				{
					SHELL* shA = (SHELL*)((*shellA)->client_data);
					SHELL* shB = (SHELL*)((*shellB)->client_data);
					if(!DisjunctBounds(shA->bounds, shB->bounds))
						partition_shells(*shellA, *shellB);
				}
		}

	for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
		IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_same);
}
*/

// inside/outside mark propagation: when a contour is cut during face partitioning,
// the cut contour parts are marked whether they are inside or outside the transversal
// solid. After partitioning, thi mark can be propagated to neighbouring contours,
// reducing the number of contours for which an explicit inside/outside test will
// be necessary
static void contour_propagate_mark(BREP_CONTOUR* contour);
static void wing_propagate_mark(BREP_WING* wing, unsigned int mark)
{
	BREP_CONTOUR* ocontour = BrepEdgeOtherWing(wing)->contour;
	if(ocontour == NULL || inoutmark(ocontour))
		return;

	mark_contour(ocontour, mark);
	contour_propagate_mark(ocontour);
}

static void contour_propagate_mark(BREP_CONTOUR* contour)
{
	unsigned int mark = inoutmark(contour);

	if(mark != 0)
		contour->IterateWings1A((void (*)(BREP_WING*, void*))wing_propagate_mark, (void*)mark);
}

// Propagates inside/outside mark of contours for which it was
// determined during PartitionFaces() to neighbouring contours
static void csg_propagate_mark(SOLID* solid)
{
	solid->IterateContours(contour_propagate_mark);
}

// inside/outside mark propagation only reduces the number of contours for which
// an explicit inside/outside test will be necessary. After propagation, this
// test is necessary for all contours that remained unmarked

static SOLID* global_othersolid;

static void contour_mark_unmarked(BREP_CONTOUR* contour)
{
	if (inoutmark(contour))
		return;

	unsigned int mark;
	if((mark = ContourInSolid(contour, global_othersolid)))
		{
			mark_contour(contour, mark);
			contour_propagate_mark(contour);
		}
	else
		{
			// The contour is most probably coplanar with a contour
			// of the other solid and will be handled later
			Warning("contour_mark_unmarked", "can't determine whether contour is inside or outside other solid");
		}
}

static void csg_mark_unmarked(SOLID* solid, SOLID* othersolid)
{
	global_othersolid = othersolid;
	solid->IterateContours(contour_mark_unmarked);
}


// Mark a contour that is outside the transversal solid for writing
static void mark_out(BREP_CONTOUR* contour)
{
	if(!(inoutmark(contour) & OUTMASK))
		((CONTOUR*)(contour))->DontWrite();
}

// Mark a contour that is inside the transversal solid for writing
static void mark_in(BREP_CONTOUR* contour)
{
	if(!(inoutmark(contour) & INMASK))
		((CONTOUR*)(contour))->DontWrite();
}

// Mark a contour that is inside the transversal solid for reverse writing
static void mark_in_reverse(BREP_CONTOUR* contour)
{
	if(inoutmark(contour) & INMASK)
		((CONTOUR*)(contour))->WriteReverse();
	else
		((CONTOUR*)(contour))->DontWrite();
}

static void mark_with_outmask(BREP_CONTOUR* contour)
{
	mark_contour(contour, OUTMASK);
}


// CSG main program
static void do_csg(SOLID* solidA, SOLID* solidB)
{
	solidA->IterateContours(unset_mark);
	solidB->IterateContours(unset_mark);

	if(!DisjunctBounds(((SOLID*)(solidA))->bounds, ((SOLID*)(solidB))->bounds))
		{
			BREP_SHELL_RING::iterator shellA;
			for(shellA = solidA->shells.begin(); shellA != solidA->shells.end(); shellA++)
				{
					BREP_SHELL_RING::iterator shellB;
					for(shellB = solidB->shells.begin(); shellB != solidB->shells.end(); shellB++)
						{
							SHELL* shA = (SHELL*)(*shellA);
							SHELL* shB = (SHELL*)(*shellB);
							if(!DisjunctBounds(shA->bounds, shB->bounds))
								partition_shells(*shellA, *shellB);
						}
				}
		}
	else
		{
			solidA->IterateContours(mark_with_outmask);
			solidB->IterateContours(mark_with_outmask);
		}

	csg_propagate_mark(solidA);
	csg_mark_unmarked(solidA, solidB);
	csg_propagate_mark(solidB);
	csg_mark_unmarked(solidB, solidA);

	COPLANAR_FACES_LIST::iterator cfaces;
	switch(Selection)
		{
			case UNION:
				solidA->IterateContours(mark_out);
				solidB->IterateContours(mark_out);

				for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
					IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_same);
			break;

			case INTERSECTION:
				solidA->IterateContours(mark_in);
				solidB->IterateContours(mark_in);

				for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
					IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_same);
			break;

			case DIFFERENCE:
				solidA->IterateContours(mark_out);
				solidB->IterateContours(mark_in_reverse);

				for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
					IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_opposite);
			break;
		}
}

// Prints some statistics in verbose mode
static void PrintStats()
{
	std::cerr << "Processing time: " << ((float)(clock() - start_clock)/(float)CLOCKS_PER_SEC) << " seconds" << std::endl;;
}

// Prints program usage
static void PrintUsage(std::ostream& out)
{
	out << "Usage: csg [options] fileA.raw fileB.raw\n\n";
	out << "Options:\n\n";
	out << "\t-t x: vertex tolerance (x = positive real number, default = 1e-6)\n";
	out << "\t-p x: min. coplanarity factor (x = positive real number, default = 500)\n";
	out << "\t-v: verbose mode\n";
	out << "\t-d: compute CSG difference\n";
	out << "\t-u: compute CSG union\n";
	out << "\t-i: compute CSG intersection\n";
	out << "\t-h: print usage\n\n";
}

// Sets defaults for various options
static void Defaults()
{
	verbose = false;

	SetMinimumVertexTolerance(1e-6);
	SetMinimumEdgeTolerance(1e-12);
	SetMinimumFaceTolerance(1e-12);
	SetMinCoplanarityFactor(500);

	Selection = UNION;
}

// Parse command line options
static int GetOptions(int argc, char** argv)
{
	int i, j;
	float x;

#define next_arg_parm {i++; j=1;}
#define next_arg_no_parm {if (argv[i][j+1]) {j++;} else {i++; j=1;}}

  for (i=1, j=1; i<argc && argv[i][0] == '-';) {
    switch (argv[i][j]) {
    case 't':
      if ((sscanf(&argv[i][j+1], "%f", &x) != 1 &&
	  (++i >= argc || sscanf(argv[i], "%f", &x) != 1)) ||
	  (x <= 0))
	Fatal(1, NULL, "option -t requires a positive real argument");
      else
	SetMinimumVertexTolerance((double)x);
      next_arg_parm;
      break;
    case 'p':
      if ((sscanf(&argv[i][j+1], "%f", &x) != 1 &&
	  (++i >= argc || sscanf(argv[i], "%f", &x) != 1)) ||
	  (x <= 0))
	Fatal(1, NULL, "option -p requires a positive real argument");
      else
	SetMinCoplanarityFactor((double)x);
      next_arg_parm;
      break;
    case 'v':
      verbose = true;
      next_arg_no_parm;
      break;
    case 'd':
	Selection = DIFFERENCE;
      next_arg_no_parm;
      break;
    case 'u':
	Selection = UNION;
      next_arg_no_parm;
      break;
    case 'i':
	Selection = INTERSECTION;
      next_arg_no_parm;
      break;
    case 'h':
    case '?':
      PrintUsage(std::cerr);
      exit(0);
    default:
	char text[BREP_MAX_MESSAGE_LENGTH];
	sprintf(text, "Invalid option '%c'.\n", argv[i][j]);
	Fatal(1, NULL, text);
    }
  }

  return i;
}

// Main program
int main(int argc, char** argv)
{
	char* progname;
	if((progname = strrchr(argv[0], '/')) != NULL)
		progname = progname+1;
	else
		progname = strdup(argv[0]);

	Defaults();

	int i = GetOptions(argc, argv);
	if(i >= argc-1)
		{
			std::cerr << "Usage: " << progname << " [options] file1.raw file2.raw" << std::endl;
			exit(1);
		}

	start_clock = clock();

	// Read the objects
	SOLID* solidA = ReadFile(argv[i]);
	if(!solidA)
		exit(1);

	solidA->DataCloseCallback();

	if(verbose)
		{
			std::cerr << "------------- after reading " << argv[i] << " -------------- " << std::endl;
			PrintStats();
		}

	SOLID* solidB = ReadFile(argv[i+1]);
	if(!solidB)
		exit(1);

	solidB->DataCloseCallback();

	if(verbose)
		{
			std::cerr << "------------- after reading " << argv[i+1] << " -------------- " << std::endl;
			PrintStats();
		}

		do_csg(solidA, solidB);

	if(verbose)
		{
			std::cerr << "------------- after doing CSG operation ------------------ " << std::endl;
			PrintStats();
		}

	OutputSolid(solidA, std::cout);
	OutputSolid(solidB, std::cout);

	return 0;
}


